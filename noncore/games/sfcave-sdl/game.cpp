#include <stdio.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "font.h"

#include "constants.h"
#include "game.h"
#include "player.h"
#include "random.h"
#include "sound.h"
#include "stringtokenizer.h"

#include "sfcave_game.h"
#include "gates_game.h"
#include "fly_game.h"
#include "starfield.h"

Game :: Game( SFCave *p, int w, int h, int diff )
{
	parent = p;
	sHeight = h;
	sWidth = w;
	difficulty = diff;
	replayIt = 0;
	replay = false;
	terrain = 0;
	player = 0;
	thrustChannel = -1;
}

Game :: ~Game()
{
	if ( terrain )
		delete terrain;

	if ( player )
		delete player;

	replayList.clear();
}

void Game :: init()
{
	if ( replay )
	{
		setSeed( currentSeed );
		replayIt = replayList.begin();
	}
	else
	{
		setSeed( -1 );
		replayList.clear();
	}
	
	score = 0;
	nrFrames = 0;
	press = false;

	// Load highscore
	string key = getGameName() + "_" + getGameDifficultyText() + "_highscore";
	highScore = atoi( parent->loadSetting( key, "0" ).c_str() );

	terrain->initTerrain();
	player->init();
}

void Game :: handleKeys( SDL_KeyboardEvent &key )
{
	if ( !replay && 
   	     (key.keysym.sym == SDLK_SPACE ||
   	     key.keysym.sym == SDLK_KP_ENTER ||
   	     key.keysym.sym == SDLK_RETURN ||
   	     key.keysym.sym == SDLK_UP) )
	{
		if ( key.type == SDL_KEYDOWN )
		{
			if ( !press )
				replayList.push_back( nrFrames );
			press = true;
		}
		else
		{
			if ( press )
				replayList.push_back( nrFrames );
			press = false;

		}
	}
}



string Game :: getGameDifficultyText()
{
	string ret;

	if ( difficulty == MENU_DIFFICULTY_EASY )
		ret =  "Easy";
	else if ( difficulty == MENU_DIFFICULTY_NORMAL )
		ret = "Medium";
	else if ( difficulty == MENU_DIFFICULTY_HARD )
		ret = "Hard";
	else if ( difficulty == MENU_DIFFICULTY_CUSTOM )
		ret = "Custom";

	return ret;
}

void Game :: setDifficulty( string diff )
{
	if ( diff == "Easy" )
		difficulty = MENU_DIFFICULTY_EASY;
	else if ( diff == "Medium" )
		difficulty = MENU_DIFFICULTY_NORMAL;
	else if ( diff == "Hard" )
		difficulty = MENU_DIFFICULTY_HARD;
    else if ( diff == "Custom" )
		difficulty = MENU_DIFFICULTY_CUSTOM;
		
    init();
}

void Game :: setDifficulty( int diff )
{
    difficulty = diff;
    init();
}

void Game :: update( int state )
{
	nrFrames ++;

	if ( score > highScore )
		highScore = score;


	if ( state == STATE_PLAYING )
	{
        if ( replay )
        {
            while( replayIt != replayList.end() && (*replayIt) == nrFrames-1 )
            {
                press = !press;
                replayIt ++;
            }
        }

		if ( press && thrustChannel == -1 )
			thrustChannel = SoundHandler :: playSound( SND_THRUST, -1, -1, false );

		if ( !press &&thrustChannel != -1 )
		{
			SoundHandler :: stopSound( thrustChannel, true, 300 );
			thrustChannel = -1;
		}
	}

	if ( state == STATE_CRASHING || state == STATE_CRASHED )
	{
		// fade out any trail marks remainin
		if ( player->updateCrashing() )
			parent->changeState( STATE_CRASHED );

	}
}

void Game :: preDraw( SDL_Surface *screen )
{
}

void Game :: draw( SDL_Surface *screen )
{
	char tmp[100];
	string scoreText;
	sprintf( tmp, "Score: %06ld   High Score: %06ld", score, highScore );
	FontHandler::draw( screen, FONT_WHITE_TEXT, tmp, 3, 10 );

	if ( parent->getState() == STATE_CRASHED )
	{
		string crashText;
		crashText = "Game Over";
		int x = (240 - FontHandler::TextWidth( FONT_WHITE_TEXT, (const char *)crashText.c_str() )) / 2;
		FontHandler::draw( screen, FONT_WHITE_TEXT, (const char *)crashText.c_str(), x, 150 );

		int fontHeight = FontHandler::FontHeight( FONT_WHITE_TEXT );
		crashText = "Press Middle Button to play again";
		x = (240 - FontHandler::TextWidth( FONT_WHITE_TEXT, (const char *)crashText.c_str() )) / 2;
		FontHandler::draw( screen, FONT_WHITE_TEXT, (const char *)crashText.c_str(), x, 150 + fontHeight );

		crashText = "or OK for menu";
		x = (240 - FontHandler::TextWidth( FONT_WHITE_TEXT, (const char *)crashText.c_str() )) / 2;
		FontHandler::draw( screen, FONT_WHITE_TEXT, (const char *)crashText.c_str(), x, 150 + 2*fontHeight );
	}

	if ( parent->showFPS() )
	{
		sprintf( tmp, "FPS : %d", parent->getFPS() );
		FontHandler::draw( screen, FONT_WHITE_TEXT, tmp, 20, 300 );
	}
}

void Game :: stateChanged( int from, int to )
{
	if ( from != STATE_CRASHING && to == STATE_CRASHING )
	{
		// play explosion sound
		SoundHandler :: stopSound( -1, false );
		SoundHandler :: playSound( SND_EXPLOSION );

		// Check and save highscore
		printf( "Got Highscore = %d\n", gotHighScore() );
		if ( gotHighScore() )
		{
			string key = getGameName() + "_" +  getGameDifficultyText() + "_highscore";
			parent->saveSetting( key, getHighScore() );
		}

	}
}

void Game :: setSeed( int seed )
{
    if ( seed == -1 )
        currentSeed = ((unsigned long) time((time_t *) NULL));
    else
        currentSeed = seed;
    PutSeed( currentSeed );
}

void Game :: saveReplay( string file )
{
    FILE *out;
    out = fopen( file.c_str(), "w" );
    if ( !out )
    {
        printf( "Couldn't write to /home/root/%s\n", file.c_str() );
		parent->setMenuStatusText( "Couldn't save replay file" );
        return;
    }

    // Build up string of values
    // Format is:: <landscape seed> <game type> <difficulty> <framenr> <framenr>.......
    string val;
    char tmp[20];
    sprintf( tmp, "%d %d ", currentSeed, difficulty );
    val = tmp;

    list<int>::iterator it = replayList.begin();
    while( it != replayList.end() )
    {
        sprintf( tmp, "%d ", *it );
        val += tmp;

        it++;
    }
    val += "\n";

    string line;
    sprintf( tmp, "%d\n", val.length() );
    line = tmp;
    fwrite( line.c_str(), 1, line.length(), out );

    fwrite( val.c_str(), 1, val.length(), out );

    fclose( out );
}

void Game :: loadReplay( string file )
{

    FILE *in = fopen( (const char *)file.c_str(), "r" );

    if ( in == 0 )
    {
        printf( "Couldn't load replay file %s!\n", (const char *)file.c_str() );
		parent->setMenuStatusText( "Couldn't load replay file" );
        return;
    }

    // Read next line - contains the size of the options
    char line[10+1];
    fgets( line, 10, in );

    int length = -1;
    sscanf( line, "%d", &length );
    char *data = new char[length+1];

    fread( data, 1, length, in );

    string sep  = " ";

    StringTokenizer st( data, sep );

    // print it out
    vector<string>::iterator it = st.begin();
    currentSeed = atoi( (*it).c_str() );
    ++it;
    difficulty = atoi( (*it).c_str() );
    ++it;

    replayList.clear();
    for ( ; it != st.end(); ++it )
    {
        int v = atoi( (*it).c_str() );
        replayList.push_back( v );
    }

    delete data;

    fclose( in );
}


Game *Game :: createGame( SFCave *p, int w, int h, string game, string difficulty )
{
	Game *g;

	if ( game == "SFCave" )
		g = new SFCaveGame( p, w, h, 0 );
	else if ( game == "Gates" )
		g = new GatesGame( p, w, h, 0 );
	else if ( game == "Fly" )
		g = new FlyGame( p, w, h, 0 );

	if ( g )
		g->setDifficulty( difficulty );

	return g;
}
