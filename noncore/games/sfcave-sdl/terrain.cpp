#include "SDL.h"
#include "SDL_rotozoom.h"
#include "SDL_gfxPrimitives.h"

#include "constants.h"
#include "terrain.h"
#include "random.h"
#include "util.h"
#include "starfield.h"

Terrain :: Terrain( int w, int h, bool drawTop, bool drawBottom )
{
	sWidth = w;
	sHeight = h;
	speed = 1;
	segSize = sWidth/(MAPSIZE-2)+1;
	this->drawTop = drawTop;
	this->drawBottom = drawBottom;

	stars = new StarField( true );

	SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, sWidth + 20, sHeight, 32,
											0x000000ff,0x0000ff00, 0x00ff0000, 0xff000000);
	terrainSurface = SDL_DisplayFormat( tmp );
	SDL_FreeSurface( tmp );

	initTerrain();

}

Terrain :: ~Terrain()
{
	SDL_FreeSurface( terrainSurface );
	delete stars;
}

void Terrain :: initTerrain()
{
	dir = 1;
	offset = 0;

	maxHeight = 50;

	mapTop[0] = (int)(nextInt(50)) + 5;
	mapBottom[0] = sHeight - (maxHeight - mapTop[0]);
	for ( int i = 1 ; i < MAPSIZE ; ++i )
		setPoint( i );

	SDL_FillRect( terrainSurface, 0, 0 );
	// Draw Terrain into surface
	Sint16 px[5];
	Sint16 py[5];

	for ( int i = 0 ; i < MAPSIZE ; ++i )
	{
		int left = (i*segSize);
		int right = ((i+1)*segSize);
		px[0] = left;
		py[0] = mapTop[i];
		px[1] = right;
		py[1] = mapTop[i+1];
		px[2] = right;
		py[2] = 0;
		px[3] = left;
		py[3] = 0;

		if ( drawTop )
		{
			// Only display top landscape if not running FLY_GAME
			filledPolygonRGBA( terrainSurface, px, py, 4, 0, 190, 0, 255 );
		}

		if ( drawBottom )
		{
			py[0] = mapBottom[i];
			py[1] = mapBottom[i+1];
			py[2] = sHeight;
			py[3] = sHeight;
			filledPolygonRGBA( terrainSurface, px, py, 4, 0, 190, 0, 255 );
		}

	}


}

void Terrain :: moveTerrain( int amountToMove )
{
	stars->move();

	offset += amountToMove;
	speed = offset/segSize;

//	printf( "offset - %d, speed - %d\n", offset, speed );
	if ( offset >= segSize )
	{
		for ( int i = 0 ; i < (MAPSIZE)-speed ; ++i )
		{
			mapTop[i] = mapTop[i+speed];
			mapBottom[i] = mapBottom[i+speed];
		}

		for ( int i = (MAPSIZE)-speed ; i < MAPSIZE ; ++i )
		{
			setPoint( i );
		}

		SDL_Rect dst;
		dst.x = offset;
		dst.y = 0;
		dst.w = sWidth;
		dst.h = sHeight;

		SDL_Rect dst2;
		dst2.x = 0;
		dst2.y = 0;

		SDL_BlitSurface(terrainSurface, &dst, terrainSurface, &dst2 );

		dst.x = sWidth-5;
		dst.y = 0;
		dst.w = offset;
		dst.h = sHeight;
		SDL_FillRect( terrainSurface, &dst, 0 );
		for ( int i = (MAPSIZE-1) - speed -1; i < MAPSIZE-1 ; ++i )
		{
			Sint16 px[4];
			Sint16 py[5];
			int left = ((i-1)*segSize);
			int right = ((i)*segSize);
			// printf( "left = %d, right = %d\n", left, right );

			px[0] = left;
			py[0] = mapTop[i];
			px[1] = right;
			py[1] = mapTop[i+1];
			px[2] = right;
			py[2] = 0;
			px[3] = left;
			py[3] = 0;

			if ( drawTop )
			{
				// Only display top landscape if not running FLY_GAME
				filledPolygonRGBA( terrainSurface, px, py, 4, 0, 190, 0, 255 );
			}

			if ( drawBottom )
			{
				py[0] = mapBottom[i];
				py[1] = mapBottom[i+1];
				py[2] = sHeight;
				py[3] = sHeight;
				filledPolygonRGBA( terrainSurface, px, py, 4, 0, 190, 0, 255 );
			}
		}

		offset -= speed*segSize;

	}

}

void Terrain :: setPoint( int point )
{
	if ( nextInt( 100 ) >= 80 )
		dir *= -1;

	mapTop[point] = mapTop[point-1] + (dir * nextInt( 5 ));
	if ( mapTop[point] < 0 )
	{
		mapTop[point] = 0;
		dir *= -1;
	}
	else if ( mapTop[point] >= maxHeight )
	{
		mapTop[point] = maxHeight;
		dir *= -1;
	}

	mapBottom[point] = sHeight - (maxHeight - mapTop[point]);
}

void Terrain :: increaseMaxHeight( int amount )
{
	maxHeight += amount;
}

void Terrain :: drawTerrain( SDL_Surface *screen )
{
	// Blit terrain surface onto screen

	SDL_Rect dst;
	dst.x = offset;
	dst.y = 0;
	dst.w = sWidth;
	dst.h = sHeight;
//	dst.h = maxHeight;

	SDL_Rect dst2;
	dst2.x = 0;
	dst2.y = 0;

    SDL_BlitSurface(terrainSurface, &dst, screen, &dst2 );

    stars->draw( screen );

//	dst.y = sHeight - maxHeight;
//	dst2.y = sHeight - maxHeight;
//	SDL_BlitSurface(terrainSurface, &dst, screen, &dst2 );

/*
	for ( int i = 0 ; i < MAPSIZE ; ++i )
	{
		int x1 = (i*segSize) - (offset*speed);
		int x2 = ((i+1)*segSize)-(offset*speed);
		if ( x2 >= sWidth )
			x2 = sWidth-1;
		aalineRGBA( screen, x1, mapTop[i], x2, mapTop[i+1], 0, 220, 0, 255 );
		aalineRGBA( screen, x1, mapBottom[i], x2, mapBottom[i+1], 0, 220, 0, 255 );
	}
*/
}

bool Terrain :: checkCollision( int x, int y, int h )
{
	if ( y < 0 || y > sHeight )
		return true;
	// First get segment that matches x
	SDL_LockSurface( terrainSurface );

	Uint32 c = getpixel( terrainSurface, x, y );

	SDL_UnlockSurface( terrainSurface );

	if ( c == 0 )
		return false;
	else
		return true;
}


// Test
#ifdef DEBUG_TERRAIN
SDL_Surface *screen;
Terrain *terrain;

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

 	if ( (screen=SDL_SetVideoMode(280, 320,32,videoflags)) == NULL )
 	{
 		fprintf(stderr, "Couldn't set %ix%i video mode: %s\n",WIDTH,HEIGHT,SDL_GetError());
 		exit(2);
 	}

	terrain = new Terrain( 240, 320 );

	bool done = false;
	while ( !done )
	{
		SDL_FillRect( screen, 0, 0 );
		terrain->drawTerrain( screen );
		terrain->moveTerrain( 5 );

 		SDL_Flip( screen );

		SDL_Delay( 5 );

		SDL_Event event;
		while ( SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					// Escape keypress quits the app
					if ( event.key.keysym.sym != SDLK_ESCAPE )
					{
						terrain->moveTerrain( 5 );
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

