#include <SDL_image.h>
#include "SDL_rotozoom.h"

#include "constants.h"
#include "sfcave.h"
#include "game.h"
#include "menu.h"
#include "font.h"
#include "starfield.h"

MenuOption :: MenuOption( string text, int id )
{
	menuText = text;
	menuId = id;
	nextMenu = 0;
	highlighted = false;
}

MenuOption :: ~MenuOption()
{
}


int MenuOption :: draw( SDL_Surface *screen, int y )
{
	if ( highlighted )
	{
		int x = (240 - FontHandler::TextWidth( FONT_MENU_HIGHLIGHTED, (const char *)menuText.c_str() ))/2;
		FontHandler::draw( screen, FONT_MENU_HIGHLIGHTED, (const char *)menuText.c_str(), x, y );
		return FontHandler::FontHeight( FONT_MENU_HIGHLIGHTED );
	}
	else
	{
		int x = (240 - FontHandler::TextWidth( FONT_MENU_UNHIGHLIGHTED, (const char *)menuText.c_str() ))/2;
		FontHandler::draw( screen, FONT_MENU_UNHIGHLIGHTED, (const char *)menuText.c_str(), x, y );
		return FontHandler::FontHeight( FONT_MENU_UNHIGHLIGHTED );
	}
}

void MenuOption :: setNextMenu( Menu *item, bool down )
{
	nextMenu = item;
	downMenuTree = down;
}



//----------------- Menu Class -------------

SDL_Surface * Menu :: sfcaveTextImage;
Menu *Menu :: mainMenu;
Menu *Menu :: currentMenu;

// This is the Master Menu constructor
Menu :: Menu( SFCave *p )
{
	parent = p;
	parentMenu = 0;
	statusText = "";

//	listItems.setAutoDelete( TRUE );

	SDL_Surface *tmp = IMG_Load( IMAGES_PATH "sfcave_text.bmp" );
	sfcaveTextImage = SDL_CreateRGBSurface(SDL_SWSURFACE, tmp->w, tmp->h, 32,
	                                       0x000000ff,0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_BlitSurface(tmp, NULL, sfcaveTextImage, NULL);
	SDL_FreeSurface(tmp);

	// Create menu structure
	// Top level menu has 5 items - Start Game, Replays, Options, Help, and Quit
	// Replays, Option menu items hav submenus
	MenuOption *replayMenu = 0;
	MenuOption *optionsMenu = 0;
	MenuOption *item = 0;
	addMenuOption( "Start Game", MENU_STARTGAME );
	replayMenu = addMenuOption( "Replays", MENU_REPLAYS );
	optionsMenu = addMenuOption( "Options", MENU_OPTIONS );
	addMenuOption( "Help", MENU_HELP );
	addMenuOption( "Quit", MENU_QUIT );

	// Now deal with the Replays Menu
	Menu *replay = new Menu( this );
	replay->addMenuOption( "Play Replay", MENU_PLAY_REPLAY );
	replay->addMenuOption( "Load Replay", MENU_LOAD_REPLAY );
	replay->addMenuOption( "Save Replay", MENU_SAVE_REPLAY );
	item = replay->addMenuOption( "Back", MENU_BACK );
	item->setNextMenu( this, false );
	replayMenu->setNextMenu( replay );

	// Now deal with the Options Menu	currentMenu->currentMenuOption->setHighlighted( false );
	listItems.front()->highlight( true );

	Menu *options = new Menu( this );
	MenuOption *typeMenu = 0;
	MenuOption *difficultyMenu = 0;
	MenuOption *soundsMenu = 0;
	typeMenu = options->addMenuOption( "Game Type", MENU_GAME_TYPE );
	difficultyMenu = options->addMenuOption( "Difficulty", MENU_DIFFICULTY );
	soundsMenu = options->addMenuOption( "Sound", MENU_SOUNDS );
	options->addMenuOption( "Clear Scores", MENU_CLEAR_SCORES );
	item = options->addMenuOption( "Back", MENU_BACK );
	item->setNextMenu( this, false );
	optionsMenu->setNextMenu( options );

	// Game Type menu
	Menu *gameType = new Menu( options );
	item = gameType->addMenuOption( "SFCave", MENU_GAME_SFCAVE );
	item->setNextMenu( options );
	item = gameType->addMenuOption( "Gates", MENU_GAME_GATES );
	item->setNextMenu( options );
	item = gameType->addMenuOption( "Fly", MENU_GAME_FLY );
	item->setNextMenu( options );
	item = gameType->addMenuOption( "Back", MENU_BACK );
	item->setNextMenu( options );
	typeMenu->setNextMenu( gameType );

	// Game Difficulty menu
	MenuOption *customMenu = 0;
	Menu *gameDifficulty = new Menu( options );
	item = gameDifficulty->addMenuOption( "Easy", MENU_DIFFICULTY_EASY );
	item->setNextMenu( options, false );
	item = gameDifficulty->addMenuOption( "Normal", MENU_DIFFICULTY_NORMAL );
	item->setNextMenu( options, false );
	item = gameDifficulty->addMenuOption( "Hard", MENU_DIFFICULTY_HARD );
	item->setNextMenu( options, false );
	customMenu = gameDifficulty->addMenuOption( "Custom", MENU_DIFFICULTY_CUSTOM );
	item = gameDifficulty->addMenuOption( "Back", MENU_BACK );
	item->setNextMenu( options, false );
	difficultyMenu->setNextMenu( gameDifficulty );

	// Sounds Menu
	Menu *sounds = new Menu( options );
	sounds->addMenuOption( "Sound On", MENU_SOUND_ON );
	sounds->addMenuOption( "Sound Off", MENU_SOUND_OFF );
	sounds->addMenuOption( "Music On", MENU_MUSIC_ON );
	sounds->addMenuOption( "Music Off", MENU_MUSIC_OFF );
	item = sounds->addMenuOption( "Back", MENU_BACK );
	item->setNextMenu( options, false );
	soundsMenu->setNextMenu( sounds );
	
	// Custom Menu
	Menu *custom = new Menu( gameDifficulty );
	Menu *updown = new Menu( custom );
	item = custom->addMenuOption( "Thrust", MENU_CUSTOM_THRUST );
	item->setNextMenu( updown );
	item = custom->addMenuOption( "Gravity", MENU_CUSTOM_GRAVITY );
	item->setNextMenu( updown );
	item = custom->addMenuOption( "Max Speed Up", MENU_CUSTOM_MAXSPEEDUP );
	item->setNextMenu( updown );
	item = custom->addMenuOption( "Max Speed Down", MENU_CUSTOM_MAXSPEEDDOWN );
	item->setNextMenu( updown );
	item = custom->addMenuOption( "Back", MENU_BACK );
	item->setNextMenu( gameDifficulty, false );
	customMenu->setNextMenu( custom );

    // Up down menu
	item = updown->addMenuOption( "Increase", MENU_CUSTOM_INCREASE );
	item = updown->addMenuOption( "Decrease", MENU_CUSTOM_DECREASE );
	item = updown->addMenuOption( "Save", MENU_CUSTOM_SAVE );
	item->setNextMenu( custom, false );
	item = updown->addMenuOption( "Cancel", MENU_CUSTOM_CANCEL );
	item->setNextMenu( custom, false );

	// Set static variables for menu selection up
	mainMenu = this;
	currentMenuOption = 0;

	resetToTopMenu();

	angle = 0;

	stars = new StarField;
}

// This is a private constructor used for creating sub menus - only called by the Master Menu
Menu :: Menu( Menu *p )
{
	parentMenu = p;
//	listItems.setAutoDelete( TRUE );
	currentMenuOption = 0;
}

Menu :: ~Menu()
{
	if ( this == mainMenu )
	{
		SDL_FreeSurface( sfcaveTextImage );
		delete stars;
	}
}

void Menu :: draw( SDL_Surface *screen )
{
	// draw stafield
	stars->draw( screen );
	stars->move( );

	// Draw the spinning SFCave logo
	SDL_Surface *rotozoom_pic;
	SDL_Rect dest;

	angle += 2;
	if ( angle > 359 )
		angle = 0;
	if ((rotozoom_pic=rotozoomSurface (sfcaveTextImage, angle*1, 1, SMOOTHING_ON))!=NULL)
	{
		dest.x = (screen->w - rotozoom_pic->w)/2;
		dest.y = 10;
		dest.w = rotozoom_pic->w;
		dest.h = rotozoom_pic->h;
		SDL_BlitSurface( rotozoom_pic, NULL, screen, &dest );
		SDL_FreeSurface(rotozoom_pic);
	}

	// Draw what game is selected
	char text[100];
	sprintf( text, "Current Game: %s", (const char *)parent->getCurrentGame()->getGameName().c_str() );
//	Menu::scoreFont.PutString( screen, (240 - Menu::scoreFont.TextWidth( (const char *)text ))/2, 120, (const char *)text );
	FontHandler::draw( screen, FONT_WHITE_TEXT, (const char *)text, (240 - FontHandler::TextWidth( FONT_WHITE_TEXT,(const char *)text ))/2, 120 );
	sprintf( text, "Difficulty: %s", (const char *)parent->getCurrentGame()->getGameDifficultyText().c_str() );
//	Menu::scoreFont.PutString( screen, (240 - Menu::scoreFont.TextWidth( (const char *)text ))/2, 120 + Menu::scoreFont.FontHeight()+2, (const char *)text );
	FontHandler::draw( screen, FONT_WHITE_TEXT, (const char *)text, (240 - FontHandler::TextWidth( FONT_WHITE_TEXT,(const char *)text ))/2, 120 + FontHandler::FontHeight( FONT_WHITE_TEXT ) );

	if ( statusText != "" )
		FontHandler::draw( screen, FONT_WHITE_TEXT, (const char *)statusText.c_str(), (240 - FontHandler::TextWidth( FONT_WHITE_TEXT,(const char *)statusText.c_str() ))/2, 120 + (2*FontHandler::FontHeight( FONT_WHITE_TEXT ))+6 );
//		Menu::scoreFont.PutString( screen, (240 - Menu::scoreFont.TextWidth( (const char *)statusText ))/2, 120 + (Menu::scoreFont.FontHeight()*2)+6, (const char *)statusText );


	// Loop round each menu option and draw it
	int y = 155;
	list<MenuOption *>::iterator it;
	for ( it = currentMenu->listItems.begin(); it != currentMenu->listItems.end() ; ++it )
	{
		y += (*it)->draw( screen, y ) + 2;
	}
}

int Menu :: handleKeys( SDL_KeyboardEvent &key )
{
	if ( key.type != SDL_KEYDOWN )
		return -1;

	switch( key.keysym.sym )
	{
		case SDLK_DOWN:
		{
			// Move to next menu item
			currentMenu->currentMenuOption->highlight( false );

			list<MenuOption *>::iterator it;
			for ( it = currentMenu->listItems.begin(); it != currentMenu->listItems.end() ; ++it )
			{
				if ( (*it) == currentMenu->currentMenuOption )
				{
					it++;
					break;
				}
			}

			if ( it == currentMenu->listItems.end() )
				it = currentMenu->listItems.begin();

			currentMenu->currentMenuOption = *it;
			currentMenu->currentMenuOption->highlight( true );

			break;
		}
		case SDLK_UP:
		{
			// Move to previous menu item
			currentMenu->currentMenuOption->highlight( false );
			list<MenuOption *>::iterator it;
			bool reachedBeginning = false;
			for ( it = (currentMenu->listItems).end()--;  ; --it )
			{
				if ( (*it) == currentMenu->currentMenuOption )
				{

					if ( it == currentMenu->listItems.begin( ) )
					{
						reachedBeginning = true;
						break;
					}
					else
						it--;
					break;
				}

			}

			if ( reachedBeginning )
				currentMenu->currentMenuOption = currentMenu->listItems.back();
			else
				currentMenu->currentMenuOption = *it;

			currentMenu->currentMenuOption->highlight( true );

			break;
		}
		case SDLK_LEFT:
			if ( currentMenu->parentMenu != 0 )
			{
            	statusText = "";
				currentMenu = currentMenu->parentMenu;

				return -1;
			}
			break;

		case SDLK_RETURN:
		case SDLK_SPACE:
		{
        	statusText = "";
			// select menu item
			int id = currentMenu->currentMenuOption->getMenuId();

			// if the current item has a child menu then move to that menu
			Menu *next = currentMenu->currentMenuOption->getNextMenu();
			if ( next != 0 )
			{
				bool down = currentMenu->currentMenuOption->isDownMenuTree();
				currentMenu = next;
				if ( down )
					initCurrentMenu();
			}

			return id;

			break;
		}

		default:
			break;
	}

	return -1;
}

MenuOption *Menu :: addMenuOption( string text, int id )
{
	MenuOption *item = new MenuOption( text, id );

	listItems.push_back( item );

	return item;
}

void Menu :: resetToTopMenu()
{
	currentMenu = mainMenu;
	initCurrentMenu();
}

void Menu :: initCurrentMenu()
{
	if ( currentMenu->currentMenuOption != 0 )
		currentMenu->currentMenuOption->highlight( false );
	currentMenu->currentMenuOption = currentMenu->listItems.front();
	currentMenu->currentMenuOption->highlight( true );
}
