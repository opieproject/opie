/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
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
/*
 * XML Backend for the OPIE-Contact Database.
 */

#ifndef _OPimContactAccessBackend_XML_
#define _OPimContactAccessBackend_XML_

#include <opie2/ocontactaccessbackend.h>
#include <opie2/ocontactaccess.h>

#include <qlist.h>
#include <qdict.h>

namespace Opie {
/* the default xml implementation */
/**
 * This class is the XML implementation of a Contact backend
 * it does implement everything available for OPimContact.
 * @see OPimAccessBackend for more information of available methods
 */
class OPimContactAccessBackend_XML : public OPimContactAccessBackend {
 public:
	OPimContactAccessBackend_XML ( const QString& appname, const QString& filename = QString::null );

	bool save();

	bool load ();

	void clear ();

	bool wasChangedExternally();

	QArray<int> allRecords() const;

	OPimContact find ( int uid ) const;

	QArray<int> queryByExample ( const OPimContact &query, int settings, const QDateTime& d = QDateTime() );

	QArray<int> matchRegexp(  const QRegExp &r ) const;

	const uint querySettings();

	bool hasQuerySettings (uint querySettings) const;

	// Currently only asc implemented..
	QArray<int> sorted( bool asc,  int , int ,  int );
	bool add ( const OPimContact &newcontact );

	bool replace ( const OPimContact &contact );

	bool remove ( int uid );
	bool reload();

 private:

	enum journal_action { ACTION_ADD, ACTION_REMOVE, ACTION_REPLACE };

	void addContact_p( const OPimContact &newcontact );

	/* This function loads the xml-database and the journalfile */
	bool load( const QString filename, bool isJournal );


	void updateJournal( const OPimContact& cnt, journal_action action );
	void removeJournal();

 protected:
	bool m_changed;
	QString m_journalName;
	QString m_fileName;
	QString m_appName;
	QList<OPimContact> m_contactList;
	QDateTime m_readtime;

	QDict<OPimContact> m_uidToContact;
};

}

#endif
