/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef XINECONTROL_H
#define XINECONTROL_H

#include "lib.h"
#include <qobject.h>

#include "mediaplayerstate.h"

class XineControl : public QObject  {
    Q_OBJECT
public:
    XineControl( QWidget *videoContainerWidget, XineVideoWidget *xineWidget, 
                 MediaPlayerState &_mediaPlayerState, 
                 QObject *parent = 0, const char *name =0 );
    ~XineControl();

    bool hasVideo() const { return hasVideoChannel; }
    bool hasAudio() const { return hasAudioChannel; }

public slots:
    void play( const QString& fileName );
    void stop( bool );

    /**
     * Pause the media stream
     * @param if pause or not
     */
    void pause( bool );

    /**
     * Set videos fullscreen
     * @param yes or no
     */
    void setFullscreen( bool );

    /**
     *
     */
    long currentTime();
    void seekTo( long );
    // get length of media file and set it
    void length();
    long position();

    /**
     * Proceed to the next media file in playlist
     */
    void nextMedia();

    /**
     * Get as much info about the stream from xine as possible
     */
    QString getMetaInfo();

    /**
     * get the error code and "translate" it for the user
     *
     */
    QString getErrorCode();


    void videoResized ( const QSize &s );

    /**
     * Set the gamma value of the video output
     *  @param int value between -100 and 100, 0 is original
     */
    void setGamma( int );


private:
    XINE::Lib *libXine;
    long m_currentTime;
    long m_position;
    int m_length;
    QString m_fileName;
    bool disabledSuspendScreenSaver : 1;
    bool hasVideoChannel : 1;
    bool hasAudioChannel : 1;
    MediaPlayerState &mediaPlayerState;

signals:
    void positionChanged( long );
};


#endif
