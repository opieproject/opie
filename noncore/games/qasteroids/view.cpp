/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************//*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#include "view.h"

#include <qpe/resource.h>

#include <qapplication.h>
#include <qkeycode.h>
#include <qaccel.h>

#include <stdlib.h>
#include <math.h>

#define IMG_BACKGROUND "qasteroids/bg.png"

#define REFRESH_DELAY           33
#define SHIP_SPEED              0.3
#define MISSILE_SPEED           10.0
#define SHIP_STEPS              64
#define ROTATE_RATE             2
#define SHIELD_ON_COST          1
#define SHIELD_HIT_COST         30
#define BRAKE_ON_COST           4

#define MAX_ROCK_SPEED          2.5
#define MAX_POWERUP_SPEED       1.5
#define MAX_SHIP_SPEED		8
#define MAX_BRAKES              5
#define MAX_SHIELDS             5
#define MAX_FIREPOWER		5

#define TEXT_SPEED              4

#define PI_X_2                  6.283185307
#ifndef M_PI
#define M_PI 3.141592654
#endif

struct
{
    int id;
    const char *path;
    int frames;
}
kas_animations [] =
{
//    { ID_ROCK_LARGE,       "rock1/rock1\%1.png",       32 },
    { ID_ROCK_MEDIUM,      "rock2/rock2\%1.png",       32 },
    { ID_ROCK_SMALL,       "rock3/rock3\%1.png",       32 },
    { ID_SHIP,             "ship/ship\%1.png",         32 },
    { ID_MISSILE,          "missile/missile.png",      1 },
    { ID_BIT,              "bits/bits\%1.png",         16 },
    { ID_EXHAUST,          "exhaust/exhaust.png",      1 },
    { ID_ENERGY_POWERUP,   "powerups/energy.png",      1 },
//    { ID_TELEPORT_POWERUP, "powerups/teleport%1.png", 12 },
    { ID_BRAKE_POWERUP,    "powerups/brake.png",       1 },
    { ID_SHIELD_POWERUP,   "powerups/shield.png",      1 },
    { ID_SHOOT_POWERUP,    "powerups/shoot.png",       1 },
    { ID_SHIELD,           "shield/shield\%1.png",      6 },
    { 0,                   0,                          0 }
};



KAsteroidsView::KAsteroidsView( QWidget *parent, const char *name )
    : QWidget( parent, name ),
      field(200, 200),
      view(&field,this)
{
    view.setVScrollBarMode( QScrollView::AlwaysOff );
    view.setHScrollBarMode( QScrollView::AlwaysOff );
    rocks.setAutoDelete( TRUE );
    missiles.setAutoDelete( TRUE );
    bits.setAutoDelete( TRUE );
    powerups.setAutoDelete( TRUE );
    exhaust.setAutoDelete( TRUE );

    QPixmap pm( Resource::loadPixmap(IMG_BACKGROUND) );
    field.setBackgroundPixmap( pm );

    textSprite = new QCanvasText( &field );
    QFont font( "helvetica", 14 );
    textSprite->setFont( font );

    shield = 0;
    shieldOn = FALSE;
    refreshRate = REFRESH_DELAY;

    readSprites();

    shieldTimer = new QTimer( this );
    connect( shieldTimer, SIGNAL(timeout()), this, SLOT(hideShield()) );
    mTimerId = -1;

    shipPower = MAX_POWER_LEVEL;
    vitalsChanged = TRUE;
    can_destroy_powerups = FALSE;

    mPaused = TRUE;
}

// - - -

KAsteroidsView::~KAsteroidsView()
{
}

// - - -

void KAsteroidsView::reset()
{
    rocks.clear();
    missiles.clear();
    bits.clear();
    powerups.clear();
    exhaust.clear();

    shotsFired = 0;
    shotsHit = 0;

    rockSpeed = 1.0;
    powerupSpeed = 1.0;
    mFrameNum = 0;
    mPaused = FALSE;

    ship->hide();
    shield->hide();
/*
    if ( mTimerId >= 0 ) {
	killTimer( mTimerId );
	mTimerId = -1;
    }
*/
}

// - --

void KAsteroidsView::newGame()
{
    if ( shieldOn )
    {
      shield->hide();
      shieldOn = FALSE;
    }
    reset();
    if ( mTimerId < 0 )
	mTimerId = startTimer( REFRESH_DELAY );
    emit updateVitals();
}

// - - -

void KAsteroidsView::endGame()
{
}

void KAsteroidsView::pause( bool p )
{
    if ( !mPaused && p ) {
	if ( mTimerId >= 0 ) {
	    killTimer( mTimerId );
	    mTimerId = -1;
	}
    } else if ( mPaused && !p )
	mTimerId = startTimer( REFRESH_DELAY );
    mPaused = p;
}

// - - -

void KAsteroidsView::newShip()
{
    ship->move( field.width()/2, field.height()/2, 0 );
    shield->move( field.width()/2, field.height()/2, 0 );
    ship->setVelocity( 0.0, 0.0 );
    shipDx = 0;
    shipDy = 0;
    shipAngle = 0;
    rotateL = FALSE;
    rotateR = FALSE;
    thrustShip = FALSE;
    shootShip = FALSE;
    brakeShip = FALSE;
    teleportShip = FALSE;
    shieldOn = TRUE;
    shootDelay = 0;
    shipPower = MAX_POWER_LEVEL;
    rotateRate = ROTATE_RATE;
    rotateSlow = 0;

    mBrakeCount = 0;
    mTeleportCount = 0;
    mShootCount = 0;

    ship->show();
    shield->show();
    mShieldCount = 1;   // just in case the ship appears on a rock.
    shieldTimer->start( 1000, TRUE );
}

void KAsteroidsView::setShield( bool s )
{
    if ( shieldTimer->isActive() && !s ) {
	shieldTimer->stop();
	hideShield();
    } else {
	shieldOn = s && mShieldCount;
    }
}

void KAsteroidsView::brake( bool b )
{
    if ( mBrakeCount )
    {
	if ( brakeShip && !b )
	{
	    rotateL = FALSE;
	    rotateR = FALSE;
	    thrustShip = FALSE;
	    rotateRate = ROTATE_RATE;
	}

	brakeShip = b;
    }
}

// - - -

void KAsteroidsView::readSprites()
{
    QString sprites_prefix = Resource::findPixmap( IMG_BACKGROUND );
    int sep = sprites_prefix.findRev( "/" );

    sprites_prefix.truncate( sep );

    int i = 0;
    while ( kas_animations[i].id )
    {
	animation.insert( kas_animations[i].id,
	    new QCanvasPixmapArray( sprites_prefix + "/" + kas_animations[i].path,
				    kas_animations[i].frames ) );
	i++;
    }

    ship = new QCanvasSprite( animation[ID_SHIP], &field );
    ship->hide();

    shield = new KShield( animation[ID_SHIELD], &field );
    shield->hide();
}

// - - -

void KAsteroidsView::addRocks( int num )
{
    for ( int i = 0; i < num; i++ )
    {
	KRock *rock = new KRock( animation[ID_ROCK_MEDIUM], &field,
			     ID_ROCK_MEDIUM, randInt(2), randInt(2) ? -1 : 1 );
	double dx = (2.0 - randDouble()*4.0) * rockSpeed;
	double dy = (2.0 - randDouble()*4.0) * rockSpeed;
	rock->setVelocity( dx, dy );
	rock->setFrame( randInt( rock->frameCount() ) );
	if ( dx > 0 )
	{
	    if ( dy > 0 )
		rock->move( 5, 5, 0 );
	    else
		rock->move( 5, field.height() - 25, 0 );
	}
	else
	{
	    if ( dy > 0 )
		rock->move( field.width() - 25, 5, 0 );
	    else
		rock->move( field.width() - 25, field.height() - 25, 0 );
	}
	rock->show( );
	rocks.append( rock );
    }
}

// - - -

void KAsteroidsView::showText( const QString &text, const QColor &color, bool scroll )
{
    textSprite->setTextFlags( AlignLeft | AlignVCenter );
    textSprite->setText( text );
    textSprite->setColor( color );

    if ( scroll ) {
	textSprite->move( (field.width()-textSprite->boundingRect().width()) / 2,
			    -textSprite->boundingRect().height() );
	textDy = TEXT_SPEED;
    } else {
	textSprite->move( (field.width()-textSprite->boundingRect().width()) / 2,
			  (field.height()-textSprite->boundingRect().height()) / 2 );
	textDy = 0;
    }
    textSprite->show();
}

// - - -

void KAsteroidsView::hideText()
{
    textDy = -TEXT_SPEED;
}

// - - -

void KAsteroidsView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    field.resize(width()-4, height()-4);
    view.resize(width(),height());
}

// - - -

void KAsteroidsView::timerEvent( QTimerEvent * )
{
    field.advance();

    QCanvasSprite *rock;

    // move rocks forward
    for ( rock = rocks.first(); rock; rock = rocks.next() ) {
	((KRock *)rock)->nextFrame();
	wrapSprite( rock );
    }

    wrapSprite( ship );

    // check for missile collision with rocks.
    processMissiles();

    // these are generated when a ship explodes
    for ( KBit *bit = bits.first(); bit; bit = bits.next() )
    {
	if ( bit->expired() )
	{
	    bits.removeRef( bit );
	}
	else
	{
	    bit->growOlder();
	    bit->setFrame( ( bit->frame()+1 ) % bit->frameCount() );
	}
    }

    for ( KExhaust *e = exhaust.first(); e; e = exhaust.next() )
	exhaust.removeRef( e );

    // move / rotate ship.
    // check for collision with a rock.
    processShip();

    // move powerups and check for collision with player and missiles
    processPowerups();

    if ( textSprite->visible() )
    {
	if ( textDy < 0 &&
	     textSprite->boundingRect().y() <= -textSprite->boundingRect().height() ) {
	    textSprite->hide();
	} else {
	    textSprite->moveBy( 0, textDy );
	}
	if ( textSprite->boundingRect().y() > (field.height()-textSprite->boundingRect().height())/2 )
	    textDy = 0;
    }

    if ( vitalsChanged && !(mFrameNum % 10) ) {
	emit updateVitals();
	vitalsChanged = FALSE;
    }

    mFrameNum++;
}

void KAsteroidsView::wrapSprite( QCanvasItem *s )
{
    int x = int(s->x() + s->boundingRect().width() / 2);
    int y = int(s->y() + s->boundingRect().height() / 2);

    if ( x > field.width() )
	s->move( s->x() - field.width(), s->y() );
    else if ( x < 0 )
	s->move( field.width() + s->x(), s->y() );

    if ( y > field.height() )
	s->move( s->x(), s->y() - field.height() );
    else if ( y < 0 )
	s->move( s->x(), field.height() + s->y() );
}

// - - -

void KAsteroidsView::rockHit( QCanvasItem *hit )
{
    KPowerup *nPup = 0;
    int rnd = static_cast<int>(randDouble()*30.0) % 30;
    switch( rnd )
    {
      case 4:
      case 5:
	nPup = new KPowerup( animation[ID_ENERGY_POWERUP], &field,
			     ID_ENERGY_POWERUP );
	break;
      case 10:
//        nPup = new KPowerup( animation[ID_TELEPORT_POWERUP], &field,
//                             ID_TELEPORT_POWERUP );
	break;
      case 15:
	nPup = new KPowerup( animation[ID_BRAKE_POWERUP], &field,
				  ID_BRAKE_POWERUP );
	break;
      case 20:
	nPup = new KPowerup( animation[ID_SHIELD_POWERUP], &field,
				  ID_SHIELD_POWERUP );
	break;
      case 24:
      case 25:
	nPup = new KPowerup( animation[ID_SHOOT_POWERUP], &field,
				  ID_SHOOT_POWERUP );
	break;
    }
    if ( nPup )
    {
	double r = 0.5 - randDouble();
	nPup->move( hit->x(), hit->y(), 0 );
	nPup->setVelocity( hit->xVelocity() + r, hit->yVelocity() + r );
	nPup->show( );
	powerups.append( nPup );
    }

    if ( hit->rtti() == ID_ROCK_LARGE || hit->rtti() == ID_ROCK_MEDIUM )
    {
	// break into smaller rocks
	double addx[4] = { 1.0, 1.0, -1.0, -1.0 };
	double addy[4] = { -1.0, 1.0, -1.0, 1.0 };

	double dx = hit->xVelocity();
	double dy = hit->yVelocity();

	double maxRockSpeed = MAX_ROCK_SPEED * rockSpeed;
	if ( dx > maxRockSpeed )
	    dx = maxRockSpeed;
	else if ( dx < -maxRockSpeed )
	    dx = -maxRockSpeed;
	if ( dy > maxRockSpeed )
	    dy = maxRockSpeed;
	else if ( dy < -maxRockSpeed )
	    dy = -maxRockSpeed;

	QCanvasSprite *nrock;

	for ( int i = 0; i < 4; i++ )
	{
	    double r = rockSpeed/2 - randDouble()*rockSpeed;
	    if ( hit->rtti() == ID_ROCK_LARGE )
	    {
		nrock = new KRock( animation[ID_ROCK_MEDIUM], &field,
			       ID_ROCK_MEDIUM, randInt(2), randInt(2) ? -1 : 1 );
		emit rockHit( 0 );
	    }
	    else
	    {
		nrock = new KRock( animation[ID_ROCK_SMALL], &field,
			       ID_ROCK_SMALL, randInt(2), randInt(2) ? -1 : 1 );
		emit rockHit( 1 );
	    }

	    nrock->move( hit->x(), hit->y(), 0 );
	    nrock->setVelocity( dx+addx[i]*rockSpeed+r, dy+addy[i]*rockSpeed+r );
	    nrock->setFrame( randInt( nrock->frameCount() ) );
	    nrock->show( );
	    rocks.append( nrock );
	}
    }
    else if ( hit->rtti() == ID_ROCK_SMALL )
	emit rockHit( 2 );
    rocks.removeRef( (QCanvasSprite *)hit );
    if ( rocks.count() == 0 )
	emit rocksRemoved();
}

void KAsteroidsView::reducePower( int val )
{
    shipPower -= val;
    if ( shipPower <= 0 )
    {
	shipPower = 0;
	thrustShip = FALSE;
	if ( shieldOn )
	{
	    shieldOn = FALSE;
	    shield->hide();
	}
    }
    vitalsChanged = TRUE;
}

void KAsteroidsView::addExhaust( double x, double y, double dx,
				 double dy, int count )
{
    for ( int i = 0; i < count; i++ )
    {
	KExhaust *e = new KExhaust( animation[ID_EXHAUST], &field );
	e->move( x + 2 - randDouble()*4, y + 2 - randDouble()*4 );
	e->setVelocity( dx, dy );
	e->show( );
	exhaust.append( e );
    }
}

void KAsteroidsView::processMissiles()
{
    KMissile *missile;

    // if a missile has hit a rock, remove missile and break rock into smaller
    // rocks or remove completely.
    QPtrListIterator<KMissile> it(missiles);

    for ( ; it.current(); ++it )
    {
	missile = it.current();
	missile->growOlder();

	if ( missile->expired() )
	{
	    missiles.removeRef( missile );
	    continue;
	}

	wrapSprite( missile );

	QCanvasItemList hits = missile->collisions( TRUE );
	QCanvasItemList::Iterator hit;
	for ( hit = hits.begin(); hit != hits.end(); ++hit )
	{
	    if ( (*hit)->rtti() >= ID_ROCK_LARGE &&
		 (*hit)->rtti() <= ID_ROCK_SMALL )
	    {
		shotsHit++;
		rockHit( *hit );
		missiles.removeRef( missile );
		break;
	    }
	}
    }
}

// - - -

void KAsteroidsView::processShip()
{
    if ( ship->visible() )
    {
	if ( shieldOn )
	{
	    shield->show();
	    reducePower( SHIELD_ON_COST );
	    static int sf = 0;
	    sf++;

	    if ( sf % 2 )
		shield->setFrame( (shield->frame()+1) % shield->frameCount() );
	    shield->move( ship->x() - 5, ship->y() - 5 );

	    QCanvasItemList hits = shield->collisions( TRUE );
	    QCanvasItemList::Iterator it;
	    for ( it = hits.begin(); it != hits.end(); ++it )
	    {
		if ( (*it)->rtti() >= ID_ROCK_LARGE &&
		     (*it)->rtti() <= ID_ROCK_SMALL )
		{
		    int factor;
		    switch ( (*it)->rtti() )
		    {
			case ID_ROCK_LARGE:
			    factor = 3;
			    break;

			case ID_ROCK_MEDIUM:
			    factor = 2;
			    break;

			default:
			    factor = 1;
		    }

		    if ( factor > mShieldCount )
		    {
			// shield not strong enough
			shieldOn = FALSE;
			break;
		    }
		    rockHit( *it );
		    // the more shields we have the less costly
		    reducePower( factor * (SHIELD_HIT_COST - mShieldCount*2) );
		}
	    }
	}

	if ( !shieldOn )
	{
	    shield->hide();
	    QCanvasItemList hits = ship->collisions( TRUE );
	    QCanvasItemList::Iterator it;
	    for ( it = hits.begin(); it != hits.end(); ++it )
	    {
		if ( (*it)->rtti() >= ID_ROCK_LARGE &&
		     (*it)->rtti() <= ID_ROCK_SMALL )
		{
		    KBit *bit;
		    for ( int i = 0; i < 8; i++ )
		    {
		      bit = new KBit( animation[ID_BIT], &field );
		      bit->move( ship->x() + 5 - randDouble() * 10,
				 ship->y() + 5 - randDouble() * 10,
				 randInt(bit->frameCount()) );
		      bit->setVelocity( 1-randDouble()*2,
					1-randDouble()*2 );
		      bit->setDeath( 60 + randInt(60) );
		      bit->show( );
		      bits.append( bit );
		    }
		    ship->hide();
		    shield->hide();
		    emit shipKilled();
		    break;
		}
	    }
	}


	if ( rotateSlow )
	    rotateSlow--;

	if ( rotateL )
	{
	    shipAngle -= rotateSlow ? 1 : rotateRate;
	    if ( shipAngle < 0 )
		shipAngle += SHIP_STEPS;
	}

	if ( rotateR )
	{
	    shipAngle += rotateSlow ? 1 : rotateRate;
	    if ( shipAngle >= SHIP_STEPS )
		shipAngle -= SHIP_STEPS;
	}

	double angle = shipAngle * PI_X_2 / SHIP_STEPS;
	double cosangle = cos( angle );
	double sinangle = sin( angle );

	if ( brakeShip )
	{
	    thrustShip = FALSE;
	    rotateL = FALSE;
	    rotateR = FALSE;
	    rotateRate = ROTATE_RATE;
	    if ( fabs(shipDx) < 2.5 && fabs(shipDy) < 2.5 )
	    {
		shipDx = 0.0;
		shipDy = 0.0;
		ship->setVelocity( shipDx, shipDy );
		brakeShip = FALSE;
	    }
	    else
	    {
		double motionAngle = atan2( -shipDy, -shipDx );
		if ( angle > M_PI )
		    angle -= PI_X_2;
		double angleDiff = angle - motionAngle;
		if ( angleDiff > M_PI )
		    angleDiff = PI_X_2 - angleDiff;
		else if ( angleDiff < -M_PI )
		    angleDiff = PI_X_2 + angleDiff;
		double fdiff = fabs( angleDiff );
		if ( fdiff > 0.08 )
		{
		    if ( angleDiff > 0 )
			rotateL = TRUE;
		    else if ( angleDiff < 0 )
			rotateR = TRUE;
		    if ( fdiff > 0.6 )
			rotateRate = mBrakeCount + 1;
		    else if ( fdiff > 0.4 )
			rotateRate = 2;
		    else
			rotateRate = 1;

		    if ( rotateRate > 5 )
			rotateRate = 5;
		}
		else if ( fabs(shipDx) > 1 || fabs(shipDy) > 1 )
		{
		    thrustShip = TRUE;
		    // we'll make braking a bit faster
		    shipDx += cosangle/6 * (mBrakeCount - 1);
		    shipDy += sinangle/6 * (mBrakeCount - 1);
		    reducePower( BRAKE_ON_COST );
		    addExhaust( ship->x() + 10 - cosangle*11,
				ship->y() + 10 - sinangle*11,
				shipDx-cosangle, shipDy-sinangle,
				mBrakeCount+1 );
		}
	    }
	}

	if ( thrustShip )
	{
	    // The ship has a terminal velocity, but trying to go faster
	    // still uses fuel (can go faster diagonally - don't care).
	    double thrustx = cosangle/8;
	    double thrusty = sinangle/8;
	    if ( fabs(shipDx + thrustx) < MAX_SHIP_SPEED )
		shipDx += thrustx;
	    if ( fabs(shipDy + thrusty) < MAX_SHIP_SPEED )
		shipDy += thrusty;
	    ship->setVelocity( shipDx, shipDy );
	    reducePower( 1 );
	    addExhaust( ship->x() + 10 - cosangle*10,
			ship->y() + 10 - sinangle*10,
			shipDx-cosangle, shipDy-sinangle, 3 );
	}

	ship->setFrame( shipAngle >> 1 );

	if ( shootShip )
	{
	    if ( !shootDelay && (int)missiles.count() < mShootCount + 2 )
	    {
	      KMissile *missile = new KMissile( animation[ID_MISSILE], &field );
	      missile->move( 11+ship->x()+cosangle*11,
			     11+ship->y()+sinangle*11, 0 );
	      missile->setVelocity( shipDx + cosangle*MISSILE_SPEED,
				    shipDy + sinangle*MISSILE_SPEED );
	      missile->show( );
	      missiles.append( missile );
	      shotsFired++;
	      reducePower( 1 );

	      shootDelay = 5;
	    }

	    if ( shootDelay )
	      shootDelay--;
	}

	if ( teleportShip )
	{
	    int ra = rand() % 10;
	    if( ra == 0 )
	    ra += rand() % 20;
	    int xra = ra * 60 + ( (rand() % 20) * (rand() % 20) );
	    int yra = ra * 50 - ( (rand() % 20) * (rand() % 20) );
	    ship->move( xra, yra );
	}

	vitalsChanged = TRUE;
    }
}

// - - -

void KAsteroidsView::processPowerups()
{
    if ( !powerups.isEmpty() )
    {
	// if player gets the powerup remove it from the screen, if option
	// "Can destroy powerups" is enabled and a missile hits the powerup
	// destroy it

	KPowerup *pup;
	QPtrListIterator<KPowerup> it( powerups );

	for( ; it.current(); ++it )
	{
	    pup = it.current();
	    pup->growOlder();

	    if( pup->expired() )
	    {
		powerups.removeRef( pup );
		continue;
	    }

	    wrapSprite( pup );

	    QCanvasItemList hits = pup->collisions( TRUE );
	    QCanvasItemList::Iterator it;
	    for ( it = hits.begin(); it != hits.end(); ++it )
	    {
		if ( (*it) == ship )
		{
		    switch( pup->rtti() )
		    {
		      case ID_ENERGY_POWERUP:
			shipPower += 150;
			if ( shipPower > MAX_POWER_LEVEL )
			    shipPower = MAX_POWER_LEVEL;
			break;
		      case ID_TELEPORT_POWERUP:
			mTeleportCount++;
			break;
		      case ID_BRAKE_POWERUP:
			if ( mBrakeCount < MAX_BRAKES )
			    mBrakeCount++;
			break;
		      case ID_SHIELD_POWERUP:
			if ( mShieldCount < MAX_SHIELDS )
			    mShieldCount++;
			break;
		      case ID_SHOOT_POWERUP:
			if ( mShootCount < MAX_FIREPOWER )
			    mShootCount++;
			break;
		    }

		    powerups.removeRef( pup );
		    vitalsChanged = TRUE;
		}
		else if ( (*it) == shield )
		{
		    powerups.removeRef( pup );
		}
		else if ( (*it)->rtti() == ID_MISSILE )
		{
		    if ( can_destroy_powerups )
		    {
		      powerups.removeRef( pup );
		    }
		}
	    }
	}
    }         // -- if( powerups.isEmpty() )
}

// - - -

void KAsteroidsView::hideShield()
{
    shield->hide();
    mShieldCount = 0;
    shieldOn = FALSE;
}

double KAsteroidsView::randDouble()
{
    int v = rand();
    return (double)v / (double)RAND_MAX;
}

int KAsteroidsView::randInt( int range )
{
    return rand() % range;
}
