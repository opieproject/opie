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

	thrust = 0.4;
	gravity = 0.6;
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
	currentThrust = 0;
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
			int c = (int)((150.0/50) * (50.0 - (pos.x() - trail[i].x() ) ));
			boxRGBA( screen, trail[i].x(), trail[i].y(), trail[i].x() + 2, trail[i].y() + 2, 255, (int)(1.5*c), 0, c );
		}
	}
}

void Player :: move( bool up )
{
    // Find enpty trail and move others
	moveTrails();

	if ( up )
		currentThrust -= thrust;
	else
		currentThrust += gravity;

	if ( currentThrust > maxDownSpeed )
		currentThrust = maxDownSpeed;
	else if ( currentThrust < -maxUpSpeed )
		currentThrust = -maxUpSpeed;

	pos.moveBy( 0, (int)(currentThrust) );
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

void Player :: setMovementInfo( double up, double grav, double maxUp, double maxDown )
{
	thrust = up;
	gravity = grav;
	maxUpSpeed = maxUp;
	maxDownSpeed = maxDown;
}


void Player :: incValue( int valueType )
{
    switch( valueType )
    {
        case PLAYER_THRUST:
            thrust += 0.1;
            break;
        case PLAYER_GRAVITY:
            gravity += 0.1;
            break;
        case PLAYER_MAX_SPEED_UP:
            maxUpSpeed += 0.1;
             break;
       case PLAYER_MAX_SPEED_DOWN:
            maxDownSpeed += 0.1;
            break;
    }
}

void Player :: decValue( int valueType )
{
    switch( valueType )
    {
        case PLAYER_THRUST:
            thrust -= 0.1;
            break;
        case PLAYER_GRAVITY:
            gravity -= 0.1;
            break;
       case PLAYER_MAX_SPEED_UP:
            maxUpSpeed -= 0.1;
            break;
        case PLAYER_MAX_SPEED_DOWN:
            maxDownSpeed -= 0.1;
            break;
    }
}

void Player :: setValue( int valueType, double val )
{    
    switch( valueType )
    {
        case PLAYER_THRUST:
            thrust = val;
            break;
        case PLAYER_GRAVITY:
            gravity = val;
            break;
       case PLAYER_MAX_SPEED_UP:
            maxUpSpeed = val;
            break;
        case PLAYER_MAX_SPEED_DOWN:
            maxDownSpeed = val;
            break;
    }
}

double Player :: getValue( int valueType )
{
    double val;
    switch( valueType )
    {
        case PLAYER_THRUST:
            val = thrust;
            break;
        case PLAYER_GRAVITY:
            val = gravity;
            break;
        case PLAYER_MAX_SPEED_UP:
            val = maxUpSpeed;
            break;
        case PLAYER_MAX_SPEED_DOWN:
            val = maxDownSpeed;
            break;
    }
    
    return val;
}

string Player :: getValueTypeString( int valueType )
{
    string val;
    switch( valueType )
    {
        case PLAYER_THRUST:
            val = "thrust";
            break;
        case PLAYER_GRAVITY:
            val = "gravity";
            break;
        case PLAYER_MAX_SPEED_UP:
            val = "maxupspeed";
            break;
        case PLAYER_MAX_SPEED_DOWN:
            val = "maxdownspeed";
            break;
    }

    return val;
}

string Player :: getValueString( int valueType )
{
    char val[50];
    switch( valueType )
    {
        case PLAYER_THRUST:
            sprintf( val, "Thrust - %lf", thrust );
            break;
        case PLAYER_GRAVITY:
            sprintf( val, "Gravity - %lf", gravity );
            break;
        case PLAYER_MAX_SPEED_UP:
            sprintf( val, "Max Speed Up - %lf", maxUpSpeed );
            break;
        case PLAYER_MAX_SPEED_DOWN:
            sprintf( val, "Max Speed Down - %lf", maxDownSpeed );
            break;
    }
    
    string ret = val;
    return ret;
}

