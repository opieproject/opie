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

/* QT */
#include <qdir.h>
#include <qfileinfo.h>
#include <qtextstream.h>

/* STD */
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

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
    QString fileName = f.file() + ".new";
    ensurePathExists( fileName );
    QFile file( fileName );

    //write data in temporary .new file
    if ( !file.open( IO_WriteOnly|IO_Raw ) )
    {
        qWarning( "open failed" );
        return FALSE;
    }
    int total_written = file.writeBlock( data );
    file.close();
    //check if every was written
    if ( total_written != int( data.size() ) || !f.writeLink() )
    {
        QFile::remove( fileName );
        return FALSE;
    }
    qDebug( "total written %d out of %d", total_written, data.size() );

    //rename temporary .new file in original filenam
    if ( !renameFile( fileName,  f.file() ) )
        QFile::remove( fileName);
    return TRUE;
}

/*!
  Saves \a text as the document specified by \a f.

  The text is saved in UTF8 format.

  Returns whether the operation succeeded.
*/
bool FileManager::saveFile( const DocLnk &f, const QString &text )
{
    QString fileName = f.file() + ".new";
    ensurePathExists( fileName );
    QFile file( fileName );

    //write data in temporary .new file
    if ( !file.open( IO_WriteOnly|IO_Raw ) )
    {
        qWarning( "open failed" );
        return FALSE;
    }

    QCString cstr = text.utf8();
    int total_written;
    total_written = file.writeBlock( cstr.data(), cstr.length() );
    file.close();
    if ( total_written != int( cstr.length()) || !f.writeLink() )
    {
        QFile::remove( fileName );
        return FALSE;
    }

    // okay now rename the file..
    if ( !renameFile( fileName,  f.file() ) )
        QFile::remove( fileName);
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
    QFile srcFile( src.file() );
    if ( !srcFile.open( IO_ReadOnly ) )
        return FALSE;

    QString fileName = dest.file() + ".new";

    ensurePathExists( fileName );

    bool ok = TRUE;
    ok = copyFile( src.file(), fileName );

    if ( ok )
        ok = dest.writeLink();

    if ( ok )
    {
        // okay now rename the file...
        if ( !renameFile( fileName, dest.file() )  )
            // remove the tmp file, otherwise, it will just lay around...
            QFile::remove( fileName );
    }
    else
    {
        QFile::remove( fileName );
    }
    return ok;
}

bool FileManager::copyFile( const QString & src, const QString & dest )
{
    //open read file
    QFile srcFile( src );
    if( !srcFile.open( IO_ReadOnly|IO_Raw) )
    {
        qWarning( "open read failed %s, %s", src.latin1(), dest.latin1() );
        return FALSE;
    }

    //open write file
    QFile destFile( dest );
    if( !destFile.open( IO_WriteOnly|IO_Raw ) )
    {
        qWarning( "open write failed %s, %s", src.latin1(), dest.latin1() );
        srcFile.close();
        return FALSE;
    }

    //copy content
    const int bufsize = 16384;
    char buffer[bufsize];
    bool ok = TRUE;
    int bytesRead = 0;
    while ( ok && !srcFile.atEnd() )
    {
        bytesRead = srcFile.readBlock( buffer, bufsize );
        if ( bytesRead < 0 )
            ok = FALSE;
        while ( ok && bytesRead > 0 )
        {
            int bytesWritten = destFile.writeBlock( buffer, bytesRead );
            if ( bytesWritten < 0 )
                ok = FALSE;
            else
                bytesRead -= bytesWritten;
        }
    }
    srcFile.close();
    destFile.close();
    // Set file permissions
    struct stat status;
    if( stat( QFile::encodeName( src ), &status ) == 0 )
    {
        chmod( QFile::encodeName( dest ), status.st_mode );
    }
    return ok;
}


bool FileManager::renameFile( const QString & src, const QString & dest )
{
    if( rename( QFile::encodeName( src ), QFile::encodeName( dest ) ) == -1);
    {
        if ( errno != 2 && errno != 11 ) //ignore ENOENT and EAGAIN - bug in system?
        {
            qWarning( "problem renaming file %s to %s, errno: %d", src.latin1(), dest.latin1(), errno );
            return false;
        }
    }
    return true;
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
    if ( !fl->open( IO_ReadOnly ) )
    {
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
    if ( fl->open( IO_WriteOnly ) )
    {
        f.writeLink();
    }
    else
    {
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
  Ensures that the path \a fileName exists, by creating required directories.
  Returns TRUE if successful.
*/
bool FileManager::ensurePathExists( const QString &fileName )
{
    QDir directory = QFileInfo( fileName ).dir();
    if ( !directory.exists() )
    {
        if ( !directory.mkdir( directory.absPath() ) )
            return FALSE;
    }
    return TRUE;
}
