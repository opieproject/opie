#include "oyatzee.h"

#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/filemanager.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdir.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qpe/qpetoolbar.h>
#include <qtoolbutton.h>
#include <qvbox.h>
#include <qwidgetstack.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qregexp.h>

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

OYatzee::OYatzee( QWidget *parent , const char *name, WFlags fl ) : QMainWindow( parent , name , fl )
{
	QWidget *thing = new QWidget( this );
	setCentralWidget( thing );
	
	QVBoxLayout *vbox = new QVBoxLayout( thing );

	sb = new Scoreboard( thing , "sb" );
	dw = new DiceWidget( thing , "dw" );
	
	vbox->addWidget( sb );
	vbox->addWidget( dw );

	setPlayerNumber( 2 );
	setRoundsNumber( 1 );
	
	connect( dw->rollButton, SIGNAL( clicked() ), this , SLOT( slotRollDices() ) );
}

OYatzee::~OYatzee()
{
}

void OYatzee::detectPosibilities()
{
	posibilities.clear();
	qDebug( "running detectPosibilities()" );
	
	Dice *d = dw->diceList.first();

	QValueListInt numbers;
	
	for ( ; d != 0 ; d = dw->diceList.next() )
	{
		numbers.append( d->Value );
	}
	
	//the 6 numbers
	QValueListInt::Iterator it;

	for ( int i = 1 ; i < 7 ; ++i ) // check for 1-->6
	{
		bool cont = false;
		it = numbers.begin();
		for ( ; it != numbers.end() ; ++it )
		{
			if ( cont )
				continue;
		
			if ( numbers.find( i ) != numbers.end() )
			{
				posibilities.append( i );
				cont = true;
			}
		}
	}
		
	
	//3er, 4er, Yatzee
	it = numbers.begin();
	int count;
	int temp;
	int countFH = 0; //for the full-house-check

	for ( int i = 1 ; i < 7 ; ++i ) // check for 1-->6 at least 3 times
	{
		count = 0;
		temp = 0;
		it = numbers.begin();
		for ( ; it != numbers.end() ; ++it )
		{
			if ( *it == i )
			{
				count++;
				temp++;
			}
			if ( temp == 2 )
				countFH = temp;
		}
		
		if ( count >= 3 )
		{
			posibilities.append( 7 );

			//now we check if it is a full house
			if ( count == 3 && countFH == 2 ) //aka Full House
				posibilities.append( 9 );
		}
		if ( count >= 4 )
			posibilities.append( 8 );
		if ( count == 5 )                     //Yatzee
			posibilities.append( 12 );
	}
	
	//S-Straight
	if ( numbers.find( 3 ) != numbers.end() && numbers.find( 4 ) != numbers.end() )
	{
		bool isLong = false;
		bool isShort = true;
		//L-Straight
		if ( numbers.find( 2 ) != numbers.end() && numbers.find( 5 ) != numbers.end() )
		{
			isShort = true;
		
			//12345 or 23456
			if ( numbers.find( 1 ) != numbers.end() || numbers.find( 6) != numbers.end() )
				isLong = true;
		}
		//1234
		if ( numbers.find( 1 ) != numbers.end() && numbers.find( 2 ) != numbers.end() )
			isShort = true;
		//3456
		if ( numbers.find( 5 ) != numbers.end() && numbers.find( 6 ) != numbers.end() )
			isShort = true;
		
		if ( isShort )
			posibilities.append( 10 );
		if ( isLong )
			posibilities.append( 11 );
	}	
	
	posibilities.append( 13 );        //Chance, well, this is allways possible

	displayPossibilites();
}

void OYatzee::displayPossibilites()
{
	qDebug( "running displayPossibilites(), %d item", posibilities.count() );
	
	for ( QValueListInt::Iterator it = posibilities.begin() ; it != posibilities.end(); ++it )
	{
		qDebug( QString::number( *it ) );
		switch ( *it )
		{
			case Ones:
				qDebug( "1er" );
				break;
			case Twos:
				qDebug( "2er" );
				break;
			case Threes:
				qDebug( "3er" );
				break;
			case Fours:
				qDebug( "4er" );
				break;
			case Fives:
				qDebug( "5er" );
				break;
			case Sixes:
				qDebug( "6er" );
				break;
			case ThreeOfAKind:
				qDebug( "3oaK" );
				break;
			case FourOfAKind:
				qDebug( "4oaK" );
				break;
			case FullHouse:
				qDebug( "Full House" );
				break;
			case SStraight:
				qDebug( "Short S" );
				break;
			case LStraight:
				qDebug( "Long S" );
				break;
			case Yatzee:
				qDebug( "Yatzee!" );
				break;
			case Chance:
				qDebug( "Chance" );
				break;
		}
	}
}

void OYatzee::setPlayerNumber( const int num )
{
	numOfPlayers = num;
}

void OYatzee::setRoundsNumber( const int num )
{
	numOfRounds = num;
}

void OYatzee::slotStartGame()
{
}

void OYatzee::slotRollDices()
{
	Dice *d = dw->diceList.first();
	
	for ( ; d != 0 ; d = dw->diceList.next() )
	{
		if ( !d->isSelected )
			d->roll();
	}
	
	detectPosibilities();
}

/*
 * Scoreboard
 */
Scoreboard::Scoreboard( QWidget *parent, const char *name ) : QWidget( parent , name )
{
}

void Scoreboard::paintEvent( QPaintEvent * )
{
	QPainter p;
	p.begin( this );

	p.drawRect( 0,0, this->width() , this->height() );
}

/*
 * Dice
 */
Dice::Dice( QWidget *parent , const char *name ) : QFrame( parent , name )
{
	QTime t = QTime::currentTime();   // set random seed
	srand(t.hour()*12+t.minute()*60+t.second()*60);

	connect( this , SIGNAL( selected() ), this , SLOT( slotSelected() ) );
}

void Dice::slotSelected()
{
	if ( isSelected )
		isSelected = false;
	else isSelected = true;

	update();
}

int Dice::hasValue()
{
	return Value;
}

void Dice::roll()
{
	Value = rand()%6;
	Value += 1;

	update();
}

void Dice::mousePressEvent( QMouseEvent* /*e*/ )
{
	    emit selected();
}

void Dice::paintEvent( QPaintEvent * )
{
	QPainter p;
	p.begin( this );

	p.drawRect( 0,0, this->width() , this->height() );

	if ( isSelected )
		p.drawRect( 20,20, 10,10 );

	paintNumber( &p );
}

void Dice::paintNumber( QPainter *p )
{
	switch ( Value )
	{
		case 1:
			p->drawText( 10,10,"1");
			break;
		case 2:
			p->drawText( 10,10,"2");
			break;
		case 3:
			p->drawText( 10,10,"3");
			break;
		case 4:
			p->drawText( 10,10,"4");
			break;
		case 5:
			p->drawText( 10,10,"5");
			break;
		case 6:
			p->drawText( 10,10,"6");
			break;
	}
}

/*
 * DiceWidget
 */
DiceWidget::DiceWidget( QWidget *parent , const char *name ) : QWidget(  parent , name )
{
	rollButton = new QPushButton( tr( "Roll" ) , this ) ;
	
	for ( int i = 0 ; i < 5 ; i++ )
	{
		//appending the 5 dices of the game
		diceList.append( new Dice( this, "wuerfel" ) );
	}
	
	QHBoxLayout *hbox = new QHBoxLayout( this );

	Dice *d = diceList.first();
	
	for ( ; d != 0 ; d = diceList.next() )
	{
		hbox->addWidget( d );
	}

	hbox->addWidget( rollButton );
}

/*
 * Player
 */
Player::Player( QString name )
{
	playerName = name;
}

