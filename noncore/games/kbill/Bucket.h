#ifndef BUCKET_H
#define BUCKET_H

#include "Picture.h"
#include "MCursor.h"

class Bucket {
public:
	Picture picture;
	MCursor cursor;
	int width, height;
	void draw();
	int clicked(int x, int y);
	void load_pix();
};

#endif
