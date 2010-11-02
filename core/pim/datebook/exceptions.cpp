/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@lists.sourceforge.net>
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

#include "exceptions.h"

#include "qlistbox.h"
#include "qpushbutton.h"
#include <qpe/qpemessagebox.h>

Exceptions::Exceptions(const Opie::OPimRecurrence::ExceptionList &exceptions, QWidget* parent, const char* name, bool modal,
                       WFlags fl) : ExceptionsBase(parent, name, modal, fl)
{
    QString item;
    Opie::OPimRecurrence::ExceptionList::ConstIterator it;

    for ( it = exceptions.begin(); it != exceptions.end(); ++it ) {
        item = QCString().sprintf("%04d%02d%02d", (*it).year(), (*it).month(), (*it).day() );;
        lbExceptions->insertItem(item);
    }

    connect( cmdRemove, SIGNAL(clicked()),
             this, SLOT(slotRemoveItem()) );
}

void Exceptions::slotRemoveItem()
{
    if(lbExceptions->currentItem() == -1)
        return;

    if ( QPEMessageBox::confirmDelete( this, tr( "Calendar" ), lbExceptions->currentText() ) )
        lbExceptions->removeItem( lbExceptions->currentItem() );
}

Opie::OPimRecurrence::ExceptionList Exceptions::getExceptions()
{
    Opie::OPimRecurrence::ExceptionList excList;
    for (unsigned int i=0; i<lbExceptions->count(); i++) {
        QString item = lbExceptions->text(i);
        QDate date( item.left(4).toInt(), item.mid(4, 2).toInt(), item.right(2).toInt() );
        excList += date;
    }
    return excList;
}
