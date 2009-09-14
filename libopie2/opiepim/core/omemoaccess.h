/*
                             This file is part of the Opie Project
                             
              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
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
/*
 * =====================================================================
 * ToDo: Define enum for query settings
 * =====================================================================
 */
#ifndef _OMEMOACCESS_H
#define _OMEMOACCESS_H

#include <qobject.h>

#include <qpe/qcopenvelope_qws.h>

#include <qvaluelist.h>
#include <qfileinfo.h>

#include <opie2/opimmemo.h>
#include <opie2/omemoaccessbackend.h>
#include <opie2/opimaccesstemplate.h>

namespace Opie {
/**
 * Class to access the notes database.
 * This is just a frontend for the real database handling which is
 * done by the backend.
 * This class is used to access the memos on a system. This class as any OPIE PIM
 * class is backend independent.
 * @author Stefan Eilers, Holger Freyther, Paul Eggleton
 * @see OPimAccessTemplate
 */
class OPimMemoAccess: public QObject, public OPimAccessTemplate<OPimMemo>
{
    Q_OBJECT

 public:



        /**
         * Create Database with memos (notes).
         * @param appname Name of application which wants access to the database
         * (e.g. "addressbook"). Specify null to disable journal.
         * @param filename The name of the database file. If not set, the default one
         * is used.
         * @param backend Pointer to an alternative Backend. If not set, we will use
         * the default backend.
         * @param handlesync If <b>true</b> the database stores the current state
         * automatically if it receives the signals <i>flush()</i> and <i>reload()</i>
         * which are used before and after synchronisation. If the application wants
         * to react itself, it should be disabled by setting it to <b>false</b>
         * @see OPimMemoAccessBackend
         */
        OPimMemoAccess (const QString appname = 0l, const QString filename = 0l,
                           OPimMemoAccessBackend* backend = 0l, bool handlesync = true);
        ~OPimMemoAccess ();


        /**
         * if the resource was changed externally.
         * You should use the signal instead of polling possible changes !
         */
        bool wasChangedExternally()const;


        /** Save notes database.
         * Save is more a "commit". After calling this function, all changes are public available.
         * @return true if successful
         */
        bool save();

        /**
         * Return identification of used records
         */
        int rtti() const;

        OPimChangeLog *changeLog() const;

 signals:
        /* Signal is emitted if the database was changed. Therefore
         * we may need to reload to stay consistent.
         * @param which Pointer to the database who created this event. This pointer
         * is useful if an application has to handle multiple databases at the same time.
         * @see reload()
         */
        void signalChanged ( const OPimMemoAccess *which );

 private:
        OPimMemoAccessBackend *m_backEnd;
        bool m_loading:1;

 private slots:
        void copMessage( const QCString &msg, const QByteArray &data );

 private:
        class Private;
        Private *d;

};

}

#endif
