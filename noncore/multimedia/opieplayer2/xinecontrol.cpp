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

#include "xinecontrol.h"
#include "xinevideowidget.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;

/* QT */
#include <qtimer.h>
#include <qmessagebox.h>

XineControl::XineControl( XINE::Lib *xine, XineVideoWidget *xineWidget,
                          MediaPlayerState &_mediaPlayerState,
                          QObject *parent, const char *name )
    : QObject( parent, name ), libXine( xine ), mediaPlayerState( _mediaPlayerState ), xineVideoWidget( xineWidget )
{
    m_wasError = false;

    xine->ensureInitialized();

    xine->setWidget( xineWidget );

    init();
}

void XineControl::init()
{
    connect( &mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( pause(bool) ) );
    connect( this, SIGNAL( positionChanged(long) ), &mediaPlayerState, SLOT( updatePosition(long) ) );
    connect( &mediaPlayerState, SIGNAL( playingToggled(bool) ), this, SLOT( stop(bool) ) );
    connect( &mediaPlayerState, SIGNAL( fullscreenToggled(bool) ), this, SLOT( setFullscreen(bool) ) );
    connect( &mediaPlayerState, SIGNAL( positionChanged(long) ),  this,  SLOT( seekTo(long) ) );
    connect( &mediaPlayerState,  SIGNAL( videoGammaChanged(int) ), this,  SLOT( setGamma(int) ) );
    connect( libXine, SIGNAL( stopped() ), this, SLOT( nextMedia() ) );
    connect( xineVideoWidget, SIGNAL( videoResized(const QSize&) ), this, SLOT( videoResized(const QSize&) ) );

    disabledSuspendScreenSaver = FALSE;
}

XineControl::~XineControl() {
#if !defined(QT_NO_COP)
    if ( disabledSuspendScreenSaver ) {
        disabledSuspendScreenSaver = FALSE;
        // Re-enable the suspend mode
        QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
    }
#endif
    delete libXine;
}

void XineControl::play( const QString& fileName ) {

    hasVideoChannel = FALSE;
    hasAudioChannel = FALSE;
    m_fileName = fileName;
    m_wasError = false;


    /*
     * If Playing Fails we will fire up an MessgaeBox
     * but present the AudioWidget so the User can
     * either Quit and change the Playlist or Continue
     */
    if ( !libXine->play( fileName, 0, 0 ) ) {
        QMessageBox::warning( 0l , tr( "Failure" ), getErrorCode() );
        m_wasError = true;
    }
    mediaPlayerState.setPlaying( true );

    MediaPlayerState::DisplayType displayType;
    if ( !libXine->hasVideo() ) {
        displayType = MediaPlayerState::Audio;
        libXine->setShowVideo( false );
        hasAudioChannel = TRUE;
    } else {
        displayType = MediaPlayerState::Video;
        libXine->setShowVideo( true );
        hasVideoChannel = TRUE;
    }
    // determine if slider is shown
    mediaPlayerState.setIsSeekable( libXine->isSeekable() );

    // which gui (video / audio)
    mediaPlayerState.setDisplayType( displayType );

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    if ( !disabledSuspendScreenSaver ) {
        disabledSuspendScreenSaver = TRUE;
        // Stop the screen from blanking and power saving state
        QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" )
            << ( displayType == MediaPlayerState::Video ? QPEApplication::Disable : QPEApplication::DisableSuspend );
    }
#endif

    length();
    position();
}

void XineControl::nextMedia() {
    mediaPlayerState.setNext();
}

void XineControl::setGamma( int value ) {
    libXine->setGamma( value );
}

void XineControl::stop( bool isSet ) {
    if ( !isSet ) {
        libXine->stop();

#if !defined(QT_NO_COP)
        if ( disabledSuspendScreenSaver ) {
            disabledSuspendScreenSaver = FALSE;
            // Re-enable the suspend mode
            QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
        }
#endif
    }
}

/**
 * Pause playback
 * @isSet
 */
void XineControl::pause( bool isSet) {
    libXine->pause( isSet );
}


/**
 * get current time in playback
 */
long XineControl::currentTime() {
    // todo: jede sekunde überprüfen
    m_currentTime = libXine->currentTime();
    return m_currentTime;
}

/**
 * Set the length of the media file
 */
void  XineControl::length() {
    m_length = libXine->length();
    mediaPlayerState.setLength( m_length );
}


/**
 * Reports the position the xine backend is at right now
 * @return long the postion in seconds
 */
long XineControl::position() {
    m_position = ( currentTime() );
    mediaPlayerState.updatePosition( m_position );
    long emitPos = (long)m_position;
    emit positionChanged( emitPos );
    if( mediaPlayerState.isPlaying() ) {
    // needs to be stopped the media is stopped
        QTimer::singleShot( 1000, this, SLOT( position() ) );
    }
    return m_position;
}

/**
 * Set videoplayback to fullscreen
 * @param isSet
 */
void XineControl::setFullscreen( bool isSet ) {
    libXine->showVideoFullScreen( isSet );
}


QString XineControl::getMetaInfo() {

    QString returnString;

    /*
     * If there was an error let us
     * change the Meta Info to contain the Error Message
     */
    if ( m_wasError ) {
        returnString = tr("Error on file '%1' with reason: ",
                          "Error when playing a file" ).arg( m_fileName );
        returnString += getErrorCode();
        returnString.replace( QRegExp("<qt>",  false), "" );
        returnString.replace( QRegExp("</qt>", false), "" );
        return returnString;
    }

    if ( !libXine->metaInfo( 0 ).isEmpty() ) {
        returnString += tr( " Title: " + libXine->metaInfo( 0 ) );
    }

    if ( !libXine->metaInfo( 1 ).isEmpty() ) {
        returnString += tr( " Comment: " + libXine->metaInfo( 1 ) );
    }

    if ( !libXine->metaInfo( 2 ).isEmpty() ) {
        returnString += tr( " Artist: " + libXine->metaInfo( 2 ) );
    }

    if ( !libXine->metaInfo( 3 ).isEmpty() ) {
        returnString += tr( " Genre: " + libXine->metaInfo( 3 ) );
    }

    if ( !libXine->metaInfo( 4 ).isEmpty() ) {
        returnString += tr( " Album: " + libXine->metaInfo( 4 ) );
    }

    if ( !libXine->metaInfo( 5 ).isEmpty() ) {
        returnString += tr( " Year: " + libXine->metaInfo( 5 ) );
    }
    return returnString;
}

QString XineControl::getErrorCode() {

    int errorCode = libXine->error();

    odebug << QString("ERRORCODE: %1 ").arg(errorCode) << oendl;

    if ( errorCode == 1 ) {
        return tr( "<qt>No input plugin found for this media type</qt>" );
    } else if ( errorCode == 2 ) {
        return tr( "<qt>No demux plugin found for this media type</qt>" );
    } else if ( errorCode == 3 ) {
        return tr( "<qt>Demuxing failed for this media type</qt>" );
    } else if ( errorCode == 4 ) {
        return tr( "<qt>Malformed MRL</qt>" );
    } else if ( errorCode == 5 ) {
        return tr( "<qt>Input failed</qt>" );
    } else {
        return tr( "<qt>Some other error</qt>" );
    }
}

/**
 * Seek to a position in the track
 * @param second the second to jump to
 */
void XineControl::seekTo( long second ) {
    libXine->seekTo( (int)second );
}

void XineControl::videoResized ( const QSize &s ) {
    libXine->resize( s );
}
