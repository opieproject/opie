//wavFile.cpp
#include "wavFile.h"
extern "C" {
#include "ima_rw.h"
}

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qmessagebox.h>

/* STD */
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

WavFile::WavFile( QObject * parent, const QString &fileName, int sampleRate,
                  int channels, int resolution, int format, unsigned short samplesPerBlock )
        : QObject( parent)
{
    owarn << "new wave file: " << fileName << oendl;
    m_samplerate = sampleRate;
    m_format = format;
    m_channels = channels;
    m_resolution = resolution;
    m_samplesperblock = samplesPerBlock;
    track.setName(fileName);
}

WavFile::~WavFile() {
    closeFile();
}

void WavFile::closeFile() {
    if(track.isOpen())
        track.close();
}

int WavFile::openFile() {
    odebug << "open play file " << track.name() << oendl;
    closeFile();

    if(!track.open(IO_ReadOnly)) {
        QString errorMsg = (QString)strerror(errno);
        odebug << "<<<<<<<<<<< " << errorMsg << oendl;
        QMessageBox::message("Note", "Error opening file.\n" + errorMsg);
        return -1;
    } 

    parseWavHeader( track.handle());
    return track.handle();
}

int WavFile::createFile() {
    if(!track.open( IO_ReadWrite | IO_Truncate )) {
        QString errorMsg = (QString)strerror(errno);
        odebug << errorMsg << oendl;
        QMessageBox::message("Note", "Error opening file.\n" + errorMsg);
        return -1;
    } 

    setWavHeader( track.handle() );
    return track.handle();
}

bool WavFile::setWavHeader(int fd) {
    strncpy(hdr.riffID, "RIFF", 4); // RIFF
    strncpy(hdr.wavID, "WAVE", 4); //WAVE
    strncpy(hdr.fmtID, "fmt ", 4); // fmt
    hdr.fmtLen = 16;

    if( m_format == WAVE_FORMAT_PCM) {
        hdr.fmtTag = 1; // PCM
//    odebug << "set header  WAVE_FORMAT_PCM" << oendl;
    }
    else {
        hdr.fmtTag = WAVE_FORMAT_DVI_ADPCM; //intel ADPCM
 //    odebug << "set header  WAVE_FORMAT_DVI_ADPCM" << oendl;
    }

    //  (*hdr).nChannels = 1;//filePara.channels;// ? 2 : 1*/; // channels
    hdr.nChannels = m_channels;// ? 2 : 1*/; // channels

    hdr.sampleRate = m_samplerate; //samples per second
    hdr.avgBytesPerSec = (m_samplerate)*( m_channels*(m_resolution/8)); // bytes per second
    hdr.nBlockAlign = m_channels*( m_resolution/8); //block align
    hdr.bitsPerSample = m_resolution; //bits per sample 8, or 16

    if( hdr.fmtTag == WAVE_FORMAT_DVI_ADPCM ) {
        hdr.bitsPerSample = 4;
        imaext.wExtSize = 2;
        hdr.fmtLen = 16 + 2 + imaext.wExtSize;

        hdr.nBlockAlign = lsx_ima_bytes_per_block(hdr.nChannels, m_samplesperblock);
        // Why we have to do this I'm not exactly sure, but the input samples per block
        // is not compatible with the ADPCM format (or so sox reports anyway)
        imaext.wSamplesPerBlock = lsx_ima_samples_in(0, hdr.nChannels, hdr.nBlockAlign, 0);
    }

    write( fd, &hdr, sizeof(hdr));

    if( hdr.fmtTag == WAVE_FORMAT_DVI_ADPCM ) {
        // Header extension
        write( fd, &imaext, sizeof(imaext));

        // fact chunk
        strncpy(factblk.factID, "fact", 4);
        factblk.dwFactSize = 4;
        factblk.dwSamplesWritten = 0;
        write( fd, &factblk, sizeof(factblk));
    }

    strncpy(datahdr.dataID, "data", 4);
    write( fd, &datahdr, sizeof(datahdr));
    
//   owarn << "writing header: bitrate " << m_resolution << ", samplerate " << m_samplerate << ",  channels " << m_channels << oendl;
    return true;
}

bool WavFile::adjustHeaders(unsigned long total) {
    // This is cheating, but we only support PCM and IMA ADPCM 
    // at the moment so we can get away with it
    int hdrsize;
    if( hdr.fmtTag == WAVE_FORMAT_DVI_ADPCM )
        hdrsize = 36 + 4 + 12;
    else 
        hdrsize = 36;

    int fd = track.handle();
    
    // RIFF size
    lseek(fd, 4, SEEK_SET);
    unsigned long size = total + hdrsize;
    write( fd, &size, sizeof(size));
    // data chunk size
    lseek( fd, hdrsize + 4, SEEK_SET);
    write( fd, &total, sizeof(total));
//  owarn << "adjusting header " << total << "" << oendl;
    return true;
}

int WavFile::parseWavHeader(int fd) {
    char string[4];
    int found;
    short fmt;
    unsigned short ch, bitrate;
    unsigned long samplerrate, longdata;

    ssize_t bytes = read(fd, &hdr, sizeof(hdr));
    if(bytes < sizeof(hdr)) {
        return -1;
    }

    if(strncmp(hdr.riffID, "RIFF", 4))
        return -1;
    if(strncmp(hdr.wavID, "WAVE", 4))
        return -1;
    if(strncmp(hdr.fmtID, "fmt ", 4))
        return -1;

    if (hdr.fmtTag != WAVE_FORMAT_PCM && hdr.fmtTag != WAVE_FORMAT_DVI_ADPCM) {
        return -1;
    }

    m_format = hdr.fmtTag;
    m_channels = hdr.nChannels;
    m_samplerate = hdr.sampleRate;
    m_resolution = hdr.bitsPerSample;
    
    if (hdr.fmtTag == WAVE_FORMAT_DVI_ADPCM) {
        m_resolution = 16;
        bytes = read(fd, &imaext, sizeof(imaext));
        if(bytes < sizeof(imaext)) {
            return -1;
        }
    }

    lseek(fd, 20 + hdr.fmtLen, SEEK_SET);
    while(true) {
        bytes = read(fd, &datahdr, sizeof(datahdr));
        if(bytes < sizeof(datahdr))
            return -1;
        if(!strncmp(datahdr.dataID, "data", 4))
            break;
        lseek(fd, datahdr.dataLen, SEEK_CUR);
    }
    m_numsamples = datahdr.dataLen;

    return 0;
}

QString WavFile::getFileName() {
    return track.name();
}

int WavFile::getfd(){
    return track.handle();
}

int WavFile::getFormat() {
    return m_format;
}

int WavFile::getResolution() {
    return m_resolution;
}

int WavFile::getSampleRate() {
    return m_samplerate;
}

int WavFile::getNumberSamples() {
    return m_numsamples;
}

int WavFile::getChannels() {
    return m_channels;
}

bool WavFile::isOpen() {
    return track.isOpen();
}
