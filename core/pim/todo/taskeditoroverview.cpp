/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "taskeditoroverview.h"

#include <opie/orecur.h>

#include <qpe/categoryselect.h>
#include <qpe/datebookmonth.h>
#include <qpe/resource.h>
#include <qpe/timestring.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qwhatsthis.h>

TaskEditorOverView::TaskEditorOverView( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    // Load icons
    // TODO - probably should be done globally somewhere else,
    //        see also quickeditimpl.cpp/h, tableview.cpp/h
    QString namestr;
    for ( unsigned int i = 1; i < 6; i++ ) {
        namestr = "todo/priority";
        namestr.append( QString::number( i ) );
        m_pic_priority[ i - 1 ] = Resource::loadPixmap( namestr );
    }

    QVBoxLayout *vb = new QVBoxLayout( this );

    QScrollView *sv = new QScrollView( this );
    vb->addWidget( sv );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QGridLayout *layout = new QGridLayout( container, 7, 2, 4, 4 ); 

    // Description
    QLabel *label = new QLabel( tr( "Description:" ), container );
    layout->addWidget( label, 0, 0 );
    QWhatsThis::add( label, tr( "Enter brief description of the task here." ) );
    cmbDesc = new QComboBox( TRUE, container );
    cmbDesc->insertItem( tr( "Complete " ) );
    cmbDesc->insertItem( tr( "Work on " ) );
    cmbDesc->insertItem( tr( "Buy " ) );
    cmbDesc->insertItem( tr( "Organize " ) );
    cmbDesc->insertItem( tr( "Get " ) );
    cmbDesc->insertItem( tr( "Update " ) );
    cmbDesc->insertItem( tr( "Create " ) );
    cmbDesc->insertItem( tr( "Plan " ) );
    cmbDesc->insertItem( tr( "Call " ) );
    cmbDesc->insertItem( tr( "Mail " ) );
    cmbDesc->clearEdit();
    layout->addMultiCellWidget( cmbDesc, 1, 1, 0, 1 );
    QWhatsThis::add( cmbDesc, tr( "Enter brief description of the task here." ) );

    // Priority
    label = new QLabel( tr( "Priority:" ), container );
    layout->addWidget( label, 2, 0 );
    QWhatsThis::add( label, tr( "Select priority of task here." ) );
    cmbPriority = new QComboBox( FALSE, container );
    cmbPriority->setMinimumHeight( 26 );
    cmbPriority->insertItem( m_pic_priority[ 0 ], tr( "Very High" ) );
    cmbPriority->insertItem( m_pic_priority[ 1 ], tr( "High" ) );
    cmbPriority->insertItem( m_pic_priority[ 2 ], tr( "Normal" ) );
    cmbPriority->insertItem( m_pic_priority[ 3 ], tr( "Low" ) );
    cmbPriority->insertItem( m_pic_priority[ 4 ], tr( "Very Low" ) );
    cmbPriority->setCurrentItem( 2 );
    layout->addWidget( cmbPriority, 2, 1 );
    QWhatsThis::add( cmbPriority, tr( "Select priority of task here." ) );

    // Category
    label = new QLabel( tr( "Category:" ), container );
    layout->addWidget( label, 3, 0 );
    QWhatsThis::add( label, tr( "Select category to organize this task with." ) );
    cmbCategory = new CategorySelect( container );
    layout->addWidget( cmbCategory, 3, 1 );
    QWhatsThis::add( cmbCategory, tr( "Select category to organize this task with." ) );

    // Recurrence
    ckbRecurrence = new QCheckBox( tr( "Recurring task" ), container );
    layout->addMultiCellWidget( ckbRecurrence, 4, 4, 0, 1 );
    QWhatsThis::add( ckbRecurrence, tr( "Click here if task happens on a regular basis.  If selected, frequency can be set on the Recurrence tab." ) );
    connect( ckbRecurrence, SIGNAL(clicked() ), this, SLOT( slotRecClicked() ) );

    // Notes
    label = new QLabel( tr( "Notes:" ), container );
    layout->addWidget( label, 5, 0 );
    QWhatsThis::add( label, tr( "Enter any additional information about this task here." ) );
    mleNotes = new QMultiLineEdit( this );
    mleNotes->setWordWrap( QMultiLineEdit::WidgetWidth );
    layout->addMultiCellWidget( mleNotes, 6, 6, 0, 1 );
    QWhatsThis::add( mleNotes, tr( "Enter any additional information about this task here." ) );
}

TaskEditorOverView::~TaskEditorOverView()
{
}

void TaskEditorOverView::load( const OTodo& todo )
{
    // Description
    cmbDesc->insertItem( todo.summary(), 0 );
    cmbDesc->setCurrentItem( 0 );

    // Priority
    cmbPriority->setCurrentItem( todo.priority() - 1 );

    // Category
    cmbCategory->setCategories( todo.categories(), "Todo List", tr( "Todo List" ) );

    // Recurrence
    ckbRecurrence->setChecked( todo.recurrence().doesRecur() );
    emit recurranceEnabled( todo.recurrence().doesRecur() );

    // Notes
    mleNotes->setText( todo.description() );

}

void TaskEditorOverView::save( OTodo &todo )
{
    // Description
    todo.setSummary( cmbDesc->currentText() );

    // Priority
    todo.setPriority( cmbPriority->currentItem() + 1 );

    // Category
    if ( cmbCategory->currentCategory() != -1 )
    {
        QArray<int> arr = cmbCategory->currentCategories();
        todo.setCategories( arr );
    }

    // Recurrence - don't need to save here...    

    // Notes
    todo.setDescription( mleNotes->text() );
}

void TaskEditorOverView::slotRecClicked()
{
    emit recurranceEnabled( ckbRecurrence->isChecked() );
}
