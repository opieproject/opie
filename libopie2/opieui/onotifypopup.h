/*
                             This file is part of the Opie Project
                             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <qwidget.h>

namespace Opie {
namespace Ui   {

/**
 * Provides a speech balloon style pop-up for short notifications.
 * Shows on top of all other widgets but does not steal focus.
 * Example usage:
 *
 * <pre>
 *  ONotifyPopup *pop = new ONotifyPopup();
 *  pop->setText( "Hello world!" );
 *  pop->setPointTo( 230, 300 );
 * </pre>
 *
 * @short A notification popup widget
 * @author Paul Eggleton (bluelightning@bluelightning.org)
 */

class ONotifyPopup: public QWidget
{
  Q_OBJECT

  public:
    ONotifyPopup( QWidget* parent=0, const char* name=0 );
    virtual ~ONotifyPopup();

    QString text() const;
    void setText( const QString &str );
    void setPointTo( int x, int y );

  signals:
    void clicked();

  protected:
    void drawBubble( QPainter *painter, bool outline );
    void resizeEvent(QResizeEvent* event);
    void paintEvent( QPaintEvent * );
    void mouseReleaseEvent( QMouseEvent * );

    QString m_text;
    QSize m_margin;
    int m_ptrheight;
    int m_ptrcentre;
    QPoint m_pointto;
};

}
}