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
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvariant.h>
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

    QDate curDate = QDate::currentDate();
    m_start = m_comp = m_due = curDate;
    QString curDateStr = TimeString::longDateString( curDate );

    // TODO - add QScrollView for contents
    QVBoxLayout *vb = new QVBoxLayout( this );

    QScrollView *sv = new QScrollView( this );
    vb->addWidget( sv );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QGridLayout *layout = new QGridLayout( container, 10, 2, 4, 4 ); 

    // Summary
    QLabel *label = new QLabel( tr( "Summary:" ), container );
    layout->addWidget( label, 0, 0 );
    cmbSum = new QComboBox( TRUE, container );
    cmbSum->insertItem( tr( "Complete " ) );
    cmbSum->insertItem( tr( "Work on " ) );
    cmbSum->insertItem( tr( "Buy " ) );
    cmbSum->insertItem( tr( "Organize " ) );
    cmbSum->insertItem( tr( "Get " ) );
    cmbSum->insertItem( tr( "Update " ) );
    cmbSum->insertItem( tr( "Create " ) );
    cmbSum->insertItem( tr( "Plan " ) );
    cmbSum->insertItem( tr( "Call " ) );
    cmbSum->insertItem( tr( "Mail " ) );
    cmbSum->clearEdit();
    layout->addMultiCellWidget( cmbSum, 1, 1, 0, 1 );

    // Priority
    label = new QLabel( tr( "Priority:" ), container );
    layout->addWidget( label, 2, 0 );
    cmbPrio = new QComboBox( FALSE, container );
    cmbPrio->setMinimumHeight( 26 );
    cmbPrio->insertItem( m_pic_priority[ 0 ], tr( "Very High" ) );
    cmbPrio->insertItem( m_pic_priority[ 1 ], tr( "High" ) );
    cmbPrio->insertItem( m_pic_priority[ 2 ], tr( "Normal" ) );
    cmbPrio->insertItem( m_pic_priority[ 3 ], tr( "Low" ) );
    cmbPrio->insertItem( m_pic_priority[ 4 ], tr( "Very Low" ) );
    cmbPrio->setCurrentItem( 2 );
    layout->addWidget( cmbPrio, 2, 1 );

    // Category
    label = new QLabel( tr( "Category:" ), container );
    layout->addWidget( label, 3, 0 );
    comboCategory = new CategorySelect( container );
    layout->addWidget( comboCategory, 3, 1 );

    // Recurrance
    CheckBox7 = new QCheckBox( tr( "Recurring task" ), container );
    layout->addMultiCellWidget( CheckBox7, 4, 4, 0, 1 );
    connect( CheckBox7, SIGNAL(clicked() ), this, SLOT( slotRecClicked() ) );

    QSpacerItem *spacer = new QSpacerItem( 5, 5, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
    layout->addItem( spacer, 5, 0 );

    // Start date
    ckbStart = new QCheckBox( tr( "Start Date:" ), container );
    layout->addWidget( ckbStart, 6, 0 );
    connect( ckbStart, SIGNAL( clicked() ), this, SLOT( slotStartChecked() ) );
    btnStart = new QPushButton( curDateStr, container );
    btnStart->setEnabled( FALSE );
    layout->addWidget( btnStart, 6, 1 );
	
    QPopupMenu *popup = new QPopupMenu( this );
    m_startBook = new DateBookMonth( popup, 0, TRUE );
    popup->insertItem( m_startBook );
    btnStart->setPopup( popup );
    connect( m_startBook, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( slotStartChanged( int, int, int ) ) );
        
    // Due date
    ckbDue = new QCheckBox( tr( "Due Date:" ), container );
    layout->addWidget( ckbDue, 7, 0 );
    connect( ckbDue, SIGNAL( clicked() ), this, SLOT( slotDueChecked() ) );
    btnDue = new QPushButton( curDateStr, container );
    btnDue->setEnabled( FALSE );
    layout->addWidget( btnDue, 7, 1 );

    popup = new QPopupMenu( this );
    m_dueBook = new DateBookMonth( popup, 0, TRUE );
    popup->insertItem( m_dueBook );
    btnDue->setPopup( popup );
    connect( m_dueBook, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( slotDueChanged( int, int, int ) ) );
    
    // Progress
    label = new QLabel( tr( "Progress:" ), container );
    layout->addWidget( label, 8, 0 );
    cmbProgress = new QComboBox( FALSE, container );
    cmbProgress->insertItem( tr( "0 %" ) );
    cmbProgress->insertItem( tr( "20 %" ) );
    cmbProgress->insertItem( tr( "40 %" ) );
    cmbProgress->insertItem( tr( "60 %" ) );
    cmbProgress->insertItem( tr( "80 %" ) );
    cmbProgress->insertItem( tr( "100 %" ) );
    layout->addWidget( cmbProgress, 8, 1 );

    // Completed
    ckbComp = new QCheckBox( tr( "Completed:" ), container );
    layout->addWidget( ckbComp, 9, 0 );
    connect( ckbComp, SIGNAL( clicked() ), this, SLOT( slotCompChecked() ) );
    btnComp = new QPushButton( curDateStr, container );
    btnComp->setEnabled( FALSE );
    layout->addWidget( btnComp, 9, 1 );

    popup = new QPopupMenu( this );
    m_compBook = new DateBookMonth( popup, 0, TRUE );
    popup->insertItem( m_compBook );
    btnComp->setPopup( popup );
    connect( m_compBook, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( slotCompChanged( int, int, int ) ) );
}

TaskEditorOverView::~TaskEditorOverView()
{
}

void TaskEditorOverView::load( const OTodo& todo )
{
    /*
     * now that we're 'preloaded' we
     * need to disable the buttons
     * holding the dat
     */
    btnDue->setEnabled( FALSE );
    btnComp->setEnabled( FALSE );
    btnStart->setEnabled( FALSE );

    /*
     * get some basic dateinfos for now
     */
    QDate date = QDate::currentDate();
    QString str = TimeString::longDateString( date );

    CheckBox7->setChecked( todo.recurrence().doesRecur() );
    emit recurranceEnabled( todo.recurrence().doesRecur() );

    ckbStart->setChecked( todo.hasStartDate() );
    btnStart->setEnabled( todo.hasStartDate() );
    if ( todo.hasStartDate() )
    {
        m_start = todo.startDate();
        btnStart->setText( TimeString::longDateString( m_start ) );
    }
    else
        btnStart->setText( str );

    ckbComp->setChecked( todo.hasCompletedDate() );
    btnComp->setEnabled( todo.hasCompletedDate() );
    if ( todo.hasCompletedDate() )
    {
        m_comp = todo.completedDate();
        btnComp->setText( TimeString::longDateString( m_comp ) );
    }
    else
        btnComp->setText( str );

    cmbProgress->setCurrentItem( todo.progress() / 20 );
    cmbSum->insertItem( todo.summary(), 0 );
    cmbSum->setCurrentItem( 0 );

    ckbDue->setChecked( todo.hasDueDate() );
    btnDue->setText( TimeString::longDateString( todo.dueDate() ) );
    btnDue->setEnabled( todo.hasDueDate() );
    m_due = todo.dueDate();

    cmbPrio->setCurrentItem( todo.priority() - 1 );
    ckbComp->setChecked( todo.isCompleted() );

    comboCategory->setCategories( todo.categories(), "Todo List", tr( "Todo List" ) );
}

void TaskEditorOverView::save( OTodo &to )
{
    /* a invalid date */
    QDate inval;
    /* save our info back */

    /* due date */
    if ( ckbDue->isChecked() )
    {
        to.setDueDate( m_due );
        to.setHasDueDate( true );
    }
    else
        to.setHasDueDate( false );

    /* start date */
    if ( ckbStart->isChecked() )
    {
        to.setStartDate( m_start );
    }
    else
        to.setStartDate( inval );

    /* comp date */
    if ( ckbComp->isChecked() )
    {
        to.setCompletedDate( m_comp );
    }
    else
        to.setCompletedDate( inval );


    if ( comboCategory->currentCategory() != -1 )
    {
        QArray<int> arr = comboCategory->currentCategories();
        to.setCategories( arr );
    }
    to.setPriority( cmbPrio->currentItem() + 1 );
    to.setCompleted( ckbComp->isChecked() );
    to.setSummary( cmbSum->currentText() );
    to.setProgress( cmbProgress->currentItem() * 20 );
}

void TaskEditorOverView::slotRecClicked()
{
    emit recurranceEnabled( CheckBox7->isChecked() );
}

void TaskEditorOverView::slotStartChecked()
{
    btnStart->setEnabled( ckbStart->isChecked() );
}

void TaskEditorOverView::slotCompChecked()
{
    btnComp->setEnabled( ckbComp->isChecked() );
}

void TaskEditorOverView::slotDueChecked()
{
    btnDue->setEnabled( ckbDue->isChecked() );
}

void TaskEditorOverView::slotStartChanged(int y, int m, int d)
{
    m_start.setYMD( y, m, d );
    btnStart->setText( TimeString::longDateString( m_start ) );
}

void TaskEditorOverView::slotCompChanged(int y, int m, int d)
{
    m_comp.setYMD( y, m, d );
    btnComp->setText( TimeString::longDateString( m_comp ) );
}

void TaskEditorOverView::slotDueChanged(int y, int m, int d)
{
    m_due.setYMD( y, m, d );
    btnDue->setText( TimeString::longDateString( m_due ) );
}
