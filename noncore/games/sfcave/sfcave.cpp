#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef QWS
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

#include "sfcave.h"

#define CAPTION "SFCave 1.6 by AndyQ"
bool movel;

int main( int argc, char *argv[] )
{
	movel = true;

#ifdef QWS
	QPEApplication a( argc, argv );
#else
	QApplication a( argc, argv );
#endif

	int speed = 3;
	for ( int i = 0 ; i < argc ; ++i )
	{
		if ( strcmp( argv[i], "-s" ) == 0 )
		{
			if ( i+1 < argc )
				speed = atoi( argv[i+1] );
		}
	}

	Main app( speed );
	a.setMainWidget( &app );
	app.show();
	app.start();
	a.exec();
}

Main :: Main( int spd, QWidget *w, char *name )
	: QMainWindow( w, name )
{
#ifdef QWS
	showMaximized();
#else
	resize( 240, 284 );
#endif

	sWidth = width();
	sHeight = height();
	segSize = sWidth/(MAPSIZE-1)+1;

//	printf( "width %d, height %d\n", sWidth, sHeight );
//	printf( "segSize = %d\n", segSize );

	setCaption( CAPTION );

	highestScore = 0;
	speed = spd; // Change to 2 for PC
	press = false;

	offscreen = new QPixmap( sWidth, sHeight );
	offscreen->fill( Qt::black );

	setUp();
    crashLineLength = -1;

	gameTimer = new QTimer( this, "game timer" );
	connect( gameTimer, SIGNAL( timeout() ),
	         this, SLOT( run() ) );
}

Main :: ~Main()
{
}

void Main :: start()
{
    gameTimer->start( 10 );

}

int Main :: nextInt( int range )
{
	return rand() % range;
}

void Main :: setUp()
{
	state = STATE_CRASHED;
	prevState = STATE_CRASHED;

	score = 0;
	offset = 0;
	maxHeight = 50;
	nrFrames = 0;
	dir = 1;
	thrust = 0;
    crashLineLength = 0;

	user.setRect( 50, sWidth/2, 4, 4 );

	blockWidth = 20;
	blockHeight = 70;

	for ( int i = 0 ; i < TRAILSIZE ; ++i )
	{
		trail[i].setX( -1 );
		trail[i].setY( 0 );
	}

	mapTop[0] = (int)(nextInt(50)) + 5;
	for ( int i = 1 ; i < MAPSIZE ; ++i )
		setPoint( i );

	for ( int i = 0 ; i < BLOCKSIZE ; ++i )
		blocks[i].setY( -1 );
}

void Main :: run()
{
	//running = true;
	//setUp();
		switch ( state )
		{
			case STATE_NEWGAME:
				setUp();
				draw();
				state = STATE_RUNNING;
				break;
			case STATE_BOSS:
				drawBoss();
				break;

			case STATE_CRASHED:
				draw();
				break;

			case STATE_RUNNING:
			{
				if ( nrFrames % 5 == 0 )
					score ++;
				if ( nrFrames % 2 == 0 )
					handleKeys();

				if ( ++nrFrames % 500 == 0 )
				{
					if ( maxHeight < sHeight - 100 )
					{
						maxHeight += 10;

						// Reduce block height
						if ( maxHeight > sHeight - 150 )
							blockHeight -= 5;
					}
				}

				if ( nrFrames % 100 == 0 )
					addBlock();

	//			if ( false )
				if ( checkCollision() )
				{
					if ( score > highestScore )
						highestScore = score;

					state = STATE_CRASHED;
				}
				else
				{
					if ( movel )
						moveLandscape();
					//movel = false;
				}

				draw();
				break;
			}
		}
}

bool Main :: checkCollision()
{
	if ( (user.y() + user.width()) >= mapBottom[10] || user.y() <= mapTop[10] )
		return true;

	for ( int i = 0 ; i < BLOCKSIZE ; ++i )
	{
		if ( blocks[i].y() != -1 )
		{
			if ( blocks[i].intersects( user ) )
				return true;
		}
	}
	return false;
}

void Main :: moveLandscape()
{
	offset++;

	if ( offset >= segSize )
	{
		offset = 0;
		for ( int i = 0 ; i < MAPSIZE-speed ; ++i )
		{
			mapTop[i] = mapTop[i+speed];
			mapBottom[i] = mapBottom[i+speed];
		}

		for ( int i = speed ; i > 0 ; --i )
			setPoint( MAPSIZE-i );
	}

	for ( int i = 0 ; i < BLOCKSIZE ; ++i )
	{
		if ( blocks[i].y() != -1 )
		{
			blocks[i].moveBy( -speed, 0 );
			if ( blocks[i].x() + blocks[i].width() < 0 )
				blocks[i].setY( -1 );
		}
	}
}

void Main :: addBlock()
{
	for ( int i = 0 ; i < BLOCKSIZE ; ++i )
	{
		if ( blocks[i].y() == -1 )
		{
			int x = sWidth;

			int y = mapTop[50] + (int)(nextInt(mapBottom[50] - mapTop[50] - blockHeight));

			blocks[i].setRect( x, y, blockWidth, blockHeight );
//			printf( "Added block @ %d %d\n", x, y );
//			printf( "mapTop = %d, mapBottom = %d", mapTop[50], mapBottom[50] );


			break;
		}
	}
}

void Main :: setPoint( int point )
{
	if ( nextInt(100) >= 80 )
		dir *= -1;

	mapTop[point] = mapTop[point-1] + (dir * nextInt( 5 ) );
	if ( mapTop[point] < 0 )
	{
		mapTop[point] = 0;
		dir *= -1;
	}
	else if ( mapTop[point] >= maxHeight )
	{
		mapTop[point] = maxHeight;
		dir *= -1;
	}

	mapBottom[point] = sHeight - (maxHeight - mapBottom[point]);
	mapBottom[point] = sHeight - (maxHeight - mapTop[point]);
}

void Main :: drawBoss()
{
	offscreen->fill( Qt::black );

	bitBlt( this, 0, 0, offscreen, 0, 0, sWidth, sHeight, Qt::CopyROP, true );
}

void Main :: draw()
{
	//printf( "Paint\n" );
	offscreen->fill( Qt::black );

	QPainter p( offscreen );
	p.setPen( Qt::white );

	for ( int i = 0 ; i < MAPSIZE -3; ++i )
	{
		p.drawLine( (i*segSize) - (offset*speed), mapTop[i], ((i+1)*segSize)-(offset*speed), mapTop[i+1] );
		p.drawLine( (i*segSize) - (offset*speed), mapBottom[i], ((i+1)*segSize)-(offset*speed), mapBottom[i+1] );
	}

	// Draw user
	p.drawRect( user );

	// Draw trails
	for ( int i = 0 ; i < TRAILSIZE ; ++i )
		if ( trail[i].x() >= 0 )
			p.drawRect( trail[i].x(), trail[i].y(), 2, 2 );

	// Draw blocks
	for ( int i = 0 ; i < BLOCKSIZE ; ++i )
		if ( blocks[i].y() != -1 )
		{
			p.fillRect( blocks[i], Qt::black );
			p.drawRect( blocks[i] );
		}

	// draw score
	QString s;
	s.sprintf( "score %06d   high score %06d", score, highestScore );
	p.drawText( 5, 10, s );


	if ( state == STATE_CRASHED )
	{
        // add next crash line

        if ( crashLineLength != -1 )
        {
            for ( int i = 0 ; i < 36 ; ++i )
            {
                int x = (int)(user.x() + (crashLineLength+nextInt(10)) * cos( (M_PI/180) * (10.0 * i) ) );
                int y = (int)(user.y() + (crashLineLength+nextInt(10)) * sin( (M_PI/180) * (10.0 * i) ) );            p.drawLine( user.x(), user.y(), x, y );
            }
        }

        if ( crashLineLength >= 15 || crashLineLength == -1 )
            p.drawText( 70, 140, QString( "Press down to start" ) );
        else
            crashLineLength ++;
	}

	p.end();
	bitBlt( this, 0, 0, offscreen, 0, 0, sWidth, sHeight, Qt::CopyROP, true );
	//printf( "endpaint\n" );
}

void Main :: handleKeys()
{
	// Find enpty trail and move others
	bool done = false;
	for ( int i = 0 ; i < TRAILSIZE ; ++i )
	{
		if ( trail[i].x() < 0 )
		{
			if ( !done )
			{
				trail[i].setX( user.x() - 5 );
				trail[i].setY( user.y() );
				done = true;
			}
		}
		else
		{
			trail[i].setX( trail[i].x() - (2) );
		}
	}

	if ( speed <= 3 )
	{
		if ( press )
			thrust -= 0.6;
		else
			thrust += 0.8;

		if ( thrust > 4.0 )
			thrust = 4.0;
		else if ( thrust < -3.5 )
			thrust = -3.5;
	}
	else
	{
		if ( press )
			thrust -= 0.5;
		else
			thrust += 0.8;

		if ( thrust > 5.0 )
			thrust = 5.0;
		else if ( thrust < -3.5 )
			thrust = -3.5;
	}
	user.moveBy( 0, (int)thrust );
}

void Main :: keyPressEvent( QKeyEvent *e )
{
   switch( e->key() )
    {
        case Qt::Key_Up:
        case Qt::Key_F9:
        case Qt::Key_Space:
            press = true;
            break;
       default:
            e->ignore();
            break;
	}
}

void Main :: keyReleaseEvent( QKeyEvent *e )
{
   switch( e->key() )
    {
        case Qt::Key_Up:
        case Qt::Key_F9:
        case Qt::Key_Space:
            press = false;
            break;
		case Qt::Key_R:
		case Qt::Key_Down:
			if ( state == STATE_CRASHED && crashLineLength >= 15 || crashLineLength == -1 )
			{
				state = STATE_NEWGAME;
			}
			else
				movel = true;
       default:
            e->ignore();
            break;
	}
}
