/*
 * Class to manage the Contacts. It was derived from the
 * abtable-class of the addressbook.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 * Copyright (C) 2000 Trolltech AS.  All rights reserved.
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation;
 *      either version 2 of the License, or (at your option) any later
 *      version.
 * =====================================================================
 * ToDo: ...
 * =====================================================================
 * Version: $Id: ocontactdb.h,v 1.1.2.4 2002-07-05 13:03:30 zecke Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb.h,v $
 * Revision 1.1.2.4  2002-07-05 13:03:30  zecke
 * Move the stuff responsible for loading and unloading
 * out of the ContactDB. This will make the switch to a real
 * database more easy.
 * Contact::insert is private so I had to workaround this 'feature'
 * CVS
 *
 * Revision 1.1.2.3  2002/07/05 11:17:19  zecke
 * Some API updates by me
 *
 * Revision 1.1.2.2  2002/07/03 06:55:54  eilers
 * Moved to LGPL
 *
 * Revision 1.1.2.1  2002/07/01 16:49:46  eilers
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


class OContactDB;
/**
 * The class responsible for managing a backend.
 *
 */
class OContactBackend {
public:
    OContactBackend() : m_parent(0 ) {}
    virtual ~OContactBackend() {}
    OContactDB* parent() { return m_parent; }
    /**
     * Save the list of Contacts to fileName
     * @param fileName the fileName
     * @param list the list of Contacts
     * @param autoreload
     * @return either success or failure
     */
    virtual bool save( const QValueList<Contact>& list  ) = 0;

    /**
     * returns the success or failure
     * it loads the Contacts to list
     */
    virtual bool load( ) = 0 ;

    /**
     * updates the journal
     */
    virtual void updateJournal( const Contact &cnt,
                           Contact::journal_action action) = 0;

protected:
    friend class OContactDB;
    QString m_journal;
    QString m_filename;
    OContactDB *m_parent;
};

/** Class to access the contacts database.
 */
class OContactDB {
 public:
	/** Create Database with contacts (addressbook).
	 * @param appname Name of application which wants access to the database
	 * (i.e. "todolist")
	 * @param fileName Path and name of the xml file. If isEmpty() it will open
	 * the default addressbook file
	 */
	OContactDB (const QString appname,
                    const QString &fileName = QString::null,
                    OContactBackend* backend = 0l);
	~OContactDB ();

	/** Returns all Contacts.
	 * @return List of all contacts.
	 */
	QValueList<Contact> allContacts()const;

	/**
	 * Finds a Contact by the uid
	 */
	Contact find( int uid )const;

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
        class OContactDBPrivate;
        OContactDBPrivate* d;
        OContactBackend *m_backEnd;
	QString m_loc_fileName;
	QString m_loc_journalName;
	QValueList<Contact> m_contactList;
	QDateTime m_readtime;
        bool m_loading:1;

	/** Load contacts database.
	 * Future: Bool for locking ...
	 * @param Name of the file to load
	 */
	void load(const QString& filename);

	void updateJournal( const Contact &cnt,
			    Contact::journal_action action);

};
#endif
