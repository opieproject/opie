/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** $Id: config.cpp,v 1.1 2002-04-15 22:40:28 leseb Exp $
**
**********************************************************************/

#include "config.h"

#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
#include <qtextcodec.h>
#endif
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/*!
  \internal
*/
QString Config::configFilename(const QString& name, Domain d)
{
    switch (d) {
	case File:
	    return name;
	case User: {
	    QDir dir = (QString(getenv("HOME")) + "/Settings");
	    if ( !dir.exists() )
		mkdir(dir.path().local8Bit(),0700);
	    return dir.path() + "/" + name + ".conf";
	}
    }
    return name;
}

/*!
  \class Config config.h
  \brief The Config class provides for saving application cofniguration state.

  You should keep a Config in existence only while you do not want others
  to be able to change the state. There is no locking currently, but there
  may be in the future.
*/

/*!
  \enum Config::ConfigGroup
  \internal
*/

/*!
  \enum Config::Domain

  \value File
  \value User

  See Config for details.
*/

/*!
  Constructs a config that will load or create a configuration with the
  given \a name in the given \a domain.

  You must call setGroup() before doing much else with the Config.

  In the default Domain, \e User,
  the configuration is user-specific. \a name should not contain "/" in
  this case, and in general should be the name of the C++ class that is
  primarily responsible for maintaining the configuration.

  In the File Domain, \a name is an absolute filename.
*/
Config::Config( const QString &name, Domain domain )
    : filename( configFilename(name,domain) )
{
    git = groups.end();
    read();

    lang = getenv("LANG");
    int i  = lang.find(".");
    if ( i > 0 )
	lang = lang.left( i );
    i = lang.find( "_" );
    if ( i > 0 )
	glang = lang.left(i);
}

/*!
  Writes any changes to disk and destroys the in-memory object.
*/
Config::~Config()
{
    if ( changed )
	write();
}

/*!
  Returns whether the current group has an entry called \a key.
*/
bool Config::hasKey( const QString &key ) const
{
    if ( groups.end() == git )
	return FALSE;
    ConfigGroup::ConstIterator it = ( *git ).find( key );
    return it != ( *git ).end();
}

/*!
  Sets the current group for subsequent reading and writing of
  entries to \a gname. Grouping allows the application to partition the namespace.

  This function must be called prior to any reading or writing
  of entries.

  The \a gname must not be empty.
*/
void Config::setGroup( const QString &gname )
{
    QMap< QString, ConfigGroup>::Iterator it = groups.find( gname );
    if ( it == groups.end() ) {
	ConfigGroup *grp = new ConfigGroup;
	git = groups.insert( gname, *grp );
	changed = TRUE;
	return;
    }
    git = it;
}

/*!
  Writes a (\a key, \a value) entry to the current group.

  \sa readEntry()
*/
void Config::writeEntry( const QString &key, const QString &value )
{
    if ( git == groups.end() ) {
	qWarning( "no group set" );
	return;
    }
    if ( (*git)[key] != value ) {
	( *git ).insert( key, value );
	changed = TRUE;
    }
}

/*!
  Writes a (\a key, \a num) entry to the current group.

  \sa readNumEntry()
*/
void Config::writeEntry( const QString &key, int num )
{
    QString s;
    s.setNum( num );
    writeEntry( key, s );
}

#ifdef Q_HAS_BOOL_TYPE
/*!
  Writes a (\a key, \a b) entry to the current group.

  \sa readBoolEntry()
*/
void Config::writeEntry( const QString &key, bool b )
{
    QString s;
    s.setNum( ( int )b );
    writeEntry( key, s );
}
#endif

/*!
  Writes a (\a key, \a lst) entry to the current group. The list
  is separated by \a sep, so the strings must not contain that character.

  \sa readListEntry()
*/
void Config::writeEntry( const QString &key, const QStringList &lst, const QChar &sep )
{
    QString s;
    QStringList::ConstIterator it = lst.begin();
    for ( ; it != lst.end(); ++it )
	s += *it + sep;
    writeEntry( key, s );
}



/*!
  Reads a string entry stored with \a key, defaulting to \a deflt if there is no entry.
*/
QString Config::readEntry( const QString &key, const QString &deflt )
{
    QString res = readEntryDirect( key+"["+lang+"]" );
    if ( !res.isNull() )
	return res;
    if ( !glang.isEmpty() ) {
	res = readEntryDirect( key+"["+glang+"]" );
	if ( !res.isNull() )
	    return res;
    }
    return readEntryDirect( key, deflt );
}

/*!
  \internal
*/
QString Config::readEntryDirect( const QString &key, const QString &deflt )
{
    if ( git == groups.end() ) {
	//qWarning( "no group set" );
	return deflt;
    }
    ConfigGroup::Iterator it = ( *git ).find( key );
    if ( it != ( *git ).end() )
	return *it;
    else
	return deflt;
}

/*!
  Reads a numeric entry stored with \a key, defaulting to \a deflt if there is no entry.
*/
int Config::readNumEntry( const QString &key, int deflt )
{
    QString s = readEntry( key );
    if ( s.isEmpty() )
	return deflt;
    else
	return s.toInt();
}

/*!
  Reads a bool entry stored with \a key, defaulting to \a deflt if there is no entry.
*/
bool Config::readBoolEntry( const QString &key, bool deflt )
{
    QString s = readEntry( key );
    if ( s.isEmpty() )
	return deflt;
    else
	return (bool)s.toInt();
}

/*!
  Reads a string list entry stored with \a key, and with \a sep as the separator.
*/
QStringList Config::readListEntry( const QString &key, const QChar &sep )
{
    QString s = readEntry( key );
    if ( s.isEmpty() )
	return QStringList();
    else
	return QStringList::split( sep, s );
}

/*!
  Removes all entries from the current group.
*/
void Config::clearGroup()
{
    if ( git == groups.end() ) {
	qWarning( "no group set" );
	return;
    }
    if ( !(*git).isEmpty() ) {
	( *git ).clear();
	changed = TRUE;
    }
}

/*!
  \internal
*/
void Config::write( const QString &fn )
{
    if ( !fn.isEmpty() )
	filename = fn;

    QFile f( filename );
    if ( !f.open( IO_WriteOnly ) ) {
	qWarning( "could not open for writing `%s'", filename.latin1() );
	git = groups.end();
	return;
    }

    QTextStream s( &f );
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
    // The below should work, but doesn't in Qt 2.3.0
    s.setCodec( QTextCodec::codecForMib( 106 ) );
#else
    s.setEncoding( QTextStream::UnicodeUTF8 );
#endif
    QMap< QString, ConfigGroup >::Iterator g_it = groups.begin();
    for ( ; g_it != groups.end(); ++g_it ) {
	s << "[" << g_it.key() << "]" << "\n";
	ConfigGroup::Iterator e_it = ( *g_it ).begin();
	for ( ; e_it != ( *g_it ).end(); ++e_it )
	    s << e_it.key() << " = " << *e_it << "\n";
    }

    f.close();
}

/*!
  Returns whether the Config is in a valid state.
*/
bool Config::isValid() const
{
    return groups.end() != git;
}

/*!
  \internal
*/
void Config::read()
{
    changed = FALSE;

    if ( !QFileInfo( filename ).exists() ) {
	git = groups.end();
	return;
    }

    QFile f( filename );
    if ( !f.open( IO_ReadOnly ) ) {
	git = groups.end();
	return;
    }

    QTextStream s( &f );
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
    // The below should work, but doesn't in Qt 2.3.0
    s.setCodec( QTextCodec::codecForMib( 106 ) );
#else
    s.setEncoding( QTextStream::UnicodeUTF8 );
#endif

    QString line;
    while ( !s.atEnd() ) {
	line = s.readLine();
	if ( !parse( line ) ) {
	    git = groups.end();
	    return;
	}
    }

    f.close();
}

/*!
  \internal
*/
bool Config::parse( const QString &l )
{
    QString line = l.stripWhiteSpace();
    if ( line[ 0 ] == QChar( '[' ) ) {
	QString gname = line;
	gname = gname.remove( 0, 1 );
	if ( gname[ (int)gname.length() - 1 ] == QChar( ']' ) )
	    gname = gname.remove( gname.length() - 1, 1 );
	ConfigGroup *grp = new ConfigGroup;
	git = groups.insert( gname, *grp );
    } else if ( !line.isEmpty() ) {
	if ( git == groups.end() )
	    return FALSE;
	int eq = line.find( '=' );
	if ( eq == -1 )
	    return FALSE;
	QString key = line.left(eq).stripWhiteSpace();
	QString value = line.mid(eq+1).stripWhiteSpace();
	( *git ).insert( key, value );
    }
    return TRUE;
}
