#ifndef __ANIMATED_IMAGE_H
#define __ANIMATED_IMAGE_H

#include "SDL.h"

class AnimatedImage
{
public:
	AnimatedImage( QString file, int nFrames );
	~AnimatedImage();

	bool nextFrame();
	void draw( SDL_Surface *screen, int x, int y );
	bool AtEnd();
	void reset() { currentFrame = 0; }
private:

	SDL_Surface *image;
	int nrFrames;
	int currentFrame;

	int frameWidth;
	int frameHeight;
};
#endif
