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

#include <qpe/sound.h>

#include <qcanvas.h>

class Helicopter : public QCanvasSprite
{

public:
    Helicopter(QCanvas*);
    ~Helicopter();
    void advance(int phase);
    void checkCollision();
    void dropman();
    void takeOff();
    void done();

    static void silenceAll();
    static void deleteAll();

    int rtti() const;
 
private:
    int hits;
    Sound chikachika;
};
