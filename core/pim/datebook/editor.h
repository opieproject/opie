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

#ifndef OPIE_DATEBOOK_EDITOR_H
#define OPIE_DATEBOOK_EDITOR_H

#include <qdialog.h>

#include <opie2/opimevent.h>

#include "descriptionmanager.h"
#include "locationmanager.h"

namespace Opie {
namespace Datebook {
    class MainWindow;
    /**
     * This is the editor interface
     */
    class Editor {
    public:
        Editor( MainWindow*, QWidget* parent );
        virtual ~Editor();

        virtual bool newEvent( const QDate& ) = 0;
        virtual bool newEvent( const QDateTime& start, const QDateTime& end, const QString &desc ) = 0;
        virtual bool edit( const OPimEvent&, bool duplicate = FALSE ) = 0;

        virtual OPimEvent event() const = 0;

    protected:
        DescriptionManager descriptions()const;
        LocationManager locations()const;
        void setDescriptions( const DescriptionManager& );
        void setLocations( const LocationManager& );

        /**
         * Is the clock AM/PM or 24h?
         */
        bool isAP()const;

        /**
         * if the week starts on monday?
         */
        bool weekStartOnMonday()const;

        QString defaultLocation()const;
        QArray<int> defaultCategories()const;
        bool alarmPreset()const;
        int alarmPresetTime()const;
        int alarmPresetUnits()const;

    private:
        MainWindow* m_win;

    };
}
}

#endif
