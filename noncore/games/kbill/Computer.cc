#include "objects.h"

int Computer::setup(int i) {
	int j, counter=0, flag;
	do {
		if (++counter>4000) return 0;
		x = game.RAND(BORDER, game.scrwidth - BORDER - net.width);
		y = game.RAND(BORDER, game.scrheight - BORDER - net.height);
		flag=1;
		/*checks for conflicting computer placement*/
		for (j=0; j<i && flag; j++)
			if (game.INTERSECT(x, y,
				net.width-bill.list[0].XOFFSET+bill.width,
				net.height, net.computers[j].x,
				net.computers[j].y,
				net.width-bill.list[0].XOFFSET+bill.width,
				net.height))
					flag=0;
	} while (!flag);
	type = game.RAND(1, net.NUM_SYS);
	os = determineOS();
	busy=0;
	return 1;
}

int Computer::find_stray() {
	int i;
	for (i=0; i < bill.MAX_BILLS; i++) {
		if (bill.list[i].state != bill.list[i].STRAY) continue;
		if (game.INTERSECT(x, y, net.width, net.height, bill.list[i].x,
			bill.list[i].y, OS.width, OS.height))
				return i;
	}
	return -1;
}

int Computer::oncomputer (int locx, int locy) {
	return (abs(locx-x) < net.width && abs(locy-y) < net.height);
}

int Computer::compatible (int system) {
	return (type==system || (type>=PC && system>=OS.PC));
}

int Computer::determineOS() {
	if (type<PC) return type;
	else return game.RAND (OS.PC, OS.NUM_OS);
}

void Computer::draw() {
	ui.draw(net.pictures[type], x, y);
	if (os != OS.OFF)
		ui.draw(OS.os[os], x + OFFSET, y + OFFSET);
}


