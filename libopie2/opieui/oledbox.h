/*
                             This file is part of the Opie Project
              =.             (C) 2002-2005 the Opie Team <opie-devel@lists.sourceforge.net>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
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

#ifndef OLEDBOX_H
#define OLEDBOX_H

/* QT */
#include <qwidget.h>
#include <qcolor.h>

class QPixmap;

namespace Opie {
namespace Ui   {

class OLedBox : public QWidget
{
  Q_OBJECT

  public:
	OLedBox( const QColor& col = red, QWidget* parent = 0, const char* name = 0 );
	virtual ~OLedBox();

	QColor color() const;
	bool isOn() const;
	void setReadOnly( bool R ) { m_readonly = R; }
	bool readOnly( void ) const { return m_readonly; }

	virtual QSize sizeHint() const;

  public slots:
	void toggle();
	void setOn( bool on );
	void setColor( const QColor& col );

  signals:
	void toggled( bool );

  protected:
	virtual void paintEvent( QPaintEvent* e );
	virtual void resizeEvent( QResizeEvent* e );

	virtual void mousePressEvent( QMouseEvent* e );

  private:
	void drawLed( QPixmap *, const QColor& col );

  private:
	QPixmap *m_pix [2];

	QColor m_color;
	bool m_on;
	bool m_readonly;

	static QPixmap *s_border_pix;
};
};
};
#endif
