#include <qdir.h>
#include <qfile.h>
#include <qmap.h>

#include "osqlbackendmanager.h"

namespace {
    class Config {
        typedef QMap<QString, QString> List;
    public:
        Config( const QString& fileName );
        /**
         * Quite simple layout in nature
         * BeginFile
         * Key = Value
         */
        bool load();
        QString value( const QString& key );
    private:
        List m_list;
        QString m_fileName;
    };
    Config::Config( const QString& fileName )
        : m_fileName( fileName ) {
    }

    bool Config::load() {
        if (!QFile::exists( m_fileName ) )
            return false;
        QFile file( m_fileName );
        if (!file.open(IO_ReadOnly ) )
            return false;
        QStringList list = QStringList::split( '\n', file.readAll() );
        QStringList::Iterator it;
        QString line;
        for (it =  list.begin(); it != list.end(); ++it ) {
            line = (*it).stripWhiteSpace();
            qWarning("Anonymous::Config:" + line );
            QStringList test = QStringList::split(' ', line );
            m_list.insert( test[0], test[2] );
        }
        return true;
    }
    QString Config::value( const QString& key ) {
        return m_list[key];
    }
};
OSQLBackEndManager::OSQLBackEndManager( const QStringList& path )
    :m_path( path )
{
}
OSQLBackEndManager::~OSQLBackEndManager() {
}
/**
 * scan dirs
 */
OSQLBackEnd::ValueList OSQLBackEndManager::scan() {
    OSQLBackEnd::ValueList list;
    if (!m_path.isEmpty() ) {
        QStringList::Iterator it;
        for ( it = m_path.begin(); it != m_path.end(); ++it ) {
            list += scanDir( (*it) );
        }
    }
    return list;
}
/**
 * scan a specified dir for *.osql
 */
OSQLBackEnd::ValueList OSQLBackEndManager::scanDir( const QString& dirName ) {
    OSQLBackEnd::ValueList list;
    QDir dir( dirName );
    if (dir.exists() ) {
        QStringList files = dir.entryList( "*.osql" );
        QStringList::Iterator it;
        for ( it = files.begin(); it != files.end(); ++it ) {
            list.append( file2backend( (*it) ) );
        }
    }
    return list;
}

/**
 * read a config file and convert it to a OSQLBackEnd
 */
OSQLBackEnd OSQLBackEndManager::file2backend( const QString& file ) {
    OSQLBackEnd end;
    qWarning("fileName: " + file  );
    Config cfg( file );
    if (cfg.load() ) {
        end.setName( cfg.value( "Name") );
        end.setVendor( cfg.value("Vendor") );
        end.setLicense( cfg.value("License") );
        end.setLibrary( cfg.value("Library").local8Bit() );
        end.setDefault( cfg.value("Default").toInt() );
        end.setPreference( cfg.value("Preference").toInt() );
    }
    return end;
}
