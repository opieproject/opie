/**********************************************************************
** OTabWidget
**
** Modified tab widget control
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

#include "otabwidget.h"

#include <qpe/resource.h>

#include <qlist.h>
#include <qtabbar.h>


OTabWidget::OTabWidget( QWidget *parent, const char *name )
    : QTabWidget( parent, name )
{
    connect( this, SIGNAL( currentChanged( QWidget * ) ),
             this, SLOT( tabChangedSlot( QWidget * ) ) );
}

OTabWidget::~OTabWidget()
{
}

void OTabWidget::addTab( QWidget *child, const QString &icon, const QString &label )
{
    Tabs.append( TabInfo( child, icon, label ) );
    QTabWidget::addTab( child, loadSmooth( icon ), QString::null );
}

void OTabWidget::tabChangedSlot( QWidget *child )
{
    TabInfoList::Iterator it;

    if ( CurrentTab != 0x0 )
    {
        changeTab( (*CurrentTab).control(), loadSmooth( (*CurrentTab).icon() ), QString::null );
    }

    for ( it = Tabs.begin(); it != Tabs.end(); ++it )
    {
        if ( (*it).control() == child )
        {
            CurrentTab = it;
            changeTab( (*CurrentTab).control(), loadSmooth( (*CurrentTab).icon() ), (*CurrentTab).label() );
        }
    }
}

QPixmap OTabWidget::loadSmooth( const QString &name )
{
    QImage image = Resource::loadImage( name );
    QPixmap pixmap;
    pixmap.convertFromImage( image.smoothScale( 16, 16 ) );
    return pixmap;
}

