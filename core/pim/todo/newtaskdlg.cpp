/*
       =.            This file is part of the OPIE Project
      .=l.            Copyright (c)  2005 Dan Williams <drw@handhelds.org>
     .>+-=
_;:,   .>  :=|.         This program is free software; you can
.> <`_,  > .  <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This program is distributed in the hope that
  + . -:.    =       it will be useful,  but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=    =    ;      Library General Public License for more
++=  -.   .`   .:       details.
:   = ...= . :.=-
-.  .:....=;==+<;          You should have received a copy of the GNU
 -_. . .  )=. =           General Public License along with
  --    :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "newtaskdlg.h"

#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qradiobutton.h>

NewTaskDlg::NewTaskDlg( const QStringList &templates, QWidget *parent )
    : QDialog( parent, QString::null, true, WStyle_ContextHelp )
{
    setCaption( tr( "New Task" ) );

    QButtonGroup *bg = new QButtonGroup( this );
    bg->hide();
    
    QVBoxLayout *layout = new QVBoxLayout( this, 10, 3 );

    QRadioButton *btn = new QRadioButton( tr( "Blank task" ), this );
    btn->setChecked( true );
    bg->insert( btn );
    layout->addWidget( btn );
    
    layout->addStretch();
    
    m_useTemplate = new QRadioButton( tr( "Using template:" ), this );
    connect( m_useTemplate, SIGNAL(toggled(bool)), this, SLOT(slotUseTemplate(bool)) );
    bg->insert( m_useTemplate );
    layout->addWidget( m_useTemplate );

    m_templateList = new QComboBox( this );
    m_templateList->insertStringList( templates );
    m_templateList->setEnabled( false );
    layout->addWidget( m_templateList );
    
    layout->addStretch();
}

QString NewTaskDlg::tempSelected() {
    QString tempStr;    
    if ( m_useTemplate->isChecked() )
        tempStr = m_templateList->currentText();

    return tempStr;
}

void NewTaskDlg::slotUseTemplate( bool on ) {
    m_templateList->setEnabled( on );
    if ( on )
        m_templateList->setFocus();
}
