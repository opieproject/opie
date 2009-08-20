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

#include "owavrecorder.h"

extern "C" {
#include "ima_rw.h"
}

namespace Opie {
namespace MM {

bool OWavRecorder::setup( const QString &filename, OWavFileParameters fileparams )
{
    m_filename = filename;
    m_fileparams = fileparams;

    if(setupSoundDevice( true )) // recording
        return setupFile();
    else
        return false;
}

bool OWavRecorder::setupFile()
{
    char pointer[] = "/tmp/opierec-XXXXXX";
    int fd = 0;

    QString actualFile;
    if( m_filename.find("/mnt",0,true) == -1
        && m_filename.find("/tmp",0,true) == -1 ) {
        // if destination file is most likely in flash (assuming jffs2)
        // we have to write to a different filesystem first

        if(( fd = mkstemp( pointer)) < 0 ) {
            perror("mkstemp failed");
            return false;
        }

        actualFile = (QString)pointer;
    }
    else {
        // just use regular file.. no moving
        actualFile = m_filename;
    }

    m_wavfile = new OWavFile(actualFile,
                            m_fileparams,
                            m_bufsize/2);

    fd = m_wavfile->createFile();
    if(fd == -1)
        return false;

    return true;
}

void OWavRecorder::record( OWavRecorderCallback *callback )
{
    int bytesWritten = 0;
    int number = 0;

    if(m_bufsize > 0) {
        char *buffer = (char *) malloc(m_bufsize);
        int state = 0;

        int samplesPerBlock = m_bufsize/2;
        int fd = m_wavfile->getfd();

        int adpcm_outsize = lsx_ima_bytes_per_block( m_fileparams.channels, samplesPerBlock );
        unsigned char adpcm_outbuf[ adpcm_outsize ];
        if( m_fileparams.format == WAVE_FORMAT_DVI_ADPCM) {
            memset( adpcm_outbuf, 0, adpcm_outsize);
            lsx_ima_init_table();
        }

        bool stopflag = false;
        while(!stopflag) {
            m_device->devRead( buffer );
            if( m_fileparams.format == WAVE_FORMAT_DVI_ADPCM) {
                lsx_ima_block_mash_i( m_fileparams.channels, (short *)buffer, samplesPerBlock, &state, adpcm_outbuf, 9 );
                number = ::write( fd, adpcm_outbuf, adpcm_outsize );
            }
            else
                number = ::write( fd, buffer, m_bufsize );
            bytesWritten += number;

            callback->recorderCallback( buffer, number, bytesWritten, stopflag );
        }
        free(buffer);

        finalize( bytesWritten );
    }
}

void OWavRecorder::finalize( int bytesWritten )
{
    m_device->closeDevice();

    if( m_wavfile->isOpen()) {
        m_wavfile->adjustHeaders( bytesWritten );
        m_wavfile->closeFile();

        QString actualFile = m_wavfile->getFileName();
        if( actualFile != m_filename ) {
            // move tmp file to regular file
            QString cmd = "mv " + actualFile + " " + m_filename;
            odebug << "moving tmp file to " + m_filename << oendl;
            system( cmd.latin1());
        }

        odebug << "finished recording" << oendl;
    }
}

QString OWavRecorder::getWriteFileName()
{
    return m_wavfile->getFileName();
}

snd_pcm_format_t OWavRecorder::getDeviceFormat()
{
    return m_device->getFormat();
}

}
}