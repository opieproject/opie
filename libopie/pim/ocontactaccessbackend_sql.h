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
 * Version: $Id: ocontactaccessbackend_sql.h,v 1.3 2004-03-14 13:50:35 alwin Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend_sql.h,v $
 * Revision 1.3  2004-03-14 13:50:35  alwin
 * namespace correction
 *
 * Revision 1.2  2003/12/08 15:18:11  eilers
 * Committing unfinished sql implementation before merging to libopie2 starts..
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

namespace Opie { namespace DB {
class OSQLDriver;
class OSQLResult;
class OSQLResultItem;

}}

/* the default xml implementation */
/**
 * This class is the SQL implementation of a Contact backend
 * it does implement everything available for OContact.
 * @see OPimAccessBackend for more information of available methods
 */
class OContactAccessBackend_SQL : public OContactAccessBackend {
 public:
	OContactAccessBackend_SQL ( const QString& appname, const QString& filename = QString::null );

	~OContactAccessBackend_SQL ();

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
	QArray<int> extractUids( Opie::DB::OSQLResult& res ) const;
	QMap<int, QString>  requestNonCustom( int uid ) const;
	QMap<QString, QString>  requestCustom( int uid ) const;
	void update();

 protected:
	bool m_changed;
	QString m_fileName;
	QArray<int> m_uids;

	Opie::DB::OSQLDriver* m_driver;
};

#endif
