/*
 * VCard Backend for the OPIE-Contact Database.
 *
 * Copyright (C) 2000 Trolltech AS.  All rights reserved.
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * =====================================================================
 * ToDo:
 *
 * =====================================================================
 * Version: $Id: ocontactaccessbackend_vcard.h,v 1.6 2003-04-13 18:07:10 zecke Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend_vcard.h,v $
 * Revision 1.6  2003-04-13 18:07:10  zecke
 * More API doc
 * QString -> const QString&
 * QString = 0l -> QString::null
 *
 * Revision 1.5  2003/03/21 10:33:09  eilers
 * Merged speed optimized xml backend for contacts to main.
 * Added QDateTime to querybyexample. For instance, it is now possible to get
 * all Birthdays/Anniversaries between two dates. This should be used
 * to show all birthdays in the datebook..
 * This change is sourcecode backward compatible but you have to upgrade
 * the binaries for today-addressbook.
 *
 * Revision 1.4  2002/12/07 13:26:22  eilers
 * Fixing bug in storing anniversary..
 *
 * Revision 1.3  2002/11/13 14:14:51  eilers
 * Added sorted for Contacts..
 *
 * Revision 1.2  2002/11/10 15:41:53  eilers
 * Bugfixes..
 *
 * Revision 1.1  2002/11/09 14:34:52  eilers
 * Added VCard Backend.
 *
 */
#ifndef __OCONTACTACCESSBACKEND_VCARD_H_
#define __OCONTACTACCESSBACKEND_VCARD_H_

#include <opie/ocontact.h>

#include "ocontactaccessbackend.h"

class VObject;

/**
 * This is the vCard 2.1 implementation of the Contact Storage
 * @see OContactAccessBackend_XML
 * @see OPimAccessBackend
 */
class OContactAccessBackend_VCard : public OContactAccessBackend {
 public:
	OContactAccessBackend_VCard ( const QString& appname, const QString& filename = QString::null );

	bool load ();
	bool reload();
	bool save();
	void clear ();

	bool add ( const OContact& newcontact );
	bool remove ( int uid );
	bool replace ( const OContact& contact );

	OContact find ( int uid ) const;
	QArray<int> allRecords() const;
	QArray<int> queryByExample ( const OContact &query, int settings, const QDateTime& d = QDateTime() );
	QArray<int> matchRegexp(  const QRegExp &r ) const;

	const uint querySettings();
	bool hasQuerySettings (uint querySettings) const;
	QArray<int> sorted( bool ascending, int sortOrder, int sortFilter, int cat );
	bool wasChangedExternally();

private:
	OContact parseVObject( VObject* obj );
	VObject* createVObject( const OContact& c );
	QString convDateToVCardDate( const QDate& c ) const;
	QDate convVCardDateToDate( const QString& datestr );
	VObject *safeAddPropValue( VObject *o, const char* prop, const QString& value );
	VObject *safeAddProp( VObject* o, const char* prop);

	bool m_dirty : 1;
	QString m_file;
	QMap<int, OContact> m_map;
};

#endif
