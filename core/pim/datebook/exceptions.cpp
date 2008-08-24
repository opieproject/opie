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

#include "exceptions.h"

#include "qlistbox.h"
#include "qpushbutton.h"
#include <qpe/qpemessagebox.h>

Exceptions::Exceptions(const QString &exceptions, QWidget* parent, const char* name, bool modal,
                       WFlags fl) : ExceptionsBase(parent, name, modal, fl) 
{
    QStringList exceptList = QStringList::split( " ", exceptions );
    QString item;
    QStringList::ConstIterator it;

    for ( it = exceptList.begin(); it != exceptList.end(); ++it ) {
        item = (*it);
        if(item.length() == 8) {
            lbExceptions->insertItem(item);
        }
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

QString Exceptions::getExceptions()
{
    QString exc = "";
    for (unsigned int i=0; i<lbExceptions->count(); i++) {
        if(i>0)
            exc += " ";
        exc += lbExceptions->text(i);
    }
    return exc;
}
