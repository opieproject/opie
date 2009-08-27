/*
                             This file is part of the Opie Project
                             Copyright (C) Holger Freyther <freyther@handhelds.org>
                             Copyright (C) Stefan Eilers <stefan@eilers-online.net>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
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
/**
 * The class responsible for managing a backend.
 * The implementation of this abstract class contains
 * the complete database handling.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 * Copyright (c) 2002 by Holger Freyther (zecke@handhelds.org)
 *
 */

#ifndef _OCONTACTACCESSBACKEND_H_
#define _OCONTACTACCESSBACKEND_H_

#include <opie2/opimcontact.h>
#include <opie2/opimaccessbackend.h>

#include <qregexp.h>

namespace Opie {
/**
 * This class represents the interface of all Contact Backends.
 * Derivates of this class will be used to access the contacts.
 * As implementation currently XML and vCard exist. This class needs to be implemented
 * if you want to provide your own storage.
 * In all queries a list of uids is passed on instead of loading the actual record!
 *
 * @see OPimContactAccessBackend_VCard
 * @see OPimContactAccessBackend_XML
 */
class OPimContactAccessBackend: public OPimAccessBackend<OPimContact> {
 public:
    OPimContactAccessBackend();


    /**
     * Return if database was changed externally.
     * This may just make sense on file based databases like a XML-File.
     * It is used to prevent to overwrite the current database content
     * if the file was already changed by something else !
     * If this happens, we have to reload before save our data.
     * If we use real databases, this should be handled by the database
     * management system themselve, therefore this function should always return false in
     * this case. It is not our problem to handle this conflict ...
     * @return <i>true</i> if the database was changed and if save without reload will
     * be dangerous. <i>false</i> if the database was not changed or it is save to write
     * in this situation.
     */
    virtual bool wasChangedExternally() = 0;

    /**
     *  Return all possible settings.
     *  @return All settings provided by the current backend
     * (i.e.: query_WildCards & query_IgnoreCase)
     */
    uint querySettings() const;

    /**
     * Check whether settings are correct.
     * @return <i>true</i> if the given settings are correct and possible.
     */
    bool hasQuerySettings (uint querySettings) const;

    /**
     * Advanced search mechanism.
     */
    UIDArray queryByExample( const UIDArray& uidlist, const OPimContact&, int settings, const QDateTime &d = QDateTime() ) const;
    /**
     * Slow and inefficent default implementation
     */
//@{
    UIDArray sorted( const UIDArray&, bool asc, int, int, const QArray<int>& )const;
    OPimBackendOccurrence::List occurrences( const QDate&, const QDate& )const;
//@}


private:
    class Private;
    Private *d;
};

}

#endif
