/***********************************************************/
/*                                                         */
/*   BFONT.c v. 1.0.2 - Billi Font Library by Diego Billi  */
/*   BFONT++ C++ port by Gianluigi Davassi                 */
/***********************************************************/
#include "iostream"
using namespace std;
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"

#include "SDL_image.h"
#include "bfont.h"

void BFont::InitFont()
{
    int x = 0, i = '!';
    Uint32 sentry = GetPixel(0,0);

    if (SDL_MUSTLOCK(Surface))
        SDL_LockSurface(Surface);

    while ( x < (Surface->w-1))
    {
        if(GetPixel(x,0) != sentry)
        {
            Chars[i].x = x;
            Chars[i].y = 1;
            Chars[i].h = Surface->h;
            for (; GetPixel(x, 0) != sentry && x < (Surface->w); ++x) ;
            Chars[i].w = (x - Chars[i].x);
            i++;
        } else
        { x++; }
    }
    Chars[' '].x = 0;
    Chars[' '].y = 0;
    Chars[' '].h = Surface->h;
    Chars[' '].w = Chars['!'].w;

    if (SDL_MUSTLOCK(Surface))
        SDL_UnlockSurface(Surface);

    h = Surface->h;

    SDL_SetColorKey(Surface, SDL_SRCCOLORKEY, GetPixel(0, Surface->h-1));
}


/* Load the font and stores it in the BFont_Info structure */
void BFont::LoadFont (const char *filename)
{
    SDL_Surface *surface(NULL);
    int x;
    // tutta roba inutile in C++.... :-)
    /*    BFont_Info *Font=NULL;
     Font = (BFont_Info *) malloc(sizeof(BFont_Info));*/

    if ((filename != NULL ) && (this != NULL))
    {
		surface = IMG_Load( filename );

        if (surface != NULL)
        {
            Surface = surface;
            for (x=0; x<256; x++)
            {
                Chars[x].x = 0;
                Chars[x].y = 0;
                Chars[x].h = 0;
                Chars[x].w = 0;
            }
            InitFont(); // Init the font
        }
    }
}

BFont * BFont :: SetFontColor(Uint8 r, Uint8 g, Uint8 b)
{
    int x,y;

    BFont *newfont;
    SDL_Surface *surface = NULL;

    Uint32 pixel;
    Uint8 old_r, old_g, old_b;
    Uint8 new_r, new_g, new_b;
    Uint32 color_key;

	newfont = new BFont(NULL);
	
	if (newfont != NULL) {

		newfont->h = h;

		for (x=0; x<256; x++) {
			newfont->Chars[x].x = Chars[x].x;
			newfont->Chars[x].y = Chars[x].y;
			newfont->Chars[x].h = Chars[x].h;
			newfont->Chars[x].w = Chars[x].w;
		}

		surface = SDL_CreateRGBSurface(SDL_SWSURFACE, Surface->w, Surface->h, 32,
									0x000000ff,0x0000ff00, 0x00ff0000, 0xff000000);
		if (surface != NULL) {

			if (SDL_MUSTLOCK(surface))       SDL_LockSurface(surface);
			if (SDL_MUSTLOCK(Surface)) SDL_LockSurface(Surface);

			color_key =  xGetPixel(Surface, 0, Surface->h-1);

			for( x=0; x < Surface->w; x++) {
				for( y=0; y < Surface->h; y++) {
					old_r = old_g = old_b = 0;
					pixel = xGetPixel(Surface,x,y);

					if (pixel != color_key) {
						SDL_GetRGB(pixel, Surface->format, &old_r,&old_g,&old_b);

						new_r = (Uint8) ((old_r * r) / 255);
						new_g = (Uint8) ((old_g * g) / 255);
						new_b = (Uint8) ((old_b * b) / 255);

						pixel = SDL_MapRGB(surface->format,new_r,new_g,new_b);
					}
					PutPixel(surface,x,y,pixel);
				}
			}

			if (SDL_MUSTLOCK(surface))       SDL_UnlockSurface(surface);
			if (SDL_MUSTLOCK(Surface)) SDL_UnlockSurface(Surface);

			SDL_SetColorKey(surface, SDL_SRCCOLORKEY, color_key);
		}

		newfont->Surface = surface;
	}
    return newfont;
}


/* Puts a single char on the surface with the specified font */
int BFont::PutChar(SDL_Surface *screen, int x, int y, char c)
{
    int r=0;
    SDL_Rect dest;

    dest.w = CharWidth(' ');
    dest.h = FontHeight();
    dest.x = x;
    dest.y = y;

    if (c != ' ')
        SDL_BlitSurface( Surface, &Chars[c], screen, &dest);

    r = dest.w;
    return r;
}


void BFont::PutString(SDL_Surface *screen, int x, int y, const char *text)
{
    int i(0);
    while (text[i]!='\0')
    {
        x  += PutChar(screen,x,y,text[i]);
        i++;
    }
}

int BFont::TextWidth(const char *text)
{
    int i(0),x(0);

    while (text[i]!='\0')
    {
        x += CharWidth(text[i]);
        i++;
    }
    return x;
}


/* counts the spaces of the strings */
int BFont::count (const char *text)
{
    char *p(NULL);
    int pos(-1),i(0);

    /* Calculate the space occupied by the text without spaces */
    while ((p=strchr(&text[pos+1],' ')) != NULL)
    {
        i++;
        pos = p - text;
    }
    return i;
}


void BFont::JustifiedPutString( SDL_Surface *screen, int y, const char *text)
{
    int spaces(0),gap,single_gap,dif;
    char *strtmp,*p;
    int pos(-1),xpos(0);


    if (strchr(text,' ') == NULL)
    {
        PutString(screen, 0, y, text);
    }
    else {
        gap = (screen->w-1) - TextWidth(text);

        if (gap <= 0) {
            PutString(screen, 0,y,text);
        } else {
            spaces = count(text);
            dif = gap % spaces;
            single_gap = (gap - dif) / spaces;
            xpos=0;
            pos = -1;
            while ( spaces > 0 )
            {
                p = strstr(&text[pos+1]," ");
                strtmp = NULL;
                strtmp = (char *) calloc ( (p - &text[pos+1]) + 1,sizeof(char));
                if (strtmp != NULL)
                {
                    strncpy (strtmp, &text[pos+1], (p - &text[pos+1]));
                    PutString(screen, xpos, y, strtmp);
                    xpos = xpos + TextWidth(strtmp) + single_gap + CharWidth(' ');
                    if (dif >= 0)
                    {
                        xpos ++;
                        dif--;
                    }
                    pos = p - text;
                    spaces--;
                    free(strtmp);
                }
            }
            strtmp = NULL;
            strtmp = (char *) calloc ( strlen( &text[pos+1]) + 1,sizeof(char));

            if (strtmp != NULL) {
                strncpy (strtmp, &text[pos+1], strlen( &text[pos+1]));
                PutString(screen, xpos, y, strtmp);
                free(strtmp);
            }
        }
    }
}


void BFont::CenteredPutString(SDL_Surface *screen, int y, const char *text)
{
    printf( "xpos - %d, %d <%s>\n", screen->w/2-TextWidth(text)/2, TextWidth(text), text );
    PutString( screen, screen->w/2-TextWidth(text)/2, y, text);
}


void BFont::RightPutString(SDL_Surface *screen, int y, const char *text)
{
    PutString( screen, screen->w - TextWidth(text) - 1, y, text);
}

void BFont::LeftPutString(SDL_Surface *screen, int y, const char *text)
{
    PutString( screen, 0, y, text);
}

/******/

void BFont::PrintString (SDL_Surface *screen, int x, int y, char *fmt, ...)
{
    va_list args;
    char *temp;
    va_start (args,fmt);

    if ( (temp = (char *) malloc(1000+1)) != NULL) {
        vsprintf(temp,fmt,args);
        PutString(screen, x, y, temp);
        free (temp);
    }
    va_end(args);
}

void BFont::CenteredPrintString(SDL_Surface *screen, int y,  char *fmt, ...)
{
    va_list args;
    char *temp;
    va_start (args,fmt);

    if ( (temp = (char *) malloc(1000+1)) != NULL) {
        vsprintf(temp,fmt,args);
        CenteredPutString(screen, y, temp);
        free (temp);
    }
    va_end(args);
}

void BFont::RightPrintString(SDL_Surface *screen, int y, char *fmt, ...)
{
    va_list args;
    char *temp;
    va_start (args,fmt);

    if ( (temp = (char *) malloc(1000+1)) != NULL) {
        vsprintf(temp,fmt,args);
        RightPutString(screen, y, temp);
        free (temp);
    }
    va_end(args);
}

void BFont::LeftPrintString( SDL_Surface *screen, int y, char *fmt, ...)
{
    va_list args;
    char *temp;
    va_start (args,fmt);

    if ( (temp = (char *) malloc(1000+1)) != NULL) {
        vsprintf(temp,fmt,args);
        LeftPutString(screen, y, temp);
        free (temp);
    }
    va_end(args);
}

void BFont::JustifiedPrintString( SDL_Surface *screen, int y, char *fmt, ...)
{
    va_list args;
    char *temp;
    va_start (args,fmt);

    if ( (temp = (char *) malloc(1000+1)) != NULL) {
        vsprintf(temp,fmt,args);
        JustifiedPutString( screen, y,temp);
        free (temp);
    }
    va_end(args);
}


void BFont::PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

Uint32 BFont :: xGetPixel(SDL_Surface *surface, int x, int y)
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

Uint32 BFont::GetPixel( Sint32 X, Sint32 Y)
{

   Uint8  *bits;
   Uint32 Bpp;

   if (X<0) puts("x too small in GetPixel!");
   if (X>=Surface->w) puts("x too big in GetPixel!");

   Bpp = Surface->format->BytesPerPixel;

   bits = ((Uint8 *)Surface->pixels)+Y*Surface->pitch+X*Bpp;

   // Get the pixel
   switch(Bpp) {
      case 1:
         return *((Uint8 *)Surface->pixels + Y * Surface->pitch + X);
         break;
      case 2:
         return *((Uint16 *)Surface->pixels + Y * Surface->pitch/2 + X);
         break;
      case 3: { // Format/endian independent
         Uint8 r, g, b;
         r = *((bits)+Surface->format->Rshift/8);
         g = *((bits)+Surface->format->Gshift/8);
         b = *((bits)+Surface->format->Bshift/8);
         return SDL_MapRGB(Surface->format, r, g, b);
         }
         break;
      case 4:
         return *((Uint32 *)Surface->pixels + Y * Surface->pitch/4 + X);
         break;
   }
}
