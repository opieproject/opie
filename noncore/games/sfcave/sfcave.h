#include <qmainwindow.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>
#include <qtimer.h>
#include <qlist.h>

#include "random.h"

#define MAPSIZE 52
#define BLOCKSIZE 6
#define TRAILSIZE 30



class SFCave : public QMainWindow
{
Q_OBJECT

public:
	int sWidth;
	int sHeight;
	int segSize;

    int currentSeed;

    QList<int> replayList;
    QListIterator<int> *replayIt;
    bool replay;
    QString replayFile;

	int blockWidth;
	int blockHeight;
    int gapHeight;
	int state;
	int prevState;
	int speed;
    int crashLineLength;

    static double UpThrustVals[3][3];
    static double DownThrustVals[3][3];
    static double MaxUpThrustVals[3][3];
    static double MaxDownThrustVals[3][3];
    static int initialGateGaps[];
    
    double thrustUp;
    double noThrust;
    double maxUpThrust;
    double maxDownThrust;

    int gateDistance;
    int nextGate;
    int lastGateBottomY;
    
    static QString menuOptions[2][5];
    int currentMenuNr;
    static int nrMenuOptions[2];
    static int currentMenuOption[2];

    static QString dificultyOption[3];
    static QString gameTypes[3];
    int currentGameType;
    int currentGameDifficulty;

	QPixmap *offscreen;
	QTimer *gameTimer;

	int score;
	int highestScore[3][3];

	int mapTop[MAPSIZE];
	int mapBottom[MAPSIZE];
	QRect blocks[BLOCKSIZE];
	QRect user;
	QPoint trail[TRAILSIZE];

	int offset;
	int maxHeight;
	int nrFrames;
	int dir;

	bool showScoreZones;

	bool press;
	double thrust;
	bool running;

	SFCave( int speed = 3, QWidget *p = 0, char *name = 0 );
	~SFCave();
	void start();
    void setSeed( int seed );
	int nextInt( int range );
	void setUp();
    void handleGameSFCave();
    void handleGameGates();
    void handleGameFly();
	bool checkFlyGameCollision();
	void moveFlyGameLandscape();
	void setFlyPoint( int point );
	bool checkCollision();
	void moveLandscape();
	void addBlock();
	void addGate();
	void setPoint( int point );
	void drawBoss();
	void draw();
	void handleKeys();

    void displayMenu();
    void dealWithMenuSelection();

	void keyPressEvent( QKeyEvent *e );
	void keyReleaseEvent( QKeyEvent *e );
    void saveScore();
    void saveReplay();
    void loadReplay();

private slots:
	void run();
};
