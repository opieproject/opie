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
#include <qcolor.h>
/*!
 * @class OTicker
 * @brief The OTicker class provides a QLabel widget that scroll its contents
 *
*/
class OTicker : public QLabel {
//class OTicker : public QFrame {
    Q_OBJECT

public:

/*!
 * @fn OTicker( QWidget* parent = 0 )
 * @brief Object constructor.
 *
 * @param parent Pointer to parent of this control.

 * Constructs a new OTicker control with parent
 */
    OTicker( QWidget* parent=0 );
/*!
 * @fn ~OTicker()
 * @brief Object destructor.
 */
    ~OTicker();
/*!
 * @fn setText()
 * @brief sets text to be displayed
 * @param text QString text to be displayed.
 *
 */
    void setText( const QString& text ) ;
/*!
 * @fn setBackgroundColor(QColor color)
 * @brief sets color of the ticker's background
 * @param color QColor color to be set.
 *
 */
    void setBackgroundColor(QColor color);
/*!
 * @fn setForegroundColor(QColor color)
 * @brief sets color of text
 * @param color QColor color of text
 *
 */
    void setForegroundColor(QColor color);
/*!
 * @fn setFrame(int style)
 * @brief sets frame style
 * @param style int Frame style to be see. See Qt::WidgetFlags.
 *
 */
    void setFrame(int style); 
/*!
 * @fn setUpdateTime(int timeout)
 * @brief sets time of update
 * @param timeout int time in milliseconds between updates.
 *
 */
    void setUpdateTime(int timeout);
/*!
 * @fn setScrollLength(int length)
 * @brief sets amount of scrolling default is 1
 * @param length int scroll length.
 *
 */
    void setScrollLength(int length); 
signals:
/*!
 * @fn mousePressed()
 * @brief signal mouse press event
 *
 */
    void mousePressed();
protected:
/*!
 * @fn timerEvent( QTimerEvent * e)
 * @brief timer timeout event
 * @param e QEvent see QEvent.
 *
 */
    void timerEvent( QTimerEvent * e);
/*!
 * @fn drawContents( QPainter *p )
 * @brief draws widget contents
 * @param p QPainter. see QPainter
 *
 */
    void drawContents( QPainter *p );
/*!
 * @fn mouseReleaseEvent( QMouseEvent *e)
 * @brief mouse release event
 * @param e QMouseEvent. see QMouseEvent.
 *
 */
    void mouseReleaseEvent( QMouseEvent *e);
private:
    QColor backgroundcolor, foregroundcolor;
    QString scrollText;
    QPixmap scrollTextPixmap;
    int pos, updateTimerTime, scrollLength;
};

#endif
