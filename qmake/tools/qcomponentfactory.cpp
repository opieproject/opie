/****************************************************************************
** $Id: qcomponentfactory.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of the QComponentFactory class
**
** Created : 990101
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qcomponentfactory_p.h"

#ifndef QT_NO_COMPONENT
#include "qsettings.h"
#include <private/qcomlibrary_p.h>
#include "qdir.h"
#include "qapplication.h"

/*!
  \class QComponentFactory qcomponentfactory.h
  \brief The QComponentFactory class provides static functions to create and register components.

    \internal

  The static convenience functions can be used both by applications to instantiate components,
  and by component servers to register components.

  The createInstance() function provides a pointer to an interface implemented in a specific
  component if the component requested has been installed properly and implements the interface.

  Use registerServer() to load a component server and register its components, and unregisterServer()
  to unregister the components. The component exported by the component server has to implement the
  QComponentRegistrationInterface.

  The static functions registerComponent() and unregisterComponent() register and unregister a single
  component in the system component registry, and should be used when implementing the
  \link QComponentRegistrationInterface::registerComponents() registerCompontents() \endlink and
  \link QComponentRegistrationInterface::unregisterComponents() unregisterCompontents() \endlink functions
  in the QComponentRegistrationInterface.

  A component is registered using a UUID, but can additionally be registered with a name, version and
  description. A component registered with a name and a version can be instantiated by client applications
  using the name and specific version number, or the highest available version number for that component by
  just using the name. A component that is registered calling

  \code
  QComponentFactory::registerComponent( QUuid(...), filename, "MyProgram.Component", 1 );
  \endcode

  can be instantiated calling either:

  \code
  QComponentFactory::createInstance( QUuid(...), IID_XYZ, (QUnknownInterface**)&iface );
  \endcode
  or
  \code
  QComponentFactory::createInstance( "MyProgram.Component", IID_XYZ, (QUnknownInterface**)&iface );
  \endcode
  or
  \code
  QComponentFactory::createInstance( "MyProgram.Component.1", IID_XYZ, (QUnknownInterface**)&iface );
  \endcode

  The first and the last way will always instantiate exactly the component registered above, while
  the second call might also return a later version of the same component. This allows smoother upgrading
  of components, and is easier to use in application source code, but should only be used when new versions
  of the component are guaranteed to work with the application.

  The component name can be anything, but should be unique on the system the component is being
  installed on. A common naming convention for components is \e application.component.

  \sa QComponentRegistrationInterface QComponentFactoryInterface
*/


static QPtrList<QComLibrary> *libraries = 0;

static void cleanup()
{
    delete libraries;
    libraries = 0;
}

static QPtrList<QComLibrary> *liblist()
{
    if ( !libraries ) {
	libraries = new QPtrList<QComLibrary>();
	libraries->setAutoDelete( TRUE );
	qAddPostRoutine( cleanup );
    }
    return libraries;
}

/*!
  Searches for the component identifier \a cid in the system component registry,
  loads the corresponding component server and queries for the interface \a iid.
  \a iface is set to the resulting interface pointer. \a cid can either be the
  UUID or the name of the component.

  The parameter \a outer is a pointer to the outer interface used
  for containment and aggregation and is propagated to the \link
  QComponentFactoryInterface::createInstance() createInstance() \endlink
  implementation of the QComponentFactoryInterface in the component server if
  provided.

  The function returns QS_OK if the interface was successfully instantiated, QE_NOINTERFACE if
  the component does not provide an interface \a iid, or QE_NOCOMPONENT if there was
  an error loading the component.

  Example:
  \code
  QInterfacePtr<MyInterface> iface;
  if ( QComponentFactory::createInstance( IID_MyInterface, CID_MyComponent, (QUnknownInterface**)&iface ) == QS_OK )
      iface->doSomething();
      ...
  }
  \endcode
*/
QRESULT QComponentFactory::createInstance( const QString &cid, const QUuid &iid, QUnknownInterface** iface, QUnknownInterface *outer )
{
    QSettings settings;
    settings.insertSearchPath( QSettings::Windows, "/Classes" );
    bool ok = FALSE;
    QString cidStr = cid;
    QRESULT res = QE_NOCOMPONENT;

    QUuid uuid( cidStr ); // try to parse, and resolve CLSID if necessary
    if ( uuid.isNull() ) {
	uuid = settings.readEntry( "/" + cid + "/CLSID/Default", QString::null, &ok );
	cidStr = uuid.toString().upper();
    }

    if ( cidStr.isEmpty() )
	return res;

    QString file = settings.readEntry( "/CLSID/" + cidStr + "/InprocServer32/Default", QString::null, &ok );
    if ( !ok )
	return res;

    QComLibrary *library = new QComLibrary( file );
    library->setAutoUnload( FALSE );

    QComponentFactoryInterface *cfIface =0;
    library->queryInterface( IID_QComponentFactory, (QUnknownInterface**)&cfIface );

    if ( cfIface ) {
	res = cfIface->createInstance( uuid, iid, iface, outer );
	cfIface->release();
    } else {
	res = library->queryInterface( iid, iface );
    }
    QLibraryInterface *libiface = 0;
    if ( library->queryInterface( IID_QLibrary, (QUnknownInterface**)&libiface ) != QS_OK || !qApp ) {
	delete library; // only deletes the object, thanks to QLibrary::Manual
    } else {
	libiface->release();
	library->setAutoUnload( TRUE );
	liblist()->prepend( library );
    }
    return res;
}

/*!
  Loads the shared library \a filename and queries for a
  QComponentRegistrationInterface. If the library implements this interface,
  the \link QComponentRegistrationInterface::registerComponents()
  registerComponents() \endlink function is called.

  Returns TRUE if the interface is found and successfully called,
  otherwise returns FALSE.
*/
QRESULT QComponentFactory::registerServer( const QString &filename )
{
    QComLibrary lib( filename );
    lib.load();
    QComponentRegistrationInterface *iface = 0;
    QRESULT res = lib.queryInterface( IID_QComponentRegistration, (QUnknownInterface**)&iface );
    if ( res != QS_OK )
	return res;
    QDir dir( filename );
    bool ok = iface->registerComponents( dir.absPath() );
    iface->release();
    return ok ? QS_OK : QS_FALSE;
}

/*!
  Loads the shared library \a filename and queries for a
  QComponentRegistrationInterface. If the library implements this interface,
  the \link QComponentRegistrationInterface::unregisterComponents()
  unregisterComponents() \endlink function is called.

  Returns TRUE if the interface is found and successfully unregistered,
  otherwise returns FALSE.
*/
QRESULT QComponentFactory::unregisterServer( const QString &filename )
{
    QComLibrary lib( filename );
    lib.load();
    QComponentRegistrationInterface *iface = 0;
    QRESULT res = lib.queryInterface( IID_QComponentRegistration, (QUnknownInterface**)&iface );
    if ( res != QS_OK )
	return res;
    bool ok = iface->unregisterComponents();
    iface->release();
    return ok ? QS_OK : QS_FALSE;
}

/*!
  Registers the component with id \a cid in the system component registry and
  returns TRUE if the component was registerd successfully, otherwise returns
  FALSE. The component is provided by the component server at \a filepath and
  registered with an optional \a name, \a version and \a description.

  This function does nothing and returns FALSE if a component with an identical
  \a cid does already exist on the system.

  A component that has been registered with a \a name can be created using both the
  \a cid and the \a name value using createInstance().

  Call this function for each component in an implementation of
  \link QComponentRegistrationInterface::registerComponents() registerComponents() \endlink.

  \sa unregisterComponent(), registerServer(), createInstance()
*/
bool QComponentFactory::registerComponent( const QUuid &cid, const QString &filepath, const QString &name, int version, const QString &description )
{
    bool ok = FALSE;
    QSettings settings;
    settings.insertSearchPath( QSettings::Windows, "/Classes" );

    QString cidStr = cid.toString().upper();
    settings.readEntry( "/CLSID/" + cidStr + "/InprocServer32/Default", QString::null, &ok );
    if ( ok ) // don't overwrite existing component
	return FALSE;

    ok = settings.writeEntry( "/CLSID/" + cidStr + "/InprocServer32/Default", filepath );
    if ( ok && !!description )
	settings.writeEntry( "/CLSID/" + cidStr + "/Default", description );

    // register the human readable part
    if ( ok && !!name ) {
	QString vName = version ? name + "." + QString::number( version ) : name;
	settings.writeEntry( "/CLSID/" + cidStr + "/ProgID/Default", vName );
	ok = settings.writeEntry( "/" + vName + "/CLSID/Default", cidStr );
	if ( ok && !!description )
	    settings.writeEntry( "/" + vName + "/Default", description );

	if ( ok && version ) {
	    settings.writeEntry( "/CLSID/" + cidStr + "/VersionIndependentProgID/Default", name );
	    QString curVer = settings.readEntry( "/" + name + "/CurVer/Default" );
	    if ( !curVer || curVer < vName ) { // no previous, or a lesser version installed
		settings.writeEntry( "/" + name + "/CurVer/Default", vName );
		ok = settings.writeEntry( "/" + name + "/CLSID/Default", cidStr );
		if ( ok && !!description )
		    settings.writeEntry( "/" + name + "/Default", description );
	    }
	}
    }

    return ok;
}

/*!
  Unregisters the component with id \a cid from the system component registry and returns
  TRUE if the component was unregistered successfully, otherwise returns FALSE.

  Call this function for each component in an implementation of
  \link QComponentRegistrationInterface::unregisterComponents() unregisterComponents() \endlink.

  \sa registerComponent(), unregisterServer()
*/
bool QComponentFactory::unregisterComponent( const QUuid &cid )
{
    QSettings settings;
    bool ok = FALSE;
    settings.insertSearchPath( QSettings::Windows, "/Classes" );

    QString cidStr = cid.toString().upper();
    if ( cidStr.isEmpty() )
	return FALSE;

    // unregister the human readable part
    QString vName = settings.readEntry( "/CLSID/" + cidStr + "/ProgID/Default", QString::null, &ok );
    if ( ok ) {
	QString name = settings.readEntry( "/CLSID/" + cidStr + "/VersionIndependentProgID/Default", QString::null );
	if ( !!name && settings.readEntry( "/" + name + "/CurVer/Default" ) == vName ) {
	    // unregistering the current version -> change CurVer to previous version
	    QString version = vName.right( vName.length() - name.length() - 1 );
	    QString newVerName;
	    QString newCidStr;
	    if ( version.find( '.' ) == -1 ) {
		int ver = version.toInt();
		// see if a lesser version is installed, and make that the CurVer
		while ( ver-- ) {
		    newVerName = name + "." + QString::number( ver );
		    newCidStr = settings.readEntry( "/" + newVerName + "/CLSID/Default" );
		    if ( !!newCidStr )
			break;
		}
	    } else {
		// oh well...
	    }
	    if ( !!newCidStr ) {
		settings.writeEntry( "/" + name + "/CurVer/Default", newVerName );
		settings.writeEntry( "/" + name + "/CLSID/Default", newCidStr );
	    } else {
		settings.removeEntry( "/" + name + "/CurVer/Default" );
		settings.removeEntry( "/" + name + "/CLSID/Default" );
		settings.removeEntry( "/" + name + "/Default" );
	    }
	}

	settings.removeEntry( "/" + vName + "/CLSID/Default" );
	settings.removeEntry( "/" + vName + "/Default" );
    }

    settings.removeEntry( "/CLSID/" + cidStr + "/VersionIndependentProgID/Default" );
    settings.removeEntry( "/CLSID/" + cidStr + "/ProgID/Default" );
    settings.removeEntry( "/CLSID/" + cidStr + "/InprocServer32/Default" );
    ok = settings.removeEntry( "/CLSID/" + cidStr + "/Default" );

    return ok;
}

#endif // QT_NO_COMPONENT
