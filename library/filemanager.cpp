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
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#ifdef Q_OS_MACX
// MacOS X does not have sendfile.. :(
// But maybe in the future.. !?
#  ifdef SENDFILE
#    include <sys/types.h>
#    include <sys/socket.h>
#  endif
#else
#  include <sys/sendfile.h>
#endif /* Q_OS_MACX */
#include <fcntl.h>

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
    if ( !fl.open( IO_WriteOnly|IO_Raw ) ) {
       qWarning("open failed");
       return FALSE;
    }
    int total_written = fl.writeBlock( data );
    fl.close();
    if ( total_written != int(data.size()) || !f.writeLink() ) {
  QFile::remove( fn );
  return FALSE;
    }
       qDebug("total written %d out of %d", total_written, data.size());
    // else rename the file...
    if ( !renameFile( fn.latin1(), f.file().latin1() ) ) {
  qWarning( "problem renaming file %s to %s, errno: %d", fn.latin1(),
      f.file().latin1(), errno );
  // remove the file...
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
       qWarning("open failed");
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
    // okay now rename the file..
    if ( !renameFile( fn.latin1(), f.file().latin1() ) ) {
       qWarning( "problem renaming file %s to %s, errno: %d", fn.latin1(),
                 f.file().latin1(), errno );

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
  if ( !renameFile( fn.latin1(), dest.file().latin1() )  ) {
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

bool FileManager::copyFile( const QString & src, const QString & dest ) {
   bool success = true;
   struct stat status;
   int read_fd=0;
   int write_fd=0;
   struct stat stat_buf;
   off_t offset = 0;
   QFile srcFile(src);
   QFile destFile(dest);

   if(!srcFile.open( IO_ReadOnly|IO_Raw)) {
         return success = false;
   }
   read_fd = srcFile.handle();
   if(read_fd != -1) {
      fstat (read_fd, &stat_buf);
      if( !destFile.open( IO_WriteOnly|IO_Raw ) )
            return success = false;
      write_fd = destFile.handle(); 
      if(write_fd != -1) {
         int err=0;
         QString msg;
#ifdef Q_OS_MACX
#ifdef SENDMAIL
	 /* FreeBSD does support a different kind of 
	  * sendfile. (eilers)
	  * I took this from Very Secure FTPd
	  * Licence: GPL
	  * Author: Chris Evans
	  * sysdeputil.c
	  */
          /* XXX - start_pos will truncate on 32-bit machines - can we
           * say "start from current pos"?
           */
          off_t written = 0;
	  int retval = 0;
          retval = sendfile(read_fd, write_fd, offset, stat_buf.st_size, NULL,
                            &written, 0);
          /* Translate to Linux-like retval */
          if (written > 0)
          {
            err = (int) written;
          }
#else /* SENDMAIL */
	  err == -1;
	  msg = "FAILURE: Using unsupported function \"sendfile()\" Need Workaround !!";
	  success = false;
#         warning "Need workaround for sendfile!!(eilers)"
#endif  /* SENDMAIL */

#else
	  err = sendfile(write_fd, read_fd, &offset, stat_buf.st_size);
	  if( err == -1) {
		  switch(err) {
		  case EBADF : msg = "The input file was not opened for reading or the output file was not opened for writing. ";
		  case EINVAL: msg = "Descriptor is not valid or locked. ";
		  case ENOMEM: msg = "Insufficient memory to read from in_fd.";
		  case EIO: msg = "Unspecified error while reading from in_fd.";
		  };
		  success = false;
	  }
#endif /* Q_OS_MACX */
	  if( !success )
		  qWarning( msg );
     } else {
         qWarning("open write failed %s, %s",src.latin1(), dest.latin1());
         success = false;
      }
   } else {
      qWarning("open read failed %s, %s",src.latin1(), dest.latin1());
      success = false;
   }
   srcFile.close();
   destFile.close();
    // Set file permissions
  if( stat( (const char *) src, &status ) == 0 ) {
      chmod( (const char *) dest, status.st_mode );
    }

  return success;
}


bool FileManager::renameFile( const QString & src, const QString & dest ) {
   if(copyFile( src, dest )) {
      if(QFile::remove(src) ) {
       return true;
      }
   }
    return false;  
}


=======
bool FileManager::copyFile( const QString & src, const QString & dest ) {
   bool success = true;
   struct stat status;
   int read_fd=0;
   int write_fd=0;
   struct stat stat_buf;
   off_t offset = 0;
   QFile srcFile(src);
   QFile destFile(dest);

   if(!srcFile.open( IO_ReadOnly|IO_Raw)) {
         return success = false;
   }
   read_fd = srcFile.handle();
   if(read_fd != -1) {
      fstat (read_fd, &stat_buf);
      if( !destFile.open( IO_WriteOnly|IO_Raw ) )
            return success = false;
      write_fd = destFile.handle();
      if(write_fd != -1) {
         int err=0;
         QString msg;
         err = sendfile(write_fd, read_fd, &offset, stat_buf.st_size);
         if( err == -1) {
            switch(err) {
            case EBADF : msg = "The input file was not opened for reading or the output file was not opened for writing. ";
            case EINVAL: msg = "Descriptor is not valid or locked. ";
            case ENOMEM: msg = "Insufficient memory to read from in_fd.";
            case EIO: msg = "Unspecified error while reading from in_fd.";
            };
            success = false;
         }
      } else {
         qWarning("open write failed %s, %s",src.latin1(), dest.latin1());
         success = false;
      }
   } else {
      qWarning("open read failed %s, %s",src.latin1(), dest.latin1());
      success = false;
   }
   srcFile.close();
   destFile.close();
    // Set file permissions
  if( stat( (const char *) src, &status ) == 0 ) {
      chmod( (const char *) dest, status.st_mode );
    }

  return success;
}


bool FileManager::renameFile( const QString & src, const QString & dest ) {
   if(copyFile( src, dest )) {
      if(QFile::remove(src) ) {
       return true;
      }
   }
    return false;
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
