#include "objects.h"

int Bucket::clicked (int x, int y) {
	return (x > 0 && y > 0 && x < width && y < height);
}

void Bucket::draw() {
	if (game.grabbed != game.BUCKET) ui.draw(picture, 0, 0);
}

void Bucket::load_pix() {
	picture.load("bucket");
	cursor.load("bucket", cursor.OWN_MASK);
	width = picture.width;
	height = picture.height;
}
