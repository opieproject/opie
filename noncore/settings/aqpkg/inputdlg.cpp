/*
                             This file is part of the OPIE Project
                             
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qvalidator.h>
#include <qapplication.h>

#include "inputdlg.h"
#include "global.h"


InputDialog :: InputDialog( const QString &label, QWidget* parent, const char* name,
			  bool modal )
    : QDialog( parent, name, modal )
{
    lineEdit = 0;

    QVBoxLayout *vbox = new QVBoxLayout( this, 6, 6 );

    QLabel* l = new QLabel( label, this );
    vbox->addWidget( l );

    lineEdit = new QLineEdit( this );
    vbox->addWidget( lineEdit );

    QHBoxLayout *hbox = new QHBoxLayout( 6 );
    vbox->addLayout( hbox, AlignRight );

    ok = new QPushButton( tr( "&OK" ), this );
    ok->setDefault( TRUE );
    QPushButton *cancel = new QPushButton( tr( "&Cancel" ), this );

    QSize bs( ok->sizeHint() );
    if ( cancel->sizeHint().width() > bs.width() )
	bs.setWidth( cancel->sizeHint().width() );

    ok->setFixedSize( bs );
    cancel->setFixedSize( bs );

    hbox->addWidget( new QWidget( this ) );
    hbox->addWidget( ok );
    hbox->addWidget( cancel );

    connect( lineEdit, SIGNAL( returnPressed() ),
	     this, SLOT( tryAccept() ) );
    connect( lineEdit, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( textChanged( const QString & ) ) );

    connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    resize( QMAX( sizeHint().width(), 240 ), sizeHint().height() );
}

/*!
  Destructor.
*/

InputDialog::~InputDialog()
{
}

void InputDialog :: setText( const QString &text )
{
    lineEdit->setText( text );
    lineEdit->selectAll();
}

QString InputDialog :: getText()
{
    return lineEdit->text();
}

QString InputDialog::getText( const QString &caption, const QString &label, 
    			              const QString &text, bool *ok, QWidget *parent,
                              const char *name )
{
    InputDialog *dlg = new InputDialog( label, parent, name, true );
    dlg->setCaption( caption );
    dlg->setText( text );

    QString result;
    *ok = dlg->exec() == QDialog::Accepted;
    if ( *ok )
	    result = dlg->getText();

    delete dlg;
    return result;
}



void InputDialog :: textChanged( const QString &s )
{
    ok->setEnabled( !s.isEmpty() );
}

void InputDialog :: tryAccept()
{
    if ( !lineEdit->text().isEmpty() )
        accept();
}
