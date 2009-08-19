/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
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

#include <qaction.h>
#include <qlineedit.h>
#include <qwhatsthis.h>

#include <opie2/oresource.h>

#include "mainwindow.h"
#include "quickeditimpl.h"



QuickEditImpl::QuickEditImpl( QWidget* parent, bool visible, const QValueList<QPixmap> &pic_priority )
    : QToolBar( (QMainWindow *)parent ), Todo::QuickEdit( (Todo::MainWindow *)parent ), m_pic_priority( pic_priority )
{
    setHorizontalStretchable( TRUE );

    QPixmap priority3 = m_pic_priority[2];

    m_prio = new QToolButton( this );
    m_prio->setPixmap( priority3 );
    m_prio->setFixedWidth( priority3.width() + 2 );
    connect(m_prio, SIGNAL(clicked() ), this, SLOT(slotPrio()) );
    QWhatsThis::add( m_prio, QWidget::tr( "Click here to set the priority of new task.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );

    m_edit = new QLineEdit( this );
    setStretchableWidget( m_edit );
    QWhatsThis::add( m_edit, QWidget::tr( "Enter description of new task here.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );

    /*
     * it's not implemented and won't be implemented for 1.0
     */
#if 0
    QAction *a = new QAction( QWidget::tr( "More" ), Opie::Core::OResource::loadPixmap( "todo/more", Opie::Core::OResource::SmallIcon ),
                              QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotMore() ) );
    a->addTo( this );
    a->setWhatsThis( QWidget::tr( "Click here to enter additional information for new task.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );
#endif

    QAction *a = new QAction( QWidget::tr( "Enter" ), Opie::Core::OResource::loadPixmap( "enter", Opie::Core::OResource::SmallIcon ),
                              QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotEnter() ) );
    a->addTo( this );
    a->setWhatsThis( QWidget::tr( "Click here to add new task.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );

    a = new QAction( QWidget::tr( "Cancel" ), Opie::Core::OResource::loadPixmap( "close", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotCancel() ) );
    a->addTo( this );
    a->setWhatsThis( QWidget::tr( "Click here to reset new task information.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );

    m_visible = visible;
    if ( !m_visible ) {
        hide();
    }

    m_menu = 0l;
    reinit();
}

QuickEditImpl::~QuickEditImpl()
{
}

OPimTodo QuickEditImpl::todo() const
{
    return m_todo;
}

QWidget* QuickEditImpl::widget()
{
    return this;
}

void QuickEditImpl::slotEnter()
{
    OPimTodo todo;

    if ( !m_edit->text().isEmpty() ) {
        todo.setUid(1 ); // new uid
        todo.setPriority( m_state );
        todo.setSummary( m_edit->text() );
        if ( ((Todo::MainWindow *)parent())->currentCatId() != 0 )
            todo.setCategories( ((Todo::MainWindow *)parent())->currentCatId() );

        m_todo = todo;
        commit();
    }
    m_todo = todo;
    reinit();
}

void QuickEditImpl::slotPrio()
{
    m_state -= 2;
    if ( m_state < 1 )
        m_state = 5;

    m_prio->setPixmap( m_pic_priority[m_state-1] );
}

void QuickEditImpl::slotMore()
{
    // TODO - implement
}

void QuickEditImpl::slotCancel()
{
    reinit();
}

void QuickEditImpl::reinit()
{
    m_state = 3;
    m_prio->setPixmap( m_pic_priority[2] );
    m_edit->clear();
}
