/*
                            This file is part of the Opie Project
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef OTICKER_H
#define OTICKER_H

#include <qwidget.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qslider.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qcolor.h>

class OTicker : public QLabel {
//class OTicker : public QFrame {
    Q_OBJECT

public:
    OTicker( QWidget* parent=0 );
    ~OTicker();
    void setText( const QString& text ) ;
    void setBackgroundColor(QColor color); //sets background 
    void setForegroundColor(QColor color); //sets text color
    void setFrame(int); //sets frame style
    void setUpdateTime(int); //sets timeout for redraws default is 50 ms
    void setScrollLength(int); //sets amount of scrolling default is 1
signals:
    void mousePressed(); // for mouse press events
protected:
    void timerEvent( QTimerEvent * );
    void drawContents( QPainter *p );
    void mouseReleaseEvent ( QMouseEvent *);
private:
    QColor backgroundcolor, foregroundcolor;
    QString scrollText;
    QPixmap scrollTextPixmap;
    int pos, updateTimerTime, scrollLength;
};

#endif
