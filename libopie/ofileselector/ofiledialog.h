/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 zecke <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#ifndef OpieFileDialog_h
#define OpieFileDialog_h

#include <qdialog.h>

//#include <opie/ofileselector.h>
#include "ofileselector.h"

class OFileDialog : public QDialog {
  Q_OBJECT
 public:
  OFileDialog(const QString &caption,
	      QWidget *, int mode, int selector,
	      const QString &dirName,
	      const QString &fileName = QString::null,
	      const MimeTypes &mimetypes = MimeTypes()  );
  QString mimetype() const;
  QString fileName() const;
  DocLnk selectedDocument()const;

  // static methods
  static QString getOpenFileName(int selector,
				 const QString& startDir = QString::null,
				 const QString &fileName = QString::null,
				 const MimeTypes& mime = MimeTypes(),
				 QWidget *wid = 0,
				 const QString &caption = QString::null );

  static QString getSaveFileName(int selector,
				 const QString& startDir = QString::null,
				 const QString& fileName = QString::null,
				 const MimeTypes& mimefilter = MimeTypes(),
				 QWidget *wid = 0,
				 const QString &caption = QString::null );

  //let's OFileSelector catch up first
  //static QString getExistingDirectory(const QString& startDir = QString::null,
  // QWidget *parent = 0,
  //			      const QString& caption = QString::null );
 private:
  class OFileDialogPrivate;
  OFileDialogPrivate *d;
  OFileSelector *file;

 private slots:
  void slotFileSelected( const QString & );
  void slotDirSelected(const QString & );
};
#endif
