/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2002 The Opie Team <opie-devel@lists.sourceforge.net>
            .=l.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/


#include "bluetoothpindlg.h"

/* OPIE */
#include <qpe/config.h>

/* QT */
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>


PinDlg::PinDlg(QWidget* parent,
               const char* name, 
               Qt::WFlags f )
        : PinDlgBase( parent, name, f )
{
    m_outgoing = false;
}

PinDlg::~PinDlg()
{
}

void PinDlg::polish()
{
    if (!m_bdaddr.isEmpty() ) {
        Config cfg("bluepin");
        cfg.setGroup(m_bdaddr);
        lnePin->setText(cfg.readEntryCrypt("pin", QString::null ) );
    }

    QString status;
    if( m_outgoing )
        status = QObject::tr("Outgoing connection to ");
    else
        status = QObject::tr("Incoming connection from ");

    status += m_remoteName;
    status += "<br>";
    status += "[" + m_bdaddr + "]";

    txtStatus->setText( status );
}

void PinDlg::setBdAddr( const QString& bdaddr )
{
    m_bdaddr = bdaddr;
}

void PinDlg::setRemoteName( const QString& name )
{
    m_remoteName = name;
}

void PinDlg::setDirection( bool outgoing )
{
    m_outgoing = outgoing;
}

QString PinDlg::pin() const
{
    return lnePin->text();
}

/*
 * Add a number to the PIN number
 */
void PinDlg::addnum()
{
    if( sender()->inherits( "QPushButton") ) { 
        const QPushButton* btn = static_cast<const QPushButton*> (sender()); 
        lnePin->setText(lnePin->text() + btn->text());
    }
}

/*
 * Delete a number from the end of the line
 */
void PinDlg::delnum()
{
    lnePin->setText(lnePin->text().remove(lnePin->text().length() - 1, 1));
}

void PinDlg::accept()
{
    if ( ckbPin->isChecked() ) {
        Config cfg("bluepin");
        cfg.setGroup( m_bdaddr );
        cfg.writeEntryCrypt("pin", lnePin->text() );
    }
    QDialog::accept();
    emit dialogClosed(TRUE);
}

void PinDlg::reject()
{
    emit dialogClosed(FALSE);
}
