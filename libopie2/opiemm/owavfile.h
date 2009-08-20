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

#ifndef OWAVFILE_H
#define OWAVFILE_H

#include <qobject.h>
#include <qfile.h>
#include <qstring.h>

#define WAVE_FORMAT_DVI_ADPCM (0x0011)
#define WAVE_FORMAT_PCM (0x0001)

typedef struct {
    char           riffID[4];
    unsigned long  riffLen;
    char           wavID[4];
    char           fmtID[4];
    unsigned long  fmtLen;
    unsigned short fmtTag;
    unsigned short nChannels;
    unsigned long  sampleRate;
    unsigned long  avgBytesPerSec;
    unsigned short nBlockAlign;
    unsigned short bitsPerSample;
} wavhdr;

typedef struct {
    unsigned short wExtSize;
    unsigned short wSamplesPerBlock;
} wavhdrext_ima;

typedef struct {
    char           factID[4];
    unsigned long  dwFactSize;
    unsigned long  dwSamplesWritten;
} wavfactblk;

typedef struct {
    char           dataID[4];
    unsigned long  dataLen;
} wavdatahdr;

typedef struct {
    int sampleRate;
    int resolution; //bitrate
    int channels; //number of channels
    int format; //wavfile format PCM.. ADPCM
} OWavFileParameters;

class OWavFile {
public:
    OWavFile( const QString &fileName );
    OWavFile( const QString &fileName, OWavFileParameters fileparams,
        unsigned short samplesPerBlock );
    ~OWavFile();
    bool adjustHeaders( unsigned long total );

    int getfd();
    int getFormat();
    int getResolution();
    int getSampleRate();
    int getNumberSamples();
    int getChannels();
    QString getFileName();
    int openFile();
    int createFile();
    void closeFile();
    bool isOpen();

private:
    OWavFileParameters m_fileparams;
    int m_numsamples;
    unsigned short m_samplesperblock;
    wavhdr hdr;
    wavhdrext_ima imaext;
    wavdatahdr datahdr;
    wavfactblk factblk;
    QFile track;

    bool setWavHeader(int fd);
    int parseWavHeader(int fd);
};

#endif
