/***************************************************************************
   AdvancedFm.cpp
  -------------------
  ** Created: Sat Mar 9 23:33:09 2002
    copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/
#define DEVELOPERS_VERSION
#include "advancedfm.h"

//  #include <opie/ofileselector.h>
//  #include <opie/ofiledialog.h>

#include <qpe/filemanager.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>
#include <qpe/mimetype.h>
#include <qpe/applnk.h>
#include <qpe/ir.h>
#include <qpe/resource.h>

#include <qtabwidget.h>
#include <qtextstream.h>
#include <qpushbutton.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfile.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qlistview.h>
#include <qmainwindow.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qspinbox.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qregexp.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <mntent.h>
#include <string.h>
#include <errno.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <sys/utsname.h>

AdvancedFm::AdvancedFm( )
        : QMainWindow( ) {
    init();
    initConnections();
    populateLocalView();
    populateRemoteView();
    currentPathCombo->setFocus();
}

AdvancedFm::~AdvancedFm() {
}

void AdvancedFm::cleanUp() {
  QString sfile=QDir::homeDirPath();
  if(sfile.right(1) != "/")
    sfile+="/._temp";
  else
    sfile+="._temp";
  QFile file( sfile);
  if(file.exists())
    file.remove();
}

void AdvancedFm::tabChanged(QWidget *) {
  if (TabWidget->currentPageIndex() == 0) {
    currentPathCombo->lineEdit()->setText( currentDir.canonicalPath());
    viewMenu->setItemChecked(viewMenu->idAt(0),TRUE);
    viewMenu->setItemChecked(viewMenu->idAt(1),FALSE);
  QString fs= getFileSystemType((const QString &) currentDir.canonicalPath());
  setCaption("AdvancedFm :: "+fs+" :: "
             +checkDiskSpace((const QString &) currentDir.canonicalPath())+" kB free" );
    
  }
  if (TabWidget->currentPageIndex() == 1) {
    currentPathCombo->lineEdit()->setText( currentRemoteDir.canonicalPath());
    viewMenu->setItemChecked(viewMenu->idAt(1),TRUE);
    viewMenu->setItemChecked(viewMenu->idAt(0),FALSE);
  QString fs= getFileSystemType((const QString &) currentRemoteDir.canonicalPath());
  setCaption("AdvancedFm :: "+fs+" :: "
             +checkDiskSpace((const QString &) currentRemoteDir.canonicalPath())+" kB free" );
  }
}


void AdvancedFm::populateLocalView() {
  QPixmap pm;
  Local_View->clear();
  currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
  currentDir.setMatchAllDirs(TRUE);
  currentDir.setNameFilter(filterStr);
  QString fileL, fileS, fileDate;
  QString fs= getFileSystemType((const QString &) currentDir.canonicalPath());
  setCaption("AdvancedFm :: "+fs+" :: "
             +checkDiskSpace((const QString &) currentDir.canonicalPath())+" kB free" );
  bool isDir=FALSE;
  const QFileInfoList *list = currentDir.entryInfoList( /*QDir::All*/ /*, QDir::SortByMask*/);
  QFileInfoListIterator it(*list);
  QFileInfo *fi;
  while ( (fi=it.current()) ) {
    if (fi->isSymLink() ) {
      QString symLink=fi->readLink();
      QFileInfo sym( symLink);
      fileS.sprintf( "%10i", sym.size() );
      fileL.sprintf( "%s ->  %s",  fi->fileName().data(),sym.filePath().data() );
      fileDate = sym.lastModified().toString();
    } else {
      fileS.sprintf( "%10i", fi->size() );
      fileL.sprintf( "%s",fi->fileName().data() );
      fileDate= fi->lastModified().toString();
      if( QDir(QDir::cleanDirPath( currentDir.canonicalPath()+"/"+fileL)).exists() ) {
        fileL+="/";
        isDir=TRUE;
      }
    }
    QFileInfo fileInfo(  currentDir.canonicalPath()+"/"+fileL);
    if(fileL !="./" && fi->exists()) {
      item= new QListViewItem( Local_View, fileL, fileS , fileDate);

      if(isDir || fileL.find("/",0,TRUE) != -1) {

        if( !QDir( fi->filePath() ).isReadable()) //is directory
          pm = Resource::loadPixmap( "lockedfolder" );
        else
          pm= Resource::loadPixmap( "folder" );
      } else if ( fs == "vfat" && fileInfo.filePath().contains("/bin") ) {
        pm = Resource::loadPixmap( "exec");
      } else if( (fileInfo.permission( QFileInfo::ExeUser)
                  | fileInfo.permission( QFileInfo::ExeGroup)
                  | fileInfo.permission( QFileInfo::ExeOther)) && fs != "vfat" ) {
        pm = Resource::loadPixmap( "exec");
      } else if( !fi->isReadable() ) {
        pm = Resource::loadPixmap( "locked" );
      } else { //everything else goes by mimetype
        MimeType mt(fi->filePath());
        pm=mt.pixmap(); //sets the correct pixmap for mimetype
        if(pm.isNull())
          pm =  Resource::loadPixmap( "UnknownDocument-14" );
      }
      item->setPixmap( 0,pm);
      if(  fi->isSymLink() && fileL.find("->",0,TRUE) != -1) {
        // overlay link image
        pm= Resource::loadPixmap( "folder" );
        QPixmap lnk = Resource::loadPixmap( "opie/symlink" );
        QPainter painter( &pm );
        painter.drawPixmap( pm.width()-lnk.width(), pm.height()-lnk.height(), lnk );
        pm.setMask( pm.createHeuristicMask( FALSE ) );
        item->setPixmap( 0, pm);
      }
    }
    isDir=FALSE;
    ++it;
  }

  if(currentDir.canonicalPath().find("dev",0,TRUE) != -1) {
    struct stat buf;
    dev_t devT;
    DIR *dir;
    struct dirent *mydirent;
    if((dir = opendir( currentDir.canonicalPath().latin1())) != NULL)
      while ((mydirent = readdir(dir)) != NULL) {
        lstat( mydirent->d_name, &buf);
        qDebug(mydirent->d_name);
        fileL.sprintf("%s", mydirent->d_name);
        devT = buf.st_dev;
        fileS.sprintf("%d, %d", (int) ( devT >>8) &0xFF, (int)devT &0xFF);
        fileDate.sprintf("%s", ctime( &buf.st_mtime));
        if( fileL.find(".") == -1 ){
          item= new QListViewItem( Local_View, fileL, fileS, fileDate);
          pm =  Resource::loadPixmap( "UnknownDocument-14" );
          item->setPixmap( 0,pm);
        }
      }

    closedir(dir);
  }

  Local_View->setSorting( 3,FALSE);
  fillCombo( (const QString &) currentDir.canonicalPath());
}


void AdvancedFm::populateRemoteView() {
  QPixmap pm;
  Remote_View->clear();
  currentRemoteDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
  currentRemoteDir.setMatchAllDirs(TRUE);
  currentRemoteDir.setNameFilter(filterStr);
  QString fileL, fileS, fileDate;

  QString fs= getFileSystemType((const QString &) currentRemoteDir.canonicalPath());
  setCaption("AdvancedFm :: "+fs+" :: "
             +checkDiskSpace((const QString &) currentRemoteDir.canonicalPath())+" kB free" );
  bool isDir=FALSE;
  const QFileInfoList *list = currentRemoteDir.entryInfoList( /*QDir::All*/ /*, QDir::SortByMask*/);
  QFileInfoListIterator it(*list);
  QFileInfo *fi;
  while ( (fi=it.current()) ) {
    if (fi->isSymLink() ){
      QString symLink=fi->readLink();
      //         qDebug("Symlink detected "+symLink);
      QFileInfo sym( symLink);
      fileS.sprintf( "%10i", sym.size() );
      fileL.sprintf( "%s ->  %s",  fi->fileName().data(),sym.filePath().data() );
      fileDate = sym.lastModified().toString();
    } else {
      //        qDebug("Not a dir: "+currentDir.canonicalPath()+fileL);
      fileS.sprintf( "%10i", fi->size() );
      fileL.sprintf( "%s",fi->fileName().data() );
      fileDate= fi->lastModified().toString();
      if( QDir(QDir::cleanDirPath( currentRemoteDir.canonicalPath()+"/"+fileL)).exists() ) {
        fileL+="/";
        isDir=TRUE;
        //     qDebug( fileL);
      }
    }
    QFileInfo fileInfo(  currentRemoteDir.canonicalPath()+"/"+fileL);
    if(fileL !="./" && fi->exists()) {
      item= new QListViewItem( Remote_View, fileL, fileS, fileDate);
      QPixmap pm;

      if(isDir || fileL.find("/",0,TRUE) != -1) {
        if( !QDir( fi->filePath() ).isReadable())
          pm = Resource::loadPixmap( "lockedfolder" );
        else
          pm= Resource::loadPixmap( "folder" );
      } else if ( fs == "vfat" && fileInfo.filePath().contains("/bin") ) {
        pm = Resource::loadPixmap( "exec");
      } else if( (fileInfo.permission( QFileInfo::ExeUser)
                  | fileInfo.permission( QFileInfo::ExeGroup)
                  | fileInfo.permission( QFileInfo::ExeOther)) && fs != "vfat" ) {
        pm = Resource::loadPixmap( "exec");
      } else if( !fi->isReadable() ) {
        pm = Resource::loadPixmap( "locked" );
      } else {
        MimeType mt(fi->filePath());
        pm=mt.pixmap(); //sets the correct pixmap for mimetype
        if(pm.isNull())
          pm =  Resource::loadPixmap( "UnknownDocument-14" );
      }
      if(  fi->isSymLink() && fileL.find("->",0,TRUE) != -1) {
        // overlay link image
        pm= Resource::loadPixmap( "folder" );
        QPixmap lnk = Resource::loadPixmap( "opie/symlink" );
        QPainter painter( &pm );
        painter.drawPixmap( pm.width()-lnk.width(), pm.height()-lnk.height(), lnk );
        pm.setMask( pm.createHeuristicMask( FALSE ) );
      }
      item->setPixmap( 0, pm);
    }
    isDir=FALSE;
    ++it;
  }

  if(currentRemoteDir.canonicalPath().find("dev",0,TRUE) != -1) {
    struct stat buf;
    DIR *dir;
    struct dirent *mydirent;
    if((dir = opendir( currentRemoteDir.canonicalPath().latin1())) != NULL)
      while ((mydirent = readdir(dir)) != NULL) {
        lstat( mydirent->d_name, &buf);
        qDebug(mydirent->d_name);
        fileL.sprintf("%s", mydirent->d_name);
        fileS.sprintf("%d,%d", (int) (buf.st_dev>>8)&0xFF, (int) buf.st_dev &0xFF);
        fileDate.sprintf("%s", ctime( &buf.st_mtime));
        if( fileL.find(".") == -1 ){
          item= new QListViewItem( Remote_View, fileL, fileS, fileDate);
          pm =  Resource::loadPixmap( "UnknownDocument-14" );
          item->setPixmap( 0,pm);
        }
      }

    closedir(dir);
  }

  Remote_View->setSorting( 3,FALSE);
  fillCombo( (const QString &) currentRemoteDir.canonicalPath() );
}

void AdvancedFm::localListClicked(QListViewItem *selectedItem) {
  if(selectedItem) {
    QString strItem=selectedItem->text(0);
    QString strSize=selectedItem->text(1);
    strSize=strSize.stripWhiteSpace();
    if(strItem.find("@",0,TRUE) !=-1 || strItem.find("->",0,TRUE) !=-1 ) { //if symlink
      QString strItem2 = strItem.right( (strItem.length() - strItem.find("->",0,TRUE)) - 4);
      if(QDir(strItem2).exists() ) {
        currentDir.cd(strItem2, TRUE);
        populateLocalView();

      }
    } else { // not a symlink
      if(strItem.find(". .",0,TRUE) && strItem.find("/",0,TRUE)!=-1 ) {
        if(QDir(QDir::cleanDirPath(currentDir.canonicalPath()+"/"+strItem)).exists() ) {
          strItem=QDir::cleanDirPath(currentDir.canonicalPath()+"/"+strItem);
          currentDir.cd(strItem,FALSE);
          populateLocalView();
    Local_View->ensureItemVisible(Local_View->firstChild());
          
        } else {
          currentDir.cdUp();
          populateLocalView();
    Local_View->ensureItemVisible(Local_View->firstChild());
        }
        if(QDir(strItem).exists()){
          currentDir.cd(strItem, TRUE);
    Local_View->ensureItemVisible(Local_View->firstChild());
          populateLocalView();
        }
      } else {
        strItem=QDir::cleanDirPath(currentDir.canonicalPath()+"/"+strItem);
        if( QFile::exists(strItem ) ) {
          //                     qDebug("clicked item "+strItem);
          //                     DocLnk doc( strItem, FALSE );
          //                     doc.execute();
          //    Local_View->clearSelection();
        }
      } //end not symlink
      chdir(strItem.latin1());
    }
    
  }
}

void AdvancedFm::remoteListClicked(QListViewItem *selectedItem) {
    
  if(selectedItem) {
    QString strItem=selectedItem->text(0);
    QString strSize=selectedItem->text(1);
    strSize=strSize.stripWhiteSpace();
    if(strItem.find("@",0,TRUE) !=-1 || strItem.find("->",0,TRUE) !=-1 ) { //if symlink
      QString strItem2 = strItem.right( (strItem.length() - strItem.find("->",0,TRUE)) - 4);
        currentRemoteDir.cd(strItem2, TRUE);
        populateRemoteView();
    } else { // not a symlink
      if(strItem.find(". .",0,TRUE) && strItem.find("/",0,TRUE)!=-1 ) {
        if(QDir(QDir::cleanDirPath( currentRemoteDir.canonicalPath()+"/"+strItem)).exists() ) {
          strItem=QDir::cleanDirPath( currentRemoteDir.canonicalPath()+"/"+strItem);
          currentRemoteDir.cd(strItem,FALSE);
          populateRemoteView();
    Remote_View->ensureItemVisible(Remote_View->firstChild());
        } else {
          currentRemoteDir.cdUp();
          populateRemoteView();
    Remote_View->ensureItemVisible(Remote_View->firstChild());
        }
        if(QDir(strItem).exists()){
          currentRemoteDir.cd(strItem, TRUE);
          populateRemoteView();
    Remote_View->ensureItemVisible(Remote_View->firstChild());
        }
      } else {
        strItem=QDir::cleanDirPath( currentRemoteDir.canonicalPath()+"/"+strItem);
        if( QFile::exists(strItem ) ) {
          //                     qDebug("clicked item "+strItem);
          //                     DocLnk doc( strItem, FALSE );
          //                     doc.execute();
          //    Remote_View->clearSelection();
        }
      } //end not symlink
      chdir(strItem.latin1());
    }
  }
}


void AdvancedFm::localListPressed( int mouse, QListViewItem *, const QPoint& , int ) {
//  qDebug("list pressed");
  switch (mouse) {
  case 1:
    break;
  case 2:
    menuTimer.start( 500, TRUE );
    qDebug("Start menu timer\n");
    break;
  };
}

void AdvancedFm::remoteListPressed( int mouse, QListViewItem*, const QPoint&, int ) {

  switch (mouse) {
  case 1:
    break;
  case 2:
    menuTimer.start( 500, TRUE );
    qDebug("Start menu timer");
    break;
  };
}


void AdvancedFm::switchToLocalTab() {
  TabWidget->setCurrentPage(0);
  Local_View->setFocus();
}

void AdvancedFm::switchToRemoteTab() {
  TabWidget->setCurrentPage(1);
  Remote_View->setFocus();
}

void AdvancedFm::readConfig() {
  Config cfg("AdvancedFm");
}

void AdvancedFm::writeConfig() {
  Config cfg("AdvancedFm");
}

void  AdvancedFm::currentPathComboChanged() {
  if (TabWidget->currentPageIndex() == 0) {
    if(QDir( currentPathCombo->lineEdit()->text()).exists()) {
      currentDir.setPath( currentPathCombo->lineEdit()->text() );
      populateLocalView();
    } else {
      QMessageBox::message(tr("Note"),tr("That directory does not exist"));
    }
  }
  if (TabWidget->currentPageIndex() == 0) {
    if(QDir( currentPathCombo->lineEdit()->text()).exists()) {
      currentRemoteDir.setPath( currentPathCombo->lineEdit()->text() );
      populateRemoteView();
    } else {
      QMessageBox::message(tr("Note"),tr("That directory does not exist"));
    }
  }
}

void  AdvancedFm::fillCombo(const QString &currentPath) {

  if (TabWidget->currentPageIndex() == 0) {
    currentPathCombo->lineEdit()->setText( currentPath);
    if( localDirPathStringList.grep(currentPath,TRUE).isEmpty() ) {
      currentPathCombo->clear();
      localDirPathStringList.prepend( currentPath );
      currentPathCombo->insertStringList( localDirPathStringList,-1);
    }
  } else {
    currentPathCombo->lineEdit()->setText( currentPath);
    if( remoteDirPathStringList.grep( currentPath,TRUE).isEmpty() ) {
      currentPathCombo->clear();
      remoteDirPathStringList.prepend( currentPath );
      currentPathCombo->insertStringList( remoteDirPathStringList,-1);
    }
  }
}

void AdvancedFm::currentPathComboActivated(const QString & currentPath) {
  if (TabWidget->currentPageIndex() == 0) {
    chdir( currentPath.latin1() );
    currentDir.cd( currentPath, TRUE);
    populateLocalView();
    update();
  } else {
    chdir( currentPath.latin1() );
    currentRemoteDir.cd( currentPath, TRUE);
    populateRemoteView();
    update();
  }
}

QStringList AdvancedFm::getPath() {
  QStringList strList;
  if (TabWidget->currentPageIndex() == 0) {
    QList<QListViewItem> * getSelectedItems( QListView * Local_View );
    QListViewItemIterator it( Local_View );
    for ( ; it.current(); ++it ) {
      if ( it.current()->isSelected() ) {
        strList <<  it.current()->text(0);
        qDebug(it.current()->text(0));
      }
    }
    return strList;
  } else {
    QList<QListViewItem> * getSelectedItems( QListView * Remote_View );
    QListViewItemIterator it( Remote_View );
    for ( ; it.current(); ++it ) {
      if ( it.current()->isSelected() ) {
        strList << it.current()->text(0);
        qDebug(it.current()->text(0));
      }
    }
    return strList;
  }
  return "";
}

void AdvancedFm::homeButtonPushed() {
  QString current = QDir::homeDirPath();
  chdir( current.latin1() );
  if (TabWidget->currentPageIndex() == 0) {
    currentDir.cd(  current, TRUE);
    populateLocalView();
  } else {
    currentRemoteDir.cd(  current, TRUE);
    populateRemoteView();
  }
  update();
}

void AdvancedFm::docButtonPushed() {
  QString current = QPEApplication::documentDir();
  chdir( current.latin1() );
  if (TabWidget->currentPageIndex() == 0) {
    currentDir.cd( current, TRUE);
    populateLocalView();
  } else {
    currentRemoteDir.cd( current, TRUE);
    populateRemoteView();
  }
  update();
}

void AdvancedFm::SDButtonPushed() {
    QString current = "/mnt/card";// this can change so fix
  chdir( current.latin1() );
  if (TabWidget->currentPageIndex() == 0) {
    currentDir.cd( current, TRUE);
    populateLocalView();
  } else {
    currentRemoteDir.cd( current, TRUE);
    populateRemoteView();
  }
  update();

}

void AdvancedFm::CFButtonPushed() {
    QString current;
    if(zaurusDevice)
        current= "/mnt/cf"; //zaurus
    else
        current = "/mnt/hda"; //ipaq
     
    chdir( current.latin1() );
    if (TabWidget->currentPageIndex() == 0) {
        currentDir.cd( current, TRUE);
        populateLocalView();
    } else {
        currentRemoteDir.cd( current, TRUE);
        populateRemoteView();
    }
    update();
}



void AdvancedFm::doAbout() {
  QMessageBox::message("AdvancedFm",tr("Advanced FileManager\n"
                       "is copyright 2002 by\n"
                       "L.J.Potter<llornkcor@handhelds.org>\n"
                       "and is licensed by the GPL"));
}

void AdvancedFm::keyReleaseEvent( QKeyEvent *e) {
  if( TabWidget->hasFocus()) 
    switch ( e->key() ) {
    case Key_Delete:
      del();
      break;
    case Key_H:
      showHidden();
      break;
    case Key_E:
      runThis();
      break;
    case Key_C:
      copy();
      break;
    case Key_A:
      copyAs();
      break;
    case Key_M:
      move();
      break;
    case Key_R:
      rn();
      break;
    case Key_I:
      fileStatus();
      break;
    case Key_U:
      upDir();
      break;
    case Key_P:
      filePerms();
      break;
    case Key_N:
      mkDir();
      break;
    case Key_1:
      switchToLocalTab();
      break;
    case Key_2:
      switchToRemoteTab();
      break;
    case Key_3:
      CFButtonPushed();
      break;
    case Key_4:
      SDButtonPushed();
      break;
    case Key_5:
      homeButtonPushed();
      break;
    case Key_6:
      docButtonPushed();
      break;
    case Key_7:
      break;
    case Key_8:
      break;
    case Key_9:
      break;
    case Key_0:
      break;
    }
}


void AdvancedFm::QPEButtonPushed() {
  QString current = QPEApplication::qpeDir();
  chdir( current.latin1() );
  if (TabWidget->currentPageIndex() == 0) {
    currentDir.cd( current, TRUE);
    populateLocalView();
  } else {
    currentRemoteDir.cd( current, TRUE);
    populateRemoteView();
  }
  update();
}

void AdvancedFm::parsetab(const QString &fileName) {

  fileSystemTypeList.clear();
  fsList.clear();
  struct mntent *me;
  FILE *mntfp = setmntent( fileName.latin1(), "r" );
  if ( mntfp ) {
    while ( (me = getmntent( mntfp )) != 0 ) {
      QString deviceName = me->mnt_fsname;
      QString filesystemType = me->mnt_type;
      QString mountDir = me->mnt_dir;
      if(deviceName != "none") {
        if( fsList.contains(filesystemType) == 0
            & filesystemType.find("proc",0,TRUE) == -1
            & filesystemType.find("cramfs",0,TRUE) == -1
            & filesystemType.find("auto",0,TRUE) == -1)
          fsList << filesystemType;
        fileSystemTypeList << mountDir+"::"+filesystemType;
      }
    }
  }
  endmntent( mntfp );
}

QString  AdvancedFm::getFileSystemType(const QString &currentText) {
  parsetab("/etc/mtab"); //why did TT forget filesystem type?
  QString current = currentText;//.right( currentText.length()-1);
  QString baseFs;
  for ( QStringList::Iterator it = fileSystemTypeList.begin(); it != fileSystemTypeList.end(); ++it ) {
    QString temp = (*it);
    QString path = temp.left(temp.find("::",0,TRUE) );
    path = path.right( path.length()-1);
    if(path.isEmpty()) baseFs = temp.right( temp.length() - temp.find("::",0,TRUE) - 2);
    if( current.find( path,0,TRUE) != -1 && !path.isEmpty()) {
      return temp.right( temp.length() - temp.find("::",0,TRUE) - 2);
    }
  }
  return baseFs;
}

QString  AdvancedFm::getDiskSpace( const QString &path) {
   struct statfs fss;
   if ( !statfs( path.latin1(), &fss ) ) {
     int blkSize = fss.f_bsize;
 //    int totalBlks = fs.f_blocks;
     int availBlks = fss.f_bavail;
 
     long mult = blkSize / 1024;
     long div = 1024 / blkSize;
     if ( !mult ) mult = 1;
     if ( !div ) div = 1;
 
     return QString::number(availBlks * mult / div);
   }
   return "";
}


void AdvancedFm::showFileMenu() {

    QString curApp;
    bool isLocalView = false;
    if (TabWidget->currentPageIndex() == 0) {
        isLocalView = TRUE;
        curApp = Local_View->currentItem()->text(0);
    }   else {
        curApp = Remote_View->currentItem()->text(0);
    }

    MimeType mt( curApp );
    const AppLnk* app = mt.application();
    QFile fi(curApp);

    QPopupMenu *m = new QPopupMenu(0);
    QPopupMenu *n = new QPopupMenu(0);
//    QPopupMenu *o = new QPopupMenu(0);

    m->insertItem(  tr( "Show Hidden Files" ), this,  SLOT( showHidden() ));
    if ( !QFileInfo(fi).isDir() ) {
//    m->insertSeparator();
//         m->insertItem( tr( "Change Directory" ), this, SLOT( doLocalCd() ));
//     } else {

        if ( app )
            m->insertItem( app->pixmap(), tr( "Open in "
                                              + app->name() ), this, SLOT( runThis() ) );
        else if( QFileInfo(fi).isExecutable() )
            m->insertItem( Resource::loadPixmap( app->name()), tr( "Execute" ), this, SLOT( runThis() ) );

        m->insertItem( Resource::loadPixmap( "txt" ), tr( "Open as text" ),this, SLOT( runText() ) );
    }

    m->insertItem(tr("Actions"),n);
    if(isLocalView)
        n->insertItem( tr( "Make Directory" ), this, SLOT( localMakDir() ));
    else
        n->insertItem( tr( "Make Directory" ), this, SLOT( remoteMakDir() ));

    n->insertItem( tr( "Make Symlink" ), this, SLOT( mkSym() ));

    n->insertSeparator();
    
    
    if(isLocalView)
        n->insertItem( tr( "Rename" ), this, SLOT( localRename() ));
    else
        n->insertItem( tr( "Rename" ), this, SLOT( remoteRename() ));

    n->insertItem( tr( "Copy" ), this, SLOT( copy() ));
    n->insertItem( tr( "Copy As" ), this, SLOT( copyAs() ));
    n->insertItem( tr( "Copy Same Dir" ), this, SLOT( copySameDir() ));
    n->insertItem( tr( "Move" ), this, SLOT( move() ));

    n->insertSeparator();

    if(isLocalView)
        n->insertItem( tr( "Delete" ), this, SLOT( localDelete() ));
    else
        n->insertItem( tr( "Delete" ), this, SLOT( remoteDelete() ));


    m->insertItem( tr( "Add To Documents" ), this, SLOT( addToDocs() ));

    m->insertItem( tr( "Run Command" ), this, SLOT( runCommand() ));
    m->insertItem( tr( "File Info" ), this, SLOT( fileStatus() ));

    m->insertSeparator();
    m->insertItem( tr( "Set Permissions" ), this, SLOT( filePerms() ));

#if defined(QT_QWS_OPIE)
    m->insertItem( tr( "Properties" ), this, SLOT( doProperties() ));
#endif    
    m->setCheckable(TRUE);
    if (!b)
        m->setItemChecked(m->idAt(0),TRUE);
    else
        m->setItemChecked(m->idAt(0),FALSE);

    if(Ir::supported())
        m->insertItem( tr( "Beam File" ), this, SLOT( doBeam() ));
    m->setFocus();
    m->exec( QCursor::pos() );
    sleep(1);
    if(m) delete m;
}


void AdvancedFm::cancelMenuTimer() {

//  qDebug("selectionChanged: cancel menu timer");
  if( menuTimer.isActive() )
    menuTimer.stop();
}

QString AdvancedFm::checkDiskSpace(const QString &path) {
  struct statfs fss;
  if ( !statfs( path.latin1(), &fss ) ) {
    int blkSize = fss.f_bsize;
//    int totalBlks = fs.f_blocks;
    int availBlks = fss.f_bavail;

    long mult = blkSize / 1024;
    long div = 1024 / blkSize;
    if ( !mult ) mult = 1;
    if ( !div ) div = 1;


    return QString::number(availBlks * mult / div);
  }
  return "";
}

void AdvancedFm::addToDocs() {
    QStringList strListPaths = getPath();
    if( strListPaths.count() > 0) {
        QString curFile;
        if (TabWidget->currentPageIndex() == 0) {
            for ( QStringList::Iterator it = strListPaths.begin(); it != strListPaths.end(); ++it ) {
                curFile = currentDir.canonicalPath()+"/"+(*it);
                qDebug(curFile);
                DocLnk f;
//                curFile.replace(QRegExp("\\..*"),"");
                f.setName((*it));
                f.setFile( curFile);
                f.writeLink();
            }
        } else {
            for ( QStringList::Iterator it = strListPaths.begin(); it != strListPaths.end(); ++it ) {
                curFile =  currentRemoteDir.canonicalPath()+"/"+(*it);
                qDebug(curFile);

                DocLnk f;
//                curFile.replace(QRegExp("\\..*"),"");
                f.setName((*it));
                f.setFile( curFile);
                f.writeLink();
            }
        }
    }
}
