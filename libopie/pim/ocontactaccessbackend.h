/**
 * The class responsible for managing a backend.
 * The implementation of this abstract class contains
 * the complete database handling.
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
 * Version: $Id: ocontactaccessbackend.h,v 1.3 2002-11-01 15:10:42 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend.h,v $
 * Revision 1.3  2002-11-01 15:10:42  eilers
 * Added regExp-search in database for all fields in a contact.
 *
 * Revision 1.2  2002/10/07 17:34:24  eilers
 * added OBackendFactory for advanced backend access
 *
 * Revision 1.1  2002/09/27 17:11:44  eilers
 * Added API for accessing the Contact-Database ! It is compiling, but
 * please do not expect that anything is working !
 * I will debug that stuff in the next time ..
 * Please read README_COMPILE for compiling !
 *
 * =====================================================================
 *
 */

#ifndef _OCONTACTACCESSBACKEND_H_
#define _OCONTACTACCESSBACKEND_H_

#include "ocontact.h"
#include "opimaccessbackend.h"

#include "qregexp.h"

class OContactAccessBackend: public OPimAccessBackend<OContact> {
 public:
	OContactAccessBackend() {}
	virtual ~OContactAccessBackend() {}


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
	virtual bool wasChangedExternally() = 0;

	virtual QArray<int> matchRegexp(  const QRegExp &r ) const = 0;

	/** Return all possible settings.
	 *  @return All settings provided by the current backend
	 * (i.e.: query_WildCards & query_IgnoreCase)
	 */
	virtual const uint querySettings() = 0;

	/** Check whether settings are correct.
	 * @return <i>true</i> if the given settings are correct and possible.
	 */
	virtual bool hasQuerySettings (uint querySettings) const = 0;


};
#endif
