#ifndef __SOUND_H
#define __SOUND_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#define NR_SOUNDS 3

class SoundHandler
{
public:
	static bool init();
	static void cleanUp();

	static int playSound( int soundNr, int channel = -1, int nrLoops = 0, int playBeforeFinished = false );
	static void stopSound( int channel, bool fadeOut, int nrMilliSecs = 1000 );
	static void setSoundsOn( bool val );
	static void setMusicOn( bool val );
	static void playMusic( string musicFile );
	static void playMusic( bool fadeIn = false );
	static void stopMusic( bool fadeOut = false );
	static void setMusicVolume( int vol );


private:
	static Mix_Music *music;
	static Mix_Chunk *sounds[NR_SOUNDS];
	static int soundChannels[NR_SOUNDS];
	static bool soundOn;
	static bool musicOn;

	SoundHandler() {}
};

#endif
