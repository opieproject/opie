/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
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

#include "tabledef.h"

#include <opie2/odebug.h>
using namespace Opie::Core;

// --- ColumnDef --------------------------------------------------------------
ColumnDef::ColumnDef(const char *sName, ColumnType type, const char *sNewValue)
{
    _sName=sName;
    _type=type;
    _sNewValue=sNewValue;
}


// --- addColumnValue ---------------------------------------------------------
void ColumnDef::addColumnValue(const QString &sValue)
{
    if( (_type & 0x00ffffff) !=typeList )
        odebug << "Column " << (const char *)_sName << " is not a list" << oendl; 
    else
        _valueList.append(sValue);
}
void ColumnDef::addColumnValue(const char *sValue)
{
    if( (_type & 0x00ffffff)!=typeList )
        odebug << "Column " << (const char *)_sName << " is not a list" << oendl; 
    else
        _valueList.append(sValue);
}

// --- TableDef ---------------------------------------------------------------
TableDef::TableDef(const char *sName)
{
    _sName=sName;
    _vColumns.setAutoDelete(TRUE);
}


// --- ~TableDef --------------------------------------------------------------
TableDef::~TableDef()
{
}

// --- addColumnDef -----------------------------------------------------------
void TableDef::addColumnDef(ColumnDef *pDef)
{
    _vColumns.append(pDef);
}
