/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <eilers.stefan@epost.de>
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

#ifndef __OPIE_OPIMACCESSFACTORY_H_
#define __OPIE_OPIMACCESSFACTORY_H_

#include <opie2/opimglobal.h>
#include <opie2/obackendfactory.h>
#include <opie2/ocontactaccess.h>
#include <opie2/otodoaccess.h>
#include <opie2/odatebookaccess.h>

using namespace Opie::Pim;

namespace Opie {


/**
 * This class is our factory for creating PIM access objects. You should use these objects to load, store and search
 * information in our PIM databases. Currently we support objects for accessing the datebook, the contact- and the
 * todolist. 
 * @see OPimGlobal for more information of used values.
 *
 * This class - as the whole PIM Api - is making use of templates
 *
 * Example for getting an access object for the XML database of todolist:
 * <pre>
 *   OPimTodoAccess* access = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::XML, "example" );
 * </pre>
 *
 * @author Stefan Eilers
 * @version 0.1
 */
 */

template<class T>
class OPimAccessFactory
{
 public: 

	// Maybe we should introduce a global class for storing such global enums 
	// (something like opimglobal.h) ? (eilers)

	OPimAccessFactory() {};

	/**
	 * Returns the selected PIM access-object.
	 * @param type Type of the selected database (addressbook, todolist or datebook)
	 * @param dbStyle Which database style should be used (xml, sql, vcard)
	 * @param appName "Name" of your application. This should be any constant string which is used
	 *                by some backends for creating special files (i.e.journal files). Please keep the
	 *                string unique for your application !
	 * @see OPimGlobal
	 */
	static T* create( OPimGlobal::PimType type, OPimGlobal::DatabaseStyle dbStyle, const QString& appName ){

		switch ( type ){
		case OPimGlobal::TODOLIST:
			return dynamic_cast<T*>( new OPimTodoAccess( OBackendFactory<OPimTodoAccessBackend>::create( type, dbStyle, appName ) ) );
		case OPimGlobal::CONTACTLIST:
			return dynamic_cast<T*>( new OPimContactAccess( QString::null, QString::null, OBackendFactory<OPimContactAccessBackend>::create( type, dbStyle, appName ) ) );
		case OPimGlobal::DATEBOOK:
			return dynamic_cast<T*>( new ODateBookAccess( OBackendFactory<ODateBookAccessBackend>::create( type, dbStyle, appName ) ) );
		default:
			return 0l;
			
		}
	}


	/**
	 * Returns the selected PIM access-object, using the default database style
	 * Which style is selected is defined in the configfile "pimaccess.conf" in 
	 * the directory "Settings"
	 * @param type Type of the selected database (addressbook, todolist or datebook)
	 * @param appName "Name" of your application. This should be any constant string which is used
	 *                by some backends for creating special files (i.e.journal files). Please keep the
	 *                string unique for your application !
	 * @see OPimGlobal
	 * 
	 */
	static T* default( OPimGlobal::PimType type, const QString& appName ){

		return create( type, OPimGlobal::DEFAULT, appName )

	}
};

}


#endif
