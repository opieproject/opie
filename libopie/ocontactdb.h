/*
 * Class to manage the Contacts. It was derived from the
 * abtable-class of the addressbook.
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
 * Version: $Id: ocontactdb.h,v 1.1.2.20 2002-09-13 11:16:21 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb.h,v $
 * Revision 1.1.2.20  2002-09-13 11:16:21  eilers
 * added prefix increment/decrement
 *
 * Revision 1.1.2.19  2002/09/12 17:01:52  eilers
 * First attempt of iterator in ocontactdb
 *
 * Revision 1.1.2.18  2002/08/31 20:22:59  zecke
 * libopie renamed to libopie-two for now
 * toMap added to ToDoEvent
 * renamed date -> dueDate
 * hasDueDate()
 * setDueDate.....
 * (Re)Implement the XML Todo
 * Iterator fixes
 *
 * Revision 1.1.2.17  2002/08/27 20:11:30  zecke
 * So nice to be able to commit again..
 *
 * Revision 1.1.2.16  2002/08/10 15:00:30  eilers
 * Improved search interface. Thanks to Zecke
 *
 * Revision 1.1.2.15  2002/08/05 09:27:49  eilers
 * i think i need hollidays..
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

#include <qpe/qcopenvelope_qws.h>
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
	 * <b>Important: </b>This function must not change foundContact
	 * if nothing was found !
	 * @param foundContact The Contact found if returnvalue is <i>true</i>
	 * @param uid The user ID of the contact
	 * @return <i>true</i> if found, else <i>false</i>
	 */
	virtual bool findContact(Contact &foundContact, int uid ) = 0;

	/** Returns all Contacts.
	 * @return UID-List of all contacts.
	 */
	virtual QValueList<int> allContacts() const = 0;

	/** "Query by example" like search.
	 * To find a special contact, just fill all known information into
	 * a new contact-object and start query with this function.
	 * All information will be connected by an "AND".
	 * @param first_contact The first result of this query, if this function returnes <i>true</i>.
	 * @param query The query form.
	 * @param querysettings The parameters how to perform the query (OContactBackend::query_*).
	 * @return <i>true</i> if something was found.
	 * @see nextFound(), OContactBackend::query_RegExp, OContactBackend::query_WildCards,
	 * OContactBackend::query_ExactMatch, OContactBackend::query_IgnoreCase
	 */
	virtual bool queryByExample ( const Contact &query, const uint querysettings ) = 0;

	/** Returns the list (all UID's) of all found contacts, found by a query.
	 * @return The list of all contacts (all UID's) found by the pervious query. The list is empty
	 *         if none found.
	 * @see queryByExample()
	 */
	virtual const QValueList<int> allFound () = 0;

	/** Return all possible settings.
	 *  @return All settings provided by the current backend
	 * (i.e.: query_WildCards & query_IgnoreCase)
	 */
	virtual const uint querySettings() = 0;

	/** Check whether settings are correct.
	 * @return <i>true</i> if the given settings are correct and possible.
	 */
	virtual bool hasQuerySettings (uint querySettings) const = 0;

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

};

/** Class to access the contacts database.
 * This is just a frontend for the real database handling which is
 * done by the backend.
 */
class OContactDB: public QObject
{
    Q_OBJECT

	    
    class Iterator{
	    friend OContactDB;
    public:
	    Iterator();
	    Iterator ( const Iterator& copy );

	    bool operator== ( const Iterator& it );
	    bool operator!= ( const Iterator& it );
	    Iterator& operator= ( const  Iterator& it );
	    Iterator& operator++ ();      // prefix
	    Iterator operator++ ( int );  // postfix
	    Iterator& operator-- ();      // prefix
	    Iterator operator-- ( int );  // postfix
	    Contact operator*() const;
	    Contact operator->() const;

	    Iterator begin();
	    Iterator end();

	    uint count() const;

    private:
	    QValueList<int> m_uids;
	    int m_cur_position;
	    bool m_end_reached;
	    OContactDB *m_db;
	    
    };

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

	/** Constants for query.
	 * Use this constants to set the query parameters.
	 * Note: <i>query_IgnoreCase</i> just make sense with one of the other attributes !
	 * @see queryByExample()
	 * - why not enum - zecke? 
	 *   -> Had some implementation problems .. Will use enum soon ! .. (se)
	 */
	typedef uint Query;
        static const Query query_WildCards  = 0x0001;
	static const Query query_IgnoreCase = 0x0002;
	static const Query query_RegExp     = 0x0004;
	static const Query query_ExactMatch = 0x0008;

	/** Returns all Contacts.
	 * @return Iterator to access the contacts. 
	 */
	Iterator allContacts();

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
	 * @param querysettings The parameters how to perform the query (OContactBackend::query_*).
	 * @return Iterator to access the found Contacts.
	 * @see nextFound(), OContactBackend::query_RegExp, OContactBackend::query_WildCards,
	 * OContactBackend::query_ExactMatch, OContactBackend::query_IgnoreCase
	 */
	Iterator queryByExample ( const Contact &query, Query querysettings );

	/** Return all possible settings.
	 *  @return All settings provided by the current backend
	 * (i.e.: query_WildCards & query_IgnoreCase)
	 */
	const uint querySettings();

	/** Check whether settings are correct.
	 * @return <i>true</i> if the given settings are correct and possible.
	 */
	bool hasQuerySettings ( Query querySettings ) const;

	/** Add Contact to database.
	 * @param newcontact The contact to add.
	 * @return <i>true</i> if added successfully.
	 */
	bool addContact (const Contact& newcontact);

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
	class OContactDBPrivate;
	OContactDBPrivate* d;
        OContactBackend *m_backEnd;
        bool m_loading:1;
	bool m_changed;

 private slots:
	void copMessage( const QCString &msg, const QByteArray &data );


};
#endif
