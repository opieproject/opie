#ifndef HORDE_H
#define HORDE_H

#include "Monster.h"
#include "Picture.h"

class Horde {			/*global structure of all bills*/
public:
	static const int MAX_BILLS  = 100;	/*max Bills per level*/
	static const int WCELS = 4;	/* # of bill walking animation frames*/
	static const int DCELS = 5;	/* # of bill dying animation frames*/
	static const int ACELS = 13;	/* # of bill switching OS frames*/
	Monster list[MAX_BILLS];	/* list of monsters in all states*/
	int width, height;
	int on_screen, off_screen;
	Picture lcels[WCELS], rcels[WCELS], acels[ACELS], dcels[DCELS];
	void load_pix();
	void setup();
	void launch(int max);
	int on(unsigned int lev);
	int max_at_once(unsigned int lev);
	int between(unsigned int lev);
	void update();
	void draw();
};

#endif
