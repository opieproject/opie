#include <SDL.h>
#include "SDL_gfxPrimitives.h"

#include "constants.h"
#include "player.h"
#include "random.h"
#include "animatedimage.h"

Player :: Player( int w, int h )
{
	sWidth = w;
	sHeight = h;

	thrustUp = 0.4;
	thrustDown = 0.6;
	maxUpSpeed = 4.0;
	maxDownSpeed = 5.0;

	explosion = new AnimatedImage( IMAGES_PATH "explosion.bmp", 15 );
	init();
}

Player :: ~Player()
{
	if ( explosion )
		delete explosion;
}

void Player :: init()
{
	// Set player position
	pos.x( 50 );
	pos.y( sWidth/2 );
	pos.h( 2 );
	pos.w( 4 );
	thrust = 0;
	crashing = false;
	crashLineLength = 0;
	crashed = false;
	explosion->reset();
	allFaded = false;
	expNextFrame = false;

	// Reset Trail
	for ( int i = 0 ; i < TRAILSIZE ; ++i )
	{
		trail[i].x( -1 );
		trail[i].y( 0 );
		trail[i].w( 2 );
		trail[i].h( 2 );
	}
}

void Player :: draw( SDL_Surface *screen )
{
	if ( !crashing )
	{
		// Draw Player
//		ellipseRGBA( screen, pos.x(), pos.y(), pos.x()+ pos.width(), pos.y()+pos.height(), 0, 255, 255, 255 );
		filledEllipseRGBA( screen, pos.x() + pos.w(), pos.y(), pos.w(), pos.h(), 0, 255, 255, 255 );

		// Draw Trail
		drawTrails( screen );
	}
	else
	{
		drawTrails( screen );

		if ( !crashed )
			explosion->draw( screen, pos.x(), pos.y() );
	}
}

void Player :: drawTrails( SDL_Surface *screen )
{
	if ( allFaded && crashing )
		return;

	for ( int i = 0 ; i < TRAILSIZE ; ++i )
	{
		if ( trail[i].x() >= 0 )
		{
//			int r = (int) ((255.0/pos.x()) * (trail[i].x));
//			int g = (int) ((150.0/pos.x()) * (trail[i].x));
			int c = (int)((150.0/50) * (50.0 - (pos.x() - trail[i].x() ) ));
//			SDL_FillRect( screen, &trail[i], SDL_MapRGBA( screen->format, r, g, 0, 0 ) ); //(int)(1.5*c), 0, 255 ) );
			boxRGBA( screen, trail[i].x(), trail[i].y(), trail[i].x() + 2, trail[i].y() + 2, 255, (int)(1.5*c), 0, c );
		}
	}
}

void Player :: move( bool up )
{
    // Find enpty trail and move others
	moveTrails();

	if ( up )
		thrust -= thrustUp;
	else
		thrust += thrustDown;

	if ( thrust > maxDownSpeed )
		thrust = maxDownSpeed;
	else if ( thrust < -maxUpSpeed )
		thrust = -maxUpSpeed;

	pos.moveBy( 0, (int)(thrust) );
}

void Player :: moveTrails()
{
    bool done = false;
	bool stillVisible = false;

	// Dont do anything here if all faded when were crashing
	if ( allFaded && crashing )
		return;

	for ( int i = 0 ; i < TRAILSIZE ; ++i )
    {
        if ( trail[i].x() < 0 )
        {
			stillVisible = true;
            if ( !crashing && !done )
            {
                trail[i].x( pos.x() - 5 );
                trail[i].y( pos.y() );
                done = true;
            }
        }
        else
            trail[i].x( trail[i].x() - 1 );
    }

	if ( !stillVisible )
		allFaded = true;
}

bool Player :: updateCrashing()
{
	crashing = true;

	moveTrails();
	if ( expNextFrame )
	{
		expNextFrame = false;
		crashed = !explosion->nextFrame();
	}
	else
		expNextFrame = true;

	return crashed;
}

void Player :: setMovementInfo( double up, double down, double maxUp, double maxDown )
{
	thrustUp = up;
	thrustDown = down;
	maxUpSpeed = maxUp;
	maxDownSpeed = maxDown;
}

