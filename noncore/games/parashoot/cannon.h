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
**********************************************************************/
#include <qcanvas.h>

#include "bullet.h"

class Cannon : public QObject, public QCanvasSprite
{
    Q_OBJECT

public:
    Cannon(QCanvas*);   //create cannon
    ~Cannon();          //destroy cannon

    enum Direction{ Left, Right, NoDir };

    void pointCannon(Direction dir);
    void setCoords();  
    double shootAngle();
    void shoot();
    int rtti() const;

int shotsFired() { return shotsfired; };

protected:
    void advance(int stage);

signals:
    void score(int);

private:
    QCanvasPixmapArray* cannonarray;
    int index;
    int cannonx;
    int cannony;
    int barrelxpos;
    int barrelypos;
    int moveDelay;
    Direction movedir;
	int shotsfired;
};
