#include <qmainwindow.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>
#include <qtimer.h>



#define MAPSIZE 52
#define BLOCKSIZE 5
#define TRAILSIZE 30

#define STATE_BOSS 0
#define STATE_RUNNING 1
#define STATE_CRASHED 2
#define STATE_NEWGAME 3

class Main : public QMainWindow
{
Q_OBJECT

public:
	int sWidth;
	int sHeight;
	int segSize;

	int blockWidth;
	int blockHeight;
	int state;
	int prevState;
	int speed;
    int crashLineLength;

	QPixmap *offscreen;
	QTimer *gameTimer;

	int score;
	int highestScore;

	int mapTop[52];
	int mapBottom[52];
	QRect blocks[5];
	QRect user;
	QPoint trail[30];

	int offset;
	int maxHeight;
	int nrFrames;
	int dir;

	bool bossMode;

	bool press;
	double thrust;
	bool running;

	Main( int speed = 3, QWidget *p = 0, char *name = 0 );
	~Main();
	void start();
	int nextInt( int range );
	void setUp();
	bool checkCollision();
	void moveLandscape();
	void addBlock();
	void setPoint( int point );
	void drawBoss();
	void draw();
	void handleKeys();

	void keyPressEvent( QKeyEvent *e );
	void keyReleaseEvent( QKeyEvent *e );

private slots:
	void run();
};
