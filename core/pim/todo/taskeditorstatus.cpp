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

#include "taskeditorstatus.h"

#include <opie/otodo.h>
#include <opie/opimmaintainer.h>
#include <opie/opimstate.h>

#include <qpe/datebookmonth.h>
#include <qpe/resource.h>
#include <qpe/timestring.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>

TaskEditorStatus::TaskEditorStatus( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QDate curDate = QDate::currentDate();
    m_start = m_comp = m_due = curDate;
    QString curDateStr = TimeString::longDateString( curDate );

    QVBoxLayout *vb = new QVBoxLayout( this );

    QScrollView *sv = new QScrollView( this );
    vb->addWidget( sv );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QGridLayout *layout = new QGridLayout( container, 7, 3, 4, 4 ); 

    // Status
    QLabel *label = new QLabel( tr( "Status:" ), container );
    layout->addWidget( label, 0, 0 );
    QWhatsThis::add( label, tr( "Click here to set the current status of this task." ) );
    cmbStatus = new QComboBox( FALSE, container );
    cmbStatus->insertItem( tr( "Started" ) );
    cmbStatus->insertItem( tr( "Postponed" ) );
    cmbStatus->insertItem( tr( "Finished" ) );
    cmbStatus->insertItem( tr( "Not started" ) );
    layout->addMultiCellWidget( cmbStatus, 0, 0, 1, 2 );
    QWhatsThis::add( cmbStatus, tr( "Click here to set the current status of this task." ) );

    // Progress
    label = new QLabel( tr( "Progress:" ), container );
    layout->addWidget( label, 1, 0 );
    QWhatsThis::add( label, tr( "Select progress made on this task here." ) );
    cmbProgress = new QComboBox( FALSE, container );
    cmbProgress->insertItem( tr( "0 %" ) );
    cmbProgress->insertItem( tr( "20 %" ) );
    cmbProgress->insertItem( tr( "40 %" ) );
    cmbProgress->insertItem( tr( "60 %" ) );
    cmbProgress->insertItem( tr( "80 %" ) );
    cmbProgress->insertItem( tr( "100 %" ) );
    layout->addMultiCellWidget( cmbProgress, 1, 1, 1, 2 );
    QWhatsThis::add( cmbProgress, tr( "Select progress made on this task here." ) );
    
    // Start date
    ckbStart = new QCheckBox( tr( "Start Date:" ), container );
    layout->addWidget( ckbStart, 2, 0 );
    QWhatsThis::add( ckbStart, tr( "Click here to set the date this task was started." ) );
    connect( ckbStart, SIGNAL( clicked() ), this, SLOT( slotStartChecked() ) );
    btnStart = new QPushButton( curDateStr, container );
    btnStart->setEnabled( FALSE );
    layout->addMultiCellWidget( btnStart, 2, 2, 1, 2 );
    QWhatsThis::add( btnStart, tr( "Click here to set the date this task was started." ) );
    QPopupMenu *popup = new QPopupMenu( this );
    m_startBook = new DateBookMonth( popup, 0, TRUE );
    popup->insertItem( m_startBook );
    btnStart->setPopup( popup );
    connect( m_startBook, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( slotStartChanged( int, int, int ) ) );
        
    // Due date
    ckbDue = new QCheckBox( tr( "Due Date:" ), container );
    layout->addWidget( ckbDue, 3, 0 );
    QWhatsThis::add( ckbDue, tr( "Click here to set the date this task needs to be completed by." ) );
    connect( ckbDue, SIGNAL( clicked() ), this, SLOT( slotDueChecked() ) );
    btnDue = new QPushButton( curDateStr, container );
    btnDue->setEnabled( FALSE );
    layout->addMultiCellWidget( btnDue, 3, 3, 1, 2 );
    QWhatsThis::add( btnDue, tr( "Click here to set the date this task needs to be completed by." ) );
    popup = new QPopupMenu( this );
    m_dueBook = new DateBookMonth( popup, 0, TRUE );
    popup->insertItem( m_dueBook );
    btnDue->setPopup( popup );
    connect( m_dueBook, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( slotDueChanged( int, int, int ) ) );
    
    // Completed
    ckbComp = new QCheckBox( tr( "Completed:" ), container );
    layout->addWidget( ckbComp, 4, 0 );
    QWhatsThis::add( ckbComp, tr( "Click here to mark this task as completed." ) );
    connect( ckbComp, SIGNAL( clicked() ), this, SLOT( slotCompChecked() ) );
    btnComp = new QPushButton( curDateStr, container );
    btnComp->setEnabled( FALSE );
    layout->addMultiCellWidget( btnComp, 4, 4, 1, 2 );
    QWhatsThis::add( btnComp, tr( "Click here to set the date this task was completed." ) );
    popup = new QPopupMenu( this );
    m_compBook = new DateBookMonth( popup, 0, TRUE );
    popup->insertItem( m_compBook );
    btnComp->setPopup( popup );
    connect( m_compBook, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( slotCompChanged( int, int, int ) ) );

    QSpacerItem *spacer = new QSpacerItem( 5, 5, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
    layout->addItem( spacer, 5, 0 );

    // Maintainer mode
    label = new QLabel( tr( "Maintainer Mode:" ), container );
    layout->addWidget( label, 6, 0 );
    QWhatsThis::add( label, tr( "Click here to set the maintainer's role." ) );
    cmbMaintMode = new QComboBox( FALSE, container );
    cmbMaintMode->insertItem( tr( "Nothing" ) );
    cmbMaintMode->insertItem( tr( "Responsible" ) );
    cmbMaintMode->insertItem( tr( "Done By" ) );
    cmbMaintMode->insertItem( tr( "Coordinating" ) );
    layout->addMultiCellWidget( cmbMaintMode, 6, 6, 1, 2 );
    QWhatsThis::add( cmbMaintMode, tr( "Click here to set the maintainer's role." ) );

    // Maintainer
    label = new QLabel( tr( "Maintainer:" ), container );
    layout->addWidget( label, 7, 0 );
    QWhatsThis::add( label, tr( "This is the name of the current task maintainer." ) );
    txtMaintainer = new QLabel( tr( "test" ), container );
    txtMaintainer->setTextFormat( QLabel::RichText );
    layout->addWidget( txtMaintainer, 7, 1 );
    QWhatsThis::add( txtMaintainer, tr( "This is the name of the current task maintainer." ) );
    tbtMaintainer = new QToolButton( container );
    tbtMaintainer->setPixmap( Resource::loadPixmap( "todo/more" ) );
    layout->addWidget( tbtMaintainer, 7, 2 );
    QWhatsThis::add( tbtMaintainer, tr( "Click here to select the task maintainer." ) );
}

TaskEditorStatus::~TaskEditorStatus()
{
}

void TaskEditorStatus::load( const OTodo &todo )
{
    QDate date = QDate::currentDate();
    QString str = TimeString::longDateString( date );

    // Status
    int state = todo.state().state();
    if ( state == OPimState::Undefined )
        state = OPimState::NotStarted;
    cmbStatus->setCurrentItem( state );

    // Progress
    cmbProgress->setCurrentItem( todo.progress() / 20 );
    
    // Start date
    ckbStart->setChecked( todo.hasStartDate() );
    btnStart->setEnabled( todo.hasStartDate() );
    if ( todo.hasStartDate() )
    {
        m_start = todo.startDate();
        btnStart->setText( TimeString::longDateString( m_start ) );
    }
    else
        btnStart->setText( str );

    // Due date
    ckbDue->setChecked( todo.hasDueDate() );
    btnDue->setText( TimeString::longDateString( todo.dueDate() ) );
    btnDue->setEnabled( todo.hasDueDate() );
    m_due = todo.dueDate();

    // Completed
    ckbComp->setChecked( todo.isCompleted() );
    btnComp->setEnabled( todo.hasCompletedDate() );
    if ( todo.hasCompletedDate() )
    {
        m_comp = todo.completedDate();
        btnComp->setText( TimeString::longDateString( m_comp ) );
    }
    else
        btnComp->setText( str );

    // Maintainer Mode
    state = todo.maintainer().mode();
    if ( state == OPimMaintainer::Undefined )
        state = OPimMaintainer::Nothing;
    cmbMaintMode->setCurrentItem( state );

    // Maintainer - not implemented yet
}

void TaskEditorStatus::save( OTodo &todo )
{
    QDate inval;

    // Status
    todo.setState( OPimState( cmbStatus->currentItem() ) );
    
    // Progress
    todo.setProgress( cmbProgress->currentItem() * 20 );

    // Start date
    if ( ckbStart->isChecked() )
    {
        todo.setStartDate( m_start );
    }
    else
        todo.setStartDate( inval );

    // Due date
    if ( ckbDue->isChecked() )
    {
        todo.setDueDate( m_due );
        todo.setHasDueDate( true );
    }
    else
        todo.setHasDueDate( false );

    // Completed
    todo.setCompleted( ckbComp->isChecked() );
    if ( ckbComp->isChecked() )
    {
        todo.setCompletedDate( m_comp );
    }
    else
        todo.setCompletedDate( inval );
    
    // Maintainer mode - not implemented yet

    // Maintainer
    /* TODO - resolve name to uid.....*/
    todo.setMaintainer( OPimMaintainer( cmbMaintMode->currentItem(), -10 ) );
}

void TaskEditorStatus::slotStartChecked()
{
    btnStart->setEnabled( ckbStart->isChecked() );
}

void TaskEditorStatus::slotCompChecked()
{
    btnComp->setEnabled( ckbComp->isChecked() );
}

void TaskEditorStatus::slotDueChecked()
{
    btnDue->setEnabled( ckbDue->isChecked() );
}

void TaskEditorStatus::slotStartChanged(int y, int m, int d)
{
    m_start.setYMD( y, m, d );
    btnStart->setText( TimeString::longDateString( m_start ) );
}

void TaskEditorStatus::slotCompChanged(int y, int m, int d)
{
    m_comp.setYMD( y, m, d );
    btnComp->setText( TimeString::longDateString( m_comp ) );
}

void TaskEditorStatus::slotDueChanged(int y, int m, int d)
{
    m_due.setYMD( y, m, d );
    btnDue->setText( TimeString::longDateString( m_due ) );
}
