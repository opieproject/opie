#include "SDL.h"
#include "SDL_image.h"

#include "constants.h"
#include "animatedimage.h"

AnimatedImage :: AnimatedImage( QString file, int nFrames )
{
	nrFrames = nFrames;
	currentFrame = 0;

	// Load image
	image = IMG_Load( (const char *)file.c_str() );
	if ( !image )
	{
		nrFrames = 0;
		image = 0;
		return;
	}

	SDL_SetColorKey(image, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB( image->format, 0, 0, 0 ) );
//	image = SDL_DisplayFormat( tmp );

//	SDL_FreeSurface( tmp );
	frameWidth = image->w/nrFrames;
	frameHeight = image->h;
}

AnimatedImage :: ~AnimatedImage()
{
	if ( image != 0 )
		SDL_FreeSurface( image );
}

bool AnimatedImage :: nextFrame()
{
	bool rc = true;
	currentFrame ++;
	if ( currentFrame >= nrFrames )
	{
		currentFrame --;
		rc = false;
	}

	return rc;
}

void AnimatedImage :: draw( SDL_Surface *screen, int x, int y )
{
	SDL_Rect dst;
	dst.x = currentFrame * frameWidth;
	dst.y = 0;
	dst.w = frameWidth;
	dst.h = frameHeight;

	SDL_Rect dst2;
	dst2.x = x - (frameWidth/2);
	dst2.y = y - (frameHeight/2);;
	SDL_BlitSurface( image, &dst, screen, &dst2 );
}

bool AnimatedImage :: AtEnd()
{
	if ( currentFrame +1 >= nrFrames || image == 0 )
		return true;
	return false;
}

