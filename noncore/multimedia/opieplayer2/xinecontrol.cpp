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


#include <qtimer.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include "xinecontrol.h"
#include "mediaplayerstate.h"
#include "videowidget.h"

extern MediaPlayerState *mediaPlayerState;
extern VideoWidget *videoUI;
XineControl::XineControl( QObject *parent, const char *name )
    : QObject( parent, name ) {
    libXine = new XINE::Lib(videoUI->vidWidget() );

    connect ( videoUI, SIGNAL( videoResized ( const QSize & )), this, SLOT( videoResized ( const QSize & )));
    connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( pause(bool) ) );
    connect( this, SIGNAL( positionChanged( long ) ), mediaPlayerState, SLOT( updatePosition( long ) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ), this, SLOT( stop( bool ) ) );
    connect( mediaPlayerState, SIGNAL( fullscreenToggled( bool ) ), this, SLOT( setFullscreen( bool ) ) );
    connect( mediaPlayerState, SIGNAL( positionChanged( long ) ),  this,  SLOT( seekTo( long ) ) );
    connect( libXine, SIGNAL( stopped() ),  this, SLOT( nextMedia() ) );

    disabledSuspendScreenSaver = FALSE;
}

XineControl::~XineControl() {
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    if ( disabledSuspendScreenSaver ) {
        disabledSuspendScreenSaver = FALSE;
        // Re-enable the suspend mode
        QCopEnvelope("QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
    }
#endif
    delete libXine;
}

void XineControl::play( const QString& fileName ) {
    hasVideoChannel=FALSE;
    hasAudioChannel=FALSE;
    m_fileName = fileName;
    libXine->play( fileName );
    mediaPlayerState->setPlaying( true );
    // default to audio view until we know how to handle video
    //   MediaDetect mdetect;
    char whichGui = mdetect.videoOrAudio( fileName );
    if (whichGui == 'f') {
        qDebug("Nicht erkannter Dateityp");
        return;
    }

    if (whichGui == 'a') {
        libXine->setShowVideo( false );
        hasAudioChannel=TRUE;
    } else {
        libXine->setShowVideo( true );
        hasVideoChannel=TRUE;
    }

    // determine if slider is shown
    //    mediaPlayerState->setIsStreaming( mdetect.isStreaming( fileName ) );
    mediaPlayerState->setIsStreaming( !libXine->isSeekable() );
    // which gui (video / audio)
    mediaPlayerState->setView( whichGui );

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    if ( !disabledSuspendScreenSaver ) {
        disabledSuspendScreenSaver = TRUE;
        // Stop the screen from blanking and power saving state
        QCopEnvelope("QPE/System", "setScreenSaverMode(int)" )
            << ( whichGui == 'v' ? QPEApplication::Disable : QPEApplication::DisableSuspend );
    }
#endif

    length();
    position();
}

void XineControl::nextMedia() {
    mediaPlayerState->setNext();
}

void XineControl::stop( bool isSet ) {
    if ( !isSet) {
        libXine->stop( );
        mediaPlayerState->setList();
        //mediaPlayerState->setPlaying( false );

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
        if ( disabledSuspendScreenSaver ) {
            disabledSuspendScreenSaver = FALSE;
            // Re-enable the suspend mode
            QCopEnvelope("QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
        }
#endif

    } else {
        // play again
    }
}

/**
 * Pause playback
 * @isSet
 */
void XineControl::pause( bool isSet) {
    if (isSet) {
        libXine->pause();
    } else {
        libXine->play( m_fileName, 0, m_currentTime);
    }
}


/**
 * get current time in playback
 */
long XineControl::currentTime() {
    // todo: jede sekunde überprüfen
    m_currentTime =  libXine->currentTime();
    return m_currentTime;
    QTimer::singleShot( 1000, this, SLOT( currentTime() ) );
}

/**
 * Set the length of the media file
 */
void  XineControl::length() {
    m_length = libXine->length();
    mediaPlayerState->setLength( m_length );
}


/**
 * Reports the position the xine backend is at right now
 * @return long the postion in seconds
 */
long XineControl::position() {
    m_position = ( currentTime()  );
    mediaPlayerState->updatePosition( m_position  );
    long emitPos = (long)m_position;
    emit positionChanged( emitPos );
    if(mediaPlayerState->isPlaying)
    // needs to be stopped the media is stopped
    QTimer::singleShot( 1000, this, SLOT( position() ) );
//    qDebug("POSITION : %d", m_position);
    return m_position;
}

/**
 * Set videoplayback to fullscreen
 * @param isSet
 */
void XineControl::setFullscreen( bool isSet ) {
    libXine->showVideoFullScreen( isSet);
}

/**
 * Seek to a position in the track
 * @param second the second to jump to
 */
void XineControl::seekTo( long second ) {
    libXine->play( m_fileName , 0, (int)second );
}

void XineControl::videoResized ( const QSize &s ) {
  libXine-> resize ( s );
}
