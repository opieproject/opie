#include "SDL.h"

#include <dirent.h>

#include <vector>
using namespace std;

#include "util.h"
#include "random.h"

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

const char *chooseRandomFile( string path, string fileType )
{
    vector<string> files;
    DIR *d = opendir( path.c_str() );
    if ( !d )
        return "";

    struct dirent *item = readdir( d );
    while ( item )
    {
        string file = string( path ) + item->d_name;

        // Rip extension from file
        int pos = file.find( ".", 1 ) + 1;
        string tmp = file.substr( pos );
        printf( "pos = %d, tmp =%s\n", pos, tmp.c_str() );
        if ( tmp.size() > 0 && fileType.find( tmp ) != -1 )
        {
            printf( "Matching <%s> - %s with <%s>\n", file.substr( pos ).c_str(), file.c_str(), fileType.c_str() );
            files.push_back( file );
        }
        item = readdir( d );
    }

    closedir( d );
    return files[nextInt( files.size() )].c_str();
}
