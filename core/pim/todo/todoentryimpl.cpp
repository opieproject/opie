/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
** Copyright (C) 2002 zecke
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

#include <opie/oclickablelabel.h>
#include <opie/todoevent.h>
#include <opie/tododb.h>

#include <qpe/categoryselect.h>
#include <qpe/datebookmonth.h>
#include <qpe/global.h>
#include <qpe/resource.h>
#include <qpe/imageedit.h>
#include <qpe/timestring.h>
#include <qpe/palmtoprecord.h>

#include <qlayout.h>
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


NewTaskDialog::NewTaskDialog( const ToDoEvent& task, QWidget *parent,
			      const char *name, bool modal, WFlags fl )
    : NewTaskDialogBase( parent, name, modal, fl ),
      todo( task )
{
    todo.setCategories( task.allCategories() );
    if ( todo.hasDate() )
	date = todo.date();
    else
	date = QDate::currentDate();

    init();
    comboPriority->setCurrentItem( task.priority() - 1 );

    checkCompleted->setChecked( task.isCompleted() );
    checkDate->setChecked( task.hasDate() );
    buttonDate->setText( TimeString::longDateString( date ) );

    txtTodo->setText( task.description() );
    lneSum->setText( task.summary() );
    cmbProg->setCurrentItem( task.progress()/20 );
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
	todo.setCategory( Qtopia::Record::idsToString( ids ) );
    }
    init();
}

void NewTaskDialog::init()
{
    if( layout() != 0 ){
      layout()->setMargin( 0 );
    }
    QPopupMenu *m1 = new QPopupMenu( this );
    picker = new DateBookMonth( m1, 0, TRUE );
    m1->insertItem( picker );
    buttonDate->setPopup( m1 );
    comboCategory->setCategories( todo.categories(), "Todo List", tr("Todo List") );

    connect( picker, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( dateChanged( int, int, int ) ) );

    buttonDate->setText( TimeString::longDateString( date ) );
    picker->setDate( date.year(), date.month(), date.day() );
    lblDown->setPixmap(Resource::loadPixmap("down") );
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

ToDoEvent NewTaskDialog::todoEntry()
{
  if( checkDate->isChecked() ){
    todo.setDate( date );
    todo.setHasDate( true );
  }else{
    todo.setHasDate( false );
  }
  if ( comboCategory->currentCategory() != -1 ) {
    QArray<int> arr = comboCategory->currentCategories();
    QStringList list;
    list = QStringList::split(";", Qtopia::Record::idsToString( arr )) ;
    todo.setCategories( list );
  }
  todo.setPriority( comboPriority->currentItem() + 1 );
  todo.setCompleted( checkCompleted->isChecked() );

  todo.setDescription( txtTodo->text() );
  todo.setSummary( lneSum->text() );
  QString text = cmbProg->currentText();
  todo.setProgress( text.remove( text.length()-1, 1 ).toUShort() );
  return todo;
}
void NewTaskDialog::slotCopy() 
{
    txtTodo->clear();
    txtTodo->setText( lneSum->text() );
}

/*!

*/

void NewTaskDialog::accept()
{
    QString strText = txtTodo->text();
    QString strSumm = lneSum->text();
    if ( strSumm.isEmpty() && strText.isEmpty() ) {
       // hmm... just decline it then, the user obviously didn't care about it
       QDialog::reject();
       return;
    }
    QDialog::accept();
}
