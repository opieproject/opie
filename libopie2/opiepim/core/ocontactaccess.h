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
 * Version: $Id: ocontactaccess.h,v 1.1 2002-09-27 17:11:44 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccess.h,v $
 * Revision 1.1  2002-09-27 17:11:44  eilers
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
 */
class OContactAccess: public QObject, public OPimAccessTemplate<OContact>
{
    Q_OBJECT

	    
	    /*    class Iterator{
	    friend OContactAccess;
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
	    OContactAccess *m_db;
	    
    };

	    */

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
	OContactAccess (const QString appname, const QString filename = 0l,
                    OContactAccessBackend* backend = 0l, bool handlesync = true);
	~OContactAccess ();

	/** Constants for query.
	 * Use this constants to set the query parameters.
	 * Note: <i>query_IgnoreCase</i> just make sense with one of the other attributes !
	 * @see queryByExample()
	 * - why not enum - zecke? 
	 *   -> Had some implementation problems .. Will use enum soon ! .. (se)
	 */
        static const int query_WildCards  = 0x0001;
	static const int query_IgnoreCase = 0x0002;
	static const int query_RegExp     = 0x0004;
	static const int query_ExactMatch = 0x0008;

	/** Return all possible settings.
	 *  @return All settings provided by the current backend
	 * (i.e.: query_WildCards & query_IgnoreCase)
	 */
	const uint querySettings();

	/** Check whether settings are correct.
	 * @return <i>true</i> if the given settings are correct and possible.
	 */
	bool hasQuerySettings ( int querySettings ) const;

	/** Add Contact to database.
	 * @param newcontact The contact to add.
	 * @return <i>true</i> if added successfully.
	 */
	bool add (const OContact& newcontact);

	/** Replace contact.
	 * Replaces given contact with contact with the user id <i>uid</i>.
	 * @param uid The user ID
	 * @param contact The new contact
	 * @return <i>true</i> if successful.
	 */
	bool replace ( const OContact& contact );

	/** Remove contact.
	 * Removes contact with the user id <i>uid</i>.
	 * @param The contact to remove
	 * @return <i>true</i> if successful.
	 */
	bool remove ( const OContact& t );

	/** Remove contact.
	 * Removes contact with the user id <i>uid</i>.
	 * @param The user id of the contact to remove
	 * @return <i>true</i> if successful.
	 */
	bool remove ( int uid );

	/** Load Database *
	 */
	bool load();
	
	/**
	 * if the resource was changed externally.
	 * You should use the signal instead of polling possible changes !
	 */
	bool wasChangedExternally()const;

	/** Reload database.
	 * You should execute this function if the external database
	 * was changed.
	 * This function will load the external database and afterwards
	 * rejoin the local changes. Therefore the local database will be set consistent.
	 */
	bool reload();

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
	bool m_changed;

 private slots:
	void copMessage( const QCString &msg, const QByteArray &data );


};
#endif
