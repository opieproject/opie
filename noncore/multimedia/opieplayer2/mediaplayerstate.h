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

// this file is based on work by trolltech

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

    bool isStreaming() const { return streaming; }
    bool isSeekable() const { return seekable; }
    bool isFullscreen() const { return fullscreen; }
    bool isScaled() const { return scaled; }
    bool isLooping() const { return looping; }
    bool isShuffled() const { return shuffled; }
    bool isUsingPlaylist() const { return usePlaylist; }
    bool isPaused() const { return paused; }
    bool isPlaying() const { return playing; }
    bool isStopped() const { return stopped; }
    long position() const { return curPosition; }
    long length() const { return curLength; }
    char view() const { return curView; }

public slots:
    void setIsStreaming( bool b );
    void setIsSeekable( bool b );
    void setFullscreen( bool b );
    void setScaled( bool b );
    void setLooping( bool b );
    void setShuffled( bool b );
    void setUsingPlaylist( bool b );
    void setPaused( bool b );
    void setPlaying( bool b );
    void setStopped( bool b );
    void setPosition( long p );
    void updatePosition( long p );
    void setLength( long l );
    void setView( char v );
    void setBlanked( bool b );
    void setVideoGamma( int v );

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
    void toggleBlank();
    void writeConfig( Config& cfg ) const;


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
    void isSeekableToggled( bool );
    void blankToggled( bool );
    void videoGammaChanged( int );
    void prev();
    void next();

private:
    bool streaming;
    bool seekable;
    bool fullscreen;
    bool scaled;
    bool blanked;
    bool looping;
    bool shuffled;
    bool usePlaylist;
    bool paused;
    bool playing;
    bool stopped;
    long curPosition;
    long curLength;
    char curView;
    int videoGamma;
    void readConfig( Config& cfg );

};


#endif // MEDIA_PLAYER_STATE_H

