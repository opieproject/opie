/*
 * SQL Backend for the OPIE-Contact Database.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * =====================================================================
 *
 *
 * =====================================================================
 * Version: $Id: ocontactaccessbackend_sql.h,v 1.1.2.1 2003-10-20 15:52:23 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend_sql.h,v $
 * Revision 1.1.2.1  2003-10-20 15:52:23  eilers
 * This sqlite-stuff drives me crazy !! This is a much better database
 * structure, but much too slow.. I think I use the stupid, but fast structure..
 *
 * Revision 1.1  2003/09/22 14:31:16  eilers
 * Added first experimental incarnation of sql-backend for addressbook.
 * Some modifications to be able to compile the todo sql-backend.
 * A lot of changes fill follow...
 *
 *
 */

#ifndef _OContactAccessBackend_SQL_
#define _OContactAccessBackend_SQL_

#include "ocontactaccessbackend.h"
#include "ocontactaccess.h"

#include <qlist.h>
#include <qdict.h>

class OSQLDriver;
class OSQLResult;
class OSQLResultItem;

/* the default xml implementation */
/**
 * This class is the SQL implementation of a Contact backend
 * it does implement everything available for OContact.
 * @see OPimAccessBackend for more information of available methods
 */
class OContactAccessBackend_SQL : public OContactAccessBackend {
 public:
	enum contactTypes{
		Home = 0,
		Business
	};

	OContactAccessBackend_SQL ( const QString& appname, const QString& filename = QString::null );

	bool save();

	bool load ();

	void clear ();

	bool wasChangedExternally();

	QArray<int> allRecords() const;

	OContact find ( int uid ) const;
	// FIXME: Add lookahead-cache support !
	//OContact find(int uid, const QArray<int>&, uint cur, Frontend::CacheDirection )const;

	QArray<int> queryByExample ( const OContact &query, int settings, 
				     const QDateTime& d );

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
	QMap<int, QString> idToContactFieldName;
	QMap<QString,int> contactFieldNameToId;

	bool createTable();

	// Helper functions for creating SQL strings for inserting data into table
	QString createPersonalString( const OContact& contact );
	QString createCustomString( const OContact& contact );
	QString createContactString( int id, int type, const OContact& contact );
	QString createHasContactString( int uid, int id, contactTypes type );

	// Creates string to create Table schema
	QString createCreateTableString( const QMap<int, QString>& idToContactFieldName );

	// Reads the query and extracts all uid's (used by update())
	QArray<int> extractUids( OSQLResult& res ) const;

	// Request personal information from table and return as map
	QMap<int, QString> requestPersonal( int uid ) const;

	// Request contact information from table for given type and uid
	// and add it to the given map
	void requestContact( contactTypes type, int uid, QMap<int,QString>* map ) const; 

	// Request custom information from table and return as map
	QMap<QString, QString>  requestCustom( int uid ) const;
	void update();
	void initFields();

 protected:
	bool m_changed;
	QString m_fileName;
	QArray<int> m_uids;

	OSQLDriver* m_driver;
};

#endif
