/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "todoentryimpl.h"

#include <qpe/categoryselect.h>
#include <qpe/datebookmonth.h>
#include <qpe/global.h>
#include <qpe/imageedit.h>
#include <qpe/task.h>
#include <qpe/timestring.h>

#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qapplication.h>


NewTaskDialog::NewTaskDialog( const Task& task, QWidget *parent,
			      const char *name, bool modal, WFlags fl )
    : NewTaskDialogBase( parent, name, modal, fl ),
      todo( task )
{
    todo.setCategories( task.categories() );
    if ( todo.hasDueDate() )
	date = todo.dueDate();
    else
	date = QDate::currentDate();

    init();
    comboPriority->setCurrentItem( task.priority() - 1 );

    checkCompleted->setChecked( task.isCompleted() );
    checkDate->setChecked( task.hasDueDate() );
    buttonDate->setText( TimeString::longDateString( date ) );

    txtTodo->setText( task.description() );
}

/*
 *  Constructs a NewTaskDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
NewTaskDialog::NewTaskDialog( int id, QWidget* parent,  const char* name, bool modal,
			      WFlags fl )
    : NewTaskDialogBase( parent, name, modal, fl ),
      date( QDate::currentDate() )
{
    if ( id != -1 ) {
	QArray<int> ids( 1 );
	ids[0] = id;
	todo.setCategories( ids );
    }
    init();
}

void NewTaskDialog::init()
{
    QPopupMenu *m1 = new QPopupMenu( this );
    picker = new DateBookMonth( m1, 0, TRUE );
    m1->insertItem( picker );
    buttonDate->setPopup( m1 );
    comboCategory->setCategories( todo.categories(), "Todo List", tr("Todo List") );

    connect( picker, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( dateChanged( int, int, int ) ) );

    buttonDate->setText( TimeString::longDateString( date ) );
    picker->setDate( date.year(), date.month(), date.day() );
}

/*
 *  Destroys the object and frees any allocated resources
 */
NewTaskDialog::~NewTaskDialog()
{
    // no need to delete child widgets, Qt does it all for us
}
void NewTaskDialog::dateChanged( int y, int m, int d )
{
    date = QDate( y, m, d );
    buttonDate->setText( TimeString::longDateString( date ) );
}

/*!
*/

Task NewTaskDialog::todoEntry()
{
    todo.setDueDate( date, checkDate->isChecked() );
    if ( comboCategory->currentCategory() != -1 ) {
	todo.setCategories( comboCategory->currentCategories() );
    }
    todo.setPriority( comboPriority->currentItem() + 1 );
    todo.setCompleted( checkCompleted->isChecked() );

    todo.setDescription( txtTodo->text() );

    return todo;
}


/*!

*/

void NewTaskDialog::accept()
{
    QString strText = txtTodo->text();
    if ( !strText || strText == "") {
       // hmm... just decline it then, the user obviously didn't care about it
       QDialog::reject();
       return;
    }
    QDialog::accept();
}
