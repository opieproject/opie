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

#ifndef ofileview_h
#define ofileview_h

#include <qobject.h>
#include <qwidget.h>
#include <qpopupmenu.h>

class QFileInfo;
class QDir;
class DocLnk;

/**
 * A OFileView is a specialised View for the
 * OFileSelector
 * With a View you can chage the user visible
 * representation of a OFileLister
 * OFileView is just a basic interface which helps you to
 * write new views
 */
class OFileSelector;
class OFileView  {
public:
    OFileView( OFileSelector* );
    OFileView();
    virtual ~OFileView();

    virtual void clear() = 0;
    virtual void addFile(const QPixmap&,
                         const QString &mine,
                         QFileInfo *info,
                         bool isSymlink = FALSE ) = 0;

    virtual void addFile(const QPixmap&,
                         const QString& mine, const QString& dir,
                         const QString& file, bool = FALSE ) = 0;

    virtual void addDir (const QPixmap&,
                         const QString &mine,
                         QFileInfo *info,
                         bool isSymlink = FALSE ) = 0;
    virtual void addDir (const QPixmap&,
                         const QString& mine, const QString& dir,
                         const QString& file, bool = FALSE) = 0;

    virtual void addSymlink(const QPixmap&,
                            const QString &mime,
                            QFileInfo *info,
                            bool isSymlink = FALSE ) = 0;

    virtual void addSymlink(const QPixmap&,
                            const QString& mine,
                            const QString& path,
                            const QString& file,
                            bool isSymlink = FALSE ) = 0;

    virtual void cd(const QString &path ) = 0;
    virtual QWidget* widget() = 0;

    virtual QString selectedName()const = 0;
    virtual QStringList selectedNames()const = 0;
    virtual QString selectedPath()const = 0;
    virtual QStringList selectedPaths()const = 0;
    virtual int fileCount() = 0;
    virtual void sort() =0;

/*signals:*/
protected:

  void fileSelected(const QString &);
  void fileSelected(const DocLnk & );
  void contextMenu();
  void changedDir(const QString &);
  void changedDir(const QDir & );

    /* updates the file name line of the FileSelector */
  void updateLine( const QString& );
  OFileSelector* selector()const;

private:
    OFileSelector* m_sel;
};

class OFileViewFactory {
 public:
  OFileViewFactory() {} ;
  virtual ~OFileViewFactory() = 0;

  OFileView* newView(QWidget *parent, const char *name );
  QString name()const;
};


#endif
