/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
// L.J.Potter added changes Fri 02-15-2002

#include <qpe/qpeapplication.h>

#ifdef Q_WS_QWS
#include <qpe/qcopenvelope_qws.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "loopcontrol.h"
#include "videowidget.h"
#include "audiodevice.h"
#include <qpe/mediaplayerplugininterface.h>
#include "mediaplayerstate.h"


extern VideoWidget *videoUI; // now only needed to tell it to play a frame
extern MediaPlayerState *mediaPlayerState;


//#define DecodeLoopDebug(x)  qDebug x
#define DecodeLoopDebug(x)


static char     *audioBuffer = NULL;
static AudioDevice  *audioDevice = NULL;
static bool     disabledSuspendScreenSaver = FALSE;
static bool     previousSuspendMode = FALSE;


pthread_t audio_tid;
pthread_attr_t  audio_attr;
bool threadOkToGo = FALSE;


class Mutex {
public:
    Mutex() {
  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  pthread_mutex_init( &mutex, &attr );
  pthread_mutexattr_destroy( &attr );
    }

    ~Mutex() {
  pthread_mutex_destroy( &mutex );
    }

    void lock() {
  pthread_mutex_lock( &mutex );
    }

    void unlock() {
  pthread_mutex_unlock( &mutex );
    }
private:
    pthread_mutex_t mutex;
};


void *startAudioThread( void *ptr ) {
    LoopControl *mpegView = (LoopControl *)ptr;
    while ( TRUE ) {
  if ( threadOkToGo && mpegView->moreAudio )
      mpegView->startAudio();
  else
      usleep( 10000 ); // Semi-buzy-wait till we are playing again
    }
    return 0;
}


Mutex *audioMutex;


LoopControl::LoopControl( QObject *parent, const char *name )
    : QObject( parent, name ) {
    isMuted = FALSE;
    connect( qApp, SIGNAL( volumeChanged(bool) ), this, SLOT( setMute(bool) ) );
//qDebug("starting loopcontrol");
    audioMutex = new Mutex;

    pthread_attr_init(&audio_attr);
#define USE_REALTIME_AUDIO_THREAD
#ifdef USE_REALTIME_AUDIO_THREAD
    // Attempt to set it to real-time round robin
    if ( pthread_attr_setschedpolicy( &audio_attr, SCHED_RR ) == 0 ) {
  sched_param params;
  params.sched_priority = 50;
  pthread_attr_setschedparam(&audio_attr,&params);
    } else {
  qDebug( "Error setting up a realtime thread, reverting to using a normal thread." );
  pthread_attr_destroy(&audio_attr);
  pthread_attr_init(&audio_attr);
    }
#endif
//qDebug("create audio thread");
    pthread_create(&audio_tid, &audio_attr, (void * (*)(void *))startAudioThread, this);
}


LoopControl::~LoopControl() {
    stop();
}


static long prev_frame = 0;
static int currentSample = 0;


void LoopControl::timerEvent( QTimerEvent *te ) {

    if ( te->timerId() == videoId )
  startVideo();

    if ( te->timerId() == sliderId ) {
  if ( hasAudioChannel && !hasVideoChannel && moreAudio ) {
      mediaPlayerState->updatePosition( audioSampleCounter );
  } else if ( hasVideoChannel && moreVideo ) {
      mediaPlayerState->updatePosition( current_frame );
  }
    }

    if ( !moreVideo && !moreAudio ) { 
        mediaPlayerState->setPlaying( FALSE );
        mediaPlayerState->setNext();
    }
}


void LoopControl::setPosition( long pos ) {
    audioMutex->lock();

    if ( hasVideoChannel && hasAudioChannel ) {
  playtime.restart();
  playtime = playtime.addMSecs( long((double)-pos * 1000.0 / framerate) );
  current_frame = pos + 1;
  mediaPlayerState->curDecoder()->videoSetFrame( current_frame, stream );
  prev_frame = current_frame - 1;
  currentSample = (int)( (double)current_frame * freq / framerate );
  mediaPlayerState->curDecoder()->audioSetSample( currentSample, stream );
  audioSampleCounter = currentSample - 1;
    } else if ( hasVideoChannel ) {
  playtime.restart();
  playtime = playtime.addMSecs( long((double)-pos * 1000.0 / framerate) );
  current_frame = pos + 1;
  mediaPlayerState->curDecoder()->videoSetFrame( current_frame, stream );
  prev_frame = current_frame - 1;
    } else if ( hasAudioChannel ) {
  playtime.restart();
  playtime = playtime.addMSecs( long((double)-pos * 1000.0 / freq) );
  currentSample = pos + 1;
  mediaPlayerState->curDecoder()->audioSetSample( currentSample, stream );
  audioSampleCounter = currentSample - 1;
    }

    audioMutex->unlock();
}


void LoopControl::startVideo() {

    if ( moreVideo ) {

        if ( mediaPlayerState->curDecoder() ) {

      if ( hasAudioChannel && !isMuted ) {

    current_frame = long( playtime.elapsed() * framerate / 1000 );

    if ( prev_frame != -1 && current_frame <= prev_frame )
        return;

      } else {
    // Don't skip
    current_frame++;
      }

      if ( prev_frame == -1 || current_frame > prev_frame ) {
    if ( current_frame > prev_frame + 1 ) {
        mediaPlayerState->curDecoder()->videoSetFrame( current_frame, stream );
    }
    moreVideo = videoUI->playVideo();
    prev_frame = current_frame;
      }

  } else {

      moreVideo = FALSE;
      killTimer( videoId );

  }

    }
}


void LoopControl::startAudio() {
    
//qDebug("start audio");
    audioMutex->lock();
    if ( moreAudio ) {

  if ( !isMuted && mediaPlayerState->curDecoder() ) {

      currentSample = audioSampleCounter + 1;

      if ( currentSample != audioSampleCounter + 1 )
    qDebug("out of sync with decoder %i %i", currentSample, audioSampleCounter);

      long samplesRead = 0;
      bool readOk=mediaPlayerState->curDecoder()->audioReadSamples( (short*)audioBuffer, channels, 1024, samplesRead, stream );
      long sampleWeShouldBeAt = long( playtime.elapsed() ) * freq / 1000;
      long sampleWaitTime = currentSample - sampleWeShouldBeAt;

// this causes drop outs not sure why its even here
//          if ( ( sampleWaitTime > 2000 ) && ( sampleWaitTime < 20000 ) ) {
//        usleep( (long)((double)sampleWaitTime * 1000000.0 / freq) );
//          }
//        else if ( sampleWaitTime <= -5000 ) {
//       qDebug("need to catch up by: %li (%i,%li)", -sampleWaitTime, currentSample, sampleWeShouldBeAt );
//       //mediaPlayerState->curDecoder()->audioSetSample( sampleWeShouldBeAt, stream );
//       currentSample = sampleWeShouldBeAt;
//         }

      audioDevice->write( audioBuffer, samplesRead * 2 * channels );

      if(  mediaPlayerState->isStreaming == FALSE)
      audioSampleCounter = currentSample + samplesRead - 1;

      moreAudio = readOk && (audioSampleCounter <= total_audio_samples);

  } else {

      moreAudio = FALSE;

  }

    }

    audioMutex->unlock();
}


void LoopControl::killTimers() {

    audioMutex->lock();

    if ( hasVideoChannel ) 
  killTimer( videoId );
    killTimer( sliderId );
    threadOkToGo = FALSE;

    audioMutex->unlock();
}


void LoopControl::startTimers() {

    audioMutex->lock();

    moreVideo = FALSE;
    moreAudio = FALSE;

    if ( hasVideoChannel ) {
  moreVideo = TRUE;
  int mSecsBetweenFrames = (int)(100 / framerate); // 10% of the real value
  videoId = startTimer( mSecsBetweenFrames );
    }

    if ( hasAudioChannel ) {
  moreAudio = TRUE;
  threadOkToGo = TRUE;
    }

    sliderId = startTimer( 300 ); // update slider every 1/3 second

    audioMutex->unlock();
}


void LoopControl::setPaused( bool pause ) {

    if ( !mediaPlayerState->curDecoder() || !mediaPlayerState->curDecoder()->isOpen() )
  return;

    if ( pause ) {
  killTimers();
    } else {
  // Force an update of the position
  mediaPlayerState->setPosition( mediaPlayerState->position() + 1 );
  mediaPlayerState->setPosition( mediaPlayerState->position() - 1 );
  // Just like we never stopped
  startTimers();
    }
}


void LoopControl::stop( bool willPlayAgainShortly ) {

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    if ( !willPlayAgainShortly && disabledSuspendScreenSaver ) {
  disabledSuspendScreenSaver = FALSE; 
  // Re-enable the suspend mode
  QCopEnvelope("QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
    }
#endif

    if ( mediaPlayerState->curDecoder() && mediaPlayerState->curDecoder()->isOpen() ) {

  killTimers();

  audioMutex->lock();

  mediaPlayerState->curDecoder()->close();

  if ( audioDevice ) {
      delete audioDevice;
      delete audioBuffer;
      audioDevice = 0;
      audioBuffer = 0;
  }

  audioMutex->unlock();

    }
}


bool LoopControl::init( const QString& filename ) {
    stop();

    audioMutex->lock();

    fileName = filename;
    stream = 0; // only play stream 0 for now
    current_frame = total_video_frames = total_audio_samples = 0;
    
    qDebug( "Using the %s decoder", mediaPlayerState->curDecoder()->pluginName() );
   
    // ### Hack to use libmpeg3plugin to get the number of audio samples if we are using the libmad plugin
    if ( mediaPlayerState->curDecoder()->pluginName() == QString("LibMadPlugin") ) {
        if ( mediaPlayerState->libMpeg3Decoder() && mediaPlayerState->libMpeg3Decoder()->open( filename )) {
            total_audio_samples = mediaPlayerState->libMpeg3Decoder()->audioSamples( 0 );
            mediaPlayerState->libMpeg3Decoder()->close();
        }
    }
    
    if ( !mediaPlayerState->curDecoder()|| !mediaPlayerState->curDecoder()->open( filename ) ) {
  audioMutex->unlock();
  return FALSE;
    }

    hasAudioChannel = mediaPlayerState->curDecoder()->audioStreams() > 0;
    hasVideoChannel = mediaPlayerState->curDecoder()->videoStreams() > 0;

    if ( hasAudioChannel ) {
  int astream = 0;

    if ( mediaPlayerState->curDecoder()->pluginName() == QString("LibMpeg3Plugin") )
        channels = 2; //dont akx me why, but it needs this hack
    else
        channels = mediaPlayerState->curDecoder()->audioChannels( astream );

    qDebug( "LC- channels = %d", channels );
  
  if ( !total_audio_samples )
      total_audio_samples = mediaPlayerState->curDecoder()->audioSamples( astream );

  total_audio_samples += 1000;

  mediaPlayerState->setLength( total_audio_samples );
  
  freq = mediaPlayerState->curDecoder()->audioFrequency( astream );
  qDebug( "LC- frequency = %d", freq );

  audioSampleCounter = 0;
   int bits_per_sample;
   if ( mediaPlayerState->curDecoder()->pluginName() == QString("LibWavPlugin") ) {
       bits_per_sample =(int) mediaPlayerState->curDecoder()->getTime();
       qDebug("using stupid hack");
   } else {
           bits_per_sample=0;
   }

  audioDevice = new AudioDevice( freq, channels, bits_per_sample);
  audioBuffer = new char[ audioDevice->bufferSize() ];
  channels = audioDevice->channels();

  //### must check which frequency is actually used.
  static const int size = 1;
  short int buf[size];
  long samplesRead = 0;
  mediaPlayerState->curDecoder()->audioReadSamples( buf, channels, size, samplesRead, stream );
    }

    if ( hasVideoChannel ) {
  total_video_frames = mediaPlayerState->curDecoder()->videoFrames( stream );

  mediaPlayerState->setLength( total_video_frames );

  framerate = mediaPlayerState->curDecoder()->videoFrameRate( stream );
  DecodeLoopDebug(( "Frame rate %g total %ld", framerate, total_video_frames ));

  if ( framerate <= 1.0 ) {
      DecodeLoopDebug(( "Crazy frame rate, resetting to sensible" ));
      framerate = 25;
  }

  if ( total_video_frames == 1 ) {
      DecodeLoopDebug(( "Cannot seek to frame" ));
  }

    }

    current_frame = 0;
    prev_frame = -1;

    connect( mediaPlayerState, SIGNAL( positionChanged( long ) ), this, SLOT( setPosition( long ) ) );
    connect( mediaPlayerState, SIGNAL( pausedToggled( bool ) ), this, SLOT( setPaused( bool ) ) );

    audioMutex->unlock();

    return TRUE;
}


void LoopControl::play() {
    qDebug("LC- play");
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    if ( !disabledSuspendScreenSaver || previousSuspendMode != hasVideoChannel ) {
  disabledSuspendScreenSaver = TRUE; 
  previousSuspendMode = hasVideoChannel;
        // Stop the screen from blanking and power saving state
  QCopEnvelope("QPE/System", "setScreenSaverMode(int)" ) 
      << ( hasVideoChannel ? QPEApplication::Disable : QPEApplication::DisableSuspend );
    }
#endif

    playtime.start();
    startTimers();
}


void LoopControl::setMute( bool on ) {
    if ( on != isMuted ) {
  isMuted = on;
  if ( !on ) {
      // Force an update of the position
      mediaPlayerState->setPosition( mediaPlayerState->position() + 1 );
      mediaPlayerState->setPosition( mediaPlayerState->position() - 1 );
      // Resume playing audio
      moreAudio = TRUE;
  }
    }
}


