
#include "xinecontrol.h"
#include "mediaplayerstate.h"

extern MediaPlayerState *mediaPlayerState;

XineControl::XineControl( QObject *parent, const char *name )
    : QObject( parent, name ) {
    libXine = new XINE::Lib();

    connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( pause(bool) ) );
    connect( this, SIGNAL( positionChanged( int position ) ),  mediaPlayerState, SLOT( updatePosition( long p ) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ), this, SLOT( stop( bool ) ) );

}

XineControl::~XineControl() {
    delete libXine;
}

void XineControl::play( const QString& fileName ) {
    libXine->play( fileName );
    mediaPlayerState->setPlaying( true );
    // default to audio view until we know how to handle video
    mediaPlayerState->setView('a');
    // determines of slider is shown
    // mediaPlayerState->setIsStreaming( false );
    // hier dann schaun welcher view
}

void XineControl::stop( bool isSet ) {
    if ( !isSet) {
        libXine->stop();
        mediaPlayerState->setNext();
        //mediaPlayerState->setPlaying( false );
    } else {
        // play again
    }
}

void XineControl::pause( bool isSet) {

    libXine->pause();
}

int XineControl::currentTime() {
    // todo: jede sekunde überprüfen
    m_currentTime =  libXine->currentTime();
    return m_currentTime;
}

void  XineControl::length() {
    m_length = libXine->length();
    mediaPlayerState->setLength( m_length );
}

int XineControl::position() {
    m_position = (m_currentTime/m_length*100);
    mediaPlayerState->setPosition( m_position  );
    return m_position;
    emit positionChanged( m_position );
}
