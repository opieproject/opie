#include "SDL_gfxPrimitives.h"

#include "constants.h"
#include "gates_game.h"
#include "random.h"

GatesGame :: GatesGame( SFCave *p, int w, int h, int diff )
	: Game( p, w, h, diff )
{
	gameName = "Gates";
	difficulty = MENU_DIFFICULTY_EASY;
	blockUpdateRate = 200;

	terrain = new Terrain( w, h );
	player = new Player( w, h );
	highScore = 0;
}

GatesGame :: ~GatesGame()
{
	// terrain and player get deleted by parent class
}

void GatesGame :: init()
{
	Game :: init();

	blockHeight = 80;
	blockWidth = 20;
	lastGateBottomY = 0;

	gateDistance = 75;
	nextGate = nextInt( 50 ) + gateDistance;
	gapHeight = 75;

	switch( difficulty )
	{
		case MENU_DIFFICULTY_EASY:
			gapHeight = 75;
			player->setMovementInfo( 0.4, 0.6, 4, 5 );
			break;
		case MENU_DIFFICULTY_NORMAL:
			gapHeight = 50;
			player->setMovementInfo( 0.4, 0.6, 4, 5 );
			break;
		case MENU_DIFFICULTY_HARD:
			gapHeight = 25;
			player->setMovementInfo( 0.6, 0.8, 6, 7 );
			break;
        case MENU_DIFFICULTY_CUSTOM:
        {
            // Read custom difficulty settings for this game
            gapHeight = parent->loadIntSetting( "Gates_custom_gapHeight", 75 );
        
            double thrust = parent->loadDoubleSetting( "Gates_custom_player_thrust", 0.4 );
            double gravity = parent->loadDoubleSetting( "Gates_custom_player_gravity", 0.6 );
            double maxUp = parent->loadDoubleSetting( "Gates_custom_player_maxupspeed", 4.0 );
            double maxDown = parent->loadDoubleSetting( "Gates_custom_player_maxdownspeed", 5.0 );
			player->setMovementInfo( thrust, gravity, maxUp, maxDown );

            break;
        }
	}

    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
        blocks[i].y( -1 );
}

void GatesGame ::  update( int state )
{
	Game::update( state );

	// Game logic goes here
	if ( state == STATE_PLAYING )
	{
		if ( nrFrames % 3 == 0 )
			score ++;

		if ( nrFrames % 500 == 0 )
		{
			if ( gapHeight > 75 )
				gapHeight -= 5;
		}

		// Slightly random gap distance
		if ( nrFrames >= nextGate )
		{
			nextGate = nrFrames + nextInt( 50 ) + gateDistance;
			addGate();
		}

		if ( checkCollisions() )
		{
			parent->changeState( STATE_CRASHING );
			return;
		}

		terrain->moveTerrain( 5 );
		moveBlocks( 5 );
		player->move( press );
	}
}

void GatesGame :: draw( SDL_Surface *screen )
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


void GatesGame :: addGate()
{
	printf( "gapHeight = %d\n", gapHeight );
    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
    {
        if ( blocks[i].y() == -1 )
        {
            int x1 = sWidth;
            int y1 = terrain->getMapTop(50);
            int b1Height = nextInt(terrain->getMapBottom( 50 ) - terrain->getMapTop(50) - gapHeight);

            // See if height between last gate and this one is too big
            if ( b1Height - 100 > lastGateBottomY )
                b1Height -= 25;
            else if ( b1Height + 100 < lastGateBottomY )
                b1Height += 25;
            lastGateBottomY = b1Height;


            int x2 = sWidth;
            int y2 = y1 + b1Height + gapHeight;
            int b2Height = terrain->getMapBottom( 50 ) - y2;


            blocks[i].setRect( x1, y1, blockWidth, b1Height );
            blocks[i+1].setRect( x2, y2, blockWidth, b2Height );

            break;
        }
    }
}

void GatesGame :: moveBlocks( int amountToMove )
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

void GatesGame :: drawBlocks( SDL_Surface *screen )
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

bool GatesGame :: checkCollisions()
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
