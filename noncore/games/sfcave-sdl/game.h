#ifndef __GAME_H
#define __GAME_H

#include <list>
using namespace std;

#include "sfcave.h"

class Terrain;
class Player;

class Game
{
public:
	Game( SFCave *p, int w, int h, int diff );
	virtual ~Game();

	virtual void init( );
	virtual void update( int state );
	virtual void preDraw( SDL_Surface * );
	virtual void draw( SDL_Surface * );

	virtual void stateChanged( int from, int to );

	void setReplay( bool val ) { replay = val; }

	void handleKeys( SDL_KeyboardEvent &key );
	QString getGameName() { return gameName; }
	int getDifficulty() { return difficulty; }
	QString getGameDifficultyText();
	void setDifficulty( int diff ) { difficulty = diff; }
	void setDifficulty( string diff );

	long getScore() { return score; }
	long getHighScore() { return highScore; }
	void increaseScore( long val ) { score += val; }
	void clearScore() { score = 0; }
	bool gotHighScore() 	{ return (score >= highScore); }
	bool isReplayAvailable() { return replayList.size() > 0; }

	Terrain *getTerrain() { return terrain; }

	void setSeed( int seed );
	void loadReplay( QString file );
	void saveReplay( QString file );

	static Game *createGame( SFCave *p, int w, int h, string game, string difficulty );

protected:
	QString gameName;

	int thrustChannel;

	int difficulty;

	SFCave *parent;
	Terrain *terrain;
	Player *player;

	int nrFrames;

	bool press;

	int sHeight;
	int sWidth;
	long score;
	long highScore;

	// Stuff for the replays
    int currentSeed;

//    QListIterator<int> *replayIt;
	list<int> replayList;
	list<int>::iterator replayIt;
//	QList<int> replayList;
    bool replay;
    QString replayFile;

private:
};

#endif
