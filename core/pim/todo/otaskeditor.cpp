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

#include <qlayout.h>

#include <opie2/odebug.h>

#include "taskeditoroverview.h"
#include "taskeditorstatus.h"
#include "taskeditoralarms.h"

#include "otaskeditor.h"

#include <qcombobox.h>

using namespace Opie::Ui;
using namespace Opie;

OTaskEditor::OTaskEditor( int cur )
    : QDialog( 0, 0, TRUE, WStyle_ContextHelp )
{
    init();
    init( cur );
}

OTaskEditor::OTaskEditor( const OPimTodo& to )
    : QDialog( 0, 0, TRUE, WStyle_ContextHelp )
{
    init();
    init( to );
}

OTaskEditor::~OTaskEditor()
{
}

void OTaskEditor::init( int cur )
{
    OPimTodo to;
    to.setUid( 1 ); // generate a new uid
    if ( cur != 0 )
        to.setCategories( cur );
    load(to);
}

void OTaskEditor::init( const OPimTodo& to )
{
    load( to );
}

OPimTodo OTaskEditor::todo() const
{
    OPimTodo to ( m_todo );
    m_overView->save( to );
    m_stat->save( to );

    // If user unchecked "recurring task" then make sure recurrence actually gets disabled
    OPimRecurrence rec = m_rec->recurrence();
    if(!m_overView->ckbRecurrence->isChecked()) {
        if(rec.type() != OPimRecurrence::NoRepeat)
            rec.setType(OPimRecurrence::NoRepeat);
    }
    to.setRecurrence( rec );

    m_alarm->save( to );

    return to;
}

void OTaskEditor::load( const OPimTodo& to )
{
    m_overView->load( to );
    m_stat->load( to );
    m_rec->setRecurrence( to.recurrence(), to.hasDueDate() ? to.dueDate() : QDate::currentDate() );
    m_alarm->load( to );

    m_todo = to;
}

void OTaskEditor::init()
{
    setCaption(tr("Task Editor") );

    QVBoxLayout* layo = new QVBoxLayout( this );
    m_tab = new OTabWidget( this );
    layo->addWidget( m_tab );

    /*
     * Add the Widgets
     */
    m_overView = new TaskEditorOverView( m_tab );
    m_tab->addTab( m_overView, "todo/info", tr("Information") );

    m_stat = new TaskEditorStatus( m_tab );
    m_tab->addTab( m_stat, "todo/TodoList", tr("Status") );

    m_alarm = new TaskEditorAlarms( m_tab );
    m_tab->addTab( m_alarm, "todo/alarm", tr("Alarms") );

//    m_remind = new TaskEditorAlarms( m_tab );
//    m_tab->addTab( m_remind, "todo/reminder", tr("Reminders") );

//    QLabel* lbl = new QLabel( m_tab );
//    lbl->setText( tr("X-Ref") );
//    m_tab->addTab( lbl, "todo/xref", tr("X-Ref") );

    m_rec = new OPimRecurrenceWidget( true, QDate::currentDate(), this );
    m_tab->addTab( m_rec, "repeat", tr("Recurrence") );


    /* signal and slots */
    connect(m_overView, SIGNAL(recurranceEnabled(bool) ),
            this, SLOT(recurranceEnabled(bool) ) );

    /* connect due date changed to the recurrence tab */
    connect(m_stat, SIGNAL(dueDateChanged(const QDate&) ),
            m_rec, SLOT(setStartDate(const QDate&) ) );
}

void OTaskEditor::showEvent( QShowEvent* )
{
    // Set appropriate focus each time the editor is shown
    m_tab->setCurrentTab( m_overView );
    m_overView->cmbDesc->setFocus();
}

void OTaskEditor::recurranceEnabled( bool enabled )
{
    m_rec->setEnabled(enabled);
    if(enabled) {
        // Default recurrence to daily if not set
        OPimRecurrence rec = m_rec->recurrence();
        if(rec.type() == OPimRecurrence::NoRepeat) {
            rec.setType(OPimRecurrence::Daily);
            m_rec->setRecurrence(rec);
        }
    }
}
