/*
 * Class to manage the Contacts.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 * Copyright (c) 2002 by Holger Freyther (zecke@handhelds.org)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation;
 *      either version 2 of the License, or (at your option) any later
 *      version.
 * =====================================================================
 * ToDo: Define enum for query settings
 * =====================================================================
 * Version: $Id: ocontactaccess.h,v 1.6 2003-01-02 14:27:12 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccess.h,v $
 * Revision 1.6  2003-01-02 14:27:12  eilers
 * Improved query by example: Search by date is possible.. First step
 * for a today plugin for birthdays..
 *
 * Revision 1.5  2002/11/13 14:14:51  eilers
 * Added sorted for Contacts..
 *
 * Revision 1.4  2002/11/01 15:10:42  eilers
 * Added regExp-search in database for all fields in a contact.
 *
 * Revision 1.3  2002/10/16 10:52:40  eilers
 * Added some docu to the interface and now using the cache infrastucture by zecke.. :)
 *
 * Revision 1.2  2002/10/14 16:21:54  eilers
 * Some minor interface updates
 *
 * Revision 1.1  2002/09/27 17:11:44  eilers
 * Added API for accessing the Contact-Database ! It is compiling, but
 * please do not expect that anything is working !
 * I will debug that stuff in the next time ..
 * Please read README_COMPILE for compiling !
 *
 * =====================================================================
 */
#ifndef _OCONTACTACCESS_H
#define _OCONTACTACCESS_H

#include <qobject.h>

#include <qpe/qcopenvelope_qws.h>

#include <qvaluelist.h>
#include <qfileinfo.h>

#include "ocontact.h"
#include "ocontactaccessbackend.h"
#include "opimaccesstemplate.h"

/** Class to access the contacts database.
 * This is just a frontend for the real database handling which is
 * done by the backend.
 * @see OPimAccessTemplate
 */
class OContactAccess: public QObject, public OPimAccessTemplate<OContact>
{
    Q_OBJECT

 public:
	/** Create Database with contacts (addressbook).
	 * @param appname Name of application which wants access to the database
	 * (i.e. "todolist")
	 * @param filename The name of the database file. If not set, the default one
	 * is used.
	 * @param backend Pointer to an alternative Backend. If not set, we will use
	 * the default backend.
	 * @param handlesync If <b>true</b> the database stores the current state
	 * automatically if it receives the signals <i>flush()</i> and <i>reload()</i>
	 * which are used before and after synchronisation. If the application wants
	 * to react itself, it should be disabled by setting it to <b>false</b>
	 * @see OContactAccessBackend
	 */
	OContactAccess (const QString appname, const QString filename = 0l,
                    OContactAccessBackend* backend = 0l, bool handlesync = true);
	~OContactAccess ();

	/** Constants for query.
	 * Use this constants to set the query parameters.
	 * Note: <i>query_IgnoreCase</i> just make sense with one of the other attributes !
	 * @see queryByExample()
	 */
	enum QuerySettings {
		WildCards  = 0x0001,
		IgnoreCase = 0x0002,
		RegExp     = 0x0004,
		ExactMatch = 0x0008,
		MatchOne   = 0x0010, // Only one Entry must match
		DateDiff   = 0x0020, // Find all entries from today until given date
		DateYear   = 0x0040, // The year matches
		DateMonth  = 0x0080, // The month matches
		DateDay    = 0x0100, // The day matches
	};

	
	ORecordList<OContact> matchRegexp(  const QRegExp &r )const;

	/** Return all Contacts in a sorted manner.
	 *  @param ascending true: Sorted in acending order.
	 *  @param sortOrder Currently not implemented. Just defined to stay compatible to otodoaccess
	 *  @param sortFilter Currently not implemented. Just defined to stay compatible to otodoaccess
	 *  @param cat Currently not implemented. Just defined to stay compatible to otodoaccess
	 */
	List sorted( bool ascending, int sortOrder, int sortFilter, int cat ) const;

	/** Return all possible settings.
	 *  @return All settings provided by the current backend
	 * (i.e.: query_WildCards & query_IgnoreCase)
	 */
	const uint querySettings();

	/** Check whether settings are correct.
	 * @return <i>true</i> if the given settings are correct and possible.
	 */
	bool hasQuerySettings ( int querySettings ) const;

	/**
	 * if the resource was changed externally.
	 * You should use the signal instead of polling possible changes !
	 */
	bool wasChangedExternally()const;


	/** Save contacts database.
	 * Save is more a "commit". After calling this function, all changes are public available.
	 * @return true if successful
	 */
	bool save();

 signals:
	/* Signal is emitted if the database was changed. Therefore
	 * we may need to reload to stay consistent.
	 * @param which Pointer to the database who created this event. This pointer
	 * is useful if an application has to handle multiple databases at the same time.
	 * @see reload()
	 */
	void signalChanged ( const OContactAccess *which );


 private:
	// class OContactAccessPrivate;
	// OContactAccessPrivate* d;
        OContactAccessBackend *m_backEnd;
        bool m_loading:1;

 private slots:
	void copMessage( const QCString &msg, const QByteArray &data );


};
#endif
