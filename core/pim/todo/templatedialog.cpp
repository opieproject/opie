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

#include <opie2/odebug.h>

#include <qpushbutton.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qlineedit.h>

#include "templatedialog.h"


using namespace Todo;

/* TRANSLATOR Todo::TemplateDialog */

TemplateDialog::TemplateDialog( QWidget* widget )
    : QDialog( widget, "TemplateDialog", TRUE )
{
    setCaption( QWidget::tr("Template Editor") );
    m_main = new QVBoxLayout(this );

    m_list = new QListView( this );
    m_main->addWidget( m_list, 100 );

    m_lne = new QLineEdit( this );
    m_main->addWidget( m_lne );

    m_btnBar = new QHBox( this );
    m_add = new QPushButton( QWidget::tr("Add"), m_btnBar );
    m_edit = new QPushButton( QWidget::tr("Edit"), m_btnBar );
    m_rem = new QPushButton( QWidget::tr("Remove"), m_btnBar );
    m_main->addWidget( m_btnBar );

    connect(m_add,  SIGNAL(clicked() ),
            this,   SLOT(slotAdd() ) );
    connect(m_edit, SIGNAL(clicked() ),
            this,   SLOT(slotEdit() ) );
    connect(m_rem,  SIGNAL(clicked() ),
            this,   SLOT(slotRemove() ) );
    connect(m_lne, SIGNAL(returnPressed() ),
            this,   SLOT(slotReturn() ) );

}
TemplateDialog::~TemplateDialog() {
    // Qt does delete our widgets
}
QListView* TemplateDialog::listView() {
    return m_list;
}
QLineEdit* TemplateDialog::edit() {
    return m_lne;
}
void TemplateDialog::slotAdd() {
}
void TemplateDialog::slotRemove() {
}
void TemplateDialog::slotEdit() {
}
void TemplateDialog::slotReturn() {
}
