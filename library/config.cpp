/**********************************************************************
** Copyright (C) 2000,2004 Trolltech AS.  All rights reserved.
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
#include <qmessagebox.h>
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
#include <qtextcodec.h>
#endif
#include <qtextstream.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define QTOPIA_INTERNAL_LANGLIST
#include "config.h"
#include "global.h"
#include <qtopia/qpeapplication.h>


/*
 * Internal Class
 */
class ConfigPrivate {
public:
    ConfigPrivate() : multilang(FALSE) {}
    ConfigPrivate(const ConfigPrivate& o) :
        trfile(o.trfile),
        trcontext(o.trcontext),
        multilang(o.multilang)
    {}
    ConfigPrivate& operator=(const ConfigPrivate& o)
    {
        trfile = o.trfile;
        trcontext = o.trcontext;
        multilang = o.multilang;
        return *this;
    }

    QString trfile;
    QCString trcontext;
    bool multilang;
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

#ifndef Q_OS_WIN32

//#define DEBUG_CONFIG_CACHE
class ConfigData
{
public:
    ConfigData() {}
    ConfigData( const ConfigGroupMap& cf, const ConfigPrivate& pri,
                struct stat sbuf )
        : cfg( cf ), priv( pri ), mtime( sbuf.st_mtime ),
          size( sbuf.st_size )
        {
            gettimeofday(&used, 0 );
        }

    ConfigGroupMap cfg;
    ConfigPrivate priv; // Owned by this object
    time_t mtime;
    unsigned int size;
    struct timeval used;
};


class ConfigCache : public QObject {
public:
    static ConfigCache* instance();

    void insert( const QString& fileName, const ConfigGroupMap& cfg,
                 const ConfigPrivate *priv );
    bool find(const QString& fileName, ConfigGroupMap& cfg,
              ConfigPrivate** priv );
protected:
    void timerEvent( QTimerEvent* );

private:
    ConfigCache();
    void remove( const QString& fileName );
    void removeLru();

private:
    QMap<QString, ConfigData> m_cached;
    unsigned int m_totalSize;
    int m_tid;
private:
    static ConfigCache* m_inst;
    static const unsigned int CONFIG_CACHE_SIZE = 8192;
    static const unsigned int CONFIG_CACHE_TIMEOUT = 1000;
};

ConfigCache* ConfigCache::m_inst = 0;
/*
 * get destroyed when qApp gets destroyed
 */
ConfigCache::ConfigCache() : QObject( qApp ), m_totalSize( 0 ), m_tid( 0 ) {}
ConfigCache* ConfigCache::instance() {
    if ( !m_inst )
        m_inst = new ConfigCache();

    return m_inst;
}

void ConfigCache::remove( const QString& fileName ) {
    QMap<QString, ConfigData>::Iterator it = m_cached.find( fileName );

    if ( it == m_cached.end() )
        return;

    m_totalSize -= (*it).size;
    m_cached.remove( it );
}

void ConfigCache::removeLru() {
    QMap<QString, ConfigData>::Iterator it  = m_cached.begin();
    QMap<QString, ConfigData>::Iterator lru = it;
    ++it;
    for (; it != m_cached.end(); ++it)
        if ((*it).used.tv_sec < (*lru).used.tv_sec ||
            ((*it).used.tv_sec == (*lru).used.tv_sec &&
             (*it).used.tv_usec < (*lru).used.tv_usec))
            lru = it;

    m_totalSize -= (*lru).size;
    m_cached.remove(lru);
}

void ConfigCache::timerEvent( QTimerEvent* ) {
    while ( m_totalSize > CONFIG_CACHE_SIZE )
        removeLru();

    killTimer(m_tid);
    m_tid = 0;
}

void ConfigCache::insert( const QString& fileName, const ConfigGroupMap& cfg,
                          const ConfigPrivate* _priv ) {


    struct stat sbuf;
    ::stat( QFile::encodeName(fileName), &sbuf );
    if ( static_cast<unsigned int>(sbuf.st_size) >= CONFIG_CACHE_SIZE>>1)
        return;

    /*
     * remove the old version and use the new one
     */
    ConfigPrivate priv = _priv ? *_priv : ConfigPrivate();
    ConfigData data( cfg, priv, sbuf );
    m_totalSize += data.size;

    remove( fileName );
    m_cached.insert( fileName, data );

    /*
     * we've overcommited allocation, let us clean up
     * soon
     */
    if ( m_totalSize >= CONFIG_CACHE_SIZE )
        if ( !m_tid )
            m_tid = startTimer(CONFIG_CACHE_TIMEOUT);
}

bool ConfigCache::find( const QString& fileName, ConfigGroupMap& cfg,
                        ConfigPrivate **ppriv ) {
    QMap<QString, ConfigData>::Iterator it = m_cached.find(fileName);
    if (it != m_cached.end()) {
        ConfigData &data = *it;
        struct stat sbuf;
        ::stat(QFile::encodeName( fileName ), &sbuf);

        if (data.mtime == sbuf.st_mtime && (int)data.size == sbuf.st_size) {
            cfg = data.cfg;

            /*
             * null pointer
             */
            if ( *ppriv == 0 )
                *ppriv = new ConfigPrivate( data.priv );
            **ppriv = data.priv;
            gettimeofday(&data.used, 0);

            return true;
        }
    }

    return false;
}

#endif


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

/* This cannot be made public because of binary compat issues */
void Config::read( QTextStream &s )
{
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
    // The below should work, but doesn't in Qt 2.3.0
    s.setCodec( QTextCodec::codecForMib( 106 ) );
#else
    s.setEncoding( QTextStream::UnicodeUTF8 );
#endif

    QStringList list = QStringList::split('\n', s.read() );

    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        if ( !parse( *it ) ) {
            git = groups.end();
            return;
        }
    }
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
    d = 0;
    read();
}


// Sharp ROM compatibility
Config::Config ( const QString &name, bool what )
	: filename( configFilename(name,what ? User : File) )
{
    git = groups.end();
    d = 0;
    read();
}

/*!
  Writes any changes to disk and destroys the in-memory object.
*/
Config::~Config()
{
    if ( changed )
        write();

    delete d;
}

/*!
  Returns whether the current group has an entry called \a key.
*/
bool Config::hasKey( const QString &key ) const
{
    if ( groups.end() == git )
	return FALSE;
    ConfigGroup::ConstIterator it = ( *git ).find( key );
    if ( it == ( *git ).end() ) {
        if ( d && !d->trcontext.isNull() ) {
            it = ( *git ).find( key + "[]" );
        } else if ( d && d->multilang ) {
            it = ( *git ).find( key + "["+lang+"]" );
            if ( it == ( *git ).end() && !glang.isEmpty() )
                it = ( *git ).find( key + "["+glang+"]" );
        }
    }
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

/*
 * ### !LocalTranslator::translate was kept out!
 *
 */

/*!
  \internal
  For compatibility, non-const version.
*/
QString Config::readEntry( const QString &key, const QString &deflt )
{
    QString r;
    if ( d && !d->trcontext.isNull() ) {
        // Still try untranslated first, becuase:
        //  1. It's the common case
        //  2. That way the value can be WRITTEN (becoming untranslated)
        r = readEntryDirect( key );
        if ( !r.isNull() )
            return r;
        r = readEntryDirect( key + "[]" );
        if ( !r.isNull() )
            return qApp->translate(d->trfile,d->trcontext,r);
    } else if ( d && d->multilang ) {
        // For compatibilitity
        r = readEntryDirect( key + "["+lang+"]" );
        if ( !r.isNull() )
            return r;
        if ( !glang.isEmpty() ) {
            r = readEntryDirect( key + "["+glang+"]" );
            if ( !r.isNull() )
                return r;
        }
    }
    r = readEntryDirect( key, deflt );
    return r;
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
    QString res = readEntry( key );
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
    QString oldGroup = git.key();

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
        return;
    }

#ifndef Q_OS_WIN32
    ConfigCache::instance()->insert( filename, groups, d );
    setGroup( oldGroup );
#endif
    changed = FALSE;
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

    QString readFilename(filename);

    if ( !QFile::exists(filename) ) {
        bool failed = TRUE;
        QFileInfo fi(filename);
        QString settingsDir = QDir::homeDirPath() + "/Settings";
        if (fi.dirPath(TRUE) == settingsDir) {
        // User setting - see if there is a default in $OPIEDIR/etc/default/
            QString dftlFile = QPEApplication::qpeDir() + "etc/default/" + fi.fileName();
            if (QFile::exists(dftlFile)) {
                readFilename = dftlFile;
                failed = FALSE;
            }
        }
        if (failed) {
            git = groups.end();
            return;
        }
    }

#ifndef Q_OS_WIN32

    if (ConfigCache::instance()->find(readFilename, groups, &d)) {
        if ( d && d->multilang ) {
            QStringList l = Global::languageList();
            lang = l[0];
            glang = l[1];
        }
        git = groups.begin();
        return;
    }
#endif

    QFile f( readFilename );
    if ( !f.open( IO_ReadOnly ) ) {
        git = groups.end();
        return;
    }

    if (f.getch()!='[') {
        git = groups.end();
        return;
    }
    f.ungetch('[');

    QTextStream s( &f );
    read( s );
    f.close();

#ifndef Q_OS_WIN32
    ConfigCache::instance()->insert(readFilename, groups, d);
#endif
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

        if ( git.key() == "Translation" ) {
            if ( key == "File" ) {
                if ( !d )
                    d = new ConfigPrivate;
                d->trfile = value;
            } else if ( key == "Context" ) {
                if ( !d )
                    d = new ConfigPrivate;
                d->trcontext = value.latin1();
            } else if ( key.startsWith("Comment") ) {
                return TRUE; // ignore comment for ts file
            } else {
                return FALSE; // Unrecognized
            }
        }

        int kl = key.length();
        if ( kl > 1 && key[kl-1] == ']' && key[kl-2] != '[' ) {
            // Old-style translation (inefficient)
            if ( !d )
                d = new ConfigPrivate;
            if ( !d->multilang ) {
                QStringList l = Global::languageList();
                lang = l[0];
                glang = l[1];
                d->multilang = TRUE;
            }
        }

        ( *git ).insert( key, value );
    }
    return TRUE;
}



bool Config::hasGroup( const QString& name )const {
    return ( groups. find ( name ) != groups. end ( ));
};

QStringList Config::groupList()const {
    QStringList sl;
    for ( ConfigGroupMap::ConstIterator it = groups. begin ( ); it != groups. end ( ); ++it )
        sl << it.key();

    return sl;
};

/////////////
// Qtopia 2.1 Functions
//
////////////

QStringList Config::allGroups()const {
    return groupList();
}

/*!
  Returns the time stamp for the config identified by \a name.  The
  time stamp represents the time the config was last committed to storage.
  Returns 0 if there is no time stamp available for the config.

  A \a domain can optionally be specified and defaults to User.
  See \l{Config()} for details.

  First availability: Qtopia 2.0
*/
long Config::timeStamp(const QString& name, Domain domain)
{
#ifdef Q_WS_WIN
    // Too slow (many conversions too and from time_t and QDataTime)
    QDateTime epoch;
    epoch.setTime_t(0);
    return epoch.secsTo(QFileInfo(Config::configFilename(name,domain)).lastModified());
#else
    QString fn = Config::configFilename(name,domain);
    struct stat b;
    if (lstat( QFile::encodeName(fn).data(), &b ) == 0)
        return b.st_mtime;
    else
        return 0;
#endif
}


/*!
  Removes the current group (and all its entries).

  The current group becomes unset.

  First availability: Qtopia 2.0
*/
void Config::removeGroup()
{
    if ( git == groups.end() ) {
        qWarning( "no group set" );
        return;
    }

    groups.remove(git.key());
    git = groups.end();
    changed = TRUE;
}

/*!
  Removes the current group (and all its entries).

  The current group becomes unset.

  First availability: Qtopia 2.0
*/
void Config::removeGroup(const QString& g)
{
    groups.remove(g);
    git = groups.end();
}



/*!
  Writes a (\a key, \a lst) entry to the current group.

  The list is
  separated by the two characters "^e", and "^" withing the strings
  is replaced by "^^", such that the strings may contain any character,
  including "^".

  Null strings are also allowed, and are recorded as "^0" in the string.

  First availability: Qtopia 2.0

  \sa readListEntry()
*/
void Config::writeEntry( const QString &key, const QStringList &lst )
{
    QString s;
    for (QStringList::ConstIterator it=lst.begin(); it!=lst.end(); ++it) {
        QString el = *it;
        if ( el.isNull() ) {
            el = "^0";
        } else {
            el.replace(QRegExp("\\^"), "^^");
        }
        s+=el;
        s+="^e"; // end of element
    }
    writeEntry(key, s);
}

/*!
  Returns the string list entry stored using \a key and with
  the escaped seperator convention described in writeListEntry().

  First availability: Qtopia 2.0
*/
QStringList Config::readListEntry( const QString &key ) const
{
    QString value = readEntry( key, QString::null );
    QStringList l;
    QString s;
    bool esc=FALSE;
    for (int i=0; i<(int)value.length(); i++) {
        if ( esc ) {
            if ( value[i] == 'e' ) { // end-of-string
                l.append(s);
                s="";
            } else if ( value[i] == '0' ) { // null string
                s=QString::null;
            } else {
                s.append(value[i]);
            }
            esc = FALSE;
        } else if ( value[i] == '^' ) {
            esc = TRUE;
        } else {
            s.append(value[i]);
            if ( i == (int)value.length()-1 )
                l.append(s);
        }
    }
    return l;
}

QString Config::readEntry( const QString &key, const QString &deflt ) const
{ return ((Config*)this)->readEntry(key,deflt); }
QString Config::readEntryCrypt( const QString &key, const QString &deflt ) const
{ return ((Config*)this)->readEntryCrypt(key,deflt); }
QString Config::readEntryDirect( const QString &key, const QString &deflt ) const
{ return ((Config*)this)->readEntryDirect(key,deflt); }
int Config::readNumEntry( const QString &key, int deflt ) const
{ return ((Config*)this)->readNumEntry(key,deflt); }
bool Config::readBoolEntry( const QString &key, bool deflt ) const
{ return ((Config*)this)->readBoolEntry(key,deflt); }
QStringList Config::readListEntry( const QString &key, const QChar &sep ) const
{ return ((Config*)this)->readListEntry(key,sep); }
