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
 * Version: $Id: ocontactaccessbackend_vcard.h,v 1.2 2002-11-10 15:41:53 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend_vcard.h,v $
 * Revision 1.2  2002-11-10 15:41:53  eilers
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

class OContactAccessBackend_VCard : public OContactAccessBackend {
 public:
	OContactAccessBackend_VCard ( QString appname, QString filename = 0l );

	bool load ();
	bool reload();
	bool save();
	void clear ();

	bool add ( const OContact& newcontact );
	bool remove ( int uid );
	bool replace ( const OContact& contact );
	
	OContact find ( int uid ) const;
	QArray<int> allRecords() const;
	QArray<int> queryByExample ( const OContact &query, int settings );
	QArray<int> matchRegexp(  const QRegExp &r ) const;

	const uint querySettings();
	bool hasQuerySettings (uint querySettings) const;
	bool wasChangedExternally();
	
private:
	OContact parseVObject( VObject* obj );
	VObject* createVObject( const OContact& c );
	QDate convVCardDateToDate( const QString& datestr );
	VObject *safeAddPropValue( VObject *o, const char* prop, const QString& value );
	VObject *safeAddProp( VObject* o, const char* prop);
	
	bool m_dirty : 1;
	QString m_file;
	QMap<int, OContact> m_map;
};

#endif
