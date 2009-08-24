/*
                             This file is part of the Opie Project
              =.             (C) 2009 Team Opie <opie@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "owavfile.h"

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

OWavFile::OWavFile( const QString &fileName )
{
    owarn << "new wave file (no params): " << fileName << oendl;
    track.setName(fileName);
}

OWavFile::OWavFile( const QString &fileName, OWavFileParameters fileparams,
        unsigned short samplesPerBlock )
{
    owarn << "new wave file: " << fileName << oendl;
    m_fileparams = fileparams;
    m_samplesperblock = samplesPerBlock;
    track.setName(fileName);
}

OWavFile::~OWavFile() {
    closeFile();
}

void OWavFile::closeFile() {
    if(track.isOpen())
        track.close();
}

int OWavFile::openFile() {
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

int OWavFile::createFile() {
    if(!track.open( IO_ReadWrite | IO_Truncate )) {
        QString errorMsg = (QString)strerror(errno);
        odebug << errorMsg << oendl;
        QMessageBox::message("Note", "Error opening file.\n" + errorMsg);
        return -1;
    }

    setWavHeader( track.handle() );
    return track.handle();
}

bool OWavFile::setWavHeader(int fd) {
    strncpy(hdr.riffID, "RIFF", 4); // RIFF
    strncpy(hdr.wavID, "WAVE", 4); //WAVE
    strncpy(hdr.fmtID, "fmt ", 4); // fmt
    hdr.riffLen = 0;
    hdr.fmtLen = 16;

    if( m_fileparams.format == WAVE_FORMAT_PCM) {
        hdr.fmtTag = 1; // PCM
//    odebug << "set header  WAVE_FORMAT_PCM" << oendl;
    }
    else {
        hdr.fmtTag = WAVE_FORMAT_DVI_ADPCM; //intel ADPCM
 //    odebug << "set header  WAVE_FORMAT_DVI_ADPCM" << oendl;
    }

    //  (*hdr).nChannels = 1;//filePara.channels;// ? 2 : 1*/; // channels
    hdr.nChannels = m_fileparams.channels;// ? 2 : 1*/; // channels

    hdr.sampleRate = m_fileparams.sampleRate; //samples per second
    hdr.avgBytesPerSec = (m_fileparams.sampleRate)*( m_fileparams.channels*(m_fileparams.resolution/8)); // bytes per second
    hdr.nBlockAlign = m_fileparams.channels*( m_fileparams.resolution/8); //block align
    hdr.bitsPerSample = m_fileparams.resolution; //bits per sample 8, or 16

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
    datahdr.dataLen = 0;
    write( fd, &datahdr, sizeof(datahdr));

//   owarn << "writing header: bitrate " << m_fileparams.resolution << ", samplerate " << m_fileparams.sampleRate << ",  channels " << m_fileparams.channels << oendl;
    return true;
}

bool OWavFile::adjustHeaders(unsigned long total) {
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

int OWavFile::parseWavHeader(int fd) {
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

    m_fileparams.format = hdr.fmtTag;
    m_fileparams.channels = hdr.nChannels;
    m_fileparams.sampleRate = hdr.sampleRate;
    m_fileparams.resolution = hdr.bitsPerSample;

    if (hdr.fmtTag == WAVE_FORMAT_DVI_ADPCM) {
        m_fileparams.resolution = 16;
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

QString OWavFile::getFileName() {
    return track.name();
}

int OWavFile::getfd(){
    return track.handle();
}

int OWavFile::getFormat() {
    return m_fileparams.format;
}

int OWavFile::getResolution() {
    return m_fileparams.resolution;
}

int OWavFile::getSampleRate() {
    return m_fileparams.sampleRate;
}

int OWavFile::getNumberSamples() {
    return m_numsamples;
}

int OWavFile::getChannels() {
    return m_fileparams.channels;
}

bool OWavFile::isOpen() {
    return track.isOpen();
}
