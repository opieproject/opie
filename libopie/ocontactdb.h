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
 * Version: $Id: ocontactdb.h,v 1.1.2.11 2002-07-21 15:21:26 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb.h,v $
 * Revision 1.1.2.11  2002-07-21 15:21:26  eilers
 * Some interface changes and minor bugfixes...
 * The search interface is able to use wildcards, regular expressions and
 * ignore cases... I love the Trolltech cClasslibrary ! :)
 *
 * Revision 1.1.2.10  2002/07/14 13:50:08  eilers
 * Interface change... REMEMBER: The search function is currently totally
 * untested !!
 *
 * Revision 1.1.2.9  2002/07/14 13:41:30  eilers
 * Some bugfixes,
 * Added interface for searching contacts in database, using "query by example"
 * like system
 *
 * Revision 1.1.2.8  2002/07/13 17:19:20  eilers
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
	OContactBackend() {}
	virtual ~OContactBackend() {}
	
	/** Save the list of Contacts into database.
	 * This function will store the local changes into the database.
	 * Therefore they will be available to the public.
	 * (thus, this is a database commit !)
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

	/** "Query by example" like search.
	 * To find a special contact, just fill all known information into
	 * a new contact-object and start query with this function.
	 * All information will be connected by an "AND".
	 * @param query The query form.
	 * @param settings The parameters how to perform the query.
	 * @return <i>NULL</i> if nothing was found or the first contact.
	 * @see nextFound(), query_RegExp, query_WildCards, query_ExactMatch, query_IgnoreCase
	 */
	virtual const Contact *queryByExample ( const Contact &query, const uint settings ) = 0;

	/** Requests a contact which was selected by queryByExample().
	 * Use this function to move through the list of selected contacts.
	 * @return Pointer to next contact or <i>NULL</i> if list empty or no 
	 * next element.
	 * @see qeryByExample
	 */
	virtual const Contact *nextFound () = 0;

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
	
	/** Constants for query.
	 * Use this constants to set the query parameters.
	 * @see queryByExample()
	 */
	static const uint query_WildCards  = 0x0001;
	static const uint query_IgnoreCase = 0x0002;
	static const uint query_RegExp     = 0x0004;
	static const uint query_ExactMatch = 0x0008;
};

/** Class to access the contacts database.
 * This is just a frontend for the real database handling which is
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
	 * @param handlesync If <b>true</b> the database stores the current state 
	 * automatically if it receives the signals <i>flush()</i> and <i>reload()</i>
	 * which are used before and after synchronisation. If the application wants 
	 * to react itself, it should be disabled by setting it to <b>false</b>
	 * @see OContactBackend 
	 */
	OContactDB (const QString appname, const QString filename = 0l,
                    OContactBackend* backend = 0l, bool handlesync = true);
	~OContactDB ();
	
	/** Returns all Contacts.
	 * @return List of all contacts.
	 */
	QValueList<Contact> allContacts() const;
	
	/** Finds a Contact by the uid.
	 * @param foundContact The Contact found if returnvalue is <i>true</i>
	 * @param uid The user ID of the contact
	 * @return <i>true</i> if found, else <i>false</i>
	 */
	bool findContact( Contact &foundContact, int uid );
	
	/** "Query by example" like search.
	 * To find a special contact, just fill all known information into
	 * a new contact-object and start query with this function.
	 * All information will be connected by an "AND".
	 * @param query The query form.
	 * @param settings The parameters how to perform the query (OContactBackend::query_*).
	 * @return <i>NULL</i> if nothing was found or the first contact.
	 * @see nextFound(), OContactBackend::query_RegExp, OContactBackend::query_WildCards, 
	 * OContactBackend::query_ExactMatch, OContactBackend::query_IgnoreCase
	 */
	const Contact *queryByExample ( const Contact &query, const uint settings ); 

	/** Requests a contact which was selected by queryByExample().
	 * Use this function to move through the list of selected contacts.
	 * @return Pointer to next contact or <i>null</i> if list empty or no 
	 * next element.
	 * @see qeryByExample
	 */
	const Contact *nextFound ();

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
		
	/** Reload database.
	 * You should execute this function if the external database
	 * was changed.
	 * This function will load the external database and afterwards
	 * rejoin the local changes. Therefore the local database will be set consistent.
	 */
	void reload();
	
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
	void signalChanged ( const OContactDB *which );

	
 private:
	/*         class OContactDBPrivate; */
	/*         OContactDBPrivate* d; */
        OContactBackend *m_backEnd;
        bool m_loading:1;
	bool m_changed;

 private slots:
	void copMessage( const QCString &msg, const QByteArray &data );

	
};
#endif
