#ifndef COMPUTER_H
#define COMPUTER_H

class Computer {	/*structure for Computers*/
public:
	int type;		/*CPU type*/
	int os;			/*current OS*/
	int x, y;		/*location*/
	int busy;		/*is the computer being used?*/
	int setup(int i);
	void draw();
	int find_stray();
	int oncomputer (int locx, int locy);
	int compatible(int system);
	int determineOS();
	static const int TOASTER = 0;	/* computer 0 is a toaster */
	static const int PC = 5;	/* type>=PC means the computer is a PC*/
	static const int OFFSET = 4;	/* offset of screen from 0,0 */
	static const int BORDER = 50;	/* BORDER pixels free on all sides*/
};

#endif
