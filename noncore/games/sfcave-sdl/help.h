
#ifndef __HELP_H
#define __help_h

#include <list>
#include <string>
using namespace std;

class SFCave;
class StarField;

class Help
{
public:
	Help( SFCave *p );
	~Help();

    void init();
	void handleKeys( SDL_KeyboardEvent &key );
	void draw( SDL_Surface *screen );
	void update();
private:

    SFCave *parent;
    StarField *stars;
    
    int textSpeed;
	list<string> textList;
	int startPos;
	int currLine;
	
	void loadText();
};

#endif
