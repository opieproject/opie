#ifndef CABLE_H
#define CABLE_H

class Cable {
public:
	int c1, c2;		/*computers connected */
	int x1,y1,x2,y2;	/*endpoints of line representing cable */
	int x, y;		/*current location of spark*/
	float fx, fy;		/*needed for line drawing*/
	int delay;		/*how much time until spark leaves*/
	int active;		/*is spark moving and from which end*/
	int index;
	void setup();
	void draw();
	void update();
	int onspark (int locx, int locy);

};

#endif
