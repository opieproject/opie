/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
/*
 * VCard Backend for the OPIE-Contact Database.
 */
#ifndef __OCONTACTACCESSBACKEND_VCARD_H_
#define __OCONTACTACCESSBACKEND_VCARD_H_

#include <opie2/opimcontact.h>

#include <opie2/ocontactaccessbackend.h>

class VObject;

namespace Opie {
/**
 * This is the vCard 2.1 implementation of the Contact Storage
 * @see OPimContactAccessBackend_XML
 * @see OPimAccessBackend
 */
class OPimContactAccessBackend_VCard : public OPimContactAccessBackend {
 public:
	OPimContactAccessBackend_VCard ( const QString& appname, const QString& filename = QString::null );

	bool load ();
	bool reload();
	bool save();
	void clear ();

	bool add ( const OPimContact& newcontact );
	bool remove ( int uid );
	bool replace ( const OPimContact& contact );

	OPimContact find ( int uid ) const;
	QArray<int> allRecords() const;
	QArray<int> queryByExample ( const OPimContact &query, int settings, const QDateTime& d = QDateTime() );
	QArray<int> matchRegexp(  const QRegExp &r ) const;

	const uint querySettings();
	bool hasQuerySettings (uint querySettings) const;
	QArray<int> sorted( bool ascending, int sortOrder, int sortFilter, int cat );
	bool wasChangedExternally();

private:
	OPimContact parseVObject( VObject* obj );
	VObject* createVObject( const OPimContact& c );
	QString convDateToVCardDate( const QDate& c ) const;
	QDate convVCardDateToDate( const QString& datestr );
	VObject *safeAddPropValue( VObject *o, const char* prop, const QString& value );
	VObject *safeAddProp( VObject* o, const char* prop);

	bool m_dirty : 1;
	QString m_file;
	QMap<int, OPimContact> m_map;
};

}

#endif
