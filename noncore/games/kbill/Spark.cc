#include "objects.h"

int Spark::delay(unsigned int lev) {
	return game.MAX (20-lev, 0);
}

void Spark::load_pix() {
	int i;
	for (i=0; i<2; i++)
		pictures[i].load("spark", i);
	width = pictures[0].width;
	height = pictures[0].height;
}
