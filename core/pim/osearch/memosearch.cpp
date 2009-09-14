/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
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

#include "memosearch.h"
#include "memoitem.h"

#include <opie2/oresource.h>

#include <qpe/config.h>

#include <qaction.h>

using namespace Opie;

MemoSearch::MemoSearch(QListView* parent, QString name)
: SearchGroup(parent, name), _memos(0)
{
    setPixmap( 0, Opie::Core::OResource::loadPixmap( "opie-notes/opie-notes", Opie::Core::OResource::SmallIcon ) );
}

MemoSearch::~MemoSearch()
{
    delete _memos;
}

void MemoSearch::load()
{
    _memos = new OPimMemoAccess();
    _memos->load();
}

int MemoSearch::search()
{
    OPimRecordList<OPimMemo> results = _memos->matchRegexp(m_search);
    for (uint i = 0; i < results.count(); i++)
        insertItem( new OPimMemo( results[i] ));
    return m_resultCount;
}

void MemoSearch::insertItem( void *rec )
{
    OPimMemo *memo = (OPimMemo*)rec;
    (void)new MemoItem( this, memo );
    m_resultCount++;
}
