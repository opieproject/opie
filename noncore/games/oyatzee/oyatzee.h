#ifndef WORDGAME_H
#define WORDGAME_H

#include <qmainwindow.h>
#include <qlabel.h>
#include <qlist.h>
#include <qmap.h>
#include <qsplitter.h>

#include <stdlib.h>       // rand() function
#include <qdatetime.h>        // seed for rand()

class Dice;
class Game;
class Scoreboard;
class DiceWidget;
class Resultboard;
class Player;

class QPoint;

typedef QList<Dice> dicesList;
typedef QList<Resultboard> resultboardList;
typedef QValueList<int> QValueListInt;
typedef QList<Player> playerList;
typedef QMap<int,int> pointMap;

class OYatzee : public QMainWindow {
	Q_OBJECT
	public:
		OYatzee( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
		~OYatzee();

		Game *g();
		DiceWidget *dw;
		Scoreboard *sb;

		QValueListInt posibilities;		
		playerList ps;
		
		void setPlayerNumber( const int num );
		void setRoundsNumber( const int num );
		
		enum {  Ones = 1, 
				Twos = 2,
				Threes = 3,
				Fours = 4,
				Fives = 5,
				Sixes = 6,
				ThreeOfAKind = 9,        //12444
				FourOfAKind = 10,        //14444
				FullHouse = 11,          //22555
				SStraight = 12,          //13456
				LStraight = 13,          //12345
				Yatzee = 14,             //55555
				Chance = 15};

	public slots:
		void slotStartGame();
		void slotRollDices();
		void slotEndRound( int );

	private:
		int numOfPlayers;
		int numOfRounds;
		int currentPlayer; /* the number of the current player */

		int oakPoints;

		void nextPlayer();

		bool lastPlayerFinished;

		/*
		 * Check what posibilities the player currently has
		 */
		void detectPosibilities();
		void displayPossibilites();

		int getPoints( const int , QValueListInt );
		
		void startGame();
		void stopGame();

};

class Dice : public QFrame
{
	Q_OBJECT
	public:
		Dice( QWidget* parent = 0, const char* name = 0 );

		bool isSelected;

		const int hasValue() const;
		void roll();

	private:
		int Value;

	private slots:
		void slotSelected();

	signals:
		void selected();
	
	protected:
		void paintEvent( QPaintEvent *e );
		void paintNumber( QPainter *p );
		virtual void mousePressEvent( QMouseEvent* );
};

class DiceWidget : public QWidget
{
	Q_OBJECT
	public:
		DiceWidget( QWidget *parent = 0, const char* name = 0 );

		QPushButton *rollButton;

		dicesList diceList;
};

class Board : public QWidget
{
	Q_OBJECT
	public:
		Board( QWidget *parent = 0, const char* name = 0 );
	
	signals:
		void clicked( QPoint );
		void item( int );
	
	protected:
		virtual void mousePressEvent( QMouseEvent* );
};

class Possibilityboard : public Board
{
	Q_OBJECT

	public:
		Possibilityboard( QWidget *parent = 0, const char* name = 0 );

		QValueListInt list;
		void setIntlist( QValueListInt& );
	
	private:
		QStringList begriffe;

	private slots:
		/*
		 * this slot returns the item the user has selected
		 */
		virtual void slotClicked(QPoint);

	protected:
		virtual void paintEvent( QPaintEvent *e );
};

class Resultboard : public Board
{
	Q_OBJECT

	public:
		Resultboard( QString playerName = 0 , QWidget *parent = 0, const char* name = 0 );
		QString pName;

		pointMap pMap;

		void updateMap( int, int );

	protected:
		virtual void paintEvent(  QPaintEvent *e );
};


class Scoreboard : public QWidget
{
	Q_OBJECT
	public:
		Scoreboard( playerList ps, QWidget *parent = 0, const char* name = 0 );

		Possibilityboard *pb;
		resultboardList rbList;
		playerList ps_;
		
		void createResultboards(const int);

		Resultboard* nextRB(int);

		
	protected:
		void paintEvent(  QPaintEvent *e );
};


class Player
{
	public:
		Player( QString name );

		QString playerName;
		int totalPoints;

		void setResults( const int , const int );

		int turn;

		void updateTotalPoints( QMap<int,int> );
		
	private:
		QValueListInt pResults; /* the individual results of the player */

		void setupResultList(); /* only in the ctor */
};

#endif // WORDGAME_H
