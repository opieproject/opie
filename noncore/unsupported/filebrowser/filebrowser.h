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
#ifndef FILEBROWSER_H
#define FILEBROWSER_H


#include <qlistview.h>
#include <qmainwindow.h>
#include <qfileinfo.h>
#include <qaction.h>
#include <qtimer.h>
#include <qstringlist.h>
class QLabel;
class InlineEdit;

class FileItem : public QListViewItem
{
public:
    FileItem( QListView * parent, const QFileInfo & fi );

    QString key( int column, bool ascending = TRUE ) const;
    QString getFilePath(){ return fileInfo.filePath(); }
    QString getFileName(){ return fileInfo.fileName(); }
    bool    isDir(){ return fileInfo.isDir(); }
    bool    isExecutable(){ return fileInfo.isExecutable(); }
    bool    isLib();
    int     launch();
    bool    rename( const QString & name );
private:
    QString sizeString( unsigned int size );
    QFileInfo fileInfo;
  QPixmap FileItem::drawThumbnail(const QFileInfo &file);
};


class FileView : public QListView
{
    Q_OBJECT

public:
    FileView( const QString & dir, QWidget * parent = 0,
        const char * name = 0, 
        bool hidden = FALSE,
        bool symlinks = FALSE,
        bool thumbnails = FALSE );

    void    setDir( const QString & dir );
    QString cd(){ return currentDir; }
    QStringList history() const { return dirHistory; }
    bool showingHidden;

  void setShowHidden(bool hidden);
  void setShowSymlinks(bool symlinks);
  void setShowThumbnails(bool thumbnails);
  bool getShowThumbnails () const { return showThumbnails; }
  void setMenuKeepsOpen(bool keepOpen);
  
public slots:
    void updateDir();
    void parentDir();
    void lastDir();

    void rename();
    void copy();
    void paste();
    void del();
    void cut();
    void newFolder();
    void viewAsText();
    void chPerm();
protected:
    void generateDir( const QString & dir );
    void resizeEvent( QResizeEvent* );
    void contentsMousePressEvent( QMouseEvent * e );
    void contentsMouseReleaseEvent( QMouseEvent * e );

protected slots:
    void itemClicked( QListViewItem * i );
    void itemDblClicked( QListViewItem * i );
    void showFileMenu();
    void cancelMenuTimer();
    void selectAll(){ QListView::selectAll( TRUE ); }
    void deselectAll(){ QListView::selectAll( FALSE ); }
    void addToDocuments();
    void run();
    void endRenaming();
private:
    QString     currentDir;
    QStringList dirHistory, flist;
    QTimer      menuTimer;
    InlineEdit * le;
    FileItem   * itemToRename;
    bool         selected;
    bool    showHidden;
    bool    showSymlinks;
  bool    showThumbnails;
  bool menuKeepsOpen; 

    bool copyFile( const QString & dest, const QString & src );

signals:
    void dirChanged();
    void textViewActivated( QWidget * w );
    void textViewDeactivated();
};

class FileBrowser : public QMainWindow
{
    Q_OBJECT

public:
    FileBrowser( QWidget * parent = 0,
         const char * name = 0, WFlags f = 0 );
    FileBrowser( const QString & dir, QWidget * parent = 0,
         const char * name = 0, WFlags f = 0 );
public slots:
        void changeCaption(const QString &);
private:
    void init(const QString & dir);
    QString      fileToCopy;
    QPopupMenu * dirMenu, * sortMenu, *viewMenu;
    FileView   * fileView;
    QAction    * pasteAction;
    QAction *lastAction;
    QAction *upAction;

    bool         copyFile( const QString & dest, const QString & src );

private slots:
    void pcmciaMessage( const QCString &msg, const QByteArray &);

    void sortName();
    void sortDate();
    void sortSize();
    void sortType();
    void updateSorting();
    void updateShowHidden();
    void updateShowSymlinks();
    void updateShowThumbnails();
    void updateDirMenu();
    void dirSelected( int id );
    void cleanUp();
    
};

#endif
