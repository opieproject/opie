#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/timeb.h>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "constants.h"

#include "sound.h"
#include "menu.h"
#include "help.h"
#include "game.h"
#include "terrain.h"
#include "random.h"
#include "sfcave.h"
#include "font.h"
#include "settings.h"
#include "util.h"

#include "sfcave_game.h"
#include "gates_game.h"
#include "fly_game.h"

void start( int argc, char *argv[] )
{
	SFCave *app = new SFCave( argc, argv );
	app->mainEventLoop();
	delete app;
}

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[])
{
	start( argc, argv );
	return 0;
}


SFCave :: SFCave( int argc, char *argv[] )
{
    setupOK = false;

	// Load settings
	string diff = loadSetting( "GameDifficulty", "Easy" );
	string game = loadSetting( "GameType", "SFCave" );
	musicPath = loadSetting( "MusicPath", SOUND_PATH );
	musicType = loadSetting( "MusicType", "mod,ogg" );
	bool soundOn = loadBoolSetting( "SoundOn", true );
	bool musicOn = loadBoolSetting( "MusicOn", true );
	if ( musicPath[musicPath.size()-1] != '/' )
	   musicPath += "/";
	printf( "musicPath %s\n", musicPath.c_str() );

    // Init main SDL Library
	initSDL( argc, argv );

	// Init font handler
	if ( !FontHandler::init() )
	{
	   printf( "Unable to initialise fonts!\n" );
	   return;
	}

	// Init SoundHandler
	if ( !SoundHandler :: init() )
		printf("Unable to open audio!\n");

	SoundHandler :: setSoundsOn( soundOn );
	SoundHandler :: setMusicOn( musicOn );

	currentGame = Game::createGame( this, WIDTH, HEIGHT, game, diff );
	if ( !currentGame )
		currentGame = new SFCaveGame( this, WIDTH, HEIGHT, 0 );
	currentGame->setSeed(-1);

	// Create menu
	menu = new Menu( this );

	// Create help screen
	help = new Help( this );

	maxFPS = 50;
	showFps = false;
	
	setupOK = true;
}

SFCave :: ~SFCave()
{
	if ( currentGame )
		delete currentGame;

	if ( menu )
		delete menu;

	if ( help )
		delete help;

	SDL_FreeSurface( screen );
	FontHandler::cleanUp();
	SoundHandler :: cleanUp();

	SDL_Quit();
}


void SFCave :: initSDL( int argc, char *argv[] )
{
	const SDL_VideoInfo *info;
	Uint8  video_bpp;
	Uint32 videoflags;

	// Initialize SDL
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}

	video_bpp = 16;

	if ( !SDL_VideoModeOK(WIDTH, HEIGHT, video_bpp, SDL_DOUBLEBUF) )
		printf( "No double buffering\n" );

	videoflags = SDL_HWSURFACE | SDL_SRCALPHA;
	while ( argc > 1 )
	{
		--argc;
		if ( strcmp(argv[argc-1], "-bpp") == 0 )
		{
			video_bpp = atoi(argv[argc]);
			--argc;
		}
		else if ( strcmp(argv[argc], "-hw") == 0 )
		{
			videoflags |= SDL_HWSURFACE;
		}
		else if ( strcmp(argv[argc], "-warp") == 0 )
		{
			videoflags |= SDL_HWPALETTE;
		}
		else if ( strcmp(argv[argc], "-fullscreen") == 0 )
		{
			videoflags |= SDL_FULLSCREEN;
		}
		else if ( strcmp(argv[argc], "-h") == 0 )
		{
			fprintf(stderr,
			"Usage: %s [-bpp N] [-warp] [-hw] [-fullscreen]\n",
								argv[0]);
			exit(1);
		}
	}

	// Set 240x320 video mode
	if ( (screen = SDL_SetVideoMode( WIDTH,HEIGHT,video_bpp,videoflags )) == NULL )
	{
		printf( "Couldn't set %ix%i video mode: %s\n",WIDTH,HEIGHT,SDL_GetError() );
		exit(2);
	}

	// Use alpha blending
	//SDL_SetAlpha(screen, SDL_RLEACCEL, 0);

	// Set title for window
	SDL_WM_SetCaption("SFCave","SFCave");
}

void SFCave :: mainEventLoop()
{
    if ( !setupOK )
        return;
        
	// Wait for a keystroke
	finish = false;
	state = 0;
	state = STATE_CRASHED;
	changeState( STATE_MENU );

	FPS = 0;
	actualFPS = 0;
	time1 = 0;

	limitFPS = true;
	while ( !finish )
	{
		// calc FPS
		calcFPS();

    	SDL_FillRect( screen, 0, 0 );

		handleGameState( );

   		SDL_Flip( screen );

		if ( limitFPS )
			FPSDelay();
		else
			SDL_Delay( 5 );

		handleEvents();
	}
}


void SFCave :: handleGameState()
{
	switch( state )
	{
		case STATE_MENU:
			SDL_FillRect( screen, 0, 0 );
			menu->draw( screen );
			break;
		case STATE_HELP:
			SDL_FillRect( screen, 0, 0 );
			help->update();
			help->draw( screen );
			break;
		case STATE_NEWGAME:
			currentGame->setReplay( false );
			currentGame->init();
			changeState( STATE_PLAYING );
			break;

		case STATE_REPLAY:
			currentGame->setReplay( true );
			currentGame->init();
			changeState( STATE_PLAYING );
			break;

		case STATE_PLAYING:
		case STATE_CRASHING:
		case STATE_CRASHED:
			currentGame->update( state );
			currentGame->draw( screen );
			break;

		case STATE_QUIT:
			finish = true;
			break;
	}
}

void SFCave :: handleEvents()
{
	SDL_Event event;

	// Check for events
	while ( SDL_PollEvent(&event) )
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				// Escape keypress quits the app
				if ( event.key.keysym.sym == SDLK_ESCAPE )
				{
					finish = true;
					break;
				}

				if ( state == STATE_MENU )
				{
					int rc = menu->handleKeys( event.key );
					if ( rc != -1 )
						handleMenuSelect( rc );
				}
				else if ( state == STATE_HELP )
				{
					help->handleKeys( event.key );
				}
				else if ( state == STATE_CRASHED )
				{
					if ( event.type == SDL_KEYDOWN )
					{
						if ( event.key.keysym.sym == SDLK_UP ||
							event.key.keysym.sym == SDLK_DOWN ||
							event.key.keysym.sym == SDLK_SPACE )
							changeState( STATE_NEWGAME );
						else if ( event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == 0 )
						{
							changeState( STATE_MENU );
							menu->resetToTopMenu();
						}
						else if ( event.key.keysym.sym == SDLK_r )
						{
							changeState( STATE_REPLAY );
						}
						else if ( event.key.keysym.sym == SDLK_s )
						{
							SoundHandler :: playSound( SND_EXPLOSION );
						}
					}
				}
				else
				{
					switch ( event.key.keysym.sym )
					{
						case SDLK_f:
							if ( event.type == SDL_KEYDOWN )
								showFps = !showFps;
							break;
						case SDLK_l:
							if ( event.type == SDL_KEYDOWN )
								limitFPS = !limitFPS;
							break;

						default:
							currentGame->handleKeys( event.key );
							break;
					}
				}

				break;
			}

			case SDL_QUIT:
				finish = true;
				break;
			default:
				break;
		}
	}
}

void SFCave :: changeState( int s )
{
	if ( state != s )
		currentGame->stateChanged( state, s );

    if ( state == STATE_MENU && s == STATE_HELP )
        help->init();
    if ( state == STATE_CRASHED && s == STATE_MENU )
	{
		SoundHandler :: stopMusic( true );

		string musicFile = chooseRandomFile( musicPath, musicType );
    	SoundHandler :: setMusicVolume( 128 );
		SoundHandler :: playMusic( musicFile );
	}
    else if ( state == STATE_MENU && (s == STATE_NEWGAME || s == STATE_REPLAY) )
    {
		SoundHandler :: stopMusic( );

        // Start the in game music
        string musicFile = INGAME_MUSIC;
    	SoundHandler :: playMusic( musicFile );
    	SoundHandler :: setMusicVolume( 25 );
    }

	state = s;
}


void SFCave :: handleMenuSelect( int menuId )
{
	switch( menuId )
	{
		case MENU_STARTGAME:
			changeState( STATE_NEWGAME );
			break;

		case MENU_HELP:
            changeState( STATE_HELP );
			break;

		case MENU_QUIT:
			changeState( STATE_QUIT );
			break;

		case MENU_PLAY_REPLAY:
			if ( currentGame->isReplayAvailable() )
				changeState( STATE_REPLAY );
			else
				setMenuStatusText( "No replay available yet" );
			break;

		case MENU_LOAD_REPLAY:
		{
    		string replayFile = getHomeDir() + "/" + currentGame->getGameName() + ".replay";

			currentGame->loadReplay( replayFile );

			break;
		}

		case MENU_SAVE_REPLAY:
		{

			if ( currentGame->isReplayAvailable() )
			{
				string replayFile = getHomeDir() + "/" + currentGame->getGameName() + ".replay";

				currentGame->saveReplay( replayFile );
			}
			else
				setMenuStatusText( "No replay available yet" );

			break;
		}
		case MENU_CLEAR_SCORES:
			break;

		case MENU_GAME_SFCAVE:
			if ( currentGame->getGameName() != "SFCave" )
			{
				int diff = currentGame->getDifficulty();
				delete currentGame;
				currentGame = new SFCaveGame( this, WIDTH, HEIGHT, 0 );
				currentGame->setDifficulty( diff );

				saveSetting( "GameType", "SFCave" );
			}
			break;

		case MENU_GAME_GATES:
			if ( currentGame->getGameName() != "Gates" )
			{
				int diff = currentGame->getDifficulty();
				delete currentGame;
				currentGame = new GatesGame( this, WIDTH, HEIGHT, 0 );
				currentGame->setDifficulty( diff );

				saveSetting( "GameType", "Gates" );
			}
			break;
			break;

		case MENU_GAME_FLY:
			if ( currentGame->getGameName() != "Fly" )
			{
				int diff = currentGame->getDifficulty();
				delete currentGame;
				currentGame = new FlyGame( this, WIDTH, HEIGHT, 0 );
				currentGame->setDifficulty( diff );

				saveSetting( "GameType", "Fly" );
			}
			break;

		case MENU_DIFFICULTY_EASY:
			currentGame->setDifficulty( MENU_DIFFICULTY_EASY );
			saveSetting( "GameDifficulty", "Easy" );
			break;

		case MENU_DIFFICULTY_NORMAL:
			currentGame->setDifficulty( MENU_DIFFICULTY_NORMAL );
			saveSetting( "GameDifficulty", "Medium" );
			break;

		case MENU_DIFFICULTY_HARD:
			currentGame->setDifficulty( MENU_DIFFICULTY_HARD );
			saveSetting( "GameDifficulty", "Hard" );
			break;

		case MENU_DIFFICULTY_CUSTOM:
			currentGame->setDifficulty( MENU_DIFFICULTY_CUSTOM );
			saveSetting( "GameDifficulty", "Custom" );
			break;

		case MENU_SOUND_ON:
			SoundHandler :: setSoundsOn( true );
			saveSetting( "SoundOn", "true" );
			break;

		case MENU_SOUND_OFF:
			SoundHandler :: setSoundsOn( false );
			saveSetting( "SoundOn", "false" );
			break;

		case MENU_MUSIC_ON:
			SoundHandler :: setMusicOn( true );
			saveSetting( "MusicOn", "true" );
			break;

		case MENU_MUSIC_OFF:
			SoundHandler :: setMusicOn( false );
			saveSetting( "MusicOn", "false" );
			break;

        case MENU_CUSTOM_THRUST:
            customPlayerMenuVal = PLAYER_THRUST;
            origValue = currentGame->getPlayer()->getValue( customPlayerMenuVal );
			setMenuStatusText( currentGame->getPlayer()->getValueString( customPlayerMenuVal ) );
            break;
        case MENU_CUSTOM_GRAVITY:
            customPlayerMenuVal = PLAYER_GRAVITY;
            origValue = currentGame->getPlayer()->getValue( customPlayerMenuVal );
			setMenuStatusText( currentGame->getPlayer()->getValueString( customPlayerMenuVal ) );
            break;
        case MENU_CUSTOM_MAXSPEEDUP:
            customPlayerMenuVal = PLAYER_MAX_SPEED_UP;
            origValue = currentGame->getPlayer()->getValue( customPlayerMenuVal );
			setMenuStatusText( currentGame->getPlayer()->getValueString( customPlayerMenuVal ) );
            break;
        case MENU_CUSTOM_MAXSPEEDDOWN:
            customPlayerMenuVal = PLAYER_MAX_SPEED_DOWN;
            origValue = currentGame->getPlayer()->getValue( customPlayerMenuVal );
			setMenuStatusText( currentGame->getPlayer()->getValueString( customPlayerMenuVal ) );
            break;
        case MENU_CUSTOM_INCREASE:
            currentGame->getPlayer()->incValue( customPlayerMenuVal );
			setMenuStatusText( currentGame->getPlayer()->getValueString( customPlayerMenuVal ) );
            break;
        case MENU_CUSTOM_DECREASE:
            currentGame->getPlayer()->decValue( customPlayerMenuVal );
			setMenuStatusText( currentGame->getPlayer()->getValueString( customPlayerMenuVal ) );
            break;
        case MENU_CUSTOM_SAVE:
        {
            // save settings
            string key = currentGame->getGameName() + "_custom_player_" + currentGame->getPlayer()->getValueTypeString( customPlayerMenuVal );
   			saveSetting( key, currentGame->getPlayer()->getValue( customPlayerMenuVal ) );

            break;
        }
        case MENU_CUSTOM_CANCEL:
            currentGame->getPlayer()->setValue( customPlayerMenuVal, origValue );
            break;

		default:
			break;
	}
}

void SFCave :: setMenuStatusText( string statusText )
{
	menu->setStatusText( statusText );
}


void SFCave :: saveSetting( string key, string val )
{
	Settings cfg( "sfcave-sdl" );
	cfg.writeSetting( key, val );
}

void SFCave :: saveSetting( string key, int val )
{
	Settings cfg( "sfcave-sdl" );
	cfg.writeSetting( key, val );
}

void SFCave :: saveSetting( string key, long val )
{
	Settings cfg( "sfcave-sdl" );
	cfg.writeSetting( key, val );
}

void SFCave :: saveSetting( string key, double val )
{
	Settings cfg( "sfcave-sdl" );
	cfg.writeSetting( key, val );
}

string SFCave :: loadSetting( string key, string defaultVal )
{
	string val;
	Settings cfg( "sfcave-sdl" );
	cfg.readSetting( key, val );

	if ( val == "" )
		val = defaultVal;

	return val;
}

bool SFCave :: loadBoolSetting( string key, bool defaultVal )
{
	bool val = defaultVal;
	Settings cfg( "sfcave-sdl" );
	cfg.readSetting( key, val );

	return val;
}

int SFCave :: loadIntSetting( string key, int defaultVal )
{
	int val = defaultVal;
	Settings cfg( "sfcave-sdl" );
	cfg.readSetting( key, val );

	return val;
}

double SFCave :: loadDoubleSetting( string key, double defaultVal )
{
	double val = defaultVal;
	Settings cfg( "sfcave-sdl" );
	cfg.readSetting( key, val );

	return val;
}


void SFCave :: calcFPS()
{
	struct timeb tp;
	ftime( &tp );
	start =(tp.time%10000)*10000 + tp.millitm;
	if ( start - time1 >= 1000 )
	{
		actualFPS = FPS;
		FPS = 0;
		time1 = start;
	}
	else
		FPS ++;
}

void SFCave :: FPSDelay()
{
	struct timeb tp;
	// Slow down polling - limit to x FPS
	ftime( &tp );
	end = abs((tp.time%10000)*10000 + tp.millitm);
	if ( end-start < (1000/maxFPS) )
	{
		if ( (1000/maxFPS)-(end-start) > 500 )
		{
			// Should never happen but in case it does sleep for 5 seconds
			printf( "WARNING WILL ROBINSON! delay = %ld - start %ld, end %ld\n", (1000/maxFPS)-(end-start), start, end );
			SDL_Delay( 5 );
		}
		else
			SDL_Delay((1000/maxFPS)-(end-start) );
	}
}
