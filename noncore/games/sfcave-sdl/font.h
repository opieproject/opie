#ifndef __FONT_H
#define __FONT_H

#include <SDL/SDL.h>
#include "bfont.h"

#define FONT_MENU_HIGHLIGHTED		1
#define FONT_MENU_UNHIGHLIGHTED		2
#define FONT_WHITE_TEXT				3
#define FONT_COLOURED_TEXT			4
#define FONT_HELP_FONT				5

class FontHandler
{
public:
	static bool init();
	static void cleanUp();

	static int TextWidth( int font, const char *text );
	static int FontHeight( int font );
	static void draw( SDL_Surface *screen, int font, const char *text, int x, int y );
	static void changeColor( int font, int r, int g, int b );

	static BFont *getFont( int font );
private:
	static BFont *menuSelFont;
	static BFont *menuUnSelFont;
	static BFont *whiteFont;
	static BFont *colouredFont;
	static BFont *helpFont;
};

#endif
