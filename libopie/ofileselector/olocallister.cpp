#include <qdir.h>
#include <qfileinfo.h>
#include <qmap.h>

#include <qpe/mimetype.h>

#include "ofileselector.h"
#include "olocallister.h"

OLocalLister::OLocalLister( OFileSelector* file )
    : OLister( file )
{
}
OLocalLister::~OLocalLister() {
}

QMap<QString, QStringList> OLocalLister::mimeTypes( const QString& curDir ) {
    QMap<QString, QStringList> mimes;

    // let's find possible mimetypes
    QDir dir( curDir );
    dir.setFilter( QDir::Files | QDir::Readable );
    dir.setSorting( QDir::Size );

    const QFileInfoList *list = dir.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;

    while( (fi=it.current() ) ) {
        /* skip .desktop */
	if( fi->extension() == QString::fromLatin1("desktop") ){
            ++it;
            continue;
	}
	MimeType type( fi->absFilePath() );

        if( !mimes.contains( type.id() ) ){
            mimes.insert( type.id(), type.id() );
	}

	++it;
    }

    return mimes;
}
/**
 * FIXME mimecheck
 * use mime check for that
 * filter dirs
 * filter filters
 * filter files
 * filter mimetypes
 */
void OLocalLister::reparse( const QString& path ) {
    QString currentMimeType;
    QDir dir( path );

    dir.setSorting( view()->sorting() );
    dir.setFilter( view()->filter() );


    const QFileInfoList *list = dir.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;

    while( (fi=it.current() ) ){

        if( fi->fileName() == QString::fromLatin1("..") ||
            fi->fileName() == QString::fromLatin1(".") ){
            ++it;
            continue;
        }
        if( fi->isSymLink() ){
            QString file = fi->dirPath( true ) + "/" + fi->readLink();
            /*
             * 5 tries to prevent dos attack
             */
            for( int i = 0; i<=4; i++) {
                QFileInfo info( file );
                if( !info.exists() ){
                    addSymlink( currentMimeType, fi, TRUE );
                    break;
                }else if( info.isDir() ){
                    if (!showDirs() )
                        break;

                    addDir( currentMimeType, fi,
                            TRUE );
                    break;
                }else if( info.isFile() ){
                    /* if not show files skip it */
                    if (!showFiles() )
                        break;

                    /* check if we comply to the mimetype */
                    MimeType type( info.absFilePath() );
                    if (compliesMime( type.id() ) )
                        addFile( currentMimeType, fi, TRUE );

                    break;
                }else if( info.isSymLink() ){
                    file = info.dirPath(true ) + "/" + info.readLink() ;
                    break;
                }else if( i == 4){
                    addSymlink( currentMimeType, fi );
                }
            }

        }else if( fi->isDir() ){
            if (showDirs() )
                addDir( currentMimeType, fi );
        }else if( fi->isFile() ){
            if ( showFiles() )
                addFile( currentMimeType, fi );
        }
        ++it;
    } // of while loop
}

