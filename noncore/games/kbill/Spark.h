#ifndef SPARK_H
#define SPARK_H

#include "Picture.h"

class Spark {
public:
	Picture pictures[2];
	int width, height;
	void draw();
	void load_pix();
	static const int speed = 5;
	int delay(unsigned int lev);
};

#endif
