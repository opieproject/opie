/*
 * GPLv2 zecke@handhelds.org
 */

#include "dcim_lister.h"
#include <lib/slavemaster.h>

#include <opie2/odebug.h>
#include <qpe/storage.h>

#include <qdir.h>
#include <qfileinfo.h>
#include <qimage.h>

DCIM_DirLister::DCIM_DirLister()
    : PDirLister( "dcim_dirlister" )
{

    /*
     * create a SlaveMaster and lets connect the signal of
     * it to our interface
     */
    SlaveHelper::slaveConnectSignals( this );
    m_mode = ListingUnknown;
}

DCIM_DirLister::~DCIM_DirLister() {}

QString DCIM_DirLister::defaultPath()const {
    m_mode = ListingStart;
    return QString::null;
}

QString DCIM_DirLister::setStartPath( const QString& str) {
    /**
     * IconView adds a '/' to path. Lets strip
     * that.
     */
    QString st = str.mid( 1 );
    if ( ListingStart == m_mode && m_map.contains( st ) ) {
        m_path = m_map[st]+ "/dcim";
        m_mode = ListingFolder;
    }else if ( m_mode == ListingFolder ) {
        m_mode = ListingFiles;
        m_path = str;
    }else if ( m_mode == ListingReFolder ) {
        m_mode = ListingFolder;
    }

    owarn << " StartPath2 " << str << " " << m_path << oendl;

    return m_path;
}


QString DCIM_DirLister::currentPath()const {
    return m_path;
}

/*
 * depending on the mode we will either
 * Find Digital Cameras
 */
QStringList DCIM_DirLister::folders()const {
    QStringList lst;

    switch( m_mode ) {
    case ListingUnknown:
    case ListingStart:
        lst = findCameras();
        break;
    case ListingFolder:
        lst = findAlbums();
        break;
    case ListingFiles:
    default:
        break;
    }

    return lst;
}

QStringList DCIM_DirLister::files()const {
    if ( m_mode != ListingFiles )
        return QStringList();
    else
        return findImages();
}

QString DCIM_DirLister::dirUp( const QString& p )const {
    QString str;

    switch( m_mode ) {
    case ListingFiles:
        m_mode = ListingReFolder;
        str = PDirLister::dirUp( p );
        break;
    case ListingFolder:
        m_mode = ListingStart;
        break;
    case ListingUnknown:
    case ListingStart:
    default:
        break;
    }

    /* down cases */
    owarn << " New String " << str << " old path " << m_mode << oendl;
    m_path = str;
    return str;
}


QStringList DCIM_DirLister::findCameras()const {
    QStringList lst;
    StorageInfo inf;

    m_map.clear();

    const QList<FileSystem> &list = inf.fileSystems();
    QListIterator<FileSystem> it( list );


    FileSystem *sys;
    for ( sys = it.current(); (sys=it.current())!=0  ; ++it )
        if ( QFileInfo( sys->path() + "/dcim/" ).exists() ) {
            lst << sys->name();
            m_map.insert( sys->name(), sys->path() );
        }

    if ( lst.isEmpty() ) {
        m_mode = ListingUnknown;
        lst << QObject::tr("Error no Camera Dir found");
    }else
        m_mode = ListingStart;

    return lst;
}

QStringList DCIM_DirLister::findAlbums()const {
    QStringList lst = QDir( m_path ).entryList( QDir::Dirs );
    lst.remove( "."  );
    lst.remove( ".." );

    return lst;
}

QStringList DCIM_DirLister::findImages()const {
    return QDir( m_path ).entryList("*.jpg *.jpeg *.png", QDir::Files );
}

void DCIM_DirLister::deleteImage( const QString& fl ) {
    QFileInfo inf( fl );
    QFile::remove( fl );
    QFile::remove( inf.dirPath ()+"/preview/"+
                   inf.fileName() );
}

void DCIM_DirLister::thumbNail( const QString& _str, int w, int h ) {
    QFileInfo inf( _str );
    QString str = QFileInfo( inf.dirPath()+"/preview"+ inf.fileName() ).exists() ?
                  inf.dirPath()+"/preview"+ inf.fileName() : _str;

    SlaveMaster::self()->thumbNail( str, w, h );
}

QImage DCIM_DirLister::image( const QString& str,  Factor f,  int m ) {
    return SlaveMaster::self()->image( str, f, m );
}

void DCIM_DirLister::imageInfo( const QString& str ) {
    SlaveMaster::self()->thumbInfo( str );
}

void DCIM_DirLister::fullImageInfo( const QString& str ) {
    SlaveMaster::self()->imageInfo( str );
}

QString DCIM_DirLister::nameToFname( const QString& name )const {
    return name;
}
