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
    streaming = false;
    seekable = true;
}


MediaPlayerState::~MediaPlayerState() {
}


void MediaPlayerState::readConfig( Config& cfg ) {
    cfg.setGroup("Options");
    fullscreen = cfg.readBoolEntry( "FullScreen" );
    scaled = cfg.readBoolEntry( "Scaling" );
    looping = cfg.readBoolEntry( "Looping" );
    shuffled = cfg.readBoolEntry( "Shuffle" );
    videoGamma = cfg.readNumEntry( "VideoGamma" );
    playing = FALSE;
    streaming = FALSE;
    paused = FALSE;
    curPosition = 0;
    curLength = 0;
    curView = 'l';
}


void MediaPlayerState::writeConfig( Config& cfg ) const {
    cfg.setGroup( "Options" );
    cfg.writeEntry( "FullScreen", fullscreen );
    cfg.writeEntry( "Scaling", scaled );
    cfg.writeEntry( "Looping", looping );
    cfg.writeEntry( "Shuffle", shuffled );
    cfg.writeEntry( "VideoGamma",  videoGamma );
}

// slots
void MediaPlayerState::setIsStreaming( bool b ) {
    streaming = b;
}

void MediaPlayerState::setIsSeekable( bool b ) {
    seekable = b;
    emit isSeekableToggled(b);
}


void MediaPlayerState::setFullscreen( bool b ) {
    if ( fullscreen == b ) {
        return;
    }
    fullscreen = b;
    emit fullscreenToggled(b);
}


void MediaPlayerState::setBlanked( bool b ) {
    if ( blanked == b ) {
        return;
    }
    blanked = b;
    emit blankToggled(b);
}


void MediaPlayerState::setScaled( bool b ) {
    if ( scaled == b ) {
        return;
    }
    scaled = b;
    emit scaledToggled(b);
}

void MediaPlayerState::setLooping( bool b ) {
    if ( looping    == b ) {
        return;
    }
    looping = b;
    emit loopingToggled(b);
}

void MediaPlayerState::setShuffled( bool b ) {
    if ( shuffled   == b ) {
        return;
    }
    shuffled = b;
    emit shuffledToggled(b);
}

void MediaPlayerState::setPaused( bool b ) {
      if ( paused  == b ) {
          paused = FALSE;
          emit pausedToggled(FALSE);
          return;
      }
     paused = b;
     emit pausedToggled(b);
}

void MediaPlayerState::setPlaying( bool b ) {
    if ( playing  == b ) {
        return;
    }
    playing = b;
    stopped = !b;
    emit playingToggled(b);
}

void MediaPlayerState::setStopped( bool b ) {
    if ( stopped  == b ) {
        return;
    }
    stopped = b;
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
    setFullscreen( !fullscreen );
}

void MediaPlayerState::toggleScaled() {
    setScaled( !scaled);
}

void MediaPlayerState::toggleLooping() {
    setLooping( !looping);
}

void MediaPlayerState::toggleShuffled() {
    setShuffled( !shuffled);
}

void MediaPlayerState::togglePaused() {
    setPaused( !paused);
}

void MediaPlayerState::togglePlaying() {
    setPlaying( !playing);
}

void MediaPlayerState::toggleBlank() {
    setBlanked( !blanked);
}



