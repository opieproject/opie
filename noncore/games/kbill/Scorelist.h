#ifndef SCORELIST_H
#define SCORELIST_H

class Scorelist {
public:
	char name[10][21];
	unsigned level[10];
	unsigned score[10];
	FILE *open_file(char *mode);
	void read();
	void write();
	void recalc(char *str);
	void update();
};

#endif
