#ifndef __SFCAVE_H
#define __SFCAVE_H

#include <SDL/SDL.h>

#include "terrain.h"

class Game;
class Menu;
class Help;

class SFCave
{
public:
	SFCave( int argc, char *argv[] );
	~SFCave();

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
	void saveSetting( string key, long val );
	void saveSetting( string key, double val );
	string loadSetting( string key, string defaultVal = "" );
	bool loadBoolSetting( string key, bool defaultVal);
	int loadIntSetting( string key, int defaultVal );
	double loadDoubleSetting( string key, double defaultVal );

private:
	SDL_Surface *screen;
	bool setupOK;

	Game *currentGame;
	Menu *menu;
	Help *help;
	int state;
	bool showFps;
	string musicPath;
	string musicType;
	bool finish;

	bool limitFPS;
	int maxFPS;
	int actualFPS;
	int FPS;
	long time1;
	long start;
	long end;
	
	// This is used when the user is setting the custom
	// values in the menu
	int customPlayerMenuVal;
	double origValue;

	void handleMenuSelect( int menuId );
	void handleGameState();
	void handleEvents();
	void calcFPS();
	void FPSDelay();
};

#endif
