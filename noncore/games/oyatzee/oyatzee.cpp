#include "oyatzee.h"

#include <qmessagebox.h>
#include <qapplication.h>
#include <qdir.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qvbox.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qpoint.h>

#include <stdlib.h>

OYatzee::OYatzee( QWidget *parent , const char *name, WFlags fl ) : QMainWindow( parent , name , fl )
{
	QWidget *thing = new QWidget( this );
	setCentralWidget( thing );

	setCaption( tr( "OYatzee" ) );

	setPlayerNumber( 4 );
	setRoundsNumber( 1 );

	lastPlayerFinished = false;
	currentPlayer = 1;
	
	ps.append( new Player( "Carsten" ) );
	ps.append( new Player( "Julia" ) );
	ps.append( new Player( "Christine" ) );
	ps.append( new Player( "Stephan" ) );

	
	QVBoxLayout *vbox = new QVBoxLayout( thing );

	sb = new Scoreboard( ps, thing , "sb" );
	connect( sb->pb , SIGNAL( item( int ) ), this , SLOT( slotEndRound( int ) ) );
	
	dw = new DiceWidget( thing , "dw" );
	dw->setMaximumHeight( this->height()/4 );
	connect( dw->rollButton, SIGNAL( clicked() ), this , SLOT( slotRollDices() ) );
	
	vbox->addWidget( sb );
	vbox->addWidget( dw );
}

void OYatzee::slotEndRound( int item )
{
	qDebug( "Der User hat Nummer %d ausgewählt" , item );

	/*
	 * if the user clicked on Total, Bonus or Score and thus not on a 
	 * selectable item return and do nothing
	 */
	if ( item == 7 || item == 8 || item == 16 ) return;

	/*
	 * check if the user can really click on that item
	 */
	if ( posibilities.find( item ) == posibilities.end() ) return;
	
	QValueListInt numbers;
	
	Dice *d = dw->diceList.first();
	for ( ; d != 0 ; d = dw->diceList.next() )
	{
		numbers.append( d->hasValue() );
	}

	int points = 0;

	switch ( item )
	{
		case Ones:
			points = getPoints( 1 , numbers );
			break;
		case Twos:
			points = getPoints( 2 , numbers );
			break;
		case Threes:
			points = getPoints( 3 , numbers );
			break;
		case Fours:
			points = getPoints( 4 , numbers );
			break;
		case Fives:
			points = getPoints( 5 , numbers );
			break;
		case Sixes:
			points = getPoints( 6 , numbers );
			break;
		case ThreeOfAKind:
			points = oakPoints;
			break;
		case FourOfAKind:
			points = oakPoints;
			break;
		case FullHouse:
			points = 25;
			break;
		case SStraight:
			points = 30;
			break;
		case LStraight:
			points = 40;
			break;
		case Yatzee:
			points = 50;
			break;
		case Chance:
			points = getPoints ( Chance , numbers );
	}

	sb->nextRB(currentPlayer-1)->updateMap( item , points );
	nextPlayer();

	qDebug( "Punkte: %d" , points );
}

void OYatzee::nextPlayer()
{
	currentPlayer++;

	if ( currentPlayer > numOfPlayers )
	{
		currentPlayer = 1;
	}
	
	ps.at(currentPlayer-1)->turn = 0;
}

int OYatzee::getPoints( const int num , QValueListInt l)
{
	QValueListInt::Iterator it = l.begin();
	int c = 0;
	
	if ( num != Chance )
	{	
		for ( ; it != l.end() ; ++it )
		{
			if ( *it == num )
				c++;
		}

		return c * num;
	}
	else
	{
		for ( ; it != l.end() ; ++it )
		{
			c += *it;
		}
		return c;
	}
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
		numbers.append( d->hasValue() );
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
			posibilities.append( 9 );

			//now we check if it is a full house
			if ( count == 3 && countFH == 2 ) //aka Full House
				posibilities.append( 11 );
			if ( count >= 4 )
				posibilities.append( 10 );
			if ( count == 5 )                     //Yatzee
				posibilities.append( 14 );
		
			oakPoints = count * i;
		}
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
			posibilities.append( 12 );
		if ( isLong )
			posibilities.append( 13 );
	}	
	
	posibilities.append( 13 );        //Chance, well, this is allways possible

	displayPossibilites();
}

void OYatzee::displayPossibilites()
{
//X 	for ( QValueListInt::Iterator it = posibilities.begin() ; it != posibilities.end(); ++it )
//X 	{
//X 		qDebug( QString::number( *it ) );
//X 		switch ( *it )
//X 		{
//X 			case Ones:
//X 				qDebug( "1er" );
//X 				break;
//X 			case Twos:
//X 				qDebug( "2er" );
//X 				break;
//X 			case Threes:
//X 				qDebug( "3er" );
//X 				break;
//X 			case Fours:
//X 				qDebug( "4er" );
//X 				break;
//X 			case Fives:
//X 				qDebug( "5er" );
//X 				break;
//X 			case Sixes:
//X 				qDebug( "6er" );
//X 				break;
//X 			case ThreeOfAKind:
//X 				qDebug( "3oaK" );
//X 				break;
//X 			case FourOfAKind:
//X 				qDebug( "4oaK" );
//X 				break;
//X 			case FullHouse:
//X 				qDebug( "Full House" );
//X 				break;
//X 			case SStraight:
//X 				qDebug( "Short S" );
//X 				break;
//X 			case LStraight:
//X 				qDebug( "Long S" );
//X 				break;
//X 			case Yatzee:
//X 				qDebug( "Yatzee!" );
//X 				break;
//X 			case Chance:
//X 				qDebug( "Chance" );
//X 				break;
//X 		}
//X 	}

	sb->pb->setIntlist(  posibilities );
	sb->pb->update();
}

void OYatzee::startGame()
{
	/*
	 * TODO
	 */
}

void OYatzee::stopGame(){}

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
	qDebug( "Roll nummer: %d" , ps.at( currentPlayer-1 )->turn );

	if ( ps.at( currentPlayer-1 )->turn == 3 )
	{
		QMessageBox::information(   this, 
									"OYatzee",
									tr( "Only three rolls per turn allowed." ) );
		return;
	}
		
	Dice *d = dw->diceList.first();
	
	for ( ; d != 0 ; d = dw->diceList.next() )
	{
		if ( !d->isSelected )
			d->roll();
	}
	
//	qDebug( "Roll nummer (vorher): %d" , ps.at( currentPlayer-1 )->turn );
	ps.at(currentPlayer-1)->turn++;
//	qDebug( "Roll nummer (nachher): %d" , ps.at( currentPlayer-1 )->turn );

	detectPosibilities();
}

/*
 * Scoreboard
 */
Scoreboard::Scoreboard( playerList ps, QWidget *parent, const char *name ) : QWidget( parent , name )
{ 
	ps_ = ps;

	pb = new Possibilityboard( this , "pb" );

	createResultboards( 4 );

	QHBoxLayout *hbox = new QHBoxLayout( this );

	hbox->addWidget( pb );

	hbox->addSpacing( 25 );

	Resultboard *r = rbList.first();
	
	for ( ; r != 0 ; r = rbList.next() )
	{
		hbox->addWidget( r );
	}
}

Resultboard* Scoreboard::nextRB( int currentPlayer )
{
	Resultboard *b;

	b = rbList.at( currentPlayer );
	
	qDebug( "Anzahl: %d" ,	rbList.count() );

	return b;
}		

void Scoreboard::createResultboards(const int num)
{
	Player *p = ps_.first();
	for ( int i = 0 ; i < num ; ++i , p = ps_.next() )
	{
		QString n = p->playerName;
		rbList.append( new Resultboard( n , this ) );
	}
}

void Scoreboard::paintEvent( QPaintEvent * )
{
//X 	QPainter p;
//X 	p.begin( this );
//X 
//X 	p.drawRect( 0,0, this->width() , this->height() );
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

const int Dice::hasValue() const
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
	p->setBrush( Qt::black );
	
	int w = this->width();
	int h = this->height();
	int r = this->width();
	r /= 10;
	
	switch ( Value )
	{
		case 1:
			p->drawEllipse( (int)( 0.5*w - r ) , (int)( 0.5*h - r ) , r , r ) ;
			break;
		case 2:
			p->drawEllipse( (int)( 0.3*w - r ) , (int)( 0.3*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.7*w - r ) , (int)( 0.7*h - r ) , r , r ) ;
			break;
		case 3:
			p->drawEllipse( (int)( 0.5*w - r ) , (int)( 0.5*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.2*w - r ) , (int)( 0.2*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.8*w - r ) , (int)( 0.8*h - r ) , r , r ) ;
			break;
		case 4:
			p->drawEllipse( (int)( 0.2*w - r ) , (int)( 0.2*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.8*w - r ) , (int)( 0.8*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.8*w - r ) , (int)( 0.2*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.2*w - r ) , (int)( 0.8*h - r ) , r , r ) ;
			break;
		case 5:
			p->drawEllipse( (int)( 0.5*w - r ) , (int)( 0.5*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.2*w - r ) , (int)( 0.2*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.8*w - r ) , (int)( 0.8*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.8*w - r ) , (int)( 0.2*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.2*w - r ) , (int)( 0.8*h - r ) , r , r ) ;
			break;
		case 6:
			p->drawEllipse( (int)( 0.2*w - r ) , (int)( 0.2*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.8*w - r ) , (int)( 0.8*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.8*w - r ) , (int)( 0.2*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.2*w - r ) , (int)( 0.8*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.2*w - r ) , (int)( 0.5*h - r ) , r , r ) ;
			p->drawEllipse( (int)( 0.8*w - r ) , (int)( 0.5*h - r ) , r , r ) ;
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
	setupResultList();
	turn = 0;
}

void Player::setupResultList()
{
	for ( int i = 1 ; i < 14 ; ++i )
	{
		pResults.append( 0 );
	}
}

/*
 * TODO: muss noch genutzt werden
 */
void Player::updateTotalPoints( pointMap m )
{
	pointMap::Iterator it = m.begin();
	for ( ; it != m.end() ; ++it )
	{
		totalPoints += it.data();
	}
	
}


void Player::setResults( const int cat , const int points )
{
	QValueListInt::Iterator it = pResults.at( cat );
	*it = points;
}

/*
 * Board
 */
Board::Board( QWidget *parent , const char* name ) : QWidget ( parent , name )
{
}

void Board::mousePressEvent( QMouseEvent *e )
{
	emit clicked( e->pos() );
}

/*
 * Resultboard
 */

Resultboard::Resultboard( QString playerName , QWidget *parent , const char* name ) : Board ( parent , name )
{
	pName = playerName;
}

void Resultboard::paintEvent( QPaintEvent* )
{
	QPainter p;
	p.begin( this );
	
	const int cell_width = this->width();
	const int cell_height = this->height()/17;
	
	pointMap::Iterator it = pMap.begin();
	for ( ; it != pMap.end() ; ++it )
	{
		int i = it.key();
		qDebug( "ok: %d , %d" , i , it.data() );
		p.drawText( 0, i*cell_height , cell_width , cell_height , Qt::AlignCenter , QString::number( it.data() ) );
	}

	p.drawText( 0,0,cell_width,cell_height, Qt::AlignCenter , pName ); //Playername
}


void Resultboard::updateMap( int item , int points )
{
	pMap.insert( item , points );

	update();
}

/*
 * Possibilityboard
 */

Possibilityboard::Possibilityboard( QWidget *parent , const char* name ) : Board ( parent , name )
{
	begriffe.append( "1er" );
	begriffe.append( "2er" );
	begriffe.append( "3er" );
	begriffe.append( "4er" );
	begriffe.append( "5er" );
	begriffe.append( "6er" );
	begriffe.append( "Total" );
	begriffe.append( "Bonus" );
	begriffe.append( "3oaK" );
	begriffe.append( "4oaK" );
	begriffe.append( "Full House" );
	begriffe.append( "Short S" );
	begriffe.append( "Long S" );
	begriffe.append( "Yatzee!" );
	begriffe.append( "Chance" );
	begriffe.append( "Score" );
	
	connect( this , SIGNAL( clicked( QPoint ) ), this , SLOT( slotClicked( QPoint ) ) );
}

void Possibilityboard::slotClicked( QPoint p)
{
	emit item( p.y()/(this->height()/17) );
}

void Possibilityboard::paintEvent( QPaintEvent* )
{
	QPainter p;
	p.begin( this );

	const int cell_width = this->width();
	const int cell_height = this->height()/17;

	p.setBrush( Qt::blue );

	QValueListInt::Iterator listIt = list.begin();
	for ( ; listIt != list.end() ; ++listIt )
	{
		p.drawRect( 0 , (*listIt) * cell_height , cell_width , cell_height );
	}

	p.setBrush( Qt::black );
	p.setBrush( Qt::NoBrush );
	QStringList::Iterator begriffeIt = begriffe.begin();
	for ( int i = 1 ; i < 18 ; ++i )
	{
		p.drawText( 0 , i*cell_height , cell_width , cell_height , Qt::AlignCenter , *begriffeIt );
		++begriffeIt;
	}
}

void Possibilityboard::setIntlist( QValueListInt &l )
{
	list = l;
}

