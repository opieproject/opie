#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qwidgetstack.h>
#include <qfile.h>

#include "mediaplayer.h"
#include "playlistwidget.h"
#include "audiowidget.h"

#include "mediaplayerstate.h"



extern AudioWidget *audioUI;
extern PlayListWidget *playList;
extern MediaPlayerState *mediaPlayerState;


MediaPlayer::MediaPlayer( QObject *parent, const char *name )
    : QObject( parent, name ), volumeDirection( 0 ), currentFile( NULL ) {


    xineControl = new XineControl();
//    QPEApplication::grabKeyboard(); // EVIL
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ), this, SLOT( setPlaying( bool ) ) );
    connect( mediaPlayerState, SIGNAL( pausedToggled( bool ) ),  this, SLOT( pauseCheck( bool ) ) );
    connect( mediaPlayerState, SIGNAL( next() ), this, SLOT( next() ) );
    connect( mediaPlayerState, SIGNAL( prev() ), this, SLOT( prev() ) );

    connect( audioUI,  SIGNAL( moreClicked() ), this, SLOT( startIncreasingVolume() ) );
    connect( audioUI,  SIGNAL( lessClicked() ),  this, SLOT( startDecreasingVolume() ) );
    connect( audioUI,  SIGNAL( moreReleased() ), this, SLOT( stopChangingVolume() ) );
    connect( audioUI,  SIGNAL( lessReleased() ), this, SLOT( stopChangingVolume() ) );

}

MediaPlayer::~MediaPlayer() {
}

void MediaPlayer::pauseCheck( bool b ) {
    // Only pause if playing
    if ( b && !mediaPlayerState->playing() ) {
        mediaPlayerState->setPaused( FALSE );
    }
}

void MediaPlayer::play() {
    mediaPlayerState->setPlaying( FALSE );
    mediaPlayerState->setPlaying( TRUE );
}

void MediaPlayer::setPlaying( bool play ) {
    if ( !play ) {
        mediaPlayerState->setPaused( FALSE );
        return;
    }

    if ( mediaPlayerState->paused() ) {
        mediaPlayerState->setPaused( FALSE );
        return;
    }

    const DocLnk *playListCurrent = playList->current();
    if ( playListCurrent != NULL ) {
        currentFile = playListCurrent;
    }

    audioUI->setTickerText( currentFile->file( ) );

    xineControl->play( currentFile->file() );
}


void MediaPlayer::prev() {
    if ( playList->prev() ) {
        play();
    } else if ( mediaPlayerState->looping() ) {
        if ( playList->last() ) {
            play();
        }
    } else {
        mediaPlayerState->setList();
    }
}


void MediaPlayer::next() {
    if ( playList->next() ) {
        play();
    } else if ( mediaPlayerState->looping() ) {
        if ( playList->first() ) {
            play();
        }
    } else {
        mediaPlayerState->setList();
    }
}


void MediaPlayer::startDecreasingVolume() {
    volumeDirection = -1;
    startTimer( 100 );
    // da kommt demnächst osound denk ich mal
    //  AudioDevice::decreaseVolume();
}


void MediaPlayer::startIncreasingVolume() {
    volumeDirection = +1;
    startTimer( 100 );
    //  AudioDevice::increaseVolume();
}


void MediaPlayer::stopChangingVolume() {
    killTimers();
}


void MediaPlayer::timerEvent( QTimerEvent * ) {
    //    if ( volumeDirection == +1 )
    //  AudioDevice::increaseVolume();
    //    else if ( volumeDirection == -1 )
        //       AudioDevice::decreaseVolume();
}

void MediaPlayer::keyReleaseEvent( QKeyEvent *e) {
    switch ( e->key() ) {
////////////////////////////// Zaurus keys
      case Key_Home:
          break;
      case Key_F9: //activity
          break;
      case Key_F10: //contacts
          break;
      case Key_F11: //menu
          break;
      case Key_F12: //home
          qDebug("Blank here");
          break;
      case Key_F13: //mail
          break;
    }
}

void MediaPlayer::doBlank() {

}

void MediaPlayer::doUnblank() {

}

void MediaPlayer::cleanUp() {
//     QPEApplication::grabKeyboard();
//     QPEApplication::ungrabKeyboard();

}
