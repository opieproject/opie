//wavFile.cpp
#include "wavFile.h"
#include "qtrec.h"
extern "C" {
#include "ima_rw.h"
}

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>
using namespace Opie::Core;

/* QT */
#include <qmessagebox.h>
#include <qdir.h>

/* STD */
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <math.h>
#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

WavFile::WavFile( QObject * parent,const QString &fileName, bool makeNwFile, int sampleRate,
                  int channels, int resolution, int format, unsigned short samplesPerBlock )
        : QObject( parent)
{
    owarn << "new wave file: " << fileName << oendl;
    bool b =  makeNwFile;
    wavSampleRate=sampleRate;
    wavFormat=format;
    wavChannels=channels;
    wavResolution=resolution;
    wavSamplesPerBlock=samplesPerBlock;
    useTmpFile=false;
    if( b) {
        newFile();
    } else {
        openFile(fileName);
    }
}

bool WavFile::newFile() {
//  odebug << "Set up new file" << oendl;
    Config cfg("OpieRec");
    cfg.setGroup("Settings");

    currentFileName=cfg.readEntry("directory",QDir::homeDirPath());
    QString date;
    QDateTime dt = QDateTime::currentDateTime();
    date = dt.toString();//TimeString::dateString( QDateTime::currentDateTime(),false,true);
    date.replace(QRegExp("'"),"");
    date.replace(QRegExp(" "),"_");
    date.replace(QRegExp(":"),"-");
    date.replace(QRegExp(","),"");

    QString currentFile=date;
    if(currentFileName.right(1).find("/",0,true) == -1)
        currentFileName += "/" + date;
    else
        currentFileName += date;
    currentFileName+=".wav";

    //  odebug << "set up file for recording: "+currentFileName << oendl;
    char pointer[] = "/tmp/opierec-XXXXXX";
    int fd = 0;

    if( currentFileName.find("/mnt",0,true) == -1
        && currentFileName.find("/tmp",0,true) == -1 ) {
        // if destination file is most likely in flash (assuming jffs2)
        // we have to write to a different filesystem first

        useTmpFile = true;
        if(( fd = mkstemp( pointer)) < 0 ) {
            perror("mkstemp failed");
            return false;
        }

    //    odebug << "Opening tmp file " << pointer << "" << oendl;
        track.setName( pointer);

    } else { //just use regular file.. no moving

        useTmpFile = false;
        track.setName( currentFileName);
    }
    if(!track.open( IO_ReadWrite | IO_Truncate)) {
        QString errorMsg=(QString)strerror(errno);
        odebug << errorMsg << oendl;
        QMessageBox::message("Note", "Error opening file.\n" +errorMsg);

        return false;
    } else {
        setWavHeader( track.handle() );
    }
    return true;
}

WavFile::~WavFile() {
    closeFile();
}

void WavFile::closeFile() {
    if(track.isOpen())
        track.close();
}

int WavFile::openFile(const QString &currentFileName) {
    qWarning("open play file "+currentFileName);
    closeFile();

    track.setName(currentFileName);

    if(!track.open(IO_ReadOnly)) {
        QString errorMsg=(QString)strerror(errno);
        odebug << "<<<<<<<<<<< "+errorMsg+currentFileName << oendl;
        QMessageBox::message("Note", "Error opening file.\n" +errorMsg);
        return -1;
    } else {
        parseWavHeader( track.handle());
    }
    return track.handle();
}

bool WavFile::setWavHeader(int fd) {
    strncpy(hdr.riffID, "RIFF", 4); // RIFF
    strncpy(hdr.wavID, "WAVE", 4); //WAVE
    strncpy(hdr.fmtID, "fmt ", 4); // fmt
    hdr.fmtLen = 16;

    if( wavFormat == WAVE_FORMAT_PCM) {
        hdr.fmtTag = 1; // PCM
//    odebug << "set header  WAVE_FORMAT_PCM" << oendl;
    }
    else {
        hdr.fmtTag = WAVE_FORMAT_DVI_ADPCM; //intel ADPCM
 //    odebug << "set header  WAVE_FORMAT_DVI_ADPCM" << oendl;
    }

    //  (*hdr).nChannels = 1;//filePara.channels;// ? 2 : 1*/; // channels
    hdr.nChannels = wavChannels;// ? 2 : 1*/; // channels

    hdr.sampleRate = wavSampleRate; //samples per second
    hdr.avgBytesPerSec = (wavSampleRate)*( wavChannels*(wavResolution/8)); // bytes per second
    hdr.nBlockAlign = wavChannels*( wavResolution/8); //block align
    hdr.bitsPerSample = wavResolution; //bits per sample 8, or 16

    if( hdr.fmtTag == WAVE_FORMAT_DVI_ADPCM ) {
        hdr.bitsPerSample = 4;
        imaext.wExtSize = 2;
        hdr.fmtLen = 16 + 2 + imaext.wExtSize;

        hdr.nBlockAlign = lsx_ima_bytes_per_block(hdr.nChannels, wavSamplesPerBlock);
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
    
//   owarn << "writing header: bitrate " << wavResolution << ", samplerate " << wavSampleRate << ",  channels " << wavChannels << oendl;
    return true;
}

bool WavFile::adjustHeaders(int fd, unsigned long total) {
    // This is cheating, but we only support PCM and IMA ADPCM 
    // at the moment so we can get away with it
    int hdrsize;
    if( hdr.fmtTag == WAVE_FORMAT_DVI_ADPCM )
        hdrsize = 36 + 4 + 12;
    else 
        hdrsize = 36;
    
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

    wavFormat = hdr.fmtTag;
    wavChannels = hdr.nChannels;
    wavSampleRate = hdr.sampleRate;
    wavResolution = hdr.bitsPerSample;
    
    if (hdr.fmtTag == WAVE_FORMAT_DVI_ADPCM) {
        wavResolution = 16;
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
    wavNumberSamples = datahdr.dataLen;

    return 0;
}

QString WavFile::trackName() {
    return track.name();
}

int WavFile::wavHandle(){
    return track.handle();
}

int WavFile::getFormat() {
    return wavFormat;
}

int WavFile::getResolution() {
    return wavResolution;
}

int WavFile::getSampleRate() {
    return wavSampleRate;
}

int WavFile::getNumberSamples() {
    return wavNumberSamples;
}

bool WavFile::isTempFile() {
    return useTmpFile;
}

int WavFile::getChannels() {
    return wavChannels;
}
