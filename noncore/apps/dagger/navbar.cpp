/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "navbar.h"

#include <qpe/config.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qlineedit.h>

NavBar::NavBar( QMainWindow *parent )
    : QToolBar( QString::null, parent, QMainWindow::Top, true )
{
    // Initialize UI
    m_actionPrevPage = new QAction( tr( "Previous page" ), Resource::loadPixmap( "fastback" ),
                                       QString::null, 0, this, 0 );
    m_actionPrevPage->addTo( this );
    connect( m_actionPrevPage, SIGNAL(activated()), this, SIGNAL(prevPage()) );

    m_actionPrevVerse = new QAction( tr( "Previous verse" ), Resource::loadPixmap( "back" ),
                                     QString::null, 0, this, 0 );
    m_actionPrevVerse->addTo( this );
    connect( m_actionPrevVerse, SIGNAL(activated()), this, SIGNAL(prevVerse()) );

    m_key = new QLineEdit( this );
    setStretchableWidget( m_key );
    connect(m_key, SIGNAL(textChanged(const QString &)), this, SIGNAL(keyChanged(const QString &)) );

    m_actionNextVerse = new QAction( tr( "Next verse" ), Resource::loadPixmap( "forward" ),
                                     QString::null, 0, this, 0 );
    m_actionNextVerse->addTo( this );
    connect( m_actionNextVerse, SIGNAL(activated()), this, SIGNAL(nextVerse()) );

    m_actionNextPage = new QAction( tr( "Next page" ), Resource::loadPixmap( "fastforward" ),
                                       QString::null, 0, this, 0 );
    m_actionNextPage->addTo( this );
    connect( m_actionNextPage, SIGNAL(activated()), this, SIGNAL(nextPage()) );

    addSeparator();

    m_scrollRate = new QSpinBox( 1, 100, 1, this );
    m_scrollRate->setMinimumWidth( 35 );
    connect( m_scrollRate, SIGNAL(valueChanged(int)), this, SIGNAL(scrollRateChanged(int)) );

    m_actionScroll = new QAction( tr( "Auto-scroll" ), Resource::loadPixmap( "dagger/autoscroll" ),
                                  QString::null, 0, this, 0 );
    m_actionScroll->setToggleAction( true );
    connect( m_actionScroll, SIGNAL(toggled(bool)), this, SIGNAL(autoScroll(bool)) );
    m_actionScroll->addTo( this );

    if ( parent )
    {
        installEventFilter( parent );
        m_key->installEventFilter( parent );
    }
}

void NavBar::navBtnsEnable( bool enabled )
{
    m_actionPrevPage->setEnabled( enabled );
    m_actionPrevVerse->setEnabled( enabled );
    m_actionNextVerse->setEnabled( enabled );
    m_actionNextPage->setEnabled( enabled );
    m_scrollRate->setEnabled( enabled );
    m_actionScroll->setEnabled( enabled );
}

void NavBar::setKey( const QString &newKey )
{
    disconnect( m_key, SIGNAL(textChanged(const QString &)), 0, 0 );
    m_key->setText( newKey );
    connect(m_key, SIGNAL(textChanged(const QString &)), this, SIGNAL(keyChanged(const QString &)) );
}

void NavBar::setAutoScrollRate( int scrollRate )
{
    m_scrollRate->setValue( scrollRate );
}
