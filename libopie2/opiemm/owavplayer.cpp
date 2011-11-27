/*
                             This file is part of the Opie Project
              =.             (C) 2009 Team Opie <opie-users@lists.sourceforge.net>
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

#include "owavplayer.h"

extern "C" {
#include "ima_rw.h"
#include "unistd.h"
}

namespace Opie {
namespace MM {

bool OWavPlayer::setup( const QString &filename )
{
    m_filename = filename;

    if( setupFile() )
        return setupSoundDevice( false ); // playback
    else
        return false;
}

void OWavPlayer::play( OWavPlayerCallback *callback )
{
    if( m_bufsize > 0 && m_wavfile ) {
        char *buffer = (char *) malloc(m_bufsize);

        int samplesPerBlock = m_bufsize/2;
        int adpcm_insize = lsx_ima_bytes_per_block(m_fileparams.channels, samplesPerBlock);
        unsigned char adpcm_inbuf[ adpcm_insize ];

        if( m_fileparams.format == WAVE_FORMAT_DVI_ADPCM) {
            memset( adpcm_inbuf, 0, adpcm_insize);
            lsx_ima_init_table();
        }

        bool stopflag = false;
        bool paused = false;
        int fd = m_wavfile->getfd();
        int position = lseek( fd, 0, SEEK_CUR ); // so we can resume from the middle
        int numberSamples = m_wavfile->getNumberSamples();
        while( position < numberSamples ) {
            if ( stopflag )
                break;
            int number;
            if ( !paused ) {
                if( m_fileparams.format == WAVE_FORMAT_DVI_ADPCM) {
                    number = ::read( fd, adpcm_inbuf, adpcm_insize);
                    lsx_ima_block_expand_i(m_fileparams.channels, adpcm_inbuf, (short *)buffer, samplesPerBlock);
                }
                else
                    number = ::read( fd, buffer, m_bufsize);

                long delay = m_device->getDelay() * 100000 / m_fileparams.sampleRate;
                m_device->devWrite(buffer);
                position += number;
                usleep(delay);
            }
            else
                number = 0;
            callback->playerCallback( buffer, number, position, stopflag, paused );
        }
        free(buffer);

        // Tidy up
        finalize();
        m_wavfile->closeFile();
        delete m_wavfile;
        m_wavfile = NULL;
    }
}

bool OWavPlayer::setupFile()
{
    m_wavfile = new OWavFile( m_filename );
    int fd = m_wavfile->openFile();
    if( fd == -1 )
        return false;

    m_fileparams = m_wavfile->fileParams();
    
    return true;
}

OWavFile *OWavPlayer::wavFile()
{
    return m_wavfile;
}

}
}