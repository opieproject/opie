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
#include <qtimer.h>

class Snake : public QObject
{
	Q_OBJECT

public:
   enum Direction{ left, right, up, down};

   Snake(QCanvas*);
   ~Snake();
   void go(int newkey);
   void move(Direction dir);
   void changeHead(int last);
   void changeTail();
   void detectCrash();
   void createSnake();
   void extendSnake(); 
   int lookUpPiece(Direction currentdir, Direction newdir);
   void setScore(int amount);
   int getScore();

signals:
   void dead();
   void targethit();
   void scorechanged();

private slots:
   void moveSnake();
   void increaseSpeed();

private:
   QCanvasPixmapArray* snakeparts;
   QList<QCanvasSprite>snakelist;
   QTimer* autoMoveTimer; 
   QCanvas* canvas;
   int grow;
   int last;
   int speed;
   int score;
   Direction currentdir;
   Direction newdir;
};

