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
#include "filemanager.h"
#include "applnk.h"

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qtextcodec.h>

#include <errno.h>
#include <stdlib.h>

/*!
  \class FileManager
  \brief The FileManager class assists with AppLnk input/output.
*/

/*!
  Constructs a FileManager.
*/
FileManager::FileManager()
{
}

/*!
  Destroys a FileManager.
*/
FileManager::~FileManager()
{

}

/*!
  Saves \a data as the document specified by \a f.

  Returns whether the operation succeeded.
*/
bool FileManager::saveFile( const DocLnk &f, const QByteArray &data )
{
    QString fn = f.file() + ".new";
    ensurePathExists( fn );
    QFile fl( fn );
    if ( !fl.open( IO_WriteOnly|IO_Raw ) )
	return FALSE;
    int total_written = fl.writeBlock( data );
    fl.close();
    if ( total_written != int(data.size()) || !f.writeLink() ) {
	QFile::remove( fn );
	return FALSE;
    }
    // else rename the file...
    if ( ::rename( fn.latin1(), f.file().latin1() ) < 0 ) {
	qWarning( "problem renaming file %s to %s, errno: %d", fn.latin1(),
		  f.file().latin1(), errno );
	// remove the file...
	QFile::remove( fn );
    }
    return TRUE;
}

/*!
  Saves \a text as the document specified by \a f.

  The text is saved in UTF8 format.

  Returns whether the operation succeeded.
*/
bool FileManager::saveFile( const DocLnk &f, const QString &text )
{
    QString fn = f.file() + ".new";
    ensurePathExists( fn );
    QFile fl( fn );
    if ( !fl.open( IO_WriteOnly|IO_Raw ) ) {
	qDebug( "open failed: %s", fn.latin1() );
	return FALSE;
    }

    QCString cstr = text.utf8();
    int total_written;
    total_written = fl.writeBlock( cstr.data(), cstr.length() );
    fl.close();
    if ( total_written != int(cstr.length()) || !f.writeLink() ) {
	QFile::remove( fn );
	return FALSE;
    }
    // okay now rename the file...
    if ( ::rename( fn.latin1(), f.file().latin1() ) < 0 ) {
	qWarning( "problem renaming file %s to %s, errno: %d", fn.latin1(),
		  f.file().latin1(), errno );
	// remove the tmp file, otherwise, it will just lay around...
	QFile::remove( fn.latin1() );
    }
    return TRUE;
}


/*!
  Loads \a text from the document specified by \a f.

  The text is required to be in UTF8 format.

  Returns whether the operation succeeded.
*/
bool FileManager::loadFile( const DocLnk &f, QString &text )
{
    QString fn = f.file();
    QFile fl( fn );
    if ( !fl.open( IO_ReadOnly ) )
	return FALSE;
    QTextStream ts( &fl );
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
    // The below should work, but doesn't in Qt 2.3.0
    ts.setCodec( QTextCodec::codecForMib( 106 ) );
#else
    ts.setEncoding( QTextStream::UnicodeUTF8 );
#endif
    text = ts.read();
    fl.close();
    return TRUE;
}


/*!
  Loads \a ba from the document specified by \a f.

  Returns whether the operation succeeded.
*/
bool FileManager::loadFile( const DocLnk &f, QByteArray &ba )
{
    QString fn = f.file();
    QFile fl( fn );
    if ( !fl.open( IO_ReadOnly ) )
	return FALSE;
    ba.resize( fl.size() );
    if ( fl.size() > 0 )
	fl.readBlock( ba.data(), fl.size() );
    fl.close();
    return TRUE;
}

/*!
  Copies the document specified by \a src to the document specified
  by \a dest.

  Returns whether the operation succeeded.
*/
bool FileManager::copyFile( const AppLnk &src, const AppLnk &dest )
{
    QFile sf( src.file() );
    if ( !sf.open( IO_ReadOnly ) )
	return FALSE;

    QString fn = dest.file() + ".new";
    ensurePathExists( fn );
    QFile df( fn );
    if ( !df.open( IO_WriteOnly|IO_Raw ) )
	return FALSE;

    const int bufsize = 16384;
    char buffer[bufsize];
    bool ok = TRUE;
    int bytesRead = 0;
    while ( ok && !sf.atEnd() ) {
	bytesRead = sf.readBlock( buffer, bufsize );
	if ( bytesRead < 0 )
	    ok = FALSE;
	while ( ok && bytesRead > 0 ) {
	    int bytesWritten = df.writeBlock( buffer, bytesRead );
	    if ( bytesWritten < 0 )
		ok = FALSE;
	    else
		bytesRead -= bytesWritten;
	}
    }
    
    if ( ok )
	ok = dest.writeLink();
    
    if ( ok ) {
	// okay now rename the file...
	if ( ::rename( fn.latin1(), dest.file().latin1() ) < 0 ) {
	    qWarning( "problem renaming file %s to %s, errno: %d", fn.latin1(),
		      dest.file().latin1(), errno );
	    // remove the tmp file, otherwise, it will just lay around...
	    QFile::remove( fn.latin1() );
	}
    } else {
	QFile::remove( fn.latin1() );
    }

    return ok;
}

/*!
  Opens the document specified by \a f as a readable QIODevice.
  The caller must delete the return value.

  Returns 0 if the operation fails.
*/
QIODevice* FileManager::openFile( const DocLnk& f )
{
    QString fn = f.file();
    QFile* fl = new QFile( fn );
    if ( !fl->open( IO_ReadOnly ) ) {
	delete fl;
        fl = 0;
    }
    return fl;
}

/*!
  Opens the document specified by \a f as a writable QIODevice.
  The caller must delete the return value.

  Returns 0 if the operation fails.
*/
QIODevice* FileManager::saveFile( const DocLnk& f )
{
    QString fn = f.file();
    ensurePathExists( fn );
    QFile* fl = new QFile( fn );
    if ( fl->open( IO_WriteOnly ) ) {
	f.writeLink();
    } else {
	delete fl;
        fl = 0;
    }
    return fl;
}

/*!
  Returns whether the document specified by \a f current exists
  as a file on disk.
*/
bool FileManager::exists( const DocLnk &f )
{
    return QFile::exists(f.file());
}


/*!
  Ensures that the path \a fn exists, by creating required directories.
  Returns TRUE if successful.
*/
bool FileManager::ensurePathExists( const QString &fn )
{
    QFileInfo fi(fn);
    fi.setFile( fi.dirPath(TRUE) );
    if ( !fi.exists() ) {
	if ( system(("mkdir -p "+fi.filePath())) )
	    return FALSE;
    }

    return TRUE;
}
