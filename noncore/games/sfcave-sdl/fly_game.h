#ifndef __FLY_GAME_H
#define __FLY_GAME_H

#include "sfcave.h"
#include "flyterrain.h"
#include "player.h"
#include "game.h"
#include <SDL/SDL.h>

class FlyGame : public Game
{
public:
	FlyGame( SFCave *p, int w, int h, int diff );
	~FlyGame();

	void init();
	void update( int state );
	void draw( SDL_Surface *screen );

private:

//	int movePlayer;
	bool startScoring;

	bool checkCollisions();
};

#endif
