
/************************************************************

   BFONT v. 1.0.2 - Billi Font Library by Diego Billi
   BFONT++ C++ port by Gianluigi Davassi
************************************************************/

#ifndef __BFONT_HEADER_H__
#define __BFONT_HEADER_H__

#include <iostream>
#include "SDL.h"

class BFont
{
    int h;                  // font height
    SDL_Surface *Surface;   // font surface
    SDL_Rect Chars[256];    // characters width
    const char* name;       // font name

    BFont(const BFont&);

    void InitFont();
    int count(const char *text);
public:

    BFont(const char *__filename) // generator bill
        : name(__filename)
    { LoadFont(__filename); }

    ~BFont()
        { SDL_FreeSurface(Surface); } // screen must be free by application

    int FontHeight () // Returns the font height
    { return h; }

    void SetFontHeight (int height) // Change the font height
    { h = height ; }

    int CharWidth (char c) // Returns the character width of the specified font
    { return Chars[c].w; }

    void LoadFont      (const char *filename);     // Load and store le font in the BFont structure
    int PutChar        (SDL_Surface *screen, int x, int y, char c);     // Write a single character on the "Surface" with the current font
    int TextWidth      (const char *text );        // Returns the width, in pixels, of the text calculated with the current font

    BFont *SetFontColor(Uint8 r, Uint8 g, Uint8 b); // Returns a new font colored with the color (r,g,b)

    void PutString          ( SDL_Surface *screen, int x, int y, const char *text); // Write a string on the "Surface" with the specified font
    void LeftPutString      ( SDL_Surface *screen, int y, const char *text);  // Write a left-aligned string on the "Surface" with the specified font
    void CenteredPutString  ( SDL_Surface *screen, int y, const char *text);  // Write a center-aligned string on the "Surface" with the specified font
    void RightPutString     ( SDL_Surface *screen, int y, const char *text);  // Write a right-aligned string on the "Surface" with the specified font
    void JustifiedPutString ( SDL_Surface *screen, int y, const char *text);  // Write a justify-aligned string on the "Surface" with the specified font

    // The following functions do the same task but have the classic "printf" sintax
    void PrintString              ( SDL_Surface *screen, int x, int y, char *fmt, ...);
    void CenteredPrintString      ( SDL_Surface *screen, int y,  char *fmt, ...);
    void RightPrintString         ( SDL_Surface *screen, int y, char *fmt, ...);
    void LeftPrintString          ( SDL_Surface *screen, int y, char *fmt, ...);
    void JustifiedPrintString     ( SDL_Surface *screen, int y, char *fmt, ...);

private:
    Uint32 GetPixel( Sint32 X, Sint32 Y);
	Uint32 xGetPixel(SDL_Surface *surface, int x, int y);
    void   PutPixel( SDL_Surface *,int x, int y, Uint32 pixel);

};

#endif // __BFONT_HEADER_H__

