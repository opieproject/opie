#ifndef BILL_H
#define BILL_H

#include "Picture.h"

class Monster {		/*structure for Bills*/
public:
	int state;		/*what is it doing?*/
	int index;		/*index to animation frame*/
	Picture *cels;		/*pointer to array of animation frames*/
	int x, y;		/*location*/
	int target_x;		/*target x position*/
	int target_y;		/*target y position*/
	int target_c;		/*target computer*/
	int cargo;		/*which OS carried*/
	int x_offset;		/*accounts for width differences*/
	int y_offset;		/*'bounce' factor for OS carried*/
	int sx, sy;		/*used for drawing extra OS during switch*/

	static const int SLOW = 0;	/* speeds of moving bills */
	static const int FAST = 1;

	static const int OFF = 0;	/* Bill's states */
	static const int IN = 1;
	static const int AT = 2;
	static const int OUT = 3;
	static const int DYING = 4;
	static const int STRAY = 5;

	static const int GRAVITY = 3;	/*speed at which os drops*/

	static const int XOFFSET = 20;	/*offset from right of computer*/
	static const int YOFFSET = 3;	/*offset from top of computer*/

	void get_border();
	void enter();
	int move(int mode);
	void draw();
	void draw_std();
	void draw_at();
	void draw_stray();
	void update();
	void update_in();
	void update_at();
	void update_out();
	void update_dying();
	int clicked(int locx, int locy);
	int clickedstray(int locx, int locy);
	int step_size(unsigned int lev);

};

#endif
