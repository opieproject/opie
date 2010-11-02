/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@lists.sourceforge.net>
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

#ifndef OPIE_DATE_BOOK_MANAGER_H
#define OPIE_DATE_BOOK_MANAGER_H

#include <qlist.h>

#include <opie2/opimrecord.h>
#include <opie2/odatebookaccess.h>
#include <opie2/opimoccurrence.h>

#include "holiday.h"

namespace Opie {
namespace Datebook {
    /**
     * book manager is responsible for managing the
     * access
     * All access to the underlying API will be done
     * through this class
     */
    class BookManager {
    public:
        BookManager();
        ~BookManager();

        bool isLoaded()const;
        bool load();
        void reload();
        bool save();

        OPimEvent event( int uid );
        OPimOccurrence::List list( const QDate& from,
                                         const QDate& to );
        ODateBookAccess::List allRecords()const;

        void add( const OPimEvent& );
        void update( const OPimEvent& );
        void remove( int uid );
        void remove( const QArray<int>& );

        QList<OPimRecord> records( const QDate& from,
                                      const QDate& to );

        DateBookHoliday *holiday();

        static bool nextOccurrence( const OPimEvent &ev, const QDateTime &start, QDateTime &dt );
        QDate findRealStart( const OPimOccurrence &occ );
        OPimEvent find( const QString &str, bool caseSensitive, QDateTime &dt );

        void snoozeAlarm( const QDateTime &dt, int uid );
        void setupAlarms( const OPimEvent &ev );
        void setupAllAlarms();

    protected:
        void addAlarms( const OPimEvent &ev );
        void removeAlarms( const OPimEvent &ev );

    private:
        ODateBookAccess* m_db;
        DateBookHoliday* m_holiday;
    };
}
}

#endif
