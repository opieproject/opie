/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
**********************************************************************/

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
#include <qtextcodec.h>
#endif
#include <qtextstream.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"


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
	git = groups.insert( gname, ConfigGroup() );
	changed = TRUE;
	return;
    }
    git = it;
}

/*!
  Writes a (\a key, \a value) entry to the current group.

  \sa readEntry()
*/
void Config::writeEntry( const QString &key, const char* value )
{
    writeEntry(key,QString(value));
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

/*
  Note that the degree of protection offered by the encryption here is
  only sufficient to avoid the most casual observation of the configuration
  files. People with access to the files can write down the contents and
  decrypt it using this source code.

  Conceivably, and at some burden to the user, this encryption could
  be improved.
*/
static QString encipher(const QString& plain)
{
    // mainly, we make it long
    QString cipher;
    int mix=28730492;
    for (int i=0; i<(int)plain.length(); i++) {
	int u = plain[i].unicode();
	int c = u ^ mix;
	QString x = QString::number(c,36);
	cipher.append(QChar('a'+x.length()));
	cipher.append(x);
	mix *= u;
    }
    return cipher;
}

static QString decipher(const QString& cipher)
{
    QString plain;
    int mix=28730492;
    for (int i=0; i<(int)cipher.length();) {
	int l = cipher[i].unicode()-'a';
	QString x = cipher.mid(i+1,l); i+=l+1;
	int u = x.toInt(0,36) ^ mix;
	plain.append(QChar(u));
	mix *= u;
    }
    return plain;
}

/*!
  Writes an encrypted (\a key, \a value) entry to the current group.

  Note that the degree of protection offered by the encryption is
  only sufficient to avoid the most casual observation of the configuration
  files.

  \sa readEntry()
*/
void Config::writeEntryCrypt( const QString &key, const QString &value )
{
    if ( git == groups.end() ) {
	qWarning( "no group set" );
	return;
    }
    QString evalue = encipher(value);
    if ( (*git)[key] != evalue ) {
	( *git ).insert( key, evalue );
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
  Writes a (\a key, \a b) entry to the current group. This is equivalent
  to writing a 0 or 1 as an integer entry.

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
  Removes the \a key entry from the current group. Does nothing if
  there is no such entry.
*/

void Config::removeEntry( const QString &key )
{
    if ( git == groups.end() ) {
	qWarning( "no group set" );
	return;
    }
    ( *git ).remove( key );
    changed = TRUE;
}

/*!
  \fn bool Config::operator == ( const Config & other ) const

  Tests for equality with \a other. Config objects are equal if they refer to the same filename.
*/

/*!
  \fn bool Config::operator != ( const Config & other ) const

  Tests for inequality with \a other. Config objects are equal if they refer to the same filename.
*/

/*!
  \fn QString Config::readEntry( const QString &key, const QString &deflt ) const

  Reads a string entry stored with \a key, defaulting to \a deflt if there is no entry.
*/

/*!
  \internal
  For compatibility, non-const version.
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
  \fn QString Config::readEntryCrypt( const QString &key, const QString &deflt ) const

  Reads an encrypted string entry stored with \a key, defaulting to \a deflt if there is no entry.
*/

/*!
  \internal
  For compatibility, non-const version.
*/
QString Config::readEntryCrypt( const QString &key, const QString &deflt )
{
    QString res = readEntryDirect( key+"["+lang+"]" );
    if ( res.isNull() && glang.isEmpty() )
	res = readEntryDirect( key+"["+glang+"]" );
    if ( res.isNull() )
	res = readEntryDirect( key, QString::null );
    if ( res.isNull() )
	return deflt;
    return decipher(res);
}

/*!
  \fn QString Config::readEntryDirect( const QString &key, const QString &deflt ) const
  \internal
*/

/*!
  \internal
  For compatibility, non-const version.
*/
QString Config::readEntryDirect( const QString &key, const QString &deflt )
{
    if ( git == groups.end() ) {
	//qWarning( "no group set" );
	return deflt;
    }
    ConfigGroup::ConstIterator it = ( *git ).find( key );
    if ( it != ( *git ).end() )
	return *it;
    else
	return deflt;
}

/*!
  \fn int Config::readNumEntry( const QString &key, int deflt ) const
  Reads a numeric entry stored with \a key, defaulting to \a deflt if there is no entry.
*/

/*!
  \internal
  For compatibility, non-const version.
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
  \fn bool Config::readBoolEntry( const QString &key, bool deflt ) const
  Reads a bool entry stored with \a key, defaulting to \a deflt if there is no entry.
*/

/*!
  \internal
  For compatibility, non-const version.
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
  \fn QStringList Config::readListEntry( const QString &key, const QChar &sep ) const
  Reads a string list entry stored with \a key, and with \a sep as the separator.
*/

/*!
  \internal
  For compatibility, non-const version.
*/

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

void Config::removeGroup()
{
    if ( git == groups.end() ) {
        qWarning( "no group set" );
        return;
    }

    groups.remove(git);
    changed = TRUE;
}

/*!
  \internal
*/
void Config::write( const QString &fn )
{
    QString strNewFile;
    if ( !fn.isEmpty() )
	filename = fn;
    strNewFile = filename + ".new";

    QFile f( strNewFile );
    if ( !f.open( IO_WriteOnly|IO_Raw ) ) {
	qWarning( "could not open for writing `%s'", strNewFile.latin1() );
	git = groups.end();
	return;
    }
    
    QString str;
    QCString cstr;    
    QMap< QString, ConfigGroup >::Iterator g_it = groups.begin();

    for ( ; g_it != groups.end(); ++g_it ) {
 	str += "[" + g_it.key() + "]\n";
 	ConfigGroup::Iterator e_it = ( *g_it ).begin();
 	for ( ; e_it != ( *g_it ).end(); ++e_it )
 	    str += e_it.key() + " = " + *e_it + "\n";
    }
    cstr = str.utf8();
    
    int total_length;
    total_length = f.writeBlock( cstr.data(), cstr.length() );
    if ( total_length != int(cstr.length()) ) {
	QMessageBox::critical( 0, QObject::tr("Out of Space"),
			       QObject::tr("There was a problem creating\nConfiguration Information \nfor this program.\n\nPlease free up some space and\ntry again.") );
	f.close();
	QFile::remove( strNewFile );
	return;
    }
    
    f.close();
    // now rename the file...
    if ( rename( strNewFile, filename ) < 0 ) {
	qWarning( "problem renaming the file %s to %s", strNewFile.latin1(), 
		  filename.latin1() );
	QFile::remove( strNewFile );
    }	
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

    QStringList list = QStringList::split('\n', s.read() );
    f.close();

    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        if ( !parse( *it ) ) {
            git = groups.end();
            return;
        }
    }
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
	git = groups.insert( gname, ConfigGroup() );
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
