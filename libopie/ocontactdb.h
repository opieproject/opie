/*
 * Class to manage the Contacts. It was derived from the
 * abtable-class of the addressbook.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
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
 * Version: $Id: ocontactdb.h,v 1.1.2.8 2002-07-13 17:19:20 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb.h,v $
 * Revision 1.1.2.8  2002-07-13 17:19:20  eilers
 * Added signal handling:
 * The database will be informed if it is changed externally and if flush() or
 * reload() signals sent. The application which is using the database may
 * reload manually if this happens...
 *
 * Revision 1.1.2.7  2002/07/07 16:24:47  eilers
 * All active parts moved into the backend. It should be easily possible to
 * use a database as backend
 *
 * Revision 1.1.2.6  2002/07/07 12:40:35  zecke
 * Why was this there in the first place?
 *
 * Revision 1.1.2.5  2002/07/06 16:06:52  eilers
 * Some bugfixes and cleanup of inconsistencies
 *
 * Revision 1.1.2.4  2002/07/05 13:03:30  zecke
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

#include <qobject.h>
#include <qcopchannel_qws.h> 

// #include <qpe/categories.h>
#include <qpe/contact.h>

#include <qvaluelist.h>
#include <qfileinfo.h>


class OContactDB;
/**
 * The class responsible for managing a backend.
 * The implementation of this abstract class contains
 * the complete database handling.
 *
 */
class OContactBackend {
 public:
	OContactBackend() : m_parent(0 ) {}
	virtual ~OContactBackend() {}

	/** Get the parent of this backend.
	 * @return Pointer to the parent OContactDB class
	 */
	OContactDB* parent() { return m_parent; }
	
	/** Set the parent of this backend.
	 * @param Pointer to the parent OContactDB class
	 */
	virtual void setParent ( OContactDB * parent ) = 0;
	
	/** Save the list of Contacts into database.
	 * @return either success or failure
	 */
	virtual bool save() = 0;
	
	/**
	 * returns the success or failure
	 * it loads the Contacts to list
	 */
	virtual bool load() = 0 ;
	
	/** Reload database.
	 * You should execute this function to stay consistent if the external database
	 * was changed. 
	 * Locally changed contacts will be rejoined with the external content !
	 */
	virtual void reload() = 0;

	/** Return if database was changed externally.
	 */
	virtual bool isChangedExternally() = 0;
	
	/** Finds a Contact by the uid.
	 * @param foundContact The Contact found if returnvalue is <i>true</i>
	 * @param uid The user ID of the contact
	 * @return <i>true</i> if found, else <i>false</i>
	 */
	virtual bool findContact(Contact &foundContact, int uid ) = 0;

	/** Returns all Contacts.
	 * @return List of all contacts.
	 */
	virtual QValueList<Contact> allContacts() const = 0;

	/** Add Contact to database.
	 * @param newcontact The contact to add.
	 * @return <i>true</i> if added successfully.
	 */
	virtual bool addContact (const Contact &newcontact) = 0;

	/** Replace contact.
	 * Replaces given contact with contact with the user id <i>uid</i>.
	 * @param uid The user ID
	 * @param contact The new contact
	 * @return <i>true</i> if successful.
	 */
	virtual bool replaceContact (int uid, const Contact &contact) = 0;
	
	/** Remove contact.
	 * Removes contact with the user id <i>uid</i>.
	 * @param uid The user ID
	 * @return <i>true</i> if successful.
	 */
	virtual bool removeContact (int uid, const Contact &contact) = 0;

	protected:
		OContactDB *m_parent;
	
};

/** Class to access the contacts database.
 * This is just a frontend for the real database handling which ich
 * done by the backend.
 */
class OContactDB: public QObject 
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
	 * @param autosync If <b>true</b> the database stores the current state 
	 * automatically if it receives the signals <i>flush()</i> and <i>reload()</i>
	 * which are used before and after synchronisation. If the application wants 
	 * to react itself, it should be disabled by setting it to <b>false</b>
	 * @see OContactBackend 
	 */
	OContactDB (const QString appname, const QString filename = 0l,
                    OContactBackend* backend = 0l, bool autosync = true);
	~OContactDB ();
	
	/** Returns all Contacts.
	 * @return List of all contacts.
	 */
	QValueList<Contact> allContacts()const;
	
	/** Finds a Contact by the uid.
	 * @param foundContact The Contact found if returnvalue is <i>true</i>
	 * @param uid The user ID of the contact
	 * @return <i>true</i> if found, else <i>false</i>
	 */
	bool findContact(Contact &foundContact, int uid );
	
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
	
      	/** Check if the database was modified since last (re)load.
	 * If this function returns true, we have to reload the database to
	 * stay consistent.
	 * @return <b>true</b> if the xml-file was changed
	 */
	bool isChangedExternally();
	
	/** Reload database.
	 * You should execute this function if the external database
	 * was changed.
	 * This function will load the external database and afterwards
	 * rejoin the local changes. Therefore the local database will be set consistent.
	 */
	void reload();
	
	/** Save contacts database
	 * @param autoreload <b>true:</b> The function <i>reload()</i> will be exectuted if the
	 * external database was changed. Otherwise this operation is cancelled.
	 * @return true if successful
	 */
	bool save(bool autoreload = true);

 signals:
	/* Signal is emitted if the database was changed. Therefore
	 * we may need to reload to stay consistent.
	 * @param which Pointer to the database who created this event. This pointer
	 * is useful if an application has to handle multiple databases at the same time.
	 */ 
	void signalChanged ( const OContactDB *which );

	
 private:
	/*         class OContactDBPrivate; */
	/*         OContactDBPrivate* d; */
        OContactBackend *m_backEnd;
        bool m_loading:1;

 private slots:
	void copMessage( const QCString &msg, const QByteArray &data );

	
};
#endif
