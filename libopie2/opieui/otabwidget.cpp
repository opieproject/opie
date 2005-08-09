/*
                             This file is part of the Opie Project

                             Copyright (C) 2002, 2005 Dan Williams <drw@handhelds.org>
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

#include <opie2/otabwidget.h>

/* OPIE */
#include <opie2/oresource.h>
#include <opie2/otabbar.h>

#include <qpe/config.h>

/* QT */
#include <qcombobox.h>
#include <qwidgetstack.h>

using namespace Opie::Ui;

OTabWidget::OTabWidget( QWidget *parent, const char *name, TabStyle s, TabPosition p )
        : QWidget( parent, name )
        , m_currTab( 0l )
        , m_tabBarStyle( Global )
        , m_tabBarPosition( Top )
        , m_usingTabs( true )
        , m_tabBar( 0l )
        , m_tabList( 0l )
{
    if ( s == Global )
    {
        // Read Opie global settings for style and position
        Config config( "qpe" );
        config.setGroup( "Appearance" );

        // Style
        s = ( TabStyle ) config.readNumEntry( "TabStyle", (int) IconTab );
        if ( s <= Global || s > IconList)
            s = IconTab;

        // Position
        ( config.readEntry( "TabPosition", "Top" ) == "Bottom" ) ? p = Bottom
                                                                 : p = Top;
    }

    // Initialize widget stack for tab widgets
    m_widgetStack = new QWidgetStack( this );
    m_widgetStack->setFrameStyle( QFrame::NoFrame );
    m_widgetStack->setLineWidth( style().defaultFrameWidth() );

    // Set initial selector control style and position
    setTabStyle( s );
    setTabPosition( p );
}

OTabWidget::~OTabWidget()
{
    m_tabs.setAutoDelete( true );
    m_tabs.clear();
}

void OTabWidget::addTab( QWidget *child, const QString &icon, const QString &label )
{
    int tabid = -1;

    if ( m_usingTabs )
    {
        // Create new tab in tab bar
        QTab *tab = new QTab();

        // Set label (and icon if necessary)
        if ( m_tabBarStyle == IconTab )
        {
            tab->label = QString::null;
            tab->iconset = new QIconSet( Opie::Core::OResource::loadPixmap( icon, Opie::Core::OResource::SmallIcon ) );
        }
        else
            tab->label = label;

        tabid = m_tabBar->addTab( tab );
    }
    else
    {
        // Insert entry (with icon if necessary) into drop down list
        if ( m_tabBarStyle == IconList )
            m_tabList->insertItem( Opie::Core::OResource::loadPixmap( icon, Opie::Core::OResource::SmallIcon ), label, -1 );
        else
            m_tabList->insertItem( label );
    }

    // Add widget to stack
    m_widgetStack->addWidget( child, tabid );
    m_widgetStack->raiseWidget( child );
    m_widgetStack->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    // Keep track of tab information
    OTabInfo *tabinfo = new OTabInfo( tabid, child, icon, label );
    m_tabs.append( tabinfo );

    // Force resizing of child controls
    resizeEvent( 0x0 );

    // Make newly added tab the current one displayed
    selectTab( tabinfo );
}

void OTabWidget::removePage( QWidget *childwidget )
{
    if ( childwidget )
    {
        // Find tab information for desired widget
        OTabInfo *tab = m_tabs.first();
        while ( tab && tab->control() != childwidget )
            tab = m_tabs.next();

        if ( tab && tab->control() == childwidget )
        {
            if ( m_usingTabs )
            {
                // Remove tab from tab bar
                m_tabBar->setTabEnabled( tab->id(), false );
                m_tabBar->removeTab( m_tabBar->tab( tab->id() ) );
            }
            else
            {
                // Remove entry from drop down list
                int i = 0;
                while ( i < m_tabList->count() && m_tabList->text( i ) != tab->label() )
                    i++;
                if ( m_tabList->text( i ) == tab->label() )
                    m_tabList->removeItem( i );
            }

            // Remove widget from stack
            m_widgetStack->removeWidget( childwidget );

            // Get rid of tab information
            m_tabs.remove( tab );
            delete tab;

            // Reset current tab
            m_currTab = m_tabs.current();
            if ( !m_currTab )
                m_widgetStack->setFrameStyle( QFrame::NoFrame );

            // Redraw widget
            setUpLayout();
        }

        // Force resizing of child controls
        resizeEvent( 0x0 );
    }
}

void OTabWidget::changeTab( QWidget *widget, const QString &iconset, const QString &label)
{
    // Find tab information for desired widget
    OTabInfo *currtab = m_tabs.first();
    while ( currtab && currtab->control() != widget )
        currtab = m_tabs.next();

    if ( currtab && currtab->control() == widget )
    {
        QPixmap icon( Opie::Core::OResource::loadPixmap( iconset, Opie::Core::OResource::SmallIcon ) );

        if ( m_usingTabs )
        {
            // Update tab label and icon (if necessary)
            QTab *tab = m_tabBar->tab( currtab->id() );
            tab->setText( label );
            if ( m_tabBarStyle == IconTab )
                tab->setIconSet( icon );
        }
        else
        {
            // Update entry label and icon (if necessary)
            int i = 0;
            while ( i < m_tabList->count() && m_tabList->text( i ) != currtab->label() )
                i++;
            if ( i < m_tabList->count() && m_tabList->text( i ) == currtab->label() )
            {
                if ( m_tabBarStyle == IconList )
                    m_tabList->changeItem( icon, label, i );
                else
                    m_tabList->changeItem( label, i );
            }
        }

        // Update tab information
        currtab->setLabel( label );
        currtab->setIcon( iconset );

        // Force resizing of child controls
        resizeEvent( 0x0 );

        // Redraw widget
        setUpLayout();
    }
}

void OTabWidget::setCurrentTab( QWidget *childwidget )
{
    OTabInfo *currtab = m_tabs.first();
    while ( currtab && currtab->control() != childwidget )
    {
        currtab = m_tabs.next();
    }
    if ( currtab && currtab->control() == childwidget )
    {
        selectTab( currtab );
    }
}

void OTabWidget::setCurrentTab( const QString &tabname )
{
    OTabInfo *newtab = m_tabs.first();
    while ( newtab && newtab->label() != tabname )
    {
        newtab = m_tabs.next();
    }
    if ( newtab && newtab->label() == tabname )
    {
        selectTab( newtab );
    }
}

void OTabWidget::setCurrentTab(int tabindex)
{
    OTabInfo *newtab = m_tabs.first();
    while ( newtab && newtab->id() != tabindex )
    {
        newtab = m_tabs.next();
    }
    if ( newtab && newtab->id() == tabindex )
    {
        selectTab( newtab );
    }
}


OTabWidget::TabStyle OTabWidget::tabStyle() const
{
    return m_tabBarStyle;
}

void OTabWidget::setTabStyle( TabStyle s )
{
    // Get out if new and current styles are the same
    if ( s == m_tabBarStyle )
        return;

    // Delete current selector control
    if ( m_usingTabs )
    {
        delete m_tabBar;
        m_tabBar = 0l;
    }
    else
    {
        delete m_tabList;
        m_tabList = 0l;
    }

    // Set new style information
    m_tabBarStyle = s;
    m_usingTabs = ( m_tabBarStyle == TextTab || m_tabBarStyle == IconTab );

    // Create new selector control and populate with tab information
    if ( m_usingTabs )
    {
        // Create new tab bar selector
        m_tabBar = new OTabBar( this );
        connect( m_tabBar, SIGNAL(selected(int)), this, SLOT(slotTabBarSelected(int)) );

        // Add all current tabs to tab bar
        for (  OTabInfo *tabinfo = m_tabs.first(); tabinfo; tabinfo = m_tabs.next() )
        {
            // Create new tab in tab bar
            QTab *tab = new QTab();

            // Set label (and icon if necessary)
            if ( m_tabBarStyle == IconTab )
            {
                tab->label = QString::null;
                tab->iconset = new QIconSet( Opie::Core::OResource::loadPixmap( tabinfo->icon(), Opie::Core::OResource::SmallIcon ) );
            }
            else
                tab->label = tabinfo->label();

            // Add tab and save its Id
            int tabid = m_tabBar->addTab( tab );
            tabinfo->setId( tabid );
        }
    }
    else
    {
        // Create new drop down list selector
        m_tabList = new QComboBox( false, this );
        connect( m_tabList, SIGNAL(activated(int)), this, SLOT(slotTabListSelected(int)) );

        // Add all current tabs to drop down list
        for (  OTabInfo *tabinfo = m_tabs.first(); tabinfo; tabinfo = m_tabs.next() )
        {
            if ( m_tabBarStyle == IconList )
                m_tabList->insertItem( Opie::Core::OResource::loadPixmap( tabinfo->icon(), Opie::Core::OResource::SmallIcon ),
                                       tabinfo->label() );
            else
                m_tabList->insertItem( tabinfo->label() );
        }
    }

    // Redraw widget
    setUpLayout();
}

OTabWidget::TabPosition OTabWidget::tabPosition() const
{
    return m_tabBarPosition;
}

void OTabWidget::setTabPosition( TabPosition p )
{
    m_tabBarPosition = p;

    // If using the tab bar selector, set its shape
    if ( m_usingTabs )
    {
        ( m_tabBarPosition == Top ) ? m_tabBar->setShape( QTabBar::RoundedAbove )
                                    : m_tabBar->setShape( QTabBar::RoundedBelow );
    }

    // Redraw widget
    setUpLayout();
}

void OTabWidget::slotTabBarSelected( int id )
{
    OTabInfo *newtab = m_tabs.first();
    while ( newtab && newtab->id() != id )
        newtab = m_tabs.next();

    if ( newtab && newtab->id() == id )
        selectTab( newtab );
}

void OTabWidget::slotTabListSelected( int index )
{
    OTabInfo *newtab = m_tabs.at( index );
    if ( newtab )
        selectTab( newtab );
}

void OTabWidget::selectTab( OTabInfo *tab )
{
    if ( m_tabBarStyle == IconTab )
    {
        // Remove text label from currently selected tab
        if ( m_currTab )
        {
            m_tabBar->tab( m_currTab->id() )->setText( QString::null );
            //setUpLayout();
        }

        // Set text label for newly selected tab
        m_tabBar->tab( tab->id() )->setText( tab->label() );
        m_tabBar->setCurrentTab( tab->id() );

        setUpLayout();

        QSize t;

        t = m_tabBar->sizeHint();
        if ( t.width() > width() )
            t.setWidth( width() );
        int lw = m_widgetStack->lineWidth();
        if ( m_tabBarPosition == Bottom )
            m_tabBar->setGeometry( QMAX(0, lw-2), height() - t.height() - lw, t.width(), t.height() );
        else
            m_tabBar->setGeometry( QMAX(0, lw-2), 0, t.width(), t.height() );
    }
    else if ( m_tabBarStyle == TextTab )
    {
        m_tabBar->setCurrentTab( tab->id() );
    }

    m_widgetStack->raiseWidget( tab->control() );

    emit currentChanged( tab->control() );

    m_currTab = tab;
}

void OTabWidget::setUpLayout()
{
    if ( m_usingTabs )
    {
        m_tabBar->update();
        m_tabBar->layoutTabs();
    }
}

void OTabWidget::resizeEvent( QResizeEvent * )
{
    QSize t;

    if ( m_usingTabs )
    {
        m_tabBar->layoutTabs();
        t = m_tabBar->sizeHint();
        if ( t.width() > width() )
            t.setWidth( width() );
    }
    else
    {
        t = m_tabList->sizeHint();
        t.setWidth( width() );
    }

    int lw = m_widgetStack->lineWidth();
    if ( m_tabBarPosition == Bottom )
    {
        if ( m_usingTabs )
            m_tabBar->setGeometry( QMAX(0, lw-2), height() - t.height() - lw, t.width(), t.height() );
        else
            m_tabList->setGeometry( QMAX(0, lw-2), height() - t.height() - lw, t.width(), t.height() );

        m_widgetStack->setGeometry( 0, 0, width(), height()-t.height()+QMAX(0, lw-2) );
    }
    else
    {
        if ( m_usingTabs )
            m_tabBar->setGeometry( QMAX(0, lw-2), 0, t.width(), t.height() );
        else
            m_tabList->setGeometry( QMAX(0, lw-2), 0, t.width(), t.height() );

        m_widgetStack->setGeometry( 0, t.height()-lw, width(), height()-t.height()+QMAX( 0, lw-2 ) );
    }

    if ( autoMask() )
        updateMask();
}

int OTabWidget::currentTab()
{
    if ( m_currTab )
    {
        return m_currTab->id();
    }
    return -1;
}

QWidget* OTabWidget::currentWidget()const
{
    if ( m_currTab )
    {
        return m_currTab->control();
    }

    return 0;
}
