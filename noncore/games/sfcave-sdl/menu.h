#ifndef __MENU_H
#define __MENU_H

#include <list>
using namespace std;

#include <SDL/SDL.h>

class SFCave;
class StarField;
class Menu;

class MenuOption
{
public:
	MenuOption( string text, int id );
	~MenuOption();

	void highlight( bool val ) { highlighted = val; }
	int draw( SDL_Surface *screen, int y );
	void setNextMenu( Menu *item, bool down = true );
	Menu *getNextMenu() { return nextMenu; }
	int getMenuId() { return menuId; }
	bool isDownMenuTree() { return downMenuTree; }

private:
	int menuId;
	string menuText;
	bool highlighted;
	bool downMenuTree;

	Menu *nextMenu;
};

class Menu
{
public:
	Menu( SFCave *p );
	~Menu();

	void draw( SDL_Surface *screen );
	int handleKeys( SDL_KeyboardEvent & );
	MenuOption *addMenuOption( string text, int id );
	void resetToTopMenu();
	void initCurrentMenu();

	void setStatusText( string text ) { statusText = text; }

protected:

private:
	static SDL_Surface * sfcaveTextImage;
	int angle;

	static Menu *mainMenu;
	static Menu *currentMenu;
	Menu *parentMenu;

	StarField *stars;

	string statusText;

	SFCave *parent;
	list<MenuOption *> listItems;
	MenuOption *currentMenuOption;

	Menu( Menu* p );
};


#endif
