/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.             
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef _CONVERTER_H_
#define _CONVERTER_H_


#include "converter_base.h"


class Converter: public converter_base {
public:
    Converter(QWidget *parent, const char* name, WFlags fl);
    static QString appName() { return QString::fromLatin1("opimconverter");}

    // Slots defined in the ui-description file
    void start_conversion();
    void selectedDatabase( int num );
    void selectedDestFormat( int num );
    void selectedSourceFormat( int num );

    void closeEvent( QCloseEvent *e );
    
private:
    // Caution:
    // The order and value of the following enums must be regarding
    // the predefinition in the UI-File !!
    // If you don't understand what I am talking about: Keep your fingers away!!
    enum DataBases{
        ADDRESSBOOK = 0,
        TODOLIST    = 1,
        DATEBOOK    = 2,
    };

    enum DbFormats{
        XML = 0,
        SQL = 1,
    };

    int m_selectedDatabase;
    int m_selectedSourceFormat;
    int m_selectedDestFormat;
    bool m_criticalState;


private:
    void loadPimAccess();
    void savePimAccess();
    bool sqliteMoveAndConvert( const QString& name, const QString &src, const QString &destination );
};


#endif
