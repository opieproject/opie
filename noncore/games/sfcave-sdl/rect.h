#ifndef __RECT_H
#define __RECT_H

#include <SDL/SDL.h>

class Rect
{
public:
	Rect() { r.x = r.y = r.w = r.h = 0; }
	Rect( int x, int y, int w, int h ) { setRect( x, y, w, h ); }
	~Rect() {}

	void setRect( int x, int y, int w, int h ) { r.x = x; r.y = y; r.w = w; r.h = h; }
	SDL_Rect getRect() { return r; }
	int x() { return r.x; }
	int y() { return r.y; }
	int w() { return r.w; }
	int h() { return r.h; }

	void x( int x) { r.x = x; }
	void y( int y) { r.y = y; }
	void w( int w) { r.w = w; }
	void h( int h) { r.h = h; }

	void moveBy( int x, int y )
	{
		r.x += x;
		r.y += y;
	}

	bool intersects( Rect r2 )
	{
		int tw = r.w;
		int th = r.h;
		int rw = r2.w();
		int rh = r2.h();
		if (rw <= 0 || rh <= 0 || tw <= 0 || th <= 0) {
			return false;
		}
		int tx = r.x;
		int ty = r.y;
		int rx = r2.x();
		int ry = r2.y();
		rw += rx;
		rh += ry;
		tw += tx;
		th += ty;

		//      overflow || intersect
		return ((rw < rx || rw > tx) &&
			(rh < ry || rh > ty) &&
			(tw < tx || tw > rx) &&
			(th < ty || th > ry));
	}

private:
	SDL_Rect r;
};

#endif

