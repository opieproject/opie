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

#ifndef MEDIA_PLAYER_STATE_H
#define MEDIA_PLAYER_STATE_H


#include <qobject.h>


class MediaPlayerDecoder;
class Config;


class MediaPlayerState : public QObject {
Q_OBJECT
public:
    MediaPlayerState( QObject *parent, const char *name );
    ~MediaPlayerState();

    bool streaming();
    bool fullscreen();
    bool scaled();
    bool looping();
    bool shuffled();
    bool playlist();
    bool paused();
    bool playing();
    bool stop();
    long position();
    long length();
    char view();

public slots:
    void setIsStreaming( bool b );
    void setFullscreen( bool b );
    void setScaled( bool b );
    void setLooping( bool b );
    void setShuffled( bool b );
    void setPlaylist( bool b );
    void setPaused( bool b );
    void setPlaying( bool b );
    void setStop( bool b );
    void setPosition( long p );
    void updatePosition( long p );
    void setLength( long l );
    void setView( char v );

    void setPrev();
    void setNext();
    void setList();
    void setVideo();
    void setAudio();

    void toggleFullscreen();
    void toggleScaled();
    void toggleLooping();
    void toggleShuffled();
    void togglePlaylist();
    void togglePaused();
    void togglePlaying();

signals:
    void fullscreenToggled( bool );
    void scaledToggled( bool );
    void loopingToggled( bool );
    void shuffledToggled( bool );
    void playlistToggled( bool );
    void pausedToggled( bool );
    void playingToggled( bool );
    void stopToggled( bool );
    void positionChanged( long ); // When the slider is moved
    void positionUpdated( long ); // When the media file progresses
    void lengthChanged( long );
    void viewChanged( char );

    void prev();
    void next();

private:
    bool isStreaming;
    bool isFullscreen;
    bool isScaled;
    bool isLooping;
    bool isShuffled;
    bool usePlaylist;
    bool isPaused;
    bool isPlaying;
    bool isStoped;
    long curPosition;
    long curLength;
    char curView;

    void readConfig( Config& cfg );
    void writeConfig( Config& cfg ) const;


};


#endif // MEDIA_PLAYER_STATE_H

