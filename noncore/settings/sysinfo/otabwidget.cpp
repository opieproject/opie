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

#include <qpe/config.h>
#include <qpe/resource.h>

#include <qcombobox.h>
#include <qlist.h>
#include <qtabbar.h>
#include <qwidgetstack.h>

OTabWidget::OTabWidget( QWidget *parent, const char *name = 0x0,
                        TabStyle s = Global, TabPosition p = Top )
    : QWidget( parent, name )
{
    if ( s == Global )
    {
        Config config( "qpe" );
        config.setGroup( "Appearance" );
        tabBarStyle = ( TabStyle ) config.readNumEntry( "TabStyle", (int) IconTab );
        if ( tabBarStyle <= Global || tabBarStyle > IconList)
        {
            tabBarStyle = IconTab;
        }
        QString pos = config.readEntry( "TabPosition", "Top");
        if ( pos == "Top" )
        {
            tabBarPosition = Top;
        }
        else
        {
            tabBarPosition = Bottom;
        }
    }
    else
    {
        tabBarStyle = s;
        tabBarPosition = p;
    }

    widgetStack = new QWidgetStack( this, "widgetstack" );
    widgetStack->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    widgetStack->setLineWidth( style().defaultFrameWidth() );

    tabBarStack = new QWidgetStack( this, "tabbarstack" );

    tabBar = new QTabBar( tabBarStack, "tabbar" );
    tabBarStack->addWidget( tabBar, 0 );
    connect( tabBar, SIGNAL( selected( int ) ), this, SLOT( slotTabBarSelected( int ) ) );

    tabList = new QComboBox( false, tabBarStack, "tablist" );
    tabBarStack->addWidget( tabList, 1 );
    connect( tabList, SIGNAL( activated( int ) ), this, SLOT( slotTabListSelected( int ) ) );

    if ( tabBarStyle == TextTab || tabBarStyle == IconTab )
    {
        tabBarStack->raiseWidget( tabBar );
    }
    else if ( tabBarStyle == TextList || tabBarStyle == IconList )
    {
        tabBarStack->raiseWidget( tabList );
    }

    if ( tabBarPosition == Bottom )
    {
        tabBar->setShape( QTabBar::RoundedBelow );
    }
}

OTabWidget::~OTabWidget()
{
}

void OTabWidget::addTab( QWidget *child, const QString &icon, const QString &label )
{
    QPixmap iconset = loadSmooth( icon );

    // Add to tabBar
    QTab * tab = new QTab();
    if ( tabBarStyle == IconTab )
    {
        tab->label = QString::null;
    }
    else
    {
        tab->label = label;
    }
    if ( tabBarStyle == IconTab || tabBarStyle == IconList)
    {
        tab->iconset = new QIconSet( iconset );
    }
    int tabid = tabBar->addTab( tab );
    tabBar->setCurrentTab( tab );

    // Add to tabList

    if ( tabBarStyle == IconTab || tabBarStyle == IconList )
    {
        tabList->insertItem( iconset, label, -1 );
    }
    else
    {
        tabList->insertItem( label );
    }
    tabList->setCurrentItem( tabList->count()-1 );

    // Add child to widget list
    widgetStack->addWidget( child, tabid );
    widgetStack->raiseWidget( child );

    // Save tab information
    tabs.append( TabInfo( tabid, child, icon, label ) );

    setUpLayout( FALSE );
}

OTabWidget::TabStyle OTabWidget::tabStyle() const
{
    return tabBarStyle;
}

void OTabWidget::setTabStyle( TabStyle s )
{
    tabBarStyle = s;
}

OTabWidget::TabPosition OTabWidget::tabPosition() const
{
    return tabBarPosition;
}

void OTabWidget::setTabPosition( TabPosition p )
{
    tabBarPosition = p;
}

void OTabWidget::slotTabBarSelected( int id )
{

    TabInfoList::Iterator newtab = tabs.begin();
    while ( newtab != tabs.end() && (*newtab).id() != id )
        newtab++;
    if ( (*newtab).id() == id )
    {
        selectTab( newtab );
    }
}

void OTabWidget::slotTabListSelected( int index )
{
    TabInfoList::Iterator newtab = tabs.at( index );
    if ( newtab != tabs.end() )
    {
        selectTab( newtab );
    }
}

QPixmap OTabWidget::loadSmooth( const QString &name )
{
    QImage image = Resource::loadImage( name );
    QPixmap pixmap;
    pixmap.convertFromImage( image.smoothScale( 16, 16 ) );
    return pixmap;
}

void OTabWidget::selectTab( TabInfoList::Iterator tab )
{
    if ( tabBarStyle == IconTab )
    {
        if ( currentTab != 0x0 )
        {
            tabBar->tab( (*currentTab).id() )->label = QString::null;
        }
        tabBar->tab( (*tab).id() )->label = (*tab).label();
        currentTab = tab;
    }
    tabBar->layoutTabs();
    tabBar->update();

    widgetStack->raiseWidget( (*tab).control() );

    setUpLayout( FALSE );
}

void OTabWidget::setUpLayout( bool onlyCheck )
{
    QSize t( tabBarStack->sizeHint() );
    if ( t.width() > width() )
        t.setWidth( width() );
    int lw = widgetStack->lineWidth();
    if ( tabBarPosition == Bottom )
    {
        tabBarStack->setGeometry( QMAX(0, lw-2), height() - t.height() - lw, t.width(), t.height() );
        widgetStack->setGeometry( 0, 0, width(), height()-t.height()+QMAX(0, lw-2) );
    }
    else
    { // Top
        tabBarStack->setGeometry( QMAX(0, lw-2), 0, t.width(), t.height() );
        widgetStack->setGeometry( 0, t.height()-lw, width(), height()-t.height()+QMAX(0, lw-2));
    }

    if ( !onlyCheck )
        update();
    if ( autoMask() )
        updateMask();
}

QSize OTabWidget::sizeHint() const
{
    QSize s( widgetStack->sizeHint() );
    QSize t( tabBarStack->sizeHint() );
    return QSize( QMAX( s.width(), t.width()), s.height() + t.height() );
}

void OTabWidget::resizeEvent( QResizeEvent * )
{
    setUpLayout( FALSE );
}
