/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2009 The Opie Team <opie-devel@lists.sourceforge.net>
            .=l.             Portions copyright (C) 2003 Patrick S. Vogt <tille@handhelds.org>
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

#include "memoitem.h"

#include <opie2/oresource.h>

#include <qpe/qcopenvelope_qws.h>

using namespace Opie;
MemoItem::MemoItem(OListViewItem* parent, OPimMemo *memo)
    : ResultItem(parent)
{
    _memo = memo;
    setText( 0, memo->toShortText() );
    setIcon();
}

MemoItem::~MemoItem()
{
    delete _memo;
}

QString MemoItem::toRichText()
{
    return _memo->toRichText();
}

void MemoItem::action( int act )
{
    if ( act == 0 ) {
        QCopEnvelope e("QPE/Application/opie-notes", "edit(int)");
        e << _memo->uid();
    }
}

QIntDict<QString> MemoItem::actions()
{
    QIntDict<QString> result;
    result.insert( 0, new QString( QObject::tr("edit") ) );
    return result;
}

void MemoItem::setIcon()
{
    QPixmap icon;
    icon = Opie::Core::OResource::loadPixmap( "DocsIcon", Opie::Core::OResource::SmallIcon );
    setPixmap( 0, icon );
}
