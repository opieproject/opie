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

#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/config.h>
#include <qvaluelist.h>
#include <qobject.h>
#include <qdir.h>
#include "mediaplayerstate.h"



//#define MediaPlayerDebug(x) qDebug x
#define MediaPlayerDebug(x)


MediaPlayerState::MediaPlayerState( QObject *parent, const char *name )
        : QObject( parent, name ) {
    Config cfg( "OpiePlayer" );
    readConfig( cfg );
    isStreaming = false;
    isSeekable = true;
}


MediaPlayerState::~MediaPlayerState() {
}


void MediaPlayerState::readConfig( Config& cfg ) {
    cfg.setGroup("Options");
    isFullscreen = cfg.readBoolEntry( "FullScreen" );
    isScaled = cfg.readBoolEntry( "Scaling" );
    isLooping = cfg.readBoolEntry( "Looping" );
    isShuffled = cfg.readBoolEntry( "Shuffle" );
    usePlaylist = cfg.readBoolEntry( "UsePlayList" );
    videoGamma = cfg.readNumEntry( "VideoGamma" );
    usePlaylist = TRUE;
    isPlaying = FALSE;
    isStreaming = FALSE;
    isPaused = FALSE;
    curPosition = 0;
    curLength = 0;
    curView = 'l';
}


void MediaPlayerState::writeConfig( Config& cfg ) const {
    cfg.setGroup( "Options" );
    cfg.writeEntry( "FullScreen", isFullscreen );
    cfg.writeEntry( "Scaling", isScaled );
    cfg.writeEntry( "Looping", isLooping );
    cfg.writeEntry( "Shuffle", isShuffled );
    cfg.writeEntry( "UsePlayList", usePlaylist );
    cfg.writeEntry( "VideoGamma",  videoGamma );
}


// public stuff


bool MediaPlayerState::streaming() const {
    return isStreaming;
}

bool MediaPlayerState::seekable() const {
    return isSeekable;
}

bool MediaPlayerState::fullscreen() const {
    return isFullscreen;
}

bool MediaPlayerState::scaled() const {
    return isScaled;
}

bool MediaPlayerState::looping() const {
    return isLooping;
}

bool MediaPlayerState::shuffled() const {
    return isShuffled;
}


bool MediaPlayerState::playlist() const {
    return usePlaylist;
}

bool MediaPlayerState::paused() const {
    return isPaused;
}

bool MediaPlayerState::playing() const {
    return isPlaying;
}

bool MediaPlayerState::stop() const {
    return isStoped;
}

long MediaPlayerState::position() const {
    return curPosition;
}

long MediaPlayerState::length() const {
    return curLength;
}

char MediaPlayerState::view() const {
    return curView;
}

// slots
void MediaPlayerState::setIsStreaming( bool b ) {

    if ( isStreaming == b ) {
        return;
    }
    isStreaming = b;
}

void MediaPlayerState::setIsSeekable( bool b ) {

    //if ( isSeekable == b ) {
    //    return;
    // }
    isSeekable = b;
    emit isSeekableToggled(b);
}


void MediaPlayerState::setFullscreen( bool b ) {
    if ( isFullscreen == b ) {
        return;
    }
    isFullscreen = b;
    emit fullscreenToggled(b);
}


void MediaPlayerState::setBlanked( bool b ) {
    if ( isBlanked == b ) {
        return;
    }
    isBlanked = b;
    emit blankToggled(b);
}


void MediaPlayerState::setScaled( bool b ) {
    if ( isScaled == b ) {
        return;
    }
    isScaled = b;
    emit scaledToggled(b);
}

void MediaPlayerState::setLooping( bool b ) {
    if ( isLooping    == b ) {
        return;
    }
    isLooping = b;
    emit loopingToggled(b);
}

void MediaPlayerState::setShuffled( bool b ) {
    if ( isShuffled   == b ) {
        return;
    }
    isShuffled = b;
    emit shuffledToggled(b);
}

void MediaPlayerState::setPlaylist( bool b ) {
    if ( usePlaylist  == b ) {
        return;
    }
    usePlaylist = b;
    emit playlistToggled(b);
}

void MediaPlayerState::setPaused( bool b ) {
      if ( isPaused  == b ) {
          isPaused = FALSE;
          emit pausedToggled(FALSE);
          return;
      }
     isPaused = b;
     emit pausedToggled(b);
}

void MediaPlayerState::setPlaying( bool b ) {
    if ( isPlaying  == b ) {
        return;
    }
    isPlaying = b;
    isStoped = !b;
    emit playingToggled(b);
}

void MediaPlayerState::setStop( bool b ) {
    if ( isStoped  == b ) {
        return;
    }
    isStoped = b;
    emit stopToggled(b);
}

void MediaPlayerState::setPosition( long p ) {
    if ( curPosition == p ) {
        return;
    }
    curPosition = p;
    emit positionChanged(p);
}

void MediaPlayerState::updatePosition( long p ){
    if ( curPosition  == p ) {
        return;
    }
    curPosition = p;
    emit positionUpdated(p);
}

void MediaPlayerState::setVideoGamma( int v ){
    if ( videoGamma  == v ) {
        return;
    }
    videoGamma = v;
    emit videoGammaChanged( v );
}

void MediaPlayerState::setLength( long l ) {
    if ( curLength  == l ) {
        return;
    }
    curLength = l;
    emit lengthChanged(l);
}

void MediaPlayerState::setView( char v ) {
    if ( curView  == v ) {
        return;
    }
    curView = v;
    emit viewChanged(v);
}

void MediaPlayerState::setPrev(){
    emit prev();
}

void MediaPlayerState::setNext() {
    emit next();
}

void MediaPlayerState::setList() {
    setPlaying( FALSE );
    setView('l');
}

void MediaPlayerState::setVideo() {
    setView('v');
}

void MediaPlayerState::setAudio() {
    setView('a');
}

void MediaPlayerState::toggleFullscreen() {
    setFullscreen( !isFullscreen );
}

void MediaPlayerState::toggleScaled() {
    setScaled( !isScaled);
}

void MediaPlayerState::toggleLooping() {
    setLooping( !isLooping);
}

void MediaPlayerState::toggleShuffled() {
    setShuffled( !isShuffled);
}

void MediaPlayerState::togglePlaylist() {
    setPlaylist( !usePlaylist);
}

void MediaPlayerState::togglePaused() {
    setPaused( !isPaused);
}

void MediaPlayerState::togglePlaying() {
    setPlaying( !isPlaying);
}

void MediaPlayerState::toggleBlank() {
    setBlanked( !isBlanked);
}



