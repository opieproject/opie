/*
                            This file is part of the Opie Project

                 Copyright (c)  2002 S. Prud'homme <prudhomme@laposte.net>
                                     Dan Williams <williamsdr@acm.org>
              =.
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

#include "colorpopupmenu.h"
#include "colordialog.h"

#include <qaction.h>
#include <qlayout.h>
#include <qpainter.h>

ColorPanelButton::ColorPanelButton( const QColor& color, QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    m_color = color;

    setFixedSize( 16, 16 );
    setActive( FALSE );
}

ColorPanelButton::~ColorPanelButton()
{
}

void ColorPanelButton::setActive( bool active )
{
    m_active = active;

    if ( m_active ) {
        setFrameStyle( Panel | Sunken );
    } else {
        setFrameStyle( NoFrame );
    }
}

void ColorPanelButton::enterEvent( QEvent* e )
{
    Q_UNUSED( e )

    if ( !m_active ) {
        setFrameStyle( Panel | Sunken );
    }
}

void ColorPanelButton::leaveEvent( QEvent* e )
{
    Q_UNUSED( e )

    if ( !m_active ) {
        setFrameStyle( NoFrame );
    }
}

void ColorPanelButton::paintEvent( QPaintEvent* e )
{
    QFrame::paintEvent( e );

    QPainter painter;
    painter.begin( this );
    painter.fillRect( 2, 2, 12, 12, m_color );
    painter.setPen( Qt::black );
    painter.drawRect( 2, 2, 12, 12 );
    painter.end();
}

void ColorPanelButton::mouseReleaseEvent( QMouseEvent* e )
{
    Q_UNUSED( e )

    emit selected( m_color );
}

ColorPopupMenu::ColorPopupMenu( const QColor& color, QWidget* parent, const char* name )
    : QPopupMenu( parent, name )
{
    m_color = color;

    colorPanel = new QWidget( this );

    colorLayout = new QGridLayout(colorPanel, 5, 6);

    addColor(QColor(255, 255, 255), 0, 1);
    addColor(QColor(192, 192, 192), 0, 2);
    addColor(QColor(128, 128, 128), 0, 3);
    addColor(QColor(64, 64, 64), 0, 4);
    addColor(QColor(0, 0, 0), 0, 5);

    addColor(QColor(255, 0, 0), 1, 0);
    addColor(QColor(255, 128, 0), 1, 1);
    addColor(QColor(255, 255, 0), 1, 2);
    addColor(QColor(128, 255, 0), 1, 3);
    addColor(QColor(0, 255, 0), 1, 4);
    addColor(QColor(0, 255, 128), 1, 5);

    addColor(QColor(128, 0, 0), 2, 0);
    addColor(QColor(128, 64, 0), 2, 1);
    addColor(QColor(128, 128, 0), 2, 2);
    addColor(QColor(64, 128, 0), 2, 3);
    addColor(QColor(0, 128, 0), 2, 4);
    addColor(QColor(0, 128, 64), 2, 5);

    addColor(QColor(0, 255, 255), 3, 0);
    addColor(QColor(0, 128, 255), 3, 1);
    addColor(QColor(0, 0, 255), 3, 2);
    addColor(QColor(128, 0, 255), 3, 3);
    addColor(QColor(255, 0, 255), 3, 4);
    addColor(QColor(255, 0, 128), 3, 5);

    addColor(QColor(0, 128, 128), 4, 0);
    addColor(QColor(0, 64, 128), 4, 1);
    addColor(QColor(0, 0, 128), 4, 2);
    addColor(QColor(64, 0, 128), 4, 3);
    addColor(QColor(128, 0, 128), 4, 4);
    addColor(QColor(128, 0, 64), 4, 5);

    insertItem( colorPanel );
    insertSeparator();
    QAction* chooseColorAction = new QAction( tr( "More" ), tr( "More..." ), 0, colorPanel, "More" );
    connect( chooseColorAction, SIGNAL( activated() ), this, SLOT( moreColorClicked() ) );
    chooseColorAction->addTo( this );
    activateItemAt( 0 );
}

ColorPopupMenu::~ColorPopupMenu()
{
}

void ColorPopupMenu::addColor( const QColor& color, int row, int col )
{
    ColorPanelButton* panelButton = new ColorPanelButton( color, colorPanel );
    connect( panelButton, SIGNAL( selected( const QColor& ) ), this, SLOT( buttonSelected( const QColor& ) ) );
    colorLayout->addWidget( panelButton, row, col );
}

void ColorPopupMenu::buttonSelected( const QColor& color )
{
    m_color = color;
    emit colorSelected( color );
    hide();
}

void ColorPopupMenu::moreColorClicked()
{
    QColor color = OColorDialog::getColor( m_color );
    m_color = color;
    emit colorSelected( color );
    hide();
}
