/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
// L.J.Potter added changes Fri 02-15-2002

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H


#include <qobject.h>
#include <sys/soundcard.h>


class AudioDevicePrivate;


class AudioDevice : public QObject {
    Q_OBJECT
public:
    AudioDevice( unsigned int freq = 44000, unsigned int channels = 2, unsigned int bytesPerSample = AFMT_S16_LE );
    ~AudioDevice();

    unsigned int canWrite() const;
    void write( char *buffer, unsigned int length );
    int bytesWritten();

    unsigned int channels() const;
    unsigned int frequency() const;
    unsigned int bytesPerSample() const;
    unsigned int bufferSize() const;

    // Each volume level is from 0 to 0xFFFF
    static void getVolume( unsigned int& left, unsigned int& right, bool& muted );
    static void setVolume( unsigned int  left, unsigned int  right, bool  muted );

    static unsigned int leftVolume()  { bool muted; unsigned int l, r; getVolume( l, r, muted ); return l;     }
    static unsigned int rightVolume() { bool muted; unsigned int l, r; getVolume( l, r, muted ); return r;     }
    static bool         isMuted()     { bool muted; unsigned int l, r; getVolume( l, r, muted ); return muted; }

    static void increaseVolume() { setVolume( leftVolume() + 1968, rightVolume() + 1968, isMuted() ); }
    static void decreaseVolume() { setVolume( leftVolume() - 1966, rightVolume() - 1966, isMuted() ); }

public slots:
    // Convinence functions derived from above functions
    void setVolume( unsigned int level ) { setVolume( level, level, isMuted() ); }
    void mute() { setVolume( leftVolume(), rightVolume(), TRUE ); }
    void volumeChanged( bool muted );

signals:
    void completedIO();

private:
    AudioDevicePrivate *d;
};


#endif // AUDIODEVICE_H

