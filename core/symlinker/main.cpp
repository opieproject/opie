#include <qapplication.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include <stdlib.h>
#include <unistd.h> //symlink() 
#include <sys/stat.h> // mkdir()

#include <sys/vfs.h>
#include <mntent.h>
#include <errno.h>

static const char *listDir = "/usr/lib/ipkg/externinfo/";

static void createSymlinks( const QString &location, const QString &package )
{

    QFile inFile( location + "/usr/lib/ipkg/info/" + package + ".list" );
    mkdir( "/usr/lib/ipkg", 0777 );
    mkdir( listDir, 0777 );

    QFile outFile( listDir + package + ".list");

//    qDebug( "createSymlinks %s -> %s", inFile.name().ascii(), outFile.name().ascii() );
    

    
    if ( inFile.open(IO_ReadOnly) && outFile.open(IO_WriteOnly)) {   
	QTextStream in(&inFile);
	QTextStream out(&outFile);

	QString s;
	while ( !in.eof() ) {        // until end of file...
	    s = in.readLine();       // line of text excluding '\n'
//	    qDebug( "Read: %s", s.ascii() );
	    if (s.find(location,0,true) >= 0){
//		    qDebug( "Found!" );
		    s = s.replace(location,"");
	    }
//	    qDebug( "Read after: %s", s.ascii() );
	    
	    // for s, do link/mkdir.
	    if ( s.right(1) == "/" ) {
//		qDebug("do mkdir for %s", s.ascii());
		mkdir( s.ascii(), 0777 );
		//possible optimization: symlink directories
		//that don't exist already. -- Risky.
	    } else {
//		qDebug("do symlink for %s", s.ascii());
		QFileInfo ffi( s ); 
		//Don't try to symlink if a regular file exists already
		if ( !ffi.exists() || ffi.isSymLink() ) {
		    if (symlink( (location+s).ascii(), s.ascii() ) != 0){
			if (errno == ENOENT){
//			perror("Symlink Failed! ");
			QString e=s.ascii();
			e = e.replace(ffi.fileName(),"");
//			qDebug("DirName : %s",e.ascii() );
			system ( QString("mkdir -p ")+e.ascii() );
		        if (symlink( (location+s).ascii(), s.ascii() ) != 0)
				qDebug ("Big problem creating symlink and directory");
			}
		    }
//		    qDebug ( "Created %s" ,s.ascii() );
		    out << s << "\n";
		} else {
		    qDebug( "%s  exists already, not symlinked", s.ascii() );
		}
	    }
	}
	inFile.close();
	outFile.close();
    }
}



static void removeSymlinks( const QString &package )
{
    QFile inFile( listDir + package + ".list" );

    if ( inFile.open(IO_ReadOnly) ) {   
	QTextStream in(&inFile);

	QString s;
	while ( !in.eof() ) {        // until end of file...
	    s = in.readLine();       // line of text excluding '\n'
//	    qDebug("remove symlink %s", s.ascii());
	    QFileInfo ffi( s ); 
	    //Confirm that it's still a symlink.
	    if ( ffi.isSymLink() ){
		unlink( s.ascii() );
//	    	qDebug ( "Removed %s", s.ascii() );}
//	    else 
//		qDebug( "Not removed %s", s.ascii() );
	    }
	}
	inFile.close();
	inFile.remove();
    }
}



/*
  Slightly hacky: we can't use StorageInfo, since we don't have a
  QApplication. We look for filesystems that have the directory
  /usr/lib/ipkg/info, and assume that they are removable media
  with packages installed. This is safe even if eg. /usr is on a 
  separate filesystem, since then we would be testing for
  /usr/usr/lib/ipkg/info, which should not exist. (And if it
  does they deserve to have it treated as removable.)
 */

static void updateSymlinks()
{
    QDir lists( listDir );
    QStringList knownPackages = lists.entryList( "*.list" ); // No tr
    
    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );

    if ( mntfp ) {
	while ( (me = getmntent( mntfp )) != 0 ) {
	    QString root = me->mnt_dir;
	    if ( root == "/" ) 
		continue;

	    QString info = root + "/usr/lib/ipkg/info";
	    QDir infoDir( info );
//	    qDebug( "looking at %s", info.ascii() );
	    if ( infoDir.isReadable() ) {
		const QFileInfoList *packages = infoDir.entryInfoList( "*.list" ); // No tr
		QFileInfoListIterator it( *packages );
		QFileInfo *fi;
		while (( fi = *it )) {
		    ++it;
		    if ( knownPackages.contains( fi->fileName() ) ) {
//			qDebug( "found %s and we've seen it before", fi->fileName().latin1() );
			knownPackages.remove( fi->fileName() );
		    } else {
			//it's a new one
			createSymlinks( root, fi->baseName() );
		    }
		    
		}		
		
	    }
	}
	endmntent( mntfp );
    }
    
    for ( QStringList::Iterator it = knownPackages.begin(); 
	  it != knownPackages.end(); ++it ) {
	// strip ".info" off the end.
	removeSymlinks( (*it).left((*it).length()-5) );
    }
}



int main( int argc, char *argv[] )
{
    QApplication a( argc, argv, QApplication::Tty );

    QString command = argc > 1 ? argv[1] : "update"; // No tr
    
    if ( command == "update" ) // No tr
	updateSymlinks();
    else if ( command == "create" && argc > 3 ) // No tr
	createSymlinks( argv[2], argv[3] );
    else if ( command == "remove"  && argc > 2 ) // No tr
	removeSymlinks( argv[2] );
    else
	qWarning( "Argument error" );
}
