#include "SDL.h"
#include "constants.h"

#include "font.h"
#include "help.h"
#include "sfcave.h"
#include "starfield.h"

Help :: Help( SFCave *p )
{
    parent = p;
   	stars = new StarField( false, 200 );

    loadText();

    init();
}

Help :: ~Help()
{
    delete stars;
}

void Help :: handleKeys( SDL_KeyboardEvent &key )
{
	if ( key.type == SDL_KEYDOWN )
	{
        if ( key.keysym.sym == SDLK_SPACE )
            parent->changeState( STATE_MENU );
        else if ( key.keysym.sym == SDLK_DOWN )
            textSpeed = 5;
        else if ( key.keysym.sym == SDLK_UP )
        {
            if ( textSpeed > 0 )
                textSpeed = 0;
            else textSpeed = 1;
        }
            
	}
	else if ( key.type == SDL_KEYUP )
	{
        if ( key.keysym.sym == SDLK_DOWN )
            textSpeed = 1;
    }
}
void Help :: init()
{
	startPos = 320;
	currLine = 0;
	textSpeed = 1;
	
	// Create our coloured font
	FontHandler :: changeColor( FONT_HELP_FONT, 0, 0, 255 );
}

void Help :: draw( SDL_Surface *screen )
{
    stars->draw( screen );

    
	list<string>::iterator it = textList.begin();

	// Move to start of text
	for ( int i = 0 ; i < currLine && it != textList.end() ; ++i )
		it++;

	int pos = startPos;
	while ( pos < 320 && it != textList.end() )
	{
		// get next line
		string text = *it;

		// draw text
		FontHandler::draw( screen, FONT_COLOURED_TEXT, text.c_str(), -1, pos );
		pos += FontHandler::FontHeight( FONT_COLOURED_TEXT );
		it ++;
	}
	
}

void Help :: update()
{
    stars->move();
    
	startPos -= textSpeed;
	if ( startPos <= -FontHandler::FontHeight( FONT_COLOURED_TEXT ) )
	{
		startPos = 0;
		currLine ++;

		if ( currLine > textList.size() )
		{
			startPos = 320;
			currLine = 0;
		}
	}

}

void Help :: loadText()
{
	textList.push_back( "SFCave" );
	textList.push_back( "Written By AndyQ" );
	textList.push_back( "" );
	textList.push_back( "Instructions" );
	textList.push_back( "To return to the menu" );
	textList.push_back( "press the space or " );
	textList.push_back( "middle button." );
	textList.push_back( "" );
	textList.push_back( "To speed up the text" );
	textList.push_back( "hold the down button" );
	textList.push_back( "(releasing will return" );
	textList.push_back( "to normal speed)" );
	textList.push_back( "" );
	textList.push_back( "" );
	textList.push_back( "SFCave is a flying game" );
	textList.push_back( "writtin originally for the" );
	textList.push_back( "Sharp Zaurus." );
	textList.push_back( "" );
	textList.push_back( "The aim is to stay alive" );
	textList.push_back( "for as long as possible," );
	textList.push_back( "and get the highest score" );
	textList.push_back( "you can." );
	textList.push_back( "" );
	textList.push_back( "There are currently three" );
	textList.push_back( "game types - SFCave," );
	textList.push_back( "Gates, and Fly." );
	textList.push_back( "" );
	textList.push_back( "SFCave is a remake of" );
	textList.push_back( "the classic SFCave game." );
	textList.push_back( "Fly through the cavern" );
	textList.push_back( "avoiding all the blocks" );
	textList.push_back( "that just happen to be" );
	textList.push_back( "hanging in mid-air" );
	textList.push_back( "" );
	textList.push_back( "Gates is similar to" );
	textList.push_back( "SFCave but instead of" );
	textList.push_back( "avoiding blocks you must" );
	textList.push_back( "fly through gates without" );
	textList.push_back( "crashing." );
	textList.push_back( "" );
	textList.push_back( "Fly is a different kettle of" );
	textList.push_back( "fish altogether. Instead," );
	textList.push_back( "you are flying in the " );
	textList.push_back( "open air above a" );
	textList.push_back( "scrolling landscape and" );
	textList.push_back( "the aim is to fly as close" );
	textList.push_back( "to the land as possible." );
	textList.push_back( "The closer to the land" );
	textList.push_back( "you fly the more points" );
	textList.push_back( "you score. But beware," );
	textList.push_back( "fly too high above the" );
	textList.push_back( "land and points get" );
	textList.push_back( "deducted." );
	textList.push_back( "" );
	textList.push_back( "How to play" );
	textList.push_back( "Press the space or middle" );
	textList.push_back( "button (Zaurus only) to " );
	textList.push_back( "apply thrust (makes you" );
	textList.push_back( "go up) and release it" );
	textList.push_back( "to go down." );
	textList.push_back( "" );
	textList.push_back( "Have fun" );
	textList.push_back( "AndyQ" );
}

// Test
#ifdef DEBUG_HELP
SDL_Surface *screen;
Help *help;

void go()
{
	FontHandler :: init();

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	int videoflags = SDL_SWSURFACE ;

 	if ( (screen=SDL_SetVideoMode(240, 320,32,videoflags)) == NULL )
 	{
 		fprintf(stderr, "Couldn't set %ix%i video mode: %s\n",240,320,SDL_GetError());
 		exit(2);
 	}

	help = new Help();

	bool done = false;
	while ( !done )
	{
		SDL_FillRect( screen, 0, 0 );
		help->draw( screen );
		help->update( );

 		SDL_Flip( screen );

		SDL_Delay( 10 );

		SDL_Event event;
		while ( SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					// Escape keypress quits the app
					if ( event.key.keysym.sym != SDLK_ESCAPE )
					{
						break;
					}
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
 	}
 }




#ifdef __cplusplus
extern "C"
#endif
int main( int argc, char *argv[] )
{
	go();
}

#endif
