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

#include "taskeditoradvanced.h"

#include <opie/otodo.h>
#include <opie/opimmaintainer.h>
#include <opie/opimstate.h>

#include <qpe/resource.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qscrollview.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>

TaskEditorAdvanced::TaskEditorAdvanced( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QVBoxLayout *vb = new QVBoxLayout( this );

    QScrollView *sv = new QScrollView( this );
    vb->addWidget( sv );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QGridLayout *layout = new QGridLayout( container, 5, 3, 4, 4 ); 

    QLabel *label = new QLabel( tr( "State:" ), container );
    layout->addWidget( label, 0, 0 );

    cmbState = new QComboBox( FALSE, container );
    cmbState->insertItem( tr( "Started" ) );
    cmbState->insertItem( tr( "Postponed" ) );
    cmbState->insertItem( tr( "Finished" ) );
    cmbState->insertItem( tr( "Not started" ) );
    layout->addMultiCellWidget( cmbState, 0, 0, 1, 2 );

    label = new QLabel( tr( "Maintain Mode:" ), container );
    layout->addWidget( label, 1, 0 );

    cmbMode = new QComboBox( FALSE, container );
    cmbMode->insertItem( tr( "Nothing" ) );
    cmbMode->insertItem( tr( "Responsible" ) );
    cmbMode->insertItem( tr( "Done By" ) );
    cmbMode->insertItem( tr( "Coordinating" ) );
    layout->addMultiCellWidget( cmbMode, 1, 1, 1, 2 );

    label = new QLabel( tr( "Maintainer:" ), container );
    layout->addWidget( label, 2, 0 );

    txtMaintainer = new QLabel( tr( "test" ), container );
    txtMaintainer->setTextFormat( QLabel::RichText );
    layout->addWidget( txtMaintainer, 2, 1 );

    tbtMaintainer = new QToolButton( container );
    tbtMaintainer->setPixmap( Resource::loadPixmap( "todo/more" ) );
    layout->addWidget( tbtMaintainer, 2, 2 );

    label = new QLabel( tr( "Description:" ), container );
    layout->addWidget( label, 3, 0 );

    m_edit = new QMultiLineEdit( this );
    m_edit->setWordWrap( QMultiLineEdit::WidgetWidth );
    layout->addMultiCellWidget( m_edit, 4, 4, 0, 2 );
}

TaskEditorAdvanced::~TaskEditorAdvanced()
{
}

void TaskEditorAdvanced::load( const OTodo &todo )
{
    m_edit->setText( todo.description() );

    /* OPimState */
    int state = todo.state().state();

    /* defualt to not started */
    if ( state == OPimState::Undefined )
        state = OPimState::NotStarted;

    cmbState->setCurrentItem( state );

    /* Maintainer Mode */
    state = todo.maintainer().mode();
    if ( state == OPimMaintainer::Undefined )
        state = OPimMaintainer::Nothing;

    cmbMode->setCurrentItem( state );
}

void TaskEditorAdvanced::save( OTodo &todo )
{
    todo.setDescription( m_edit->text() );
    todo.setState( OPimState( cmbState->currentItem() ) );

    /* Fix me resolve name to uid.....*/
    todo.setMaintainer( OPimMaintainer( cmbMode->currentItem(), -10 ) );
    qWarning("save");
}
