/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#define QTOPIA_INTERNAL_LANGLIST
#define QTOPIA_INTERNAL_CONFIG_BYTEARRAY
#include "config.h"
#include "global.h"


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
  \brief The Config class provides for saving application configuration state.

  You should keep a Config in existence only while you do not want others
  to be able to change the state. There is no locking currently, but there
  may be in the future.

  \ingroup qtopiaemb
*/

/*!
  \enum Config::ConfigGroup
  \internal
*/

/*!
  \enum Config::Domain

  \value File
  \value User

  See \l{Config()} for details.
*/

/*!
  Constructs a configuration object that will load or create a
  configuration with the given \a name in the given \a domain.

  After construction, call setGroup() since almost every other
  function works in terms of the 'current group'.

  In the default Domain, \e User, the configuration is user-specific,
  and the \a name should not contain "/". This name should be globally
  unique.

  In the File Domain, \a name is an absolute filename.
*/
Config::Config( const QString &name, Domain domain )
    : filename( configFilename(name,domain) )
{
    git = groups.end();
    read();
    QStringList l = Global::languageList();
    lang = l[0];
    glang = l[1];
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
  Returns TRUE if the current group has an entry called \a key;
  otherwise returns FALSE.
*/
bool Config::hasKey( const QString &key ) const
{
    if ( groups.end() == git )
	return FALSE;
    ConfigGroup::ConstIterator it = ( *git ).find( key );
    return it != ( *git ).end();
}

/*!
  Sets the current group for subsequent reading and writing of entries
  to \a gname. Grouping allows the application to partition the
  namespace.

  This function \e must be called prior to any reading or writing of
  entries.

  The \a gname must not be empty.

  \sa writeEntry() readEntry() readListEntry() readNumEntry() readBoolEntry()
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
  Writes a weakly encrypted (\a key, \a value) entry to the current group.

  Note that the degree of protection offered by the encryption is
  only sufficient to avoid the most casual observation of the configuration
  files.

  \sa readEntryCrypt()
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

  \sa readNumEntry() readBoolEntry()
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
  Writes a (\a key, \a lst) entry to the current group. The list is
  separated by \a sep, so the strings must not contain that character.

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
  Writes a (\a key, \a byteArray) entry to the current group.  The byteArray is stored as
  a base64 encoded string.
*/
void Config::writeEntry( const QString &key, const QByteArray byteArray) {
  writeEntry(key, encodeBase64(byteArray));
}

/*!
  Removes the \a key entry from the current group. Does nothing if
  there is no such entry.

  \sa writeEntry() clearGroup()
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

  Tests for equality with \a other. Config objects are equal if they
  refer to the same filename.

  \sa operator!=()
*/

/*!
  \fn bool Config::operator != ( const Config & other ) const

  Tests for inequality with \a other. Config objects are equal if they
  refer to the same filename.

  \sa operator==()
*/

/*!
  \fn QString Config::readEntry( const QString &key, const QString &deflt ) const

  Returns the string entry for \a key, defaulting to \a deflt if there
  is no entry for the given \a key.

  \sa writeEntry()
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

  Returns the unencrypted string entry for the encrypted entry stored
  using \a key, defaulting to \a deflt if there is no entry for the
  given \a key.

  \sa writeEntryCrypt()
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

  Returns the integer entry stored using \a key, defaulting to \a
  deflt if there is no entry for the given \a key.

  \sa writeEntry()
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
  Returns the QByteArray stored using \a key.  Returns an empty array if
  no matching key is found.
*/
QByteArray Config::readByteArrayEntry(const QString& key) {
  QByteArray empty;
  return readByteArrayEntry(key, empty);
}

/*!
  Returns the QByteArray stored using \a key.  Returns \a dflt if
  no matching key is found.
*/
QByteArray Config::readByteArrayEntry(const QString& key, const QByteArray dflt) {
  QString s = readEntry(key);
  if (s.isEmpty())
    return dflt;
  return decodeBase64(s);
}

/*!
  \internal
  Decodes base64 encoded \a encoded and returns a QByteArray containing
  the decoded data.
*/
QByteArray Config::decodeBase64( const QString &encoded ) const
{
  QByteArray buffer;
  uint len = encoded.length();
  buffer.resize( len * 3 / 4 + 2);
  uint bufCount = 0;
  uint pos = 0, decodedCount = 0;
  char src[4];
  char *destPtr = buffer.data();
  
  while (pos < len ) {
    decodedCount = 4;
    int x = 0;
    while ( (x < 4) && (pos < len ) ) {
      src[x] = encoded[pos];
      pos++;
      if (src[x] == '\r' || src[x] == '\n' || src[x] == ' ')
	x--;
      x++;
    }
    if (x > 1) {
      decodedCount = parse64base(src, destPtr);
      destPtr += decodedCount;
      bufCount += decodedCount;
    }
  }
  
  return buffer;
}

/*!
  \fn bool Config::readBoolEntry( const QString &key, bool deflt ) const

  Returns the boolean entry stored (as an integer) using \a key,
  defaulting to \a deflt if there is no entry for the given \a key.

  \sa writeEntry()
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

  Returns the string list entry stored using \a key and with \a sep as
  the separator.

  These entries are stored as a single string, with each element
  separated by \a sep.

  \sa writeEntry()
*/

/*!
  \internal
  For compatibility, non-const version.
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

  \sa removeEntry()
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
  Returns TRUE if the Config is in a valid state; otherwise returns
  FALSE.
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

/*!
 \internal 
 Encodes \a origData using base64 mapping and returns a QString containing the
 encoded form.
*/
QString Config::encodeBase64(const QByteArray origData) {
  // follows simple algorithm from rsync code
  uchar *in = (uchar*)origData.data();
  
  int inbytes = origData.size();
  int outbytes = ((inbytes * 8) + 5) / 6;
//   int spacing = (outbytes-1)/76;
  int padding = 4-outbytes%4; if ( padding == 4 ) padding = 0;
  
//   QByteArray outbuf(outbytes+spacing+padding);
  QByteArray outbuf(outbytes+padding);
  uchar* out = (uchar*)outbuf.data();

  const char *b64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  
  for (int i = 0; i < outbytes; i++) {
//     if ( i && i%76==0 )
//       *out++ = '\n';
    int byte = (i * 6) / 8;
    int bit = (i * 6) % 8;
    if (bit < 3) {
      if (byte >= inbytes)
	abort();
      *out = (b64[(in[byte] >> (2 - bit)) & 0x3F]);
    } else {
      if (byte + 1 == inbytes) {
	*out = (b64[(in[byte] << (bit - 2)) & 0x3F]);
      } else {
	*out = (b64[(in[byte] << (bit - 2) |
		     in[byte + 1] >> (10 - bit)) & 0x3F]);
      }
    }
    ++out;
  }
  ASSERT(out == (uchar*)outbuf.data() + outbuf.size() - padding);
  while ( padding-- )
    *out++='=';
  
  return QString(outbuf);
}

/*!
 \internal
*/
int Config::parse64base(char *src, char *bufOut) const
{
  char c, z;
  char li[4];
  int processed;

  //conversion table without table...
  for (int x = 0; x < 4; x++) {
    c = src[x];

    if ( (int) c >= 'A' && (int) c <= 'Z')
      li[x] = (int) c - (int) 'A';
    if ( (int) c >= 'a' && (int) c <= 'z')
      li[x] = (int) c - (int) 'a' + 26;
    if ( (int) c >= '0' && (int) c <= '9')
      li[x] = (int) c - (int) '0' + 52;
    if (c == '+')
      li[x] = 62;
    if (c == '/')
      li[x] = 63;
  }

  processed = 1;
  bufOut[0] = (char) li[0] & (32+16+8+4+2+1);	//mask out top 2 bits
  bufOut[0] <<= 2;
  z = li[1] >> 4;
  bufOut[0] = bufOut[0] | z;		//first byte retrived

  if (src[2] != '=') {
    bufOut[1] = (char) li[1] & (8+4+2+1);	//mask out top 4 bits
    bufOut[1] <<= 4;
    z = li[2] >> 2;
    bufOut[1] = bufOut[1] | z;		//second byte retrived
    processed++;

    if (src[3] != '=') {
      bufOut[2] = (char) li[2] & (2+1);	//mask out top 6 bits
      bufOut[2] <<= 6;
      z = li[3];
      bufOut[2] = bufOut[2] | z;	//third byte retrieved
      processed++;
    }
  }
  return processed;
}
