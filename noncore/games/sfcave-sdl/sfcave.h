#ifndef __SFCAVE_H
#define __SFCAVE_H

#include "SDL.h"

#include "terrain.h"

class Game;
class Menu;
class Help;

class SFCave
{
public:
	SFCave( int argc, char *argv[] );
	~SFCave();

	void drawGameScreen();
	void initSDL( int argc, char *argv[] );
	void mainEventLoop();

	void setCrashed( bool val );
	void changeState( int s );
	int getState() { return state; }
	Game *getCurrentGame() { return currentGame; }
	int getFPS() { return actualFPS; }
	bool showFPS() { return showFps; }

	void setMenuStatusText( string statusText );

	void saveSetting( string key, string val );
	void saveSetting( string key, int val );
	string loadSetting( string key, string defaultVal = "" );
private:
	SDL_Surface *screen;

	Game *currentGame;
	Menu *menu;
	Help *help;
	int state;
	int maxFPS;
	int actualFPS;
	bool showFps;
	string musicPath;
	string musicType;

	void handleMenuSelect( int menuId );
};

#endif
