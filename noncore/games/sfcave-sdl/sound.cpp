#include "constants.h"
#include "sound.h"

Mix_Chunk *SoundHandler :: sounds[NR_SOUNDS];
Mix_Music *SoundHandler :: music;
int SoundHandler :: soundChannels[NR_SOUNDS];
bool SoundHandler :: soundOn;
bool SoundHandler :: musicOn;

bool SoundHandler :: init( )
{
	// We're going to be requesting certain things from our audio
	// device, so we set them up beforehand
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16; //AUDIO_S16; /* 16-bit stereo */
	int audio_channels = 2;
	int audio_buffers = 1024;//4096;

	// This is where we open up our audio device.  Mix_OpenAudio takes
	// as its parameters the audio format we'd /like/ to have.
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
	{
		printf("Unable to open audio!\n");
		return false;
	}

	// We're going to pre-load the sound effects that we need right here
	sounds[SND_EXPLOSION] = Mix_LoadWAV( SOUND_PATH "explosion.wav");
	sounds[SND_THRUST] = Mix_LoadWAV( SOUND_PATH "thrust.wav");

	music = 0;

	soundOn = true;

	return true;
}

void SoundHandler :: cleanUp()
{
	// Free audio sounds
	Mix_FreeChunk( sounds[SND_EXPLOSION] );
	Mix_FreeChunk( sounds[SND_THRUST] );

	if ( music )
		Mix_FreeMusic( music );

	Mix_CloseAudio();
}

int SoundHandler :: playSound( int soundNr, int channel, int nrLoops, int playBeforeFinished )
{
	if ( !soundOn )
		return -1;

	if ( soundNr >= NR_SOUNDS )
		return -1;

	Mix_Chunk *chunk = sounds[soundNr];
    if( channel == -1 || !Mix_Playing( channel ) )
		channel = Mix_PlayChannel(-1, sounds[soundNr], nrLoops);

	Mix_Volume( channel, MIX_MAX_VOLUME );
	return channel;
}

void SoundHandler :: stopSound( int channel, bool fadeOut, int nrMilliSecs )
{
	if ( !soundOn )
		return;

	if ( !fadeOut )
		Mix_HaltChannel( channel );
	else
	{
		Mix_FadeOutChannel( channel, nrMilliSecs );
	}
}

void SoundHandler :: playMusic( string musicFile )
{
	if ( !soundOn )
		return;

	// If music already exists - stop it playing if necessary and free it up
	if ( music )
	{
		stopMusic();
		Mix_FreeMusic( music );
	}

	// Load music
	music = Mix_LoadMUS( musicFile.c_str() );
	if(!music)
	{
		printf("Mix_LoadMUS(%s): %s\n", musicFile.c_str(), Mix_GetError());
		// this might be a critical error...
	}

	playMusic();
}

void SoundHandler :: playMusic( bool fade )
{
	if ( !soundOn )
		return;

	if ( music )
	{
		Mix_VolumeMusic( MIX_MAX_VOLUME );
		Mix_RewindMusic();
		
		if ( fade )
    		Mix_FadeInMusic( music, -1, 1000 );
        else
    		Mix_PlayMusic( music, -1 );

	}
}

void SoundHandler :: stopMusic( bool fadeOut )
{
	if ( !music || !Mix_PlayingMusic() )
		return;

	if ( fadeOut && Mix_FadingMusic() == MIX_NO_FADING )
	{
		Mix_FadeOutMusic( 1000 );
	}
	else
	{
		Mix_HaltMusic();
	}

}

void SoundHandler :: setMusicVolume( int vol )
{
    Mix_VolumeMusic( vol );
}

void SoundHandler :: setSoundsOn( bool val )
{
	soundOn = val;
}

void SoundHandler :: setMusicOn( bool val )
{
	musicOn = val;
	
	if ( !musicOn )
	   stopMusic();
    else
        playMusic( true );
}
