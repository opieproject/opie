#ifndef __FLYTERRAIN_H
#define __FLYTERRAIN_H

#include <SDL/SDL.h>

#include "terrain.h"

class FlyTerrain : public Terrain
{
public:
	FlyTerrain( int w, int h );
	~FlyTerrain();

	void drawTerrain( SDL_Surface *screen );
	int getScore( int difficulty, int dist );

	void displayScoreZones( bool val ) { showScoreZones = val; }

protected:
	bool showScoreZones;

	static int flyScoreZones[][3];

	void setPoint( int point );
};


#endif

