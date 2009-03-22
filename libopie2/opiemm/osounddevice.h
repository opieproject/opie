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

#ifndef OSOUNDDEVICE_H
#define OSOUNDDEVICE_H

#include <qstring.h>
#include <alsa/asoundlib.h>

class OSoundDevice {
public:
    OSoundDevice( QString deviceName );
    ~OSoundDevice();
    bool openDevice(bool record = false);
    bool closeDevice(bool drop = false);
    unsigned int getChannels();
    snd_pcm_format_t getFormat();
    unsigned int getRate();
    bool setDeviceChannels(unsigned int);
    bool setDeviceRate(unsigned int);
    bool setDeviceFormat(snd_pcm_format_t);
    bool reset();
    int init();

    int devRead(char *buffer);
    int devWrite(char *buffer);

private:
    snd_pcm_format_t devForm; 
    unsigned int devCh, devRate;
    QString dspstr, mixstr;
    bool selectMicInput();

protected:
    snd_pcm_t *m_handle;
    snd_pcm_hw_params_t *m_hwparams;
    QString m_deviceName;
    snd_pcm_uframes_t m_frames;
};

#endif
