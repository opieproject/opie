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
	Dice *d = dw->diceList.first();

	QValueList<int> numbers;
	
	for ( ; d != 0 ; d = dw->diceList.next() )
	{
		numbers.append( d->Value );
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
	qDebug( QString::number( Value ) );
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
		d->roll();
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

