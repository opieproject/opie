#ifndef WORDGAME_H
#define WORDGAME_H

#include <qmainwindow.h>
#include <qlabel.h>
#include <qlist.h>

#include <stdlib.h>       // rand() function
#include <qdatetime.h>        // seed for rand()

class Dice;
class Game;
class Scoreboard;
class DiceWidget;

typedef QList<Dice> dicesList;
typedef QValueList<int> QValueListInt;

class OYatzee : public QMainWindow {
	Q_OBJECT
	public:
		OYatzee( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
		~OYatzee();

		Game *g;
		DiceWidget *dw;
		Scoreboard *sb;

		QValueListInt posibilities;		
		
		void setPlayerNumber( const int num );
		void setRoundsNumber( const int num );
		
		enum { 
				Ones=1, 
				Twos = 2,
				Threes = 3,
				Fours = 4,
				Fives = 5,
				Sixes = 6,
				ThreeOfAKind = 7,        //12444
				FourOfAKind = 8,         //14444
				FullHouse = 9,           //22555
				SStraight = 10,          //13456
				LStraight = 11,          //12345
				Yatzee = 12,             //55555
				Chance = 13};

	public slots:
		void slotStartGame();
		void slotRollDices();

	private:
		int numOfPlayers;
		int numOfRounds;

		void detectPosibilities();
		void displayPossibilites();

};

class Dice : public QFrame
{
	Q_OBJECT
	public:
		Dice( QWidget* parent = 0, const char* name = 0 );

		int Value;
		bool isSelected;

		int hasValue();
		void roll();
		virtual void mousePressEvent( QMouseEvent* );

	private slots:
		void slotSelected();

	signals:
		void selected();
	
	protected:
		void paintEvent(  QPaintEvent *e );
		void paintNumber( QPainter *p );
};

class DiceWidget : public QWidget
{
	Q_OBJECT
	public:
		DiceWidget( QWidget *parent = 0, const char* name = 0 );

		QPushButton *rollButton;

		dicesList diceList;
};

class Scoreboard : public QWidget
{
	Q_OBJECT
	public:
		Scoreboard( QWidget *parent = 0, const char* name = 0 );

	protected:
		void paintEvent(  QPaintEvent *e );
};

class Player
{
	public:
		Player( QString name );

		QString playerName;
		int totalPoints;
};

#endif // WORDGAME_H
