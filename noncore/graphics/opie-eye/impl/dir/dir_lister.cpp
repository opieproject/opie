/*
 * GPLv2 zecke@handhelds.org
 */

#include "dir_lister.h"

#include <lib/slavemaster.h>

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;

/* QT */
#include <qdir.h>
#include <qfileinfo.h>

Dir_DirLister::Dir_DirLister( bool list )
    : PDirLister( "dir_dir_lister" )
{
    m_allFiles = list;
    owarn << "All Files " << m_allFiles << "" << oendl; 

    SlaveMaster* master = SlaveMaster::self();
    connect( master, SIGNAL(sig_start()), this, SIGNAL(sig_start()) );
    connect( master, SIGNAL(sig_end()), this, SIGNAL(sig_end()) );
    connect( master, SIGNAL(sig_thumbInfo(const QString&, const QString&)),
             this, SIGNAL(sig_thumbInfo(const QString&, const QString&)) );
    connect( master, SIGNAL(sig_fullInfo(const QString&, const QString&)),
             this, SIGNAL(sig_fullInfo(const QString&, const QString&)) );
    connect( master, SIGNAL(sig_thumbNail(const QString&, const QPixmap&)),
             this, SIGNAL(sig_thumbNail(const QString&, const QPixmap&)) );

}

QString Dir_DirLister::defaultPath()const {
    return QPEApplication::documentDir();
}

QString Dir_DirLister::setStartPath( const QString& path ) {
    m_currentDir.cd( path );
    if (!m_currentDir.exists() )
        m_currentDir.cd(defaultPath());


    return m_currentDir.absPath();
}

QString Dir_DirLister::currentPath()const {
    return m_currentDir.absPath();
}


QStringList Dir_DirLister::folders()const {
    return m_currentDir.entryList( QDir::Dirs );
}

QStringList Dir_DirLister::files()const {
    if ( m_allFiles )
        return m_currentDir.entryList( QDir::Files );
    else {
        QStringList out;
        QStringList list = m_currentDir.entryList(  QDir::Files | QDir::Readable );
        for (QStringList::Iterator it = list.begin(); it != list.end();++it ) {
            QFileInfo inf( *it );
            QString ext = inf.extension(false).lower();
            if( ext == QString::fromLatin1("jpg") ||
                ext == QString::fromLatin1("jpeg" ) ||
                ext == QString::fromLatin1("png" ) ||
                ext == QString::fromLatin1("bmp" ) ||
                ext == QString::fromLatin1("gif" ) )
                out.append( *it );
        }
        return out;
    }
}

void Dir_DirLister::deleteImage( const QString& fl) {
    QFile::remove( fl );
}

void Dir_DirLister::thumbNail( const QString& str, int w, int h) {
    SlaveMaster::self()->thumbNail( str, w, h );
}

QImage Dir_DirLister::image( const QString& str, Factor f, int m) {
    return SlaveMaster::self()->image( str, f, m );
}

void Dir_DirLister::imageInfo( const QString& str) {
    SlaveMaster::self()->thumbInfo( str );
}

void Dir_DirLister::fullImageInfo( const QString& str) {
    SlaveMaster::self()->imageInfo( str );
}
