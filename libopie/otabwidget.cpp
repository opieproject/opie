/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
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

#include "otabwidget.h"

#include <qpe/config.h>
#include <qpe/resource.h>
#include <opie/otabbar.h>

#include <qcombobox.h>
#include <qwidgetstack.h>

OTabWidget::OTabWidget( QWidget *parent, const char *name, TabStyle s, TabPosition p )
    : QWidget( parent, name )
{
    if ( s == Global )
    {
        Config config( "qpe" );
        config.setGroup( "Appearance" );
        s = ( TabStyle ) config.readNumEntry( "TabStyle", (int) IconTab );
        if ( s <= Global || s > IconList)
        {
            s = IconTab;
        }
        QString pos = config.readEntry( "TabPosition", "Top");
        if ( pos == "Bottom" )
        {
            p = Bottom;
        }
        else
        {
            p = Top;
        }
    }

    widgetStack = new QWidgetStack( this, "widgetstack" );
    widgetStack->setFrameStyle( QFrame::NoFrame );
    widgetStack->setLineWidth( style().defaultFrameWidth() );

    tabBarStack = new QWidgetStack( this, "tabbarstack" );

    tabBar = new OTabBar( tabBarStack, "tabbar" );
    tabBarStack->addWidget( tabBar, 0 );
    connect( tabBar, SIGNAL( selected( int ) ), this, SLOT( slotTabBarSelected( int ) ) );

    tabList = new QComboBox( false, tabBarStack, "tablist" );
    tabBarStack->addWidget( tabList, 1 );
    connect( tabList, SIGNAL( activated( int ) ), this, SLOT( slotTabListSelected( int ) ) );

    setTabStyle( s );
    setTabPosition( p );

    currentTab= 0x0;
}

OTabWidget::~OTabWidget()
{
}

void OTabWidget::addTab( QWidget *child, const QString &icon, const QString &label )
{
    QPixmap iconset = loadSmooth( icon );

    QTab *tab = new QTab();
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

    if ( tabBarStyle == IconTab || tabBarStyle == IconList )
    {
        tabList->insertItem( iconset, label, -1 );
    }
    else
    {
        tabList->insertItem( label );
    }

    widgetStack->addWidget( child, tabid );
    widgetStack->raiseWidget( child );
    widgetStack->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    OTabInfo *tabinfo = new OTabInfo( tabid, child, icon, label );
    tabs.append( tabinfo );
    selectTab( tabinfo );
}

void OTabWidget::removePage( QWidget *childwidget )
{
  if ( childwidget )
    {
        OTabInfo *tab = tabs.first();
        while ( tab && tab->control() != childwidget )
        {
            tab = tabs.next();
        }
        if ( tab && tab->control() == childwidget )
        {
            tabBar->setTabEnabled( tab->id(), FALSE );
            tabBar->removeTab( tabBar->tab( tab->id() ) );
            int i = 0;
            while ( i < tabList->count() && tabList->text( i ) != tab->label() )
            {
                i++;
            }
            if ( tabList->text( i ) == tab->label() )
            {
                tabList->removeItem( i );
            }
            widgetStack->removeWidget( childwidget );
            tabs.remove( tab );
            delete tab;
            currentTab = tabs.current();
            if ( !currentTab )
            {
                widgetStack->setFrameStyle( QFrame::NoFrame );
            }

            setUpLayout();
        }
    }
}

void OTabWidget::setCurrentTab( QWidget *childwidget )
{
    OTabInfo *newtab = tabs.first();
    while ( newtab && newtab->control() != childwidget )
    {
        newtab = tabs.next();
    }
    if ( newtab && newtab->control() == childwidget )
    {
        selectTab( newtab );
    }
}

void OTabWidget::setCurrentTab( const QString &tabname )
{
    OTabInfo *newtab = tabs.first();
    while ( newtab && newtab->label() != tabname )
    {
        newtab = tabs.next();
    }
    if ( newtab && newtab->label() == tabname )
    {
        selectTab( newtab );
    }
}

void OTabWidget::setCurrentTab(int tabindex) {
    OTabInfo *newtab = tabs.first();
    while ( newtab && newtab->id() != tabindex )
    {
        newtab = tabs.next();
    }
    if ( newtab && newtab->id() == tabindex )
    {
        selectTab( newtab );
    }
}


OTabWidget::TabStyle OTabWidget::tabStyle() const
{
    return tabBarStyle;
}

void OTabWidget::setTabStyle( TabStyle s )
{
    tabBarStyle = s;
    if ( tabBarStyle == TextTab || tabBarStyle == IconTab )
    {
        QTab *currtab;
        for (  OTabInfo *tabinfo = tabs.first(); tabinfo; tabinfo = tabs.next() )
        {
            currtab = tabBar->tab( tabinfo->id() );
            if ( tabBarStyle == IconTab )
            {
                currtab->iconset = new QIconSet( loadSmooth( tabinfo->icon() ) );
                if ( tabinfo == currentTab )
                    currtab->setText( tabinfo->label() );
                else
                    currtab->setText( QString::null );
            }
            else
            {
                currtab->iconset = 0x0;
                currtab->setText( tabinfo->label() );
            }
        }
        tabBarStack->raiseWidget( tabBar );
    }
    else if ( tabBarStyle == TextList || tabBarStyle == IconList )
    {
        tabList->clear();
        for (  OTabInfo *tabinfo = tabs.first(); tabinfo; tabinfo = tabs.next() )
        {
            if ( tabBarStyle == IconList )
            {
                tabList->insertItem( loadSmooth( tabinfo->icon() ), tabinfo->label() );
            }
            else
            {
                tabList->insertItem( tabinfo->label() );
            }
        }
        tabBarStack->raiseWidget( tabList );
    }
    setUpLayout();
}

OTabWidget::TabPosition OTabWidget::tabPosition() const
{
    return tabBarPosition;
}

void OTabWidget::setTabPosition( TabPosition p )
{
    tabBarPosition = p;
    if ( tabBarPosition == Top )
    {
        tabBar->setShape( QTabBar::RoundedAbove );
    }
    else
    {
        tabBar->setShape( QTabBar::RoundedBelow );
    }
    setUpLayout();
}

void OTabWidget::slotTabBarSelected( int id )
{
    OTabInfo *newtab = tabs.first();
    while ( newtab && newtab->id() != id )
    {
        newtab = tabs.next();
    }
    if ( newtab && newtab->id() == id )
    {
        selectTab( newtab );
    }
}

void OTabWidget::slotTabListSelected( int index )
{
    OTabInfo *newtab = tabs.at( index );
    if ( newtab )
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

void OTabWidget::selectTab( OTabInfo *tab )
{
    if ( tabBarStyle == IconTab )
    {
        if ( currentTab )
        {
            tabBar->tab( currentTab->id() )->setText( QString::null );
            setUpLayout();
        }
        tabBar->tab( tab->id() )->setText( tab->label() );
        tabBar->setCurrentTab( tab->id() );
        setUpLayout();
        tabBar->update();
    }
    else
    {
        tabBar->setCurrentTab( tab->id() );
    }

    widgetStack->raiseWidget( tab->control() );

    emit currentChanged( tab->control() );

    currentTab = tab;
}

void OTabWidget::setUpLayout()
{
    tabBar->layoutTabs();
    QSize t( tabBarStack->sizeHint() );
    if ( tabBarStyle == IconTab )
    {
        if ( t.width() > width() )
            t.setWidth( width() );
    }
    else
    {
        t.setWidth( width() );
    }
    int lw = widgetStack->lineWidth();
    if ( tabBarPosition == Bottom )
    {
        tabBarStack->setGeometry( QMAX(0, lw-2), height() - t.height() - lw, t.width(), t.height() );
        widgetStack->setGeometry( 0, 0, width(), height()-t.height()+QMAX(0, lw-2) );
    }
    else
    {
        tabBarStack->setGeometry( QMAX(0, lw-2), 0, t.width(), t.height() );
        widgetStack->setGeometry( 0, t.height()-lw, width(), height()-t.height()+QMAX( 0, lw-2 ) );
    }

    if ( autoMask() )
        updateMask();
}

QSize OTabWidget::sizeHint() const
{
    QSize s( widgetStack->sizeHint() );
    QSize t( tabBarStack->sizeHint() );
    return QSize( QMAX( s.width(), t.width() ), s.height() + t.height() );
}

void OTabWidget::resizeEvent( QResizeEvent * )
{
    setUpLayout();
}

int OTabWidget::getCurrentTab()
{
    if ( currentTab )
    {
        return currentTab->id();
    }
    return -1;
}

