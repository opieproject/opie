#include "SDL_gfxPrimitives.h"

#include "constants.h"
#include "fly_game.h"
#include "random.h"

FlyGame :: FlyGame( SFCave *p, int w, int h, int diff )
	: Game( p, w, h, diff )
{
	gameName = "Fly";
	difficulty = MENU_DIFFICULTY_EASY;

	terrain = new FlyTerrain( w, h );
	player = new Player( w, h );
	highScore = 0;
}

FlyGame :: ~FlyGame()
{
}

void FlyGame :: init()
{
	Game :: init();

	switch( difficulty )
	{
		case MENU_DIFFICULTY_EASY:
			player->setMovementInfo( 0.3, 0.2, 1.5, 1.5 );
			break;
		case MENU_DIFFICULTY_NORMAL:
			player->setMovementInfo( 0.35, 0.4, 2.5, 3 );
			break;
		case MENU_DIFFICULTY_HARD:
			player->setMovementInfo( 0.4, 0.6, 4, 5 );
			break;
	}

	startScoring = false;
}

void FlyGame ::  update( int state )
{
	Game::update( state );

	if ( state == STATE_PLAYING )
	{

		if ( nrFrames % 3 == 0 )
		{
		    int diff = terrain->getMapBottom( 10 ) - player->getY();
			int tmpScore = ((FlyTerrain *)terrain)->getScore( 1, diff );
//			printf( "diff - %d  score - %d\n", diff, tmpScore );
			if ( !startScoring )
			{
				if ( tmpScore > 0 )
					startScoring = true;
			}

			if ( startScoring )
			{
				// Update score
				// get distance between landscape and ship

				// the closer the difference is to 0 means more points
				score += tmpScore;
			}
		}

		if ( checkCollisions() )
		{
//			printf( "Crashed!\n" );
			parent->changeState( STATE_CRASHING );
			return;
		}

		// Game logic goes here
		terrain->moveTerrain( 5 );
		player->move( press );
	}
}

void FlyGame :: draw( SDL_Surface *screen )
{
	Game::preDraw( screen );

	// Screen drawing goes here
	terrain->drawTerrain( screen );

	player->draw( screen );

	Game::draw( screen );
}


bool FlyGame :: checkCollisions()
{
	bool ret = false;

	// Check collision with landscape

	return terrain->checkCollision( player->getX(), player->getY(), player->getHeight() );
}
