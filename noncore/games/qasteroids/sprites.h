/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#ifndef __SPRITES_H__
#define __SPRITES_H__

#include <qcanvas.h>

#define ID_ROCK_LARGE           1024
#define ID_ROCK_MEDIUM          1025
#define ID_ROCK_SMALL           1026

#define ID_MISSILE              1030

#define ID_BIT                  1040
#define ID_EXHAUST              1041

#define ID_ENERGY_POWERUP       1310
#define ID_TELEPORT_POWERUP     1311
#define ID_BRAKE_POWERUP        1312
#define ID_SHIELD_POWERUP       1313
#define ID_SHOOT_POWERUP        1314

#define ID_SHIP                 1350
#define ID_SHIELD               1351

#define MAX_SHIELD_AGE          350
#define MAX_POWERUP_AGE         500
#define MAX_MISSILE_AGE         20

class KMissile : public QCanvasSprite
{
public:
    KMissile( QCanvasPixmapArray *s, QCanvas *c ) : QCanvasSprite( s, c )
        { myAge = 0; }

    virtual int rtti() const { return ID_MISSILE; }

    void growOlder() { myAge++; }
    bool expired() { return myAge > MAX_MISSILE_AGE; }

private:
    int myAge;
};

class KBit : public QCanvasSprite
{
public:
    KBit( QCanvasPixmapArray *s, QCanvas *c ) : QCanvasSprite( s, c )
	{  death = 7; }

    virtual int rtti() const {  return ID_BIT; }

    void setDeath( int d ) { death = d; }
    void growOlder() { death--; }
    bool expired() { return death <= 0; }

private:
    int death;
};

class KExhaust : public QCanvasSprite
{
public:
    KExhaust( QCanvasPixmapArray *s, QCanvas *c ) : QCanvasSprite( s, c )
	{  death = 1; }

    virtual int rtti() const {  return ID_EXHAUST; }

    void setDeath( int d ) { death = d; }
    void growOlder() { death--; }
    bool expired() { return death <= 0; }

private:
    int death;
};

class KPowerup : public QCanvasSprite
{
public:
  KPowerup( QCanvasPixmapArray *s, QCanvas *c, int t ) : QCanvasSprite( s, c ),
        myAge( 0 ), type(t) { }

  virtual int rtti() const { return type; }

  void growOlder() { myAge++; }
  bool expired() const { return myAge > MAX_POWERUP_AGE; }

protected:
  int myAge;
  int type;
};

class KRock : public QCanvasSprite
{
public:
    KRock (QCanvasPixmapArray *s, QCanvas *c, int t, int sk, int st) : QCanvasSprite( s, c )
        { type = t; skip = cskip = sk; step = st; }

    void nextFrame()
	{
	    if (cskip-- <= 0) {
		setFrame( (frame()+step+frameCount())%frameCount() );
		cskip = QABS(skip);
	    }
	}

    virtual int rtti() const { return type; }

private:
    int type;
    int skip;
    int cskip;
    int step;
};

class KShield : public QCanvasSprite
{
public:
  KShield( QCanvasPixmapArray *s, QCanvas *c )
      : QCanvasSprite( s, c ) {}

  virtual int rtti() const { return ID_SHIELD; }
};

#endif
