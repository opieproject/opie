/****************************************************************************
** $Id: qgpluginmanager.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QGPluginManager class
**
** Copyright (C) 2000-2001 Trolltech AS.  All rights reserved.
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

#include "qgpluginmanager_p.h"
#ifndef QT_NO_COMPONENT
#include "qcomlibrary_p.h"
#include "qmap.h"
#include "qdir.h"

/*
  The following co-occurrence code is borrowed from Qt Linguist.

  How similar are two texts? The approach used here relies on
  co-occurrence matrices and is very efficient.

  Let's see with an example: how similar are "here" and "hither"?  The
  co-occurrence matrix M for "here" is M[h,e] = 1, M[e,r] = 1,
  M[r,e] = 1 and 0 elsewhere; the matrix N for "hither" is N[h,i] = 1,
  N[i,t] = 1, ..., N[h,e] = 1, N[e,r] = 1 and 0 elsewhere.  The union
  U of both matrices is the matrix U[i,j] = max { M[i,j], N[i,j] },
  and the intersection V is V[i,j] = min { M[i,j], N[i,j] }. The score
  for a pair of texts is

      score = (sum of V[i,j] over all i, j) / (sum of U[i,j] over all i, j),

  a formula suggested by Arnt Gulbrandsen. Here we have

      score = 2 / 6,

  or one third.

  The implementation differs from this in a few details.  Most
  importantly, repetitions are ignored; for input "xxx", M[x,x] equals
  1, not 2.
*/

/*
  Every character is assigned to one of 20 buckets so that the
  co-occurrence matrix requires only 20 * 20 = 400 bits, not
  256 * 256 = 65536 bits or even more if we want the whole Unicode.
  Which character falls in which bucket is arbitrary.

  The second half of the table is a replica of the first half, because of
  laziness.
*/
static const char indexOf[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//      !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
    0,  2,  6,  7,  10, 12, 15, 19, 2,  6,  7,  10, 12, 15, 19, 0,
//  0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    1,  3,  4,  5,  8,  9,  11, 13, 14, 16, 2,  6,  7,  10, 12, 15,
//  @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  6,  10, 11, 12, 13, 14,
//  P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
    15, 12, 16, 17, 18, 19, 2,  10, 15, 7,  19, 2,  6,  7,  10, 0,
//  `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  6,  10, 11, 12, 13, 14,
//  p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~
    15, 12, 16, 17, 18, 19, 2,  10, 15, 7,  19, 2,  6,  7,  10, 0,

    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  2,  6,  7,  10, 12, 15, 19, 2,  6,  7,  10, 12, 15, 19, 0,
    1,  3,  4,  5,  8,  9,  11, 13, 14, 16, 2,  6,  7,  10, 12, 15,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  6,  10, 11, 12, 13, 14,
    15, 12, 16, 17, 18, 19, 2,  10, 15, 7,  19, 2,  6,  7,  10, 0,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  6,  10, 11, 12, 13, 14,
    15, 12, 16, 17, 18, 19, 2,  10, 15, 7,  19, 2,  6,  7,  10, 0
};

/*
  The entry bitCount[i] (for i between 0 and 255) is the number of
  bits used to represent i in binary.
*/
static const char bitCount[256] = {
    0,  1,  1,  2,  1,  2,  2,  3,  1,  2,  2,  3,  2,  3,  3,  4,
    1,  2,  2,  3,  2,  3,  3,  4,  2,  3,  3,  4,  3,  4,  4,  5,
    1,  2,  2,  3,  2,  3,  3,  4,  2,  3,  3,  4,  3,  4,  4,  5,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    1,  2,  2,  3,  2,  3,  3,  4,  2,  3,  3,  4,  3,  4,  4,  5,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    3,  4,  4,  5,  4,  5,  5,  6,  4,  5,  5,  6,  5,  6,  6,  7,
    1,  2,  2,  3,  2,  3,  3,  4,  2,  3,  3,  4,  3,  4,  4,  5,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    3,  4,  4,  5,  4,  5,  5,  6,  4,  5,  5,  6,  5,  6,  6,  7,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    3,  4,  4,  5,  4,  5,  5,  6,  4,  5,  5,  6,  5,  6,  6,  7,
    3,  4,  4,  5,  4,  5,  5,  6,  4,  5,  5,  6,  5,  6,  6,  7,
    4,  5,  5,  6,  5,  6,  6,  7,  5,  6,  6,  7,  6,  7,  7,  8
};

class QCoMatrix
{
public:
    /*
      The matrix has 20 * 20 = 400 entries. This requires 50 bytes, or
      13 words. Some operations are performed on words for more
      efficiency.
    */
    union {
	Q_UINT8 b[52];
	Q_UINT32 w[13];
    };

    QCoMatrix() { memset( b, 0, 52 ); }
    QCoMatrix( const char *text ) {
	char c = '\0', d;
	memset( b, 0, 52 );
	while ( (d = *text) != '\0' ) {
	    setCoocc( c, d );
	    if ( (c = *++text) != '\0' ) {
		setCoocc( d, c );
		text++;
	    }
	}
    }

    void setCoocc( char c, char d ) {
	int k = indexOf[(uchar) c] + 20 * indexOf[(uchar) d];
	b[k >> 3] |= k & 0x7;
    }

    int worth() const {
	int result = 0;
	for ( int i = 0; i < 50; i++ )
	    result += bitCount[b[i]];
	return result;
    }

    static QCoMatrix reunion( const QCoMatrix& m, const QCoMatrix& n )
    {
	QCoMatrix p;
	for ( int i = 0; i < 13; i++ )
	    p.w[i] = m.w[i] | n.w[i];
	return p;
    }
    static QCoMatrix intersection( const QCoMatrix& m, const QCoMatrix& n )
    {
	QCoMatrix p;
	for ( int i = 0; i < 13; i++ )
	    p.w[i] = m.w[i] & n.w[i];
	return p;
    }
};

/*
  Returns an integer between 0 (dissimilar) and 15 (very similar)
  depending on  how similar the string is to \a target.

  This function is efficient, but its results might change in future
  versions of Qt as the algorithm evolves.

  \code
    QString s( "color" );
    a = similarity( s, "color" );  // a == 15
    a = similarity( s, "colour" ); // a == 8
    a = similarity( s, "flavor" ); // a == 4
    a = similarity( s, "dahlia" ); // a == 0
  \endcode
*/
static int similarity( const QString& s1, const QString& s2 )
{
    QCoMatrix m1( s1 );
    QCoMatrix m2( s2 );
    return ( 15 * (QCoMatrix::intersection(m1, m2).worth() + 1) ) /
	   ( QCoMatrix::reunion(m1, m2).worth() + 1 );
}

/*!
  \class QPluginManager qpluginmanager.h
  \reentrant
  \brief The QPluginManager class provides basic functions to access a certain kind of functionality in libraries.
  \ingroup componentmodel

  \internal

  A common usage of components is to extend the existing functionality in an application using plugins. The application
  defines interfaces that abstract a certain group of functionality, and a plugin provides a specialized implementation
  of one or more of those interfaces.

  The QPluginManager template has to be instantiated with an interface definition and the IID for this interface.

  \code
  QPluginManager<MyPluginInterface> *manager = new QPluginManager<MyPluginInterface>( IID_MyPluginInterface );
  \endcode

  It searches a specified directory for all shared libraries, queries for components that implement the specific interface and
  reads information about the features the plugin wants to add to the application. The component can provide the set of features
  provided by implementing either the QFeatureListInterface or the QComponentInformationInterface. The strings returned by the implementations
  of

  \code
  QStringList QFeatureListInterface::featureList() const
  \endcode

  or

  \code
  QString QComponentInformationInterface::name() const
  \endcode

  respectively, can then be used to access the component that provides the requested feature:

  \code
  MyPluginInterface *iface;
  manager->queryInterface( "feature", &iface );
  if ( iface )
      iface->execute( "feature" );
  \endcode

  The application can use a QPluginManager instance to create parts of the user interface based on the list of features
  found in plugins:

  \code
  QPluginManager<MyPluginInterface> *manager = new QPluginManager<MyPluginInterface>( IID_ImageFilterInterface );
  manager->addLibraryPath(...);

  QStringList features = manager->featureList();
  for ( QStringList::Iterator it = features.begin(); it != features.end(); ++it ) {
      MyPluginInterface *iface;
      manager->queryInterface( *it, &iface );

      // use QAction to provide toolbuttons and menuitems for each feature...
  }
  \endcode
*/

/*!
  \fn QPluginManager::QPluginManager( const QUuid& id, const QStringList& paths = QString::null, const QString &suffix = QString::null, bool cs = TRUE )

  Creates an QPluginManager for interfaces \a id that will load all shared library files in the \a paths + \a suffix.
  If \a cs is FALSE the manager will handle feature strings case insensitive.

  \warning
  Setting the cs flag to FALSE requires that components also convert to lower case when comparing with passed strings, so this has
  to be handled with care and documented very well.

  \sa QApplication::libraryPaths()
*/


/*!
  \fn QRESULT QPluginManager::queryInterface(const QString& feature, Type** iface) const

  Sets \a iface to point to the interface providing \a feature.

  \sa featureList(), library()
*/



#include <qptrlist.h>

QGPluginManager::QGPluginManager( const QUuid& id, const QStringList& paths, const QString &suffix, bool cs )
    : interfaceId( id ), plugDict( 17, cs ), casesens( cs ), autounload( TRUE )
{
    // Every QLibrary object is destroyed on destruction of the manager
    libDict.setAutoDelete( TRUE );
    for ( QStringList::ConstIterator it = paths.begin(); it != paths.end(); ++it ) {
	QString path = *it;
	addLibraryPath( path + suffix );
    }
}

QGPluginManager::~QGPluginManager()
{
    if ( !autounload ) {
	QDictIterator<QLibrary> it( libDict );
	while ( it.current() ) {
	    QLibrary *lib = it.current();
	    ++it;
	    lib->setAutoUnload( FALSE );
	}
    }
}

void QGPluginManager::addLibraryPath( const QString& path )
{
    if ( !enabled() || !QDir( path ).exists( ".", TRUE ) )
	return;

#if defined(Q_OS_WIN32)
    QString filter = "dll";
#elif defined(Q_OS_MACX)
    QString filter = "dylib";
#elif defined(Q_OS_UNIX)
    QString filter = "so";
#endif

    QStringList plugins = QDir(path).entryList( "*." + filter );
    for ( QStringList::Iterator p = plugins.begin(); p != plugins.end(); ++p ) {
	QString lib = QDir::cleanDirPath( path + "/" + *p );
	if ( libList.contains( lib ) )
	    continue;
	libList.append( lib );
    }
}

const QLibrary* QGPluginManager::library( const QString& feature ) const
{
    if ( !enabled() || feature.isEmpty() )
	return 0;

    // We already have a QLibrary object for this feature
    QLibrary *library = 0;
    if ( ( library = plugDict[feature] ) )
	return library;

    // Find the filename that matches the feature request best
    QMap<int, QStringList> map;
    QStringList::ConstIterator it = libList.begin();
    int best = 0;
    int worst = 15;
    while ( it != libList.end() ) {
	if ( (*it).isEmpty() || libDict[*it] ) {
	    ++it;
	    continue;
	}
	QString basename = QFileInfo(*it).baseName();
	int s = similarity( feature, basename );
	if ( s < worst )
	    worst = s;
	if ( s > best )
	    best = s;
	map[s].append( basename + QChar(0xfffd) + *it );
	++it;
    }

    if ( map.isEmpty() )
	return 0; // no libraries to add

    // Start with the best match to get the library object
    QGPluginManager *that = (QGPluginManager*)this;
    for ( int s = best; s >= worst; --s ) {
	QStringList group = map[s];
	group.sort(); // sort according to the base name
	QStringList::ConstIterator git = group.begin();
	while ( git != group.end() ) {
	    QString lib = (*git).mid( (*git).find( QChar(0xfffd) ) + 1 );
	    QString basename = (*git).left( (*git).find( QChar(0xfffd) ) );
	    ++git;

	    QStringList sameBasename;
	    while( git != group.end() &&
		   basename == (*git).left( (*git).find( QChar(0xfffd) ) )  ) {
		sameBasename << (*git).mid( (*git).find( QChar(0xfffd) ) + 1 );
		++git;
	    }

	    if ( sameBasename.isEmpty() ) {
		that->addLibrary( new QComLibrary( lib ) );
	    } else {
		QPtrList<QComLibrary> same;
		same.setAutoDelete( TRUE );
		for ( QStringList::ConstIterator bit = sameBasename.begin();
		      bit != sameBasename.end(); ++bit )
		    same.append( new QComLibrary( *bit ) );
		QComLibrary* bestMatch = 0;
		for ( QComLibrary* candidate = same.first(); candidate; candidate = same.next() )
		    if ( candidate->qtVersion() && candidate->qtVersion() <= QT_VERSION
			 && ( !bestMatch || candidate->qtVersion() > bestMatch->qtVersion() ) )
			bestMatch = candidate;
		if ( bestMatch ) {
		    same.find( bestMatch );
		    that->addLibrary( same.take() );
		}
	    }

	    if ( ( library = that->plugDict[feature] ) )
		return library;
	}
    }
    return 0;
}

QStringList QGPluginManager::featureList() const
{
    QStringList features;

    if ( !enabled() )
	return features;

    QGPluginManager *that = (QGPluginManager*)this;
    QStringList theLibs = libList;
    QStringList phase2Libs;
    QStringList phase2Deny;

    /* In order to get the feature list we need to add all interesting
      libraries. If there are libraries with the same base name, we
      prioritze the one that fits our Qt version number and ignore the
      others  */
    QStringList::Iterator it;
    for ( it = theLibs.begin(); it != theLibs.end(); ++it  ) {
	if ( (*it).isEmpty() || libDict[*it] )
	    continue;
	QComLibrary* library = new QComLibrary( *it );
	if ( library->qtVersion() == QT_VERSION ) {
	    that->addLibrary( library );
	    phase2Deny << QFileInfo( *it ).baseName();
	} else {
	    delete library;
	    phase2Libs << *it;
	}
    }
    for ( it = phase2Libs.begin(); it != phase2Libs.end(); ++it  )
	if ( !phase2Deny.contains( QFileInfo( *it ).baseName() ) )
	    that->addLibrary( new QComLibrary( *it ) );

    for ( QDictIterator<QLibrary> pit( plugDict ); pit.current(); ++pit )
	features << pit.currentKey();

    return features;
}

bool QGPluginManager::addLibrary( QLibrary* lib )
{
    if ( !enabled() || !lib )
	return FALSE;

    QComLibrary* plugin = (QComLibrary*)lib;
    bool useful = FALSE;

    QUnknownInterface* iFace = 0;
    plugin->queryInterface( interfaceId, &iFace );
    if ( iFace ) {
	QFeatureListInterface *fliFace = 0;
	QComponentInformationInterface *cpiFace = 0;
	iFace->queryInterface( IID_QFeatureList, (QUnknownInterface**)&fliFace );
	if ( !fliFace )
	    plugin->queryInterface( IID_QFeatureList, (QUnknownInterface**)&fliFace );
	if ( !fliFace ) {
	    iFace->queryInterface( IID_QComponentInformation, (QUnknownInterface**)&cpiFace );
	    if ( !cpiFace )
		plugin->queryInterface( IID_QComponentInformation, (QUnknownInterface**)&cpiFace );
	}
	QStringList fl;
	if ( fliFace )
	    // Map all found features to the library
	    fl = fliFace->featureList();
	else if ( cpiFace )
	    fl << cpiFace->name();

	for ( QStringList::Iterator f = fl.begin(); f != fl.end(); f++ ) {
	    QLibrary *old = plugDict[*f];
	    if ( !old ) {
		useful = TRUE;
		plugDict.replace( *f, plugin );
	    } else {
		// we have old *and* plugin, which one to pick?
		QComLibrary* first = (QComLibrary*)old;
		QComLibrary* second = (QComLibrary*)plugin;
		bool takeFirst = TRUE;
		if ( first->qtVersion() != QT_VERSION ) {
		    if ( second->qtVersion() == QT_VERSION )
			takeFirst = FALSE;
		    else if ( second->qtVersion() < QT_VERSION &&
			      first->qtVersion() > QT_VERSION )
			takeFirst = FALSE;
		}
		if ( !takeFirst ) {
		    useful = TRUE;
		    plugDict.replace( *f, plugin );
		    qWarning("%s: Discarding feature %s in %s!",
			     (const char*) QFile::encodeName( plugin->library()),
			     (*f).latin1(),
			     (const char*) QFile::encodeName( old->library() ) );
		} else {
		    qWarning("%s: Feature %s already defined in %s!",
			     (const char*) QFile::encodeName( old->library() ),
			     (*f).latin1(),
			     (const char*) QFile::encodeName( plugin->library() ) );
		}
	    }
	}
	if ( fliFace )
	    fliFace->release();
	if ( cpiFace )
	    cpiFace->release();
	iFace->release();
    }

    if ( useful ) {
	libDict.replace( plugin->library(), plugin );
	if ( !libList.contains( plugin->library() ) )
	    libList.append( plugin->library() );
	return TRUE;
    }
    delete plugin;
    return FALSE;
}


bool QGPluginManager::enabled() const
{
#ifdef QT_SHARED
    return TRUE;
#else
    return FALSE;
#endif
}

QRESULT QGPluginManager::queryUnknownInterface(const QString& feature, QUnknownInterface** iface) const
{
    QComLibrary* plugin = 0;
    plugin = (QComLibrary*)library( feature );
    return plugin ? plugin->queryInterface( interfaceId, (QUnknownInterface**)iface ) : QE_NOINTERFACE;
}

#endif //QT_NO_COMPONENT
