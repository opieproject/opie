/*
 * Class to manage the Contacts. It was derived from the 
 * abtable-class of the addressbook. 
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 * Copyright (C) 2000 Trolltech AS.  All rights reserved.
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation; either version 2 of
 *	the License, or (at your option) any later version.
 * =====================================================================
 * ToDo: ...
 * =====================================================================
 * Version: $Id: ocontactdb.h,v 1.1.2.1 2002-07-01 16:49:46 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb.h,v $
 * Revision 1.1.2.1  2002-07-01 16:49:46  eilers
 * First attempt for cross reference
 *
 * =====================================================================
 * This database emits the following signal if the xml-file is changed:
 * Channel: "QPE/PIM", Message: "addressbookUpdated()"
 * Every Application which receives this message should reload the 
 * database and update the widgets to stay synchronized.. 
 */
#ifndef _OCONTACTDB_H
#define _OCONTACTDB_H

// #include <qpe/categories.h>
#include <qpe/contact.h>
#include <qvaluelist.h> 
#include <qfileinfo.h> 

/** Class to access the contacts database.
 */
class OContactDB {
 public: 
	/** Create Database with contacts (addressbook).
	 * @param appname Name of application which wants access to the database 
	 * (i.e. "todolist")
	 * @param fileName Path and name of the xml file. If "NULL" it will open
	 * the default addressbook file
	 */
	OContactDB (const QString appname, const QString &fileName = QString::null);
	~OContactDB ();

	/** Returns all Contacts.
	 * @return List of all contacts.
	 */
	QValueList<Contact> getAllContacts();

	/** Add Contact to database.
	 * @param newcontact The contact to add.
	 * @return <i>true</i> if added successfully.
	 */
	bool addContact (const Contact &newcontact);

	/** Replace contact. 
	 * Replaces given contact with contact with the user id <i>uid</i>.
	 * @param uid The user ID
	 * @param contact The new contact
	 * @return <i>true</i> if successful.
	 */
	bool replaceContact (int uid, const Contact &contact);

	/** Remove contact. 
	 * Removes contact with the user id <i>uid</i>.
	 * @param uid The user ID
	 * @return <i>true</i> if successful.
	 */
	bool removeContact (int uid, const Contact &contact);


	/** Check if the xml-file was modified since last (re)load.
	 * If this function returns true, we have to reload the database to
	 * stay consistent. 
	 * @return <b>true</b> if the xml-file was changed
	 */
	bool isChangedExternally();

	/** Reload database.
	 * You should execute this function if the external xml-file
	 * was changed to stay consistent.
	 * This function will load the xml-file and afterwards
	 * the journalfile. Therefore the database will be set consistent.
	 */
	void OContactDB::reload();

	/** Save contacts database
	 * @param autoreload <b>true:</b> The function <i>reload()</i> will be exectuted if the 
	 * external xml-file was changed. Otherwise this operation is cancelled.
	 * @return true if successful
	 */
	bool save(bool autoreload = true);

 private:
	QString loc_fileName;
	QString loc_journalName;
	QValueList<Contact> contactList;
	QDateTime readtime;

	/** Load contacts database.
	 * Future: Bool for locking ...
	 * @param Name of the file to load
	 */
	void load(QString filename);

	void updateJournal( const Contact &cnt,
			    Contact::journal_action action);

};
#endif
