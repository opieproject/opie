#ifndef __SFCAVE_GAME_H
#define __SFCAVE_GAME_H

#include "SDL.h"

#include "rect.h"

#include "sfcave.h"
#include "terrain.h"
#include "player.h"
#include "game.h"

class SFCaveGame : public Game
{
public:
	SFCaveGame( SFCave *p, int w, int h, int diff );
	~SFCaveGame();

	void init();
	void update( int state );
	void draw( SDL_Surface *screen );

private:

	int blockDistance;
	int blockHeight;
	int blockWidth;
	int blockUpdateRate;

	Rect blocks[BLOCKSIZE];

	void addBlock();
	void moveBlocks( int amountToMove );
	void drawBlocks( SDL_Surface *screen );
	bool checkCollisions();

};

#endif
