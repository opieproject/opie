#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include <stdlib.h>

#include "starfield.h"
#include "random.h"
#include "util.h"

#define VERTICAL_VELOCITY 0

StarField :: StarField( bool side, int nStars, int mx, int my, int minz, int maxz )
{
    nrStars = nStars;
	maxX = mx;
	maxY = my;
	minZ = minz;
	maxZ = maxz;

	min_brightness = 50;
	top_star_speed = 6;

	sideways = side;

	if ( !sideways )
	{
		x = new int[nrStars];
		y = new int[nrStars];
		z = new int[nrStars];

		star_color = 0;
		vel_x = 0;
		vel_y = 0;
		pos_x = 0;
		pos_y = 0;
	}
	else
	{
		star_color = new int[nrStars];
		vel_x = new int[nrStars];
		vel_y = new int[nrStars];
		pos_x = new int[nrStars];
		pos_y = new int[nrStars];

		x = 0;
		y = 0;
		z = 0;
	}

	init();
}

StarField :: ~StarField()
{
	if ( star_color )
		delete []star_color;
	if ( vel_x )
		delete []vel_x;
	if ( vel_y )
		delete []vel_y;
	if ( pos_x )
		delete []pos_x;
	if ( pos_y )
		delete []pos_y;

	if ( x )
		delete []x;
	if ( y )
		delete []y;
	if ( z )
		delete []z;
}

void StarField :: init()
{
	if ( !sideways )
	{
		for ( int i = 0 ; i < nrStars ; ++i )
		{
			newStar( i );
			z[i] = (int)(Random() * (double)(maxZ - minZ)) + minZ	;
		}
	}
	else
	{
		int brightness;

		//Initialise each star
		for(int i = 0; i < nrStars ; i++)
		{
			//Inialise velocities
			vel_x[i] = -(int)floor( (Random() * top_star_speed)+1 );
			vel_y[i] = VERTICAL_VELOCITY;

			//Initialise positions randomly
			pos_x[i] = (int)floor((Random() * 240));
			pos_y[i] = (int)floor((Random() * 320));

			//The Faster it goes, the Dimmer it is
			if (vel_x[i] != 0)
			{
				brightness = (int)(255 / fabs(vel_x[i]) );
				if (brightness < min_brightness)
					brightness = min_brightness;
			}
			else
				brightness = 255;

			star_color[i] = brightness;
		}
	}
}

void StarField :: newStar( int starNr )
{
	if ( !sideways )
	{
		x[starNr] = (int)(Random() * (double)maxX) + 1;
		y[starNr] = (int)(Random() * (double)maxY) + 1;
		z[starNr] = maxZ;

		int i = (int)(Random() * 4.0);
		if(i < 2)
			x[starNr] = -x[starNr];
		if(i == 0 || i == 2)
			y[starNr] = -y[starNr];
	}
}

void StarField :: move( )
{
	if ( !sideways )
	{
		int amountToMove = 16;
		for(int i = 0; i < nrStars; i++)
		{
			// Rotate star
			z[i] = z[i] - amountToMove;
			if(z[i] < minZ)
				newStar(i);
		}
	}
	else
	{
		for(int i = 0; i < nrStars ; i++)
		{
			//Check speed limits x
			if (vel_x[i] > top_star_speed)
				vel_x[i] = top_star_speed;
			else if (vel_x[i] < -top_star_speed)
				vel_x[i] = -top_star_speed;

			//Check speed limits y
			if (vel_y[i] > top_star_speed)
				vel_y[i] = top_star_speed;
			else if (vel_y[i] < -top_star_speed)
				vel_y[i] = -top_star_speed;



			//Move Star
			pos_x[i] += vel_x[i];
			pos_y[i] += vel_y[i];

			if (pos_x[i] < 0)
				pos_x[i] = pos_x[i] + 240;

			if (pos_x[i] > 240)
				pos_x[i] = pos_x[i] - 240;
			if (pos_y[i] < 0)
				pos_y[i] = pos_y[i] + 320;

			if (pos_y[i] > 320)
				pos_y[i] = pos_y[i] - 320;
		}
	}
}

void StarField :: draw( SDL_Surface *screen, int w, int h )
{
	if ( !sideways )
	{
		int scrW = w / 2;
		int scrH = h / 2;
		for(int i = 0; i < nrStars; i++)
		{
			int sx = (x[i] * 256) / z[i] + scrW;
			int sy = (y[i] * 256) / z[i] + scrH;
			if(sx < 0 || sx > w || sy < 0 || sy > h)
			{
				newStar(i);
			}
			else
			{
				int size = (z[i] * 3) / maxZ;

				SDL_Rect r;
				r.x = sx;
				r.y = sy;
				r.w = 3 - size;
				r.h = 3 - size;

				SDL_FillRect( screen, &r, SDL_MapRGB( screen->format, 255, 255, 255 ) );
			}
		}
	}
	else
	{
		SDL_LockSurface( screen );
		for(int i = 0; i < nrStars ; i++)
		{

			Uint32 c = getpixel( screen, pos_x[i], pos_y[i] );


			if ( c == 0 )
				lineRGBA( screen, pos_x[i], pos_y[i], pos_x [i]+ vel_x[i], pos_y[i] + vel_y[i], star_color[i], star_color[i], star_color[i], 255 );

			//*** NOTE : if the velocity of the stars never changes then the values such as 'pos_x[i] + vel_x[i]' could be precalculated for each star ***
		}
		SDL_UnlockSurface( screen );
	}
}



// Test
#ifdef DEBUG_STARS
SDL_Surface *screen;
StarField *stars;

void go()
{
	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	int videoflags = SDL_SWSURFACE ;

 	if ( (screen=SDL_SetVideoMode(240, 320,32,videoflags)) == NULL )
 	{
 		fprintf(stderr, "Couldn't set %ix%i video mode: %s\n",240,320,SDL_GetError());
 		exit(2);
 	}

	stars = new StarField( false, 200 );

	bool done = false;
	while ( !done )
	{
		SDL_FillRect( screen, 0, 0 );
		stars->draw( screen );
		stars->move( );

 		SDL_Flip( screen );

		SDL_Delay( 10 );

		SDL_Event event;
		while ( SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					// Escape keypress quits the app
					if ( event.key.keysym.sym != SDLK_ESCAPE )
					{
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




#ifdef __cplusplus
extern "C"
#endif
int main( int argc, char *argv[] )
{
	go();
}

#endif
