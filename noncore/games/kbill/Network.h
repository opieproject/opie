#ifndef NETWORK_H
#define NETWORK_H

#include "Computer.h"
#include "Cable.h"

class Network {	/*structure for global network of computers*/
public:
	static const int MAX_COMPUTERS = 20;	/* max computers on screen */
	static const int NUM_SYS = 6;		/* number of computer types */
	Picture pictures[NUM_SYS+1];		/* array of cpu pictures */
	int width, height;			/* size of cpu picture */
	int units;				/* number of cpus in network */
	int win, base, off;			/* number in each state */
	Computer computers[MAX_COMPUTERS];	/* array of cpu info */
	Cable cables[MAX_COMPUTERS];
	int ncables;
	void setup();
	void load_pix();
	void draw();
	void update();
	void toasters();
	int on(int lev);

};

#endif
