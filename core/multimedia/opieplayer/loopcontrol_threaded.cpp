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

#define _REENTRANT

#include <qpe/qpeapplication.h>
#include <qimage.h>
#include <qpainter.h>
#ifdef Q_WS_QWS
#include <qpe/qcopenvelope_qws.h>
#endif
#include <qpe/mediaplayerplugininterface.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "loopcontrol.h"
#include "audiodevice.h"
#include "videowidget.h"
#include "audiowidget.h"
#include "mediaplayerstate.h"


#if defined(QT_QWS_CUSTOM) || defined(QT_QWS_IPAQ)
#define USE_REALTIME_AUDIO_THREAD
#endif


extern VideoWidget *videoUI; // now only needed to tell it to play a frame
extern MediaPlayerState *mediaPlayerState;


#define DecodeLoopDebug(x)  qDebug x
//#define DecodeLoopDebug(x)


static char	    *audioBuffer = NULL;
static AudioDevice  *audioDevice = NULL;
static bool	    disabledSuspendScreenSaver = FALSE;


pthread_t	video_tid;
pthread_attr_t  video_attr;
pthread_t	audio_tid;
pthread_attr_t  audio_attr;


bool emitPlayFinished = FALSE;
bool emitChangePos = FALSE;


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
/*
    bool locked() {
	switch ( pthread_mutex_trylock( &mutex ) ) {
	    case EBUSY:
		return TRUE;
	    case 0:
		pthread_mutex_unlock( &mutex );
	    default:
		return FALSE;
	}
    }
*/
private:
    pthread_mutex_t mutex;
};


class currentFrameObj {
public:
    currentFrameObj() : value( 0 ) { }
    void set( long f ) {
	mutex.lock();
	value = f;
	mediaPlayerState->curDecoder()->videoSetFrame( f, 0 );
	mutex.unlock();
    }
    long get() {
	return value;
    }
private:
    long value;
    Mutex mutex;
};


Mutex *videoMutex;
Mutex *audioMutex;
Mutex *globalMutex;


clock_t	begin;


LoopControl::LoopControl( QObject *parent, const char *name )
    : QObject( parent, name ) {
    isMuted = FALSE;
    connect( qApp, SIGNAL( volumeChanged(bool) ), this, SLOT( setMute(bool) ) );
    timerid = startTimer( 200 );
    videoMutex = new Mutex;
    audioMutex = new Mutex;
    globalMutex = new Mutex;
    //begin = clock();
}


LoopControl::~LoopControl() {
    stop();
    killTimer( timerid );
}


static bool sendingNewPos = FALSE;
static long prev_frame = 0;
static int currentSample = 0;


void LoopControl::timerEvent( QTimerEvent* ) {
    // We need to emit playFinished from the main thread, not one of the
    // decoding threads else we'll have all kinds of yucky things happen (reentrance).
    // playFinished will eventually call stop() which stops these threads.
    if ( emitPlayFinished ) {
	emitPlayFinished = FALSE;
        mediaPlayerState->setPlaying( FALSE );
    }

    if ( emitChangePos ) {

	emitChangePos = FALSE;

	if ( hasVideoChannel && hasAudioChannel ) {
	    sendingNewPos = TRUE;
	    mediaPlayerState->setPosition( current_frame );
	} else if ( hasVideoChannel ) {
	    sendingNewPos = TRUE;
	    mediaPlayerState->setPosition( current_frame );
	} else if ( hasAudioChannel ) {
	    sendingNewPos = TRUE;
	    mediaPlayerState->setPosition( audioSampleCounter );
	}

    }
}




void LoopControl::setPosition( long pos ) {
    if ( sendingNewPos ) {
	sendingNewPos = FALSE;
	return;
    }

    if ( hasVideoChannel && hasAudioChannel ) {
	videoMutex->lock();
	audioMutex->lock();
qDebug("setting position");
	playtime.restart();
	playtime = playtime.addMSecs( -pos * 1000 / framerate );
	//begin = clock() - (double)pos * CLOCKS_PER_SEC / framerate;
	current_frame = pos + 1;
	mediaPlayerState->curDecoder()->videoSetFrame( current_frame, stream );
	prev_frame = current_frame - 1;
	currentSample = (int)( current_frame * freq / framerate );
	mediaPlayerState->curDecoder()->audioSetSample( currentSample, stream );
	audioSampleCounter = currentSample - 1;
	audioMutex->unlock();
	videoMutex->unlock();
    } else if ( hasVideoChannel ) {
	videoMutex->lock();
	playtime.restart();
	playtime = playtime.addMSecs( -pos * 1000 / framerate );
	//begin = clock() - (double)pos * CLOCKS_PER_SEC / framerate;
	current_frame = pos + 1;
	mediaPlayerState->curDecoder()->videoSetFrame( current_frame, stream );
	videoMutex->unlock();
	prev_frame = current_frame - 1;
    } else if ( hasAudioChannel ) {
	audioMutex->lock();
	playtime.restart();
	playtime = playtime.addMSecs( -pos * 1000 / freq );
	//begin = clock() - (double)pos * CLOCKS_PER_SEC / freq;
	currentSample = pos + 1; // (int)( current_frame * freq / framerate );
	mediaPlayerState->curDecoder()->audioSetSample( currentSample, stream );
	audioSampleCounter = currentSample - 1;
	audioMutex->unlock();
    }
}


void *startVideoThread( void *ptr ) {
    LoopControl *mpegView = (LoopControl *)ptr;
    mpegView->startVideo();
    return 0;
}

void *startAudioThread( void *ptr ) {
    LoopControl *mpegView = (LoopControl *)ptr;
    mpegView->startAudio();
    return 0;
}

void LoopControl::startVideo() {
    moreVideo = TRUE;

    while ( moreVideo ) {

        if ( mediaPlayerState->curDecoder() && hasVideoChannel ) {

	    if ( hasAudioChannel && !isMuted ) {

		bool done = FALSE;

		do {


/*
		    videoMutex->lock();
		    current_frame = int( (double)playtime.elapsed() * (double)framerate / 1000.0 );
		    //current_frame = ( clock() - begin ) * (double)framerate / CLOCKS_PER_SEC;
	
		    // Sync to Audio
//		    current_frame = (long)((double)(audioSampleCounter - 1000) * framerate / (double)freq);

		    long mSecsToNextFrame = 0;

		    if ( current_frame == prev_frame ) {
			int nf = current_frame + 1;
			if ( nf > 0 && nf != total_video_frames )
			    // mSecsToNextFrame = long(double(nf * CLOCKS_PER_SEC) / framerate) - ( clock() - begin );
			    mSecsToNextFrame = long(double(nf * 1000) / framerate) - ( playtime.elapsed() );
		    }
		    videoMutex->unlock();

		    if ( mSecsToNextFrame ) {
			usleep( mSecsToNextFrame ); // wait a bit

			videoMutex->lock();
			// This should now be the next frame
			current_frame = int( (double)playtime.elapsed() * (double)framerate / 1000.0 );
			//current_frame = ( clock() - begin ) * (double)framerate / CLOCKS_PER_SEC;
			videoMutex->unlock();
		    }

		    videoMutex->lock();
		    done = current_frame >= prev_frame;
		    videoMutex->unlock();
*/
		    videoMutex->lock();
		    current_frame = int( (double)playtime.elapsed() * (double)framerate / 1000.0 );
		    done = current_frame >= prev_frame;
		    videoMutex->unlock();
		    if ( !done )
			usleep( 1000 ); // wait a bit

		} while ( !done );

//		qDebug("elapsed: %i %i (%f)", int( playtime.elapsed() ), current_frame, framerate );

	    } else {
		videoMutex->lock();
		current_frame++;
		videoMutex->unlock();
	    }

	    videoMutex->lock();
	    bool check = current_frame && current_frame > prev_frame;
	    videoMutex->unlock();

	    if ( check ) {
		videoMutex->lock();
		if ( current_frame > prev_frame + 1 ) {
		    qDebug("skipped a frame");
		    mediaPlayerState->curDecoder()->videoSetFrame( current_frame, stream );
		}
		prev_frame = current_frame;
		if ( moreVideo = videoUI->playVideo() ) 
		    emitChangePos = TRUE;
		videoMutex->unlock();
	    }

	} else 
	    moreVideo = FALSE;

    }

    if ( !moreVideo && !moreAudio ) 
	emitPlayFinished = TRUE;

    pthread_exit(NULL);
}

void LoopControl::startAudio() {
    moreAudio = TRUE;
    
    while ( moreAudio ) {

	if ( !isMuted && mediaPlayerState->curDecoder() && hasAudioChannel ) {

	    audioMutex->lock();
	    currentSample = mediaPlayerState->curDecoder()->audioGetSample( stream );

	    if ( currentSample == 0 )
		currentSample = audioSampleCounter + 1;

	    if ( currentSample != audioSampleCounter + 1 )
		qDebug("out of sync with decoder %i %i", currentSample, audioSampleCounter);
	    audioMutex->unlock();

/*
	    int sampleWeShouldBeAt = int( playtime.elapsed() ) * freq / 1000;

	    if ( sampleWeShouldBeAt - currentSample > 20000 ) {
		mediaPlayerState->curDecoder()->audioSetSample( sampleWeShouldBeAt, stream );
		currentSample = sampleWeShouldBeAt;
	    }
*/
	    long samplesRead = 0;

	    const long samples = 1024;

	    moreAudio = !mediaPlayerState->curDecoder()->audioReadSamples( (short*)audioBuffer, channels, samples, samplesRead, stream );

	    audioMutex->lock();
	    long sampleWeShouldBeAt = long( playtime.elapsed() ) * freq / 1000;
	    //long sampleWeShouldBeAt = long( clock() - begin ) * (double) freq / CLOCKS_PER_SEC;
	    long sampleWaitTime = currentSample - sampleWeShouldBeAt;
	    audioMutex->unlock();

	    if ( sampleWaitTime >= 0 && sampleWaitTime <= 2000 ) {
		//qDebug("sampleWaitTime: %i", sampleWaitTime);
		usleep( ( sampleWaitTime * 1000000 ) / ( freq ) );
	    } else {
		audioMutex->lock();
		if ( sampleWaitTime <= -2000 ) {
		    qDebug("need to catch up by: %li (%i,%li)", -sampleWaitTime, currentSample, sampleWeShouldBeAt );
		    mediaPlayerState->curDecoder()->audioSetSample( sampleWeShouldBeAt, stream );
		    currentSample = sampleWeShouldBeAt;
		}
		audioMutex->unlock();
	    }

	    audioDevice->write( audioBuffer, samplesRead * 2 * channels );

	    audioMutex->lock();
//	    audioSampleCounter += samplesRead;
	    audioSampleCounter = currentSample + samplesRead - 1;
	    audioMutex->unlock();

	    if ( !hasVideoChannel )
		emitChangePos = TRUE;

	    //qDebug("currentSample: %i audioSampleCounter: %i total_audio_samples: %i", currentSample, audioSampleCounter, total_audio_samples);
//	    qDebug("current: %i counter: %i total: %i", currentSample, audioSampleCounter, (int)total_audio_samples);
	    moreAudio = audioSampleCounter <= total_audio_samples;

	} else {

	    if ( mediaPlayerState->curDecoder() && hasAudioChannel )
	        usleep( 100000 ); // Check every 1/10 sec to see if mute is off
	    else
		moreAudio = FALSE;

	}
    }

    qDebug( "End of file" );

    if ( !moreVideo && !moreAudio ) 
	emitPlayFinished = TRUE;

    pthread_exit(NULL);
}

void LoopControl::killTimers() {
    if ( hasVideoChannel ) {
	if ( pthread_self() != video_tid ) {
	    if ( pthread_cancel(video_tid) == 0 ) {
		void *thread_result = 0;
		if ( pthread_join(video_tid,&thread_result) != 0 )
		    qDebug("thread join error 1");
		pthread_attr_destroy(&video_attr);
	    }
	}
    }
    if ( hasAudioChannel ) {
	if ( pthread_self() != audio_tid ) {
	    if ( pthread_cancel(audio_tid) == 0 ) {
		void *thread_result = 0;
		if ( pthread_join(audio_tid,&thread_result) != 0 )
		    qDebug("thread join error 2");
		pthread_attr_destroy(&audio_attr);
	    }
	}
    }
}

void LoopControl::startTimers() {
    moreVideo = FALSE;
    moreAudio = FALSE;

    if ( hasVideoChannel ) {
	moreVideo = TRUE;
	pthread_attr_init(&video_attr);
	pthread_create(&video_tid, &video_attr, (void * (*)(void *))startVideoThread, this);
    }

    if ( hasAudioChannel ) {
	moreAudio = TRUE;
	pthread_attr_init(&audio_attr);
#ifdef USE_REALTIME_AUDIO_THREAD
	pthread_attr_setschedpolicy(&audio_attr,SCHED_RR); // Real-time round robin
	//qDebug("min: %i, max: %i", sched_get_priority_min( SCHED_RR ), sched_get_priority_max( SCHED_RR ) );
	sched_param params;
	params.sched_priority = 50;
	pthread_attr_setschedparam(&audio_attr,&params);
#endif
	pthread_create(&audio_tid, &audio_attr, (void * (*)(void *))startAudioThread, this);
    }
}




void LoopControl::setPaused( bool pause ) {
    static int whenPaused = 0;

    if ( !mediaPlayerState->curDecoder() || !mediaPlayerState->curDecoder()->isOpen() )
	return;

    if ( pause ) {
	// Remember where we are
	whenPaused = playtime.elapsed();
	killTimers();
    } else {
	// Just like we never stopped
	playtime.restart();
	playtime = playtime.addMSecs( -whenPaused );
	whenPaused = 0;
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

	mediaPlayerState->curDecoder()->close();

	if ( audioDevice ) {
	    delete audioDevice;
	    delete audioBuffer;
	    audioDevice = 0;
	    audioBuffer = 0;
	}

    }
}


bool LoopControl::init( const QString& filename ) {
    stop();
    fileName = filename;
    stream = 0; // only play stream 0 for now
    current_frame = total_video_frames = total_audio_samples = 0;
    
    qDebug( "Using the %s decoder", mediaPlayerState->curDecoder()->pluginName() );
   
    // ### Hack to use libmpeg3plugin to get the number of audio samples if we are using the libmad plugin
    if ( mediaPlayerState->curDecoder()->pluginName() == QString("LibMadPlugin") ) {
	if ( mediaPlayerState->libMpeg3Decoder() && mediaPlayerState->libMpeg3Decoder()->open( filename ) ) {
	    total_audio_samples = mediaPlayerState->libMpeg3Decoder()->audioSamples( 0 );
	    mediaPlayerState->libMpeg3Decoder()->close();
	}
    }

    if ( !mediaPlayerState->curDecoder()|| !mediaPlayerState->curDecoder()->open( filename ) )
	return FALSE;

    hasAudioChannel = mediaPlayerState->curDecoder()->audioStreams() > 0;
    hasVideoChannel = mediaPlayerState->curDecoder()->videoStreams() > 0;

    if ( hasAudioChannel ) {
	int astream = 0;

	channels = mediaPlayerState->curDecoder()->audioChannels( astream );
	DecodeLoopDebug(( "channels = %d\n", channels ));
	
	if ( !total_audio_samples )
	    total_audio_samples = mediaPlayerState->curDecoder()->audioSamples( astream );

	mediaPlayerState->setLength( total_audio_samples );
	
	freq = mediaPlayerState->curDecoder()->audioFrequency( astream );
	DecodeLoopDebug(( "frequency = %d\n", freq ));

	audioSampleCounter = 0;

	static const int bytes_per_sample = 2; //16 bit

	audioDevice = new AudioDevice( freq, channels, bytes_per_sample );
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

    videoMutex->lock();
    current_frame = 0;
    prev_frame = -1;
    videoMutex->unlock();

    connect( mediaPlayerState, SIGNAL( positionChanged( long ) ), this, SLOT( setPosition( long ) ) );
    connect( mediaPlayerState, SIGNAL( pausedToggled( bool ) ), this, SLOT( setPaused( bool ) ) );

    //setBackgroundColor( black );
    return TRUE;
}


void LoopControl::play() {

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    if ( !disabledSuspendScreenSaver ) {
	disabledSuspendScreenSaver = TRUE; 
        // Stop the screen from blanking and power saving state
	QCopEnvelope("QPE/System", "setScreenSaverMode(int)" ) 
	    << ( hasVideoChannel ? QPEApplication::Disable : QPEApplication::DisableSuspend );
    }
#endif

    //begin = clock();
    playtime.start();
    startTimers();
    //updateGeometry();
}


void LoopControl::setMute( bool on ) {
    if ( isMuted != on ) {
	isMuted = on;
	if ( isMuted ) {
	} else {
	    int frame = current_frame; // mediaPlayerState->curDecoder()->videoGetFrame( stream );
	    playtime.restart();
	    playtime = playtime.addMSecs( -frame * 1000 / framerate );
	    //begin = clock() - (double)frame * CLOCKS_PER_SEC / framerate;
	    mediaPlayerState->curDecoder()->audioSetSample( frame*freq/framerate, stream );
	}
    }
}


