#ifndef __PLAYER_H
#define __PLAYER_H

#include "rect.h"

class SDL_Surface;
class AnimatedImage;

class Player
{
public:
	Player( int w, int h );
	~Player();

	void init();
	void draw( SDL_Surface *screen );
	void drawTrails( SDL_Surface *screen );
	void move( bool up );
	void moveTrails();
	Rect getPos() { return pos; }
	int getX() { return pos.x(); }
	int getY() { return pos.y(); }
	int getHeight() { return pos.h(); }
	bool updateCrashing();
	void setMovementInfo( double up, double down, double maxUp, double maxDown );

private:
	AnimatedImage *explosion;

	int sWidth;
	int sHeight;

	bool expNextFrame;
	bool allFaded;
	bool crashing;
	bool crashed;
	int crashLineLength;
	Rect pos;
	double thrust;

	double thrustUp;
	double thrustDown;
	double maxUpSpeed;
	double maxDownSpeed;

	Rect trail[TRAILSIZE];

};

#endif
