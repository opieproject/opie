#ifndef __TERRAIN_H
#define __TERRAIN_H

#include <SDL.h>

class StarField;
class Terrain
{
public:
	Terrain( int w, int h, bool drawTop = true, bool drawBottom = true );
	virtual ~Terrain();

	virtual void initTerrain();
	virtual void moveTerrain( int amountToMove );
	virtual bool checkCollision( int x, int y, int h );
	virtual void drawTerrain( SDL_Surface *screen );

	int getMapTop( int pos ) { return mapTop[pos]; }
	int getMapBottom( int pos ) { return mapBottom[pos]; }
	int getMaxHeight() { return maxHeight; }
	void increaseMaxHeight( int amount );

	int offset;
protected:

	int sWidth;
	int sHeight;

	int drawTop;
	int drawBottom;

	int mapTop[MAPSIZE];
	int mapBottom[MAPSIZE];
	int maxTop;
	int maxBottom;

	int maxHeight;
	int dir;
	int speed;
	int segSize;

	SDL_Surface *terrainSurface;
	StarField *stars;

	void setPoint( int point );
};


#endif

