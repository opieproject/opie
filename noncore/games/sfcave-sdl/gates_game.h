#ifndef __GATES_GAME_H
#define __GATES_GAME_H

#include "SDL.h"

#include "rect.h"

#include "sfcave.h"
#include "terrain.h"
#include "player.h"
#include "game.h"

class GatesGame : public Game
{
public:
	GatesGame( SFCave *p, int w, int h, int diff );
	~GatesGame();

	void init();
	void update( int state );
	void draw( SDL_Surface *screen );

private:

    int gapHeight;

    int gateDistance;
    int nextGate;
    int lastGateBottomY;

    int blockDistance;
	int blockHeight;
	int blockWidth;
	int blockUpdateRate;

	Rect blocks[BLOCKSIZE];

	void addGate();
	void moveBlocks( int amountToMove );
	void drawBlocks( SDL_Surface *screen );
	bool checkCollisions();

};

#endif
