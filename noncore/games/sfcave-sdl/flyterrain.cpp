#include <SDL/SDL_gfxPrimitives.h>

#include "constants.h"
#include "flyterrain.h"
#include "random.h"


int FlyTerrain :: flyScoreZones[][3] = { 	{ 0, 20, 5 },
						{ 20, 30, 2 },
						{ 30, 40, 0 },
						{ 40, 100, -1 },
						{ 100, 300, -2 },
						{ -1, -1, -1 } };

FlyTerrain :: FlyTerrain( int w, int h )
		: Terrain( w, h, false, true )
{
	showScoreZones = true;
}

FlyTerrain :: ~FlyTerrain()
{
}

void FlyTerrain :: setPoint( int point )
{
    static int fly_difficulty_levels[] = { 5, 10, 15 };
    if ( nextInt(100) >= 75 )
        dir *= -1;

    int prevPoint = mapBottom[point-1];
    
    int nextPoint = prevPoint + (dir * nextInt( fly_difficulty_levels[0] ) );

    if ( nextPoint > sHeight )
    {
        nextPoint = sHeight;
        dir *= -1;
    }
    else if ( nextPoint < maxHeight )
    {
        nextPoint = maxHeight;
        dir *= 1;
    }

    mapBottom[point] = nextPoint;
}

void FlyTerrain :: drawTerrain( SDL_Surface *screen )
{
	Terrain::drawTerrain( screen );
	int tmpOffset = offset + speed*segSize;
	
	for ( int i = 0 ; i < MAPSIZE -1; ++i )
	{

		if ( showScoreZones )
		{
			int r = 0;
			int g = 0;
			int b = 0;
			for ( int j = 1 ; flyScoreZones[j][0] != -1 ; ++j )
			{
				if ( flyScoreZones[j][2] == 0 )
				{
					g = 255;
					b = r = 0;
				}
				else if ( flyScoreZones[j][2] < 0 )
				{
					r = 255;
					b = g = 0;
				}
				else
				{
					b = 255;
					r = g = 0;
				}

				lineRGBA( screen, (i*segSize) - tmpOffset, mapBottom[i]-flyScoreZones[j][0], ((i+1)*segSize)-tmpOffset, mapBottom[i+1]-flyScoreZones[j][0], r, g, b, 255 );
				
			}

		}        
	}
}

int FlyTerrain :: getScore( int difficulty, int dist )
{
	int score = 0;
	for ( int i = 0 ; flyScoreZones[i][0] != -1 ; ++i )
	{
		if ( flyScoreZones[i][0] <= dist && flyScoreZones[i][1] > dist )
		{
			score = flyScoreZones[i][2];
			break;
		}
	}
	
	return score;
}

