#include "SDL_gfxPrimitives.h"

#include "constants.h"
#include "sfcave_game.h"
#include "random.h"

SFCaveGame :: SFCaveGame( SFCave *p, int w, int h, int diff )
	: Game( p, w, h, diff )
{
	gameName = "SFCave";
	difficulty = MENU_DIFFICULTY_EASY;
	blockUpdateRate = 200;

	terrain = new Terrain( w, h );
	player = new Player( w, h );
	highScore = 0;
}

SFCaveGame :: ~SFCaveGame()
{
}

void SFCaveGame :: init()
{
	Game :: init();

	blockDistance = 50;
	blockHeight = 80;
	blockWidth = 20;

	switch( difficulty )
	{
		case MENU_DIFFICULTY_EASY:
			blockDistance = 50;
			break;
		case MENU_DIFFICULTY_NORMAL:
			blockDistance = 40;
			break;
		case MENU_DIFFICULTY_HARD:
			blockDistance = 30;
			break;
	}

    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
        blocks[i].y( -1 );
}

void SFCaveGame ::  update( int state )
{
	Game::update( state );

	if ( state == STATE_PLAYING )
	{
		if ( nrFrames % 3 == 0 )
			score ++;

		if ( nrFrames % 200 == 0 )
		{
			if ( terrain->getMaxHeight() < sHeight - 100 )
			{
				terrain->increaseMaxHeight( 10 );

				// Reduce block height
				if ( terrain->getMaxHeight() > sHeight - 150 )
					blockHeight -= 5;
			}
		}

		if ( checkCollisions() )
		{
//			printf( "Crashed!\n" );
			parent->changeState( STATE_CRASHING );
			return;
		}

		if ( nrFrames % blockDistance == 0 )
			addBlock();

		// Game logic goes here
		terrain->moveTerrain( 5 );
		moveBlocks( 5 );
		player->move( press );
	}
}

void SFCaveGame :: draw( SDL_Surface *screen )
{
	Game::preDraw( screen );

	if ( parent->getState() == STATE_PLAYING )
	{
		// Screen drawing goes here
		terrain->drawTerrain( screen );

		player->draw( screen );

		drawBlocks( screen );
	}
	else
	{
		// Screen drawing goes here
		terrain->drawTerrain( screen );

		drawBlocks( screen );

		player->draw( screen );
	}

	Game::draw( screen );
}

void SFCaveGame :: addBlock()
{
    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
    {
        if ( blocks[i].y() == -1 )
        {
            int x = sWidth;

            int y = terrain->getMapTop( MAPSIZE-1 ) + (int)(nextInt(terrain->getMapBottom( MAPSIZE-1 ) - terrain->getMapTop( MAPSIZE-1 ) - blockHeight));

            blocks[i].setRect( x, y, blockWidth, blockHeight );

            break;
        }
    }
}

void SFCaveGame :: moveBlocks( int amountToMove )
{
    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
    {
        if ( blocks[i].y() != -1 )
        {
            blocks[i].moveBy( -amountToMove, 0 );
            if ( blocks[i].x() + blocks[i].y() < 0 )
                blocks[i].y( -1 );
        }
    }
}

void SFCaveGame :: drawBlocks( SDL_Surface *screen )
{
    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
	{
        if ( blocks[i].y() != -1 )
        {
        	SDL_Rect r = blocks[i].getRect();
			SDL_FillRect( screen, &r, SDL_MapRGB( screen->format, 100, 100, 255 ) );
        }
	}
}

bool SFCaveGame :: checkCollisions()
{
	// Check collisions with blocks
	for ( int i = 0 ; i < BLOCKSIZE ; ++i )
    {
        if ( blocks[i].y() != -1 )
        {
            if ( blocks[i].intersects( player->getPos() ) )
                return true;
        }
    }
	// Check collision with landscape
	return terrain->checkCollision( player->getX(), player->getY(), player->getHeight() );
}
