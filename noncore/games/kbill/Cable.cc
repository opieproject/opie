#include "objects.h"

void Cable::setup() {
	c1 = game.RAND(0, net.units-1);
	do {
		c2 = game.RAND(0, net.units-1);
	} while (c2 == c1);
	active = index = 0;
	delay = spark.delay(game.level);
	x1 = net.computers[c1].x + net.width/3;
	x2 = net.computers[c2].x + net.width/3;
	y1 = net.computers[c1].y + net.height/2;
	y2 = net.computers[c2].y + net.height/2;
}

void Cable::update () {
	if (active) {
		if ((net.computers[c1].os == OS.WINGDOWS) ==
			(net.computers[c2].os == OS.WINGDOWS))
				active=0;
		else if (net.computers[c1].os == OS.WINGDOWS ||
			net.computers[c2].os == OS.WINGDOWS)
		{
			int dir, xdist, ydist,c;
			float sx, sy;
			dir = (net.computers[c2].os == OS.WINGDOWS);
			if (dir)
				{xdist=x1-x; ydist=y1-y;}
			else
				{xdist=x2-x; ydist=y2-y;}
			sx = xdist >= 0 ? 1.0 : -1.0;
			sy = ydist >= 0 ? 1.0 : -1.0;
			xdist = abs(xdist);
			ydist = abs(ydist);
			if (xdist==0 && ydist==0) {
				if (dir==0) c=c2; else c=c1;
				if (!net.computers[c].busy) {
					if (net.computers[c].os == OS.OFF)
						net.off--;
					else
						net.base--;
					net.win++;
					net.computers[c].os=OS.WINGDOWS;
				}
				active=0;
			}
			else if (game.MAX (xdist, ydist) < spark.speed) {
				if (dir)
					{x = x1; y = y1;}
				else
					{x = x2; y = y2;}
			}
			else {
				fx+=(xdist*spark.speed*sx)/(xdist+ydist);
				fy+=(ydist*spark.speed*sy)/(xdist+ydist);
				x = (int)fx;
				y = (int)fy;
			}
			index = 1 - index;
		}
	}
	else {
		if ((net.computers[c1].os == OS.WINGDOWS) ==
			(net.computers[c2].os == OS.WINGDOWS))
				delay = spark.delay(game.level);
		else if (net.computers[c1].os == OS.WINGDOWS ||
			net.computers[c2].os == OS.WINGDOWS)
		{
			if (delay>0) delay--;
			else {
				active = 1;
				if (net.computers[c1].os == OS.WINGDOWS)
					{fx=x=x1; fy=y=y1;}
				else
					{fx=x=x2; fy=y=y2;}
			}
		}
	}
}

int Cable::onspark (int locx, int locy) {
	if (!active) return 0;
	return (abs(locx-x) < spark.width
		&& abs(locy-y) < spark.height);
}

void Cable::draw() {
	int rx = x - spark.width/2;
	int ry = y - spark.height/2;
	ui.draw_line(x1,y1,x2,y2);
	if (active)
		ui.draw(spark.pictures[index], rx, ry);
}

