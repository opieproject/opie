#include <qstring.h>
#ifdef KDEVER
#include <kapplication.h>
#include <kstandarddirs.h>
#endif
#include "objects.h"

FILE *Scorelist::open_file(char *mode) {
// 	QString file;
// 	file.sprintf("%s/kbill/scores", (const char *)locate("data",""));
// 	//kdDebug() << locate("data","") << endl;
// 	return fopen (file, mode);
}

void Scorelist::read() {
// 	FILE *scorefile = open_file("r");
// 	int i;
// 	if (scorefile) {
// 		for (i=0; i<10; i++) {
// 			fgets (name[i], 21, scorefile);
// 			fscanf (scorefile, "%d%d\n", &(level[i]), &(score[i]));
// 		}
// 		fclose(scorefile);
// 	}
// 	else
// 		for (i=0; i<10; i++) {
// 			strcpy(name[i], "me");
// 			level[i] = score[i] = 0;
// 		}
}

void Scorelist::write() {
// 	int i, j;
// 	FILE *scorefile = open_file("w");
// 	if (!scorefile) return;
// 	for (i=0; i<10; i++) {
// 		fputs(name[i], scorefile);
// 		for (j=strlen(name[i]); j<25; j++)
// 			fputc(' ', scorefile);
// 		fprintf (scorefile, " %d %d\n", level[i], score[i]);
// 	}
// 	fclose(scorefile);
}

/*  Add new high score to list   */
void Scorelist::recalc (char *str) {
// 	int i;
// 	if (score[9] >= game.score) return;
// 	for (i=9; i>0; i--) {
// 		if (score[i-1] < game.score) {
// 			strcpy (name[i], name[i-1]);
// 			level[i] = level[i-1];
// 			score[i] = score[i-1];
// 		}
// 		else break;
// 	}
// 	strcpy (name[i], str);
// 	level[i] = game.level;
// 	score[i] = game.score;
}

void Scorelist::update() {
// 	char str[500], temp[40];
// 	int i, j;
// 	strcpy (str,"High Scores:\n\n");
// 	strcat (str, "Name                 Level    Score\n");
// 	for (i=0; i<10; i++) {
// 		strcat (str, name[i]);
// 		for (j=strlen(name[i]); j<21; j++) strcat (str, " ");
// 		sprintf (temp, "%5d  %7d\n", level[i], score[i]);
// 		strcat (str, temp);
// 	}
// 	ui.update_hsbox(str);
}

