/**********************************************************************
** EditScheme
**
** Dialog for editing color scheme
**
** Copyright (C) 2002, Dan Williams
**                    williamsdr@acm.org
**                    http://draknor.net
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "editScheme.h"

#include "opie/colorpopupmenu.h"

#include <qaction.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qscrollview.h>
#include <qtoolbutton.h>

EditScheme::EditScheme( QWidget* parent,  const char* name, bool modal, WFlags fl,
                        int max, QString list[], QString colors[] )
    : QDialog( parent, name, modal, fl )
{
    setCaption( tr( "Edit scheme" ) );
    QGridLayout* layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );


    maxCount = max;
    int i;
    QLabel* label;
    ColorPopupMenu* colorPopupMenu;
    for ( i = 0; i < max; i++ )
    {
        colorList[i] = colors[i];
        surfaceList[i] = list[i];
        label = new QLabel( tr( surfaceList[i] ), this );
        layout->addWidget( label, i, 0 );
        colorButtons[i] = new QToolButton( this, list[i] );
        colorButtons[i]->setPalette( QPalette( QColor( colors[i] ) ) );
        layout->addWidget( colorButtons[i], i, 1 );

        colorPopupMenu = new ColorPopupMenu( colors[i], 0, list[i] );
        colorButtons[i]->setPopup( colorPopupMenu );
        colorButtons[i]->setPopupDelay( 0 );
        connect( colorPopupMenu, SIGNAL( colorSelected( const QColor& ) ), this, SLOT( changeColor( const QColor& ) ) );
    }
}

EditScheme::~EditScheme()
{
}

void EditScheme::changeColor( const QColor& color )
{
    QString name( sender()->name() );
    int i;

    for ( i = 0; i < maxCount; i++ )
    {
        if ( name == colorButtons[i]->name() )
        {
            break;
        }
    }
    if ( i < maxCount && name == colorButtons[i]->name() )
    {
        colorList[i] = color.name();
        colorButtons[i]->setPalette( QPalette( QColor( colorList[i] ) ) );
    }
}
