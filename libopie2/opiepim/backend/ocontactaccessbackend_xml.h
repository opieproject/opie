/*
 * XML Backend for the OPIE-Contact Database.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * =====================================================================
 * ToDo: XML-Backend: Automatic reload if something was changed...
 *
 *
 * =====================================================================
 * Version: $Id: ocontactaccessbackend_xml.h,v 1.13 2003-03-21 10:33:09 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend_xml.h,v $
 * Revision 1.13  2003-03-21 10:33:09  eilers
 * Merged speed optimized xml backend for contacts to main.
 * Added QDateTime to querybyexample. For instance, it is now possible to get
 * all Birthdays/Anniversaries between two dates. This should be used
 * to show all birthdays in the datebook..
 * This change is sourcecode backward compatible but you have to upgrade
 * the binaries for today-addressbook.
 *
 * Revision 1.12.2.2  2003/02/11 12:17:28  eilers
 * Speed optimization. Removed the sequential search loops.
 *
 * Revision 1.12.2.1  2003/02/09 15:05:01  eilers
 * Nothing happened.. Just some cleanup before I will start..
 *
 * Revision 1.12  2003/01/03 16:58:03  eilers
 * Reenable debug output
 *
 * Revision 1.11  2003/01/03 12:31:28  eilers
 * Bugfix for calculating data diffs..
 *
 * Revision 1.10  2003/01/02 14:27:12  eilers
 * Improved query by example: Search by date is possible.. First step
 * for a today plugin for birthdays..
 *
 * Revision 1.9  2002/12/08 12:48:57  eilers
 * Moved journal-enum from ocontact into i the xml-backend..
 *
 * Revision 1.8  2002/11/14 17:04:24  eilers
 * Sorting will now work if fullname is identical on some entries
 *
 * Revision 1.7  2002/11/13 15:02:46  eilers
 * Small Bug in sorted fixed
 *
 * Revision 1.6  2002/11/13 14:14:51  eilers
 * Added sorted for Contacts..
 *
 * Revision 1.5  2002/11/01 15:10:42  eilers
 * Added regExp-search in database for all fields in a contact.
 *
 * Revision 1.4  2002/10/16 10:52:40  eilers
 * Added some docu to the interface and now using the cache infrastucture by zecke.. :)
 *
 * Revision 1.3  2002/10/14 16:21:54  eilers
 * Some minor interface updates
 *
 * Revision 1.2  2002/10/07 17:34:24  eilers
 * added OBackendFactory for advanced backend access
 *
 * Revision 1.1  2002/09/27 17:11:44  eilers
 * Added API for accessing the Contact-Database ! It is compiling, but
 * please do not expect that anything is working !
 * I will debug that stuff in the next time ..
 * Please read README_COMPILE for compiling !
 *
 *
 */

#ifndef _OContactAccessBackend_XML_
#define _OContactAccessBackend_XML_

#include "ocontactaccessbackend.h"
#include "ocontactaccess.h"

#include <qlist.h>
#include <qdict.h>

/* the default xml implementation */
class OContactAccessBackend_XML : public OContactAccessBackend {
 public:
	OContactAccessBackend_XML ( QString appname, QString filename = 0l );
	
	bool save();
	
	bool load ();

	void clear ();
	
	bool wasChangedExternally();
	
	QArray<int> allRecords() const;
	
	OContact find ( int uid ) const;
	
	QArray<int> queryByExample ( const OContact &query, int settings, const QDateTime& d = QDateTime() );

	QArray<int> matchRegexp(  const QRegExp &r ) const;
	
	const uint querySettings();
	
	bool hasQuerySettings (uint querySettings) const;
	
	// Currently only asc implemented.. 
	QArray<int> sorted( bool asc,  int , int ,  int );
	bool add ( const OContact &newcontact );
	
	bool replace ( const OContact &contact );
	
	bool remove ( int uid );
	bool reload();
	
 private:

	enum journal_action { ACTION_ADD, ACTION_REMOVE, ACTION_REPLACE };

	void addContact_p( const OContact &newcontact );
	
	/* This function loads the xml-database and the journalfile */
	bool load( const QString filename, bool isJournal );
	
	
	void updateJournal( const OContact& cnt, journal_action action );
	void removeJournal();
	
 protected:
	bool m_changed;
	QString m_journalName;
	QString m_fileName;
	QString m_appName;
	QList<OContact> m_contactList;
	QDateTime m_readtime;

	QDict<OContact> m_uidToContact;
};

#endif
