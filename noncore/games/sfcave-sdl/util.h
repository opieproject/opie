#ifndef __UTIL_H
#define __UTIL_H

#include <string>
using namespace std;

Uint32 getpixel(SDL_Surface *surface, int x, int y);
string chooseRandomFile( string path, string fileType );
string getHomeDir();
#endif
