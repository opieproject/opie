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

#include "osounddevice.h"

#include <opie2/odebug.h>

/* STD */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <alsa/asoundlib.h>

OSoundDevice::OSoundDevice( QString deviceName )
{
    devForm = SND_PCM_FORMAT_S16_LE;
    devCh = -1;
    devRate = -1;
    m_deviceName = deviceName;
    m_handle = NULL;
    snd_pcm_hw_params_malloc(&m_hwparams);
}

OSoundDevice::~OSoundDevice() {
    snd_pcm_hw_params_free(m_hwparams);
}

bool OSoundDevice::openDevice( bool record ) {
    odebug << "Opening sound device: " << m_deviceName << oendl;

    snd_pcm_stream_t stream;
    if(record)
        stream = SND_PCM_STREAM_CAPTURE;
    else
        stream = SND_PCM_STREAM_PLAYBACK;

    if (snd_pcm_open(&m_handle, m_deviceName, stream, 0) < 0) {
        owarn << "Failed to open PCM device " << m_deviceName << oendl;
        return false;
    }

    if (snd_pcm_hw_params_any(m_handle, m_hwparams) < 0) {
        owarn << "Failed to configure PCM device" << oendl;
        return false;
    }

    if (snd_pcm_hw_params_set_access(m_handle, m_hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        owarn << "Error setting access" << oendl;
        return false;
    }

    return true;
}

bool OSoundDevice::closeDevice(bool drop) {
    if(m_handle) {
        odebug << "Closing sound device" << oendl;
        if(drop)
            snd_pcm_drop(m_handle);
        else
            snd_pcm_drain(m_handle);
        snd_pcm_close(m_handle);
        m_handle = NULL;
    }
    return true;
}

bool OSoundDevice::selectMicInput() {
    // FIXME implement
    return true;
}

bool OSoundDevice::setDeviceFormat( snd_pcm_format_t form) {
    odebug << "Set device format = " << form << oendl;
    if (snd_pcm_hw_params_set_format(m_handle, m_hwparams, form) < 0) {
        owarn << "Error setting format to " << form << oendl;
        return false;
    }
    devForm = form;
    return true;
}

bool OSoundDevice::setDeviceChannels( unsigned int ch) {
    odebug << "Set channels = " << ch << oendl;
    int rc = snd_pcm_hw_params_set_channels(m_handle, m_hwparams, ch);
    if (rc < 0) {
        owarn << "Error setting channels to " << ch << ": " << QString(snd_strerror(rc)) << oendl;
        return false;
    }
    devCh = ch;
    return true;
}

bool OSoundDevice::setDeviceRate( unsigned int rate) {
    // Set sample rate. If the requested rate is not supported by the hardware,
    // use the nearest possible rate.
    unsigned int actual_rate = rate;
    int rc = snd_pcm_hw_params_set_rate_near(m_handle, m_hwparams, &actual_rate, 0);
    if (rc < 0) {
        owarn << "Error setting rate to " << rate << ": " << snd_strerror(rc) << oendl;
        return false;
    }
    if ( rate != actual_rate ) 
        owarn << "The rate " << rate << " Hz is not supported by your hardware, using " << actual_rate << " Hz instead" << oendl;
    devRate = rate;
    return true;
}

int OSoundDevice::init() {
    m_frames = 32;
    int dir = 0;
    if (snd_pcm_hw_params_set_period_size_near(m_handle,
                              m_hwparams, &m_frames, &dir) < 0) {
        owarn << "Error setting HW params" << oendl;
        return -1;
    }

    // Write the parameters to the driver
    int rc = snd_pcm_hw_params(m_handle, m_hwparams);
    if (rc < 0) {
        owarn <<  "unable to set hw parameters: " << snd_strerror(rc) << oendl;
        return -1;
    }

    // Use a buffer large enough to hold one period
    snd_pcm_hw_params_get_period_size(m_hwparams, &m_frames, &dir);

    int bpc = 2; // assume 16-bit
    if(devForm == SND_PCM_FORMAT_S8 || devForm == SND_PCM_FORMAT_U8)
        bpc = 1;

    int size = m_frames * bpc * devCh;

    return size;
}

snd_pcm_format_t OSoundDevice::getFormat() {
    return devForm;
}

unsigned int OSoundDevice::getRate() {
    return devRate;
}

unsigned int OSoundDevice::getChannels() {
    return devCh;
}

bool OSoundDevice::reset() {
    if (snd_pcm_drop(m_handle) < 0) {
       return false;
    }
    return true;
}

int OSoundDevice::devWrite(char *buffer) {
    int rc = snd_pcm_writei(m_handle, buffer, m_frames);
    if (rc == -EPIPE) {
        owarn << "underrun occurred" << oendl;
        snd_pcm_prepare(m_handle);
    } 
    else if (rc < 0) {
        owarn << "error from writei: " << snd_strerror(rc) << oendl;
    }  
    else if (rc != (int)m_frames) {
        owarn << "short write, wrote " << rc << " frames" << oendl;
    }
    return rc;
}

int OSoundDevice::devRead(char *buffer) {
    int rc = snd_pcm_readi(m_handle, buffer, m_frames);
    if (rc == -EPIPE) {
        owarn << "overrun occurred" << oendl;
        snd_pcm_prepare(m_handle);
    } 
    else if (rc < 0) {
        owarn << "error from readi: " << snd_strerror(rc) << oendl;
    }  
    else if (rc != (int)m_frames) {
        owarn << "short read, read " << rc << " frames" << oendl;
    }
    return rc;
}
