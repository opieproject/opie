#ifndef __STARFIELD_H
#define __STARFIELD_H

class StarField
{
public:
	StarField( bool side = false, int nrStars = 100, int mx = 240, int my = 320, int minz = 32, int maxz = 725 );
	~StarField();

	void init();
    void move( );
    void draw( SDL_Surface *screen, int w = 240, int h = 320 );

private:
	// 3d effect
	int *x;
	int *y;
	int *z;

	int maxX;
	int maxY;
	int minZ;
	int maxZ;

	// Sideways
	int *star_color;
	int *vel_x;
	int *vel_y;
	int *pos_x;
	int *pos_y;
	int min_brightness;
	int top_star_speed;

	bool sideways;
	int nrStars;

	void newStar( int i );
};


#endif
