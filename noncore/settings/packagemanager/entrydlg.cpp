/*
                            This file is part of the OPIE Project

               =.            Copyright (c)  2004 Dan Williams <drw@handhelds.org>
             .=l.
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

#include "entrydlg.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <qpe/qpeapplication.h>

EntryDlg::EntryDlg( const QString &label, QWidget* parent, const char* name, bool modal )
    : QDialog( parent, name, modal )
{
    QVBoxLayout *vbox = new QVBoxLayout( this, 6, 6 );

    QLabel *l = new QLabel( label, this );
    l->setAlignment( AlignLeft | AlignTop | WordBreak );
    vbox->addWidget( l );

    m_entry = new QLineEdit( this );
    vbox->addWidget( m_entry );

    connect( m_entry, SIGNAL(returnPressed()), this, SLOT(tryAccept()) );
}

void EntryDlg::setText( const QString &text )
{
    m_entry->setText( text );
    m_entry->selectAll();
}

QString EntryDlg::getText()
{
    return m_entry->text();
}

QString EntryDlg::getText( const QString &caption, const QString &label, const QString &text, bool *ok,
                           QWidget *parent, const char *name )
{
    EntryDlg *dlg = new EntryDlg( label, parent, name, true );
    dlg->setCaption( caption );
    dlg->setText( text );

    QString result;

    *ok = ( QPEApplication::execDialog( dlg ) == QDialog::Accepted );
    if ( *ok )
        result = dlg->getText();

    delete dlg;
    return result;
}
void EntryDlg::tryAccept()
{
    if ( !m_entry->text().isEmpty() )
        accept();
}
