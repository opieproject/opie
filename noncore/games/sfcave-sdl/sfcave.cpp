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
	FontHandler::init();
	SFCave app( argc, argv );
	FontHandler::cleanUp();
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
	string diff = loadSetting( "GameDifficulty", "Easy" );
	string game = loadSetting( "GameType", "SFCave" );
	musicPath = loadSetting( "MusicPath", SOUND_PATH );
	printf( "musicPath %s\n", musicPath.c_str() );
	musicType = loadSetting( "MusicType", "mod,ogg" );
	if ( musicPath[musicPath.size()-1] != '/' )
	   musicPath += "/";

    // Init main SDL Library
	initSDL( argc, argv );

	// Init SoundHandler
	if ( !SoundHandler :: init() )
		printf("Unable to open audio!\n");

	currentGame = Game::createGame( this, WIDTH, HEIGHT, game, diff );
	if ( !currentGame )
		currentGame = new SFCaveGame( this, WIDTH, HEIGHT, 0 );
	currentGame->setSeed(-1);
	menu = new Menu( this );
	
	help = new Help( this );

	maxFPS = 50;
	showFps = false;
	mainEventLoop();

	SoundHandler :: cleanUp();
	SDL_Quit();
}

SFCave :: ~SFCave()
{
	if ( currentGame )
		delete currentGame;

	if ( menu )
		delete menu;

	SDL_FreeSurface( screen );
}


void SFCave :: drawGameScreen(  )
{
	//ClearScreen(screen, "Titletext");

}

void SFCave :: initSDL( int argc, char *argv[] )
{
	const SDL_VideoInfo *info;
	Uint8  video_bpp;
	Uint32 videoflags;



	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	/* Alpha blending doesn't work well at 8-bit color */
	video_bpp = 16;

	if ( !SDL_VideoModeOK(WIDTH, HEIGHT, 16, SDL_DOUBLEBUF) )
		printf( "No double buffering\n" );

	videoflags = SDL_HWSURFACE | SDL_SRCALPHA;//|| SDL_DOUBLEBUF;// | SDL_SRCALPHA | SDL_RESIZABLE;
	while ( argc > 1 ) {
		--argc;
		if ( strcmp(argv[argc-1], "-bpp") == 0 ) {
			video_bpp = atoi(argv[argc]);
			--argc;
		} else
		if ( strcmp(argv[argc], "-hw") == 0 ) {
			videoflags |= SDL_HWSURFACE;
		} else
		if ( strcmp(argv[argc], "-warp") == 0 ) {
			videoflags |= SDL_HWPALETTE;
		} else
		if ( strcmp(argv[argc], "-fullscreen") == 0 ) {
			videoflags |= SDL_FULLSCREEN;
		} else {
			fprintf(stderr,
			"Usage: %s [-bpp N] [-warp] [-hw] [-fullscreen]\n",
								argv[0]);
			exit(1);
		}
	}

	/* Set 240x320 video mode */
	if ( (screen=SDL_SetVideoMode(WIDTH,HEIGHT,video_bpp,videoflags)) == NULL ) {
		fprintf(stderr, "Couldn't set %ix%i video mode: %s\n",WIDTH,HEIGHT,SDL_GetError());
		exit(2);
	}

	/* Use alpha blending */
	SDL_SetAlpha(screen, SDL_RLEACCEL, 0);

	/* Set title for window */
	SDL_WM_SetCaption("SFCave","SFCave");
}

void SFCave :: mainEventLoop()
{
	SDL_Event event;
	int done;

	/* Wait for a keystroke */
	done = 0;
	state = 0;
	state = STATE_CRASHED;
	changeState( STATE_MENU );

	int FPS = 0;
	bool limitFPS = true;
	actualFPS = 0;
	long time1 = 0;
	long start;
	long end;
//	long nrTimes = 0;
	struct timeb tp;
	while ( !done )
	{
		// calc FPS
    	ftime( &tp );
		start =(tp.time%10000)*10000 + tp.millitm;
//		printf( "start = %ld, time1 - %d, st-tm - %d, tp.time - %ld\n", start, time1, start-time1, (tp.time%1000)*1000 );
		if ( start - time1 >= 1000 )
		{
			actualFPS = FPS;
//			printf( "%d FPS = %d\n", nrTimes++, actualFPS );
			FPS = 0;
			time1 = start;
		}
		else
			FPS ++;

    	SDL_FillRect( screen, 0, 0 );
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
				printf( "STATE_NEWGAME\n" );
				currentGame->setReplay( false );
				currentGame->init();
				changeState( STATE_PLAYING );
				break;

			case STATE_REPLAY:
				printf( "STATE_NEWGAME\n" );
				currentGame->setReplay( true );
				currentGame->init();
				changeState( STATE_PLAYING );
				break;

			case STATE_PLAYING:
			case STATE_CRASHING:
				currentGame->update( state );
            	currentGame->draw( screen );
				break;

			case STATE_CRASHED:
				currentGame->update( state );
            	currentGame->draw( screen );

				// Display Game Over message
				break;
				
			case STATE_QUIT:
				done = 1;
				break;
		}

		/* Show */
//		if ( state != STATE_CRASHED )
    		SDL_Flip( screen );
//		SDL_UpdateRect(screen, 0, 0, 0, 0);

		if ( limitFPS )
		{
			/* Slow down polling - limit to x FPS*/
			ftime( &tp );
			end = abs((tp.time%10000)*10000 + tp.millitm);
			if ( end-start < (1000/maxFPS) )
			{
//			printf( "end - %ld, timetaken for frame = %ld, sleeping for %ld %d\n", end, end-start, (1000/maxFPS)-(end-start), actualFPS );
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
		else
			SDL_Delay( 5 );

		/* Check for events */
		while ( SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					// Escape keypress quits the app
					if ( event.key.keysym.sym != SDLK_ESCAPE )
					{
//						printf( "Key Pressed was %d %s\n", event.key.keysym.sym, SDL_GetKeyName( event.key.keysym.sym ) );

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
								    printf( "showFPS - %d\n", showFps );
									if ( event.type == SDL_KEYDOWN )
										showFps = !showFps;
									break;
								case SDLK_l:
									if ( event.type == SDL_KEYDOWN )
										limitFPS = !limitFPS;
									break;

								case SDLK_p:
									if ( event.type == SDL_KEYDOWN )
									{
										maxFPS ++;
										printf( "maxFPS - %d\n", maxFPS );
									}
									break;

								case SDLK_o:
									if ( event.type == SDL_KEYDOWN )
									{
										maxFPS --;
										printf( "maxFPS - %d\n", maxFPS );
									}
									break;

								case SDLK_n:
									currentGame->getTerrain()->offset++;
									break;

								default:
									currentGame->handleKeys( event.key );
									break;
							}
						}

						break;
					}


				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
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
		printf("playing music %s\n", musicFile.c_str() );
    	SoundHandler :: setMusicVolume( 128 );
		SoundHandler :: playMusic( musicFile );
	}
    else if ( state == STATE_MENU && (s == STATE_NEWGAME || s == STATE_REPLAY) )
    {
		SoundHandler :: stopMusic( );

        // Start the in game music
        string musicFile = SOUND_PATH "ingame.mod";
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
#ifdef QWS
		    QString replayFile = getenv( "HOME" );
#else
		    QString replayFile = ".";
#endif
    		replayFile += string( "/" ) + currentGame->getGameName() + ".replay";

			currentGame->loadReplay( replayFile );

			break;
		}

		case MENU_SAVE_REPLAY:
		{

			if ( currentGame->isReplayAvailable() )
			{
#ifdef QWS
			    QString replayFile = getenv( "HOME" );
#else
			    QString replayFile = ".";
#endif
				replayFile += string( "/" ) + currentGame->getGameName() + ".replay";

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

		case MENU_SOUND_ON:
			SoundHandler :: setSoundsOn( true );
			break;

		case MENU_SOUND_OFF:
			SoundHandler :: setSoundsOn( false );
			break;

		case MENU_MUSIC_ON:
			SoundHandler :: setMusicOn( true );
			break;

		case MENU_MUSIC_OFF:
			SoundHandler :: setMusicOn( false );
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

string SFCave :: loadSetting( string key, string defaultVal )
{
	string val;
	Settings cfg( "sfcave-sdl" );
	cfg.readSetting( key, val );

	if ( val == "" )
		val = defaultVal;

	return val;
}
