/***************************************************************************
                          inputdlg.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
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
