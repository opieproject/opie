#include "objects.h"

void Horde::setup() {
	for (int i=0; i < MAX_BILLS; i++)
		list[i].state = list[i].OFF;
	off_screen = on (game.level);
	on_screen = 0;
}

/*  Launches Bills whenever called  */
void Horde::launch(int max){
	int i, n;
	if (!max || !off_screen) return;
	n=game.RAND(1, game.MIN(max, off_screen));
	for (i=0; n; n--) {
		for (i++; i < MAX_BILLS; i++)
			if (list[i].state == list[i].OFF) break;
		if (i == MAX_BILLS) return;
		list[i++].enter();
	}
}

int Horde::on (unsigned int lev) {
	return game.MIN(8+3*lev, MAX_BILLS);
}

int Horde::max_at_once (unsigned int lev) {
	return game.MIN(2+lev/4, 12);
}

int Horde::between (unsigned int lev) {
	return game.MAX(14-lev/3, 10);
}

void Horde::load_pix () {
	int i;
	for (i=0; i < WCELS-1; i++) {
		lcels[i].load("billL", i);
		rcels[i].load("billR", i);
	}
	lcels[WCELS-1] = lcels[1]; 
	rcels[WCELS-1] = rcels[1]; 

	for (i=0; i < DCELS; i++) 
		dcels[i].load("billD", i);
	width=dcels[0].width;
	height=dcels[0].height;

	for (i=0; i < ACELS; i++)
		acels[i].load("billA", i);
}

void Horde::update() {
	int i;
	if (!(game.iteration % between(game.level)))
		launch(max_at_once(game.level));
	for (i=0; i < MAX_BILLS; i++) list[i].update();
}

void Horde::draw() {
	int i;
	for (i=0; i < MAX_BILLS; i++) list[i].draw();
}

