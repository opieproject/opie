#include "objects.h"

/*sets up network for each level*/
void Network::setup() {
	int i;	
	units = on (game.level);
	for (i=0; i<units; i++)
		if (!net.computers[i].setup(i)) {
			units = i-1;
			break;
		}
	base = units;
	off = win = 0;
	ncables = game.MIN(game.level, units/2);
	for (i=0; i<ncables; i++) cables[i].setup();
}

/*redraws the computers at their location with the proper image*/
void Network::draw () {
	int i;
	for (i=0; i<ncables; i++) cables[i].draw();
	for (i=0; i<units; i++) computers[i].draw();
}

void Network::update () {
	for (int i=0; i<ncables; i++) cables[i].update();
}

void Network::toasters () {
	for (int i=0; i<units; i++) {
		computers[i].type = computers[i].TOASTER;
		computers[i].os = OS.OFF;
	}
	ncables=0;
}

int Network::on(int lev) {
	return game.MIN(8+lev, MAX_COMPUTERS);
}

void Network::load_pix () {
	int i;
	char *name[] = {"toaster", "maccpu", "nextcpu", "sgicpu", "suncpu",
		"os2cpu", "bsdcpu"};
	for (i=0; i <= NUM_SYS; i++)
		pictures[i].load(name[i]);
	width = pictures[0].width;
	height = pictures[0].height;
}
