/*
                             This file is part of the Opie Project

                             Copyright (C)2004, 2005 Dan Williams <drw@handhelds.org>
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

#include "entrydlg.h"

#include <opie2/ofiledialog.h>
#include <opie2/oresource.h>

#include <qpe/qpeapplication.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

EntryDlg::EntryDlg( const QString &label, QWidget* parent, const char* name, bool modal )
    : QDialog( parent, name, modal )
{
    QGridLayout *layout = new QGridLayout( this, 3, 2, 2, 4 );

    QLabel *l = new QLabel( label, this );
    l->setAlignment( AlignLeft | AlignTop | WordBreak );
    layout->addMultiCellWidget( l, 0, 0, 0, 1 );

    m_entry = new QLineEdit( this );
    layout->addWidget( m_entry, 1, 0 );
    connect( m_entry, SIGNAL(returnPressed()), this, SLOT(slotTryAccept()) );

    QPushButton *btn = new QPushButton( Opie::Core::OResource::loadPixmap( "folder", Opie::Core::OResource::SmallIcon ),
                                        QString::null, this );
    btn->setMinimumHeight( AppLnk::smallIconSize()+4 );
    btn->setMaximumWidth( btn->height() );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotSelectPath()) );
    layout->addWidget( btn, 1, 1 );

    layout->setRowStretch( 2, 10 );

    resize( width(), l->height() + btn->height() + 8 );
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
void EntryDlg::slotTryAccept()
{
    if ( !m_entry->text().isEmpty() )
        accept();
}

void EntryDlg::slotSelectPath()
{
    QString path = Opie::Ui::OFileDialog::getDirectory( 0, m_entry->text() );
    if ( path.at( path.length() - 1 ) == '/' )
        path.truncate( path.length() - 1 );
    if ( !path.isNull() )
        m_entry->setText( path );
}
