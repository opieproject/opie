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

#ifndef TABLEDEF_H
#define TABLEDEF_H

#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>



// --- ColumnDef -------------------------------------------------------------
class ColumnDef
{
    public:
        enum ColumnType {
            typeString=0x1,
            typeList=0x2,
            typeUnique=0x80000000
        };

        // Constructor
        ColumnDef(const char *sName, ColumnType type, const char *sNewValue);

        // add column value
        void addColumnValue(const QString &Value);
        void addColumnValue(const char *sValue);

        // member functions
        const QString getName() { return(_sName); }
        const QString getNewValue() { return(_sNewValue); }

        // test for type
        int isType(ColumnType x) { return( (_type & 0x00ffffff)==x ); }
        int hasFlag(ColumnType x) { return( (_type & x) ); }

        // get value list
        QStringList &getValueList() { return(_valueList); }

    private:
        QString _sName;
        QString _sNewValue;
        enum ColumnType _type;
        QStringList _valueList;
};

typedef QList<ColumnDef> ColumnDefList;


// --- TableDef ---------------------------------------------------------------
class TableDef
{
    public:
        // Constructor & Destructor
        TableDef(const char *sName);
        virtual ~TableDef();

        // adds a column definition
        virtual void addColumnDef(ColumnDef *pDef);

        // movement operators
        ColumnDef *first() { return(_vColumns.first() ); }
        ColumnDef *last() { return(_vColumns.last() ); }
        ColumnDef *next() { return(_vColumns.next() ); }
        ColumnDef *prev() { return(_vColumns.prev() ); }
        ColumnDef *at(int i) { return(_vColumns.at(i)); }

    protected:
        QString _sName;
        ColumnDefList _vColumns;
};

#endif
