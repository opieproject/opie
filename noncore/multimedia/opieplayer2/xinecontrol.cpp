
#include "xinecontrol.h"
#include "mediaplayerstate.h"

extern MediaPlayerState *mediaPlayerState;

XineControl::XineControl( QObject *parent, const char *name )
    : QObject( parent, name ) {
    libXine = new XINE::Lib();

    connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( pause(bool) ) );
    connect( this, SIGNAL( positionChanged( int position ) ),  mediaPlayerState, SLOT( updatePosition( long p ) ) );

}

XineControl::~XineControl() {
    delete libXine;
}

void XineControl::play( const QString& fileName ) {
    libXine->play( fileName );
    mediaPlayerState->setPlaying( true );
    // hier dann schaun welcher view
}

void XineControl::stop() {
    libXine->stop();
    mediaPlayerState->setPlaying( false );
}

void XineControl::pause( bool ) {
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
