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

#include "owavhandler.h"

namespace Opie {
namespace MM {

OWavHandler::OWavHandler()
    : m_device(0)
{
}

OWavHandler::~OWavHandler()
{
    finalize();
}
    
bool OWavHandler::setupSoundDevice( bool record ) {
    snd_pcm_format_t sampleformat;

    if( m_fileparams.resolution == 16 )
        sampleformat = SND_PCM_FORMAT_S16;
    else
        sampleformat = SND_PCM_FORMAT_U8;

    owarn << "<<<<<<<<<<<<<<<<<<<open dsp " << m_fileparams.sampleRate << " " << m_fileparams.channels << " " << sampleformat << "" << oendl;

    m_device = new OSoundDevice( "default" );
    m_device->openDevice(record);

    m_device->setDeviceFormat( sampleformat );
    m_device->setDeviceChannels( m_fileparams.channels );
    m_device->setDeviceRate( m_fileparams.sampleRate );
    m_bufsize = m_device->init();

    return (m_bufsize > 0);
}

void OWavHandler::finalize()
{
    if( m_device ) {
        m_device->closeDevice();
        delete m_device;
        m_device = 0;
    }
}

}
}