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
	void setMovementInfo( double up, double grav, double maxUp, double maxDown );
	void incValue( int valType );
	void decValue( int valType );
	double getValue( int valueType );
    string getValueString( int valueType );
    string getValueTypeString( int valueType );
    void setValue( int valueType, double val );	

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
	double currentThrust;

	double thrust;
	double gravity;
	double maxUpSpeed;
	double maxDownSpeed;

	Rect trail[TRAILSIZE];

};

#endif
