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

#include <opie/osplitter.h>

#include <qpe/filemanager.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/mimetype.h>
#include <qpe/applnk.h>
#include <qpe/ir.h>
#include <qpe/resource.h>
#include <qpe/menubutton.h>

#include <qdatetime.h>
#include <qfile.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qimage.h>
#include <qmessagebox.h>
#include <qlineedit.h>

#include <qpe/qpemessagebox.h>

#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <mntent.h>

AdvancedFm::AdvancedFm( )
   : QMainWindow( ) {
   init();
   renameBox = 0;

   unknownXpm = Resource::loadImage(  "UnknownDocument" ).smoothScale( AppLnk::smallIconSize(),  AppLnk::smallIconSize() );

   initConnections();
   whichTab=1;
   rePopulate();
   currentPathCombo->setFocus();
   channel = new QCopChannel( "QPE/Application/advancedfm", this );
   connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
            this, SLOT( qcopReceive(const QCString&, const QByteArray&)) );
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
//   qWarning("tab changed");
   QString path = CurrentDir()->canonicalPath();
   currentPathCombo->lineEdit()->setText( path );

   if(whichTab == 1) {
      viewMenu->setItemChecked(viewMenu->idAt(0), true);
      viewMenu->setItemChecked(viewMenu->idAt(1), false);
   } else {
      viewMenu->setItemChecked(viewMenu->idAt(0), false);
      viewMenu->setItemChecked(viewMenu->idAt(1), true);
   }

   QString fs= getFileSystemType( (const QString &)  path);

   setCaption(tr("AdvancedFm :: ")+fs+" :: "
              +checkDiskSpace( (const QString &) path )+ tr(" kB free") );
   chdir( path.latin1());
}


void AdvancedFm::populateView() {

qWarning("PopulateView");
		QPixmap pm;
		QListView *thisView = CurrentView();
		QDir *thisDir = CurrentDir();
		QString path = thisDir->canonicalPath();

qWarning("path is "+path);
		thisView->clear();
		thisDir->setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
		thisDir->setMatchAllDirs(TRUE);
		thisDir->setNameFilter(filterStr);
		QString fileL, fileS, fileDate;
		QString fs= getFileSystemType((const QString &) path);
		setCaption(tr("AdvancedFm :: ")+fs+" :: "
							 +checkDiskSpace((const QString &) path)+ tr(" kB free") );
		bool isDir=FALSE;
		const QFileInfoList *list = thisDir->entryInfoList( /*QDir::All*/ /*, QDir::SortByMask*/);
		QFileInfoListIterator it(*list);
		QFileInfo *fi;
		while ( (fi=it.current()) ) {
				if (fi->isSymLink() )  {
						QString symLink=fi->readLink();
						QFileInfo sym( symLink);
						fileS.sprintf( "%10i", sym.size() );
						fileL =  fi->fileName() +" ->  " + sym.filePath().data();
						fileDate = sym.lastModified().toString();
				}  else  {
						fileS.sprintf( "%10i", fi->size() );
						fileL = fi->fileName();
						fileDate= fi->lastModified().toString();
						if( QDir(QDir::cleanDirPath( path +"/"+fileL)).exists() ) {
//           if(fileL == "..")
								fileL += "/";
								isDir=TRUE;
						}
				}
				QFileInfo fileInfo(  path + "/" + fileL);

				if(fileL !="./" && fi->exists())  {
						item = new QListViewItem( thisView, fileL, fileS , fileDate);

						if(isDir || fileL.find("/",0,TRUE) != -1) {

								if( !QDir( fi->filePath() ).isReadable()) //is directory
										pm = Resource::loadPixmap( "lockedfolder" );
								else
										pm= Resource::loadPixmap( "folder" );
						}
						else if ( fs == "vfat" && fileInfo.filePath().contains("/bin") ) {
								pm = Resource::loadPixmap( "exec");
						}
						else if( (fileInfo.permission( QFileInfo::ExeUser)
											| fileInfo.permission( QFileInfo::ExeGroup)
											| fileInfo.permission( QFileInfo::ExeOther)) && fs != "vfat" ) {
								pm = Resource::loadPixmap( "exec");
						}
						else if( !fi->isReadable() )  {
								pm = Resource::loadPixmap( "locked" );
						}
						else { //everything else goes by mimetype
								MimeType mt(fi->filePath());
								pm=mt.pixmap(); //sets the correct pixmap for mimetype
								if(pm.isNull()) {
										pm = unknownXpm;
								}
						}
						if(  fi->isSymLink() || fileL.find("->",0,TRUE) != -1) {
									//  qDebug(" overlay link image");
								pm= Resource::loadPixmap( "advancedfm/symlink" );
									//              pm= Resource::loadPixmap( "folder" );
//                QPixmap lnk = Resource::loadPixmap( "opie/symlink" );
//                QPainter painter( &pm );
//                painter.drawPixmap( pm.width()-lnk.width(), pm.height()-lnk.height(), lnk );
//                pm.setMask( pm.createHeuristicMask( FALSE ) );
						}
						item->setPixmap( 0,pm);

				}
				isDir=FALSE;
				++it;
		}

		if( path.find("dev",0,TRUE) != -1) {
				struct stat buf;
				dev_t devT;
				DIR *dir;
				struct dirent *mydirent;

				if((dir = opendir( path.latin1())) != NULL)
						while ((mydirent = readdir(dir)) != NULL) {
								lstat( mydirent->d_name, &buf);
//        qDebug(mydirent->d_name);
								fileL.sprintf("%s", mydirent->d_name);
								devT = buf.st_dev;
								fileS.sprintf("%d, %d", (int) ( devT >>8) &0xFF, (int)devT &0xFF);
								fileDate.sprintf("%s", ctime( &buf.st_mtime));
								if( fileL.find(".") == -1 ) {
										item= new QListViewItem( thisView, fileL, fileS, fileDate);
										pm = unknownXpm;
										item->setPixmap( 0,pm);
								}
						}

				closedir(dir);
		}

		thisView->setSorting( 3,FALSE);
		fillCombo( (const QString &) path );
}

void AdvancedFm::rePopulate() {
     populateView();
     setOtherTabCurrent();
     populateView();

//      int tmpTab = whichTab;
// //  qDebug("%d", tmpTab);

//    for(int i =1; i < 3; i++) {
//       TabWidget->setCurrentWidget(i - 1);
//       populateView();
//    }
//    TabWidget->setCurrentWidget( tmpTab - 1);
}

void AdvancedFm::ListClicked(QListViewItem *selectedItem) {
//qWarning("listclicked");
   if(selectedItem) {
      QString strItem=selectedItem->text(0);
//      qWarning(strItem);
      QString strSize=selectedItem->text(1);
      strSize=strSize.stripWhiteSpace();
      bool isDirectory = false;
      QString strItem2;

      if(strItem.find("@",0,TRUE) !=-1 || strItem.find("->",0,TRUE) !=-1 ) {//if symlink
         strItem2 = dealWithSymName((const QString&)strItem);
         if(QDir(strItem2).exists() )
            strItem = strItem2;
      }

      if( strItem.find(". .",0,TRUE) && strItem.find("/",0,TRUE)!=-1 ) {

         if(QDir(strItem).exists())
            isDirectory = true;
      }

      if( isDirectory ) {
         CurrentDir()->cd( strItem, TRUE);
         populateView();
         CurrentView()->ensureItemVisible( CurrentView()->firstChild());
      }
      chdir( strItem.latin1());
   }
}

void AdvancedFm::ListPressed( int mouse, QListViewItem *item, const QPoint& , int ) {
   dealWithSchmooSchmaa( item->listView());
   switch (mouse) {
   case 1:
   {
      if(renameBox != 0 ) {
         cancelRename();
      }
   }
   break;
   case 2:
      menuTimer.start( 500, TRUE );
      break;
   };
}


void AdvancedFm::switchToLocalTab() {
//qWarning("switch to local view");
   TabWidget->setCurrentWidget(0);
   Local_View->setFocus();
}

void AdvancedFm::switchToRemoteTab() {
//qWarning("switch to local view");
   TabWidget->setCurrentWidget(1);
   Remote_View->setFocus();
}

void AdvancedFm::readConfig() {
   Config cfg("AdvancedFm");
}

void AdvancedFm::writeConfig() {
   Config cfg("AdvancedFm");
}

void  AdvancedFm::currentPathComboChanged() {
   if(QDir( currentPathCombo->lineEdit()->text()).exists()) {
      CurrentDir()->setPath( currentPathCombo->lineEdit()->text() );
      populateView();
   } else {
      QMessageBox::message(tr("Note"),tr("That directory does not exist"));
   }
}

void  AdvancedFm::fillCombo(const QString &currentPath) {

   if ( whichTab == 1) {
      currentPathCombo->lineEdit()->setText( currentPath);
      if( localDirPathStringList.grep( currentPath,TRUE).isEmpty() ) {
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
   chdir( currentPath.latin1() );
   CurrentDir()->cd( currentPath, TRUE);
   populateView();
   update();
}

QStringList AdvancedFm::getPath() {
   QStringList strList;
   QListView *thisView=CurrentView();
   QList<QListViewItem> * getSelectedItems( QListView * thisView );
   QListViewItemIterator it( thisView );
   for ( ; it.current(); ++it ) {
      if ( it.current()->isSelected() ) {
         strList <<  it.current()->text(0);
//        qDebug(it.current()->text(0));
      }
   }
   return strList;
}

void AdvancedFm::homeButtonPushed() {
   QString current = QDir::homeDirPath();
   chdir( current.latin1() );
   CurrentDir()->cd(  current, TRUE);
   populateView();
   update();
}

void AdvancedFm::docButtonPushed() {
   QString current = QPEApplication::documentDir();
   chdir( current.latin1() );
   CurrentDir()->cd( current, TRUE);
   populateView();
   update();
}

void AdvancedFm::SDButtonPushed() {
   QString current = "/mnt/card";// this can change so fix
   chdir( current.latin1() );
   CurrentDir()->cd( current, TRUE);
   populateView();
   update();
}

void AdvancedFm::CFButtonPushed() {
   QString current;
   if(zaurusDevice)
      current= "/mnt/cf"; //zaurus
   else
      current = "/mnt/hda"; //ipaq
   chdir( current.latin1() );
   CurrentDir()->cd( current, TRUE);
   populateView();
   update();
}


void AdvancedFm::doAbout() {
   QMessageBox::message("AdvancedFm",tr("Advanced FileManager\n"
                                        "is copyright 2002-2003 by\n"
                                        "L.J.Potter<llornkcor@handhelds.org>\n"
                                        "and is licensed by the GPL"));
}

void AdvancedFm::keyPressEvent( QKeyEvent *e) {
//   qWarning("key %d", e->key());
//   if( CurrentView()->hasFocus()  )
   {
      switch ( e->key() ) {
      case Key_Left:
         upDir();
         break;
      case Key_Next:
         break;
      case Key_Return:
      case Key_Enter:
         navigateToSelected();
         break;
      case Key_Tab: {
         setOtherTabCurrent();
      }
         break;
      case Key_Delete:
         del();
         break;
      case Key_A:
         copyAs();
         break;
      case Key_C:
         copy();
         break;
      case Key_E:
         runThis();
         break;
      case Key_G:
      {
         currentPathCombo->lineEdit()->setFocus();
      }
      break;

      case Key_H:
         showHidden();
         break;
      case Key_I:
         fileStatus();
         break;
      case Key_M:
         move();
         break;
      case Key_N:
         mkDir();
         break;
      case Key_P:
         filePerms();
         break;
      case Key_R:
         rn();
         break;
      case Key_U:
         upDir();
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
      };
      e->accept();
   }
}

void AdvancedFm::keyReleaseEvent( QKeyEvent *e) {
   if( CurrentView()->hasFocus() )
      e->ignore();
}


void AdvancedFm::QPEButtonPushed() {
   QString current = QPEApplication::qpeDir();
   chdir( current.latin1() );
   CurrentDir()->cd( current, TRUE);
   populateView();
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
   curApp = CurrentView()->currentItem()->text(0);

   MimeType mt( curApp );
   const AppLnk* app = mt.application();
   QFile fi(curApp);
   QPopupMenu *m = new QPopupMenu(0);
   QPopupMenu *n = new QPopupMenu(0);
   //    QPopupMenu *o = new QPopupMenu(0);
   m->insertItem(  tr( "Show Hidden Files" ), this,  SLOT( showHidden() ));

   if ( QFileInfo(fi).isDir() ) {
      m->insertSeparator();
      m->insertItem( tr( "Change Directory" ), this, SLOT( doDirChange() ));
   } else {

      if ( app )
         m->insertItem( app->pixmap(), tr( "Open in "
                                           + app->name() ), this, SLOT( runThis() ) );
      else if( QFileInfo(fi).isExecutable() ) //damn opie doesnt like this
         m->insertItem( tr( "Execute" ), this, SLOT( runThis() ) );
      m->insertItem( Resource::loadPixmap( "txt" ), tr( "Open as text" ),this, SLOT( runText() ) );
   }

   m->insertItem(tr("Actions"),n);
   n->insertItem( tr( "Make Directory" ), this, SLOT( makeDir() ));

   n->insertItem( tr( "Make Symlink" ), this, SLOT( mkSym() ));

   n->insertSeparator();
   n->insertItem( tr( "Rename" ), this, SLOT( renameIt() ));

   n->insertItem( tr( "Copy" ), this, SLOT( copy() ));
   n->insertItem( tr( "Copy As" ), this, SLOT( copyAs() ));
   n->insertItem( tr( "Copy Same Dir" ), this, SLOT( copySameDir() ));
   n->insertItem( tr( "Move" ), this, SLOT( move() ));

   n->insertSeparator();
   n->insertItem( tr( "Delete" ), this, SLOT( doDelete() ));
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
   m->exec( QPoint( 4,QCursor::pos().y()) );

   if(m) delete m;
}


void AdvancedFm::cancelMenuTimer() {

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
   QDir *thisDir = CurrentDir();

   if( strListPaths.count() > 0) {
      QString curFile;
      for ( QStringList::Iterator it = strListPaths.begin(); it != strListPaths.end(); ++it ) {
         curFile = thisDir->canonicalPath()+"/"+(*it);
//                qDebug(curFile);
         QFileInfo fi(curFile);
         DocLnk f;
//                curFile.replace(QRegExp("\\..*"),"");
         f.setName(fi.baseName() );
         f.setFile( curFile);
         f.writeLink();
      }
   }
}


void AdvancedFm::customDirsToMenu() {

   Config cfg("AdvancedFm");
   cfg.setGroup("Menu");

   QStringList list = cfg.readListEntry( "CustomDir", ',');
   menuButton->insertItems(list );

//      for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
//        {
//            customDirMenu->insertItem(*it );
//        }
}

void AdvancedFm::dirMenuSelected(int item) {
   switch(item)
   {

   case -21:
   case 0:
      addCustomDir();
      break;
   case -22:
   case 1:
      removeCustomDir();
      break;
   default:
   {
//            gotoCustomDir( menuButton->text(item));
//            gotoCustomDir( customDirMenu->text(item));
   }
   break;

   };
}

void AdvancedFm::addCustomDir() {
   Config cfg("AdvancedFm");
   cfg.setGroup("Menu");
   QString dir;
   QStringList list = cfg.readListEntry( (const QString &)"CustomDir", (const QChar)',');

   dir =  CurrentDir()->canonicalPath();

   bool addIt=true;
   for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
      if( dir == (*it)) {
         addIt=false;
      }
   }
   if(addIt) {
      menuButton->insertItem(dir);
//          customDirMenu->insertItem(dir);
      list <<  dir;
   }

   cfg.writeEntry("CustomDir", list, ',');
   cfg.write();
}

void AdvancedFm::removeCustomDir() {
//  qDebug("remove custom dir");
   Config cfg("AdvancedFm");
   cfg.setGroup("Menu");
   QString dir;
   QStringList list = cfg.readListEntry( (const QString &)"CustomDir", (const QChar)',');
   QStringList list2;
   dir =  CurrentDir()->canonicalPath();
   int ramble=2;
//  int ramble=-24;
//first remove list
   if(list.grep(dir,true).isEmpty()) {
      QMessageBox::message(tr( "AdvancedFm" ),
                           tr("Cannot remove current directory\nfrom bookmarks.\nIt is not bookmarked!"));
   } else {
      for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
         if((*it) != dir) {
//current item is not our current dir, so add it to temp list
            list2 <<(*it);
         } else {
//              customDirMenu->removeItem( ramble);
            menuButton->remove( ramble);

         }
         ramble++;
//        ramble--;
      }

      cfg.writeEntry("CustomDir", list2, ',');
      cfg.write();
   }
//    customDirsToMenu();

}

void AdvancedFm::gotoCustomDir(const QString &dir) {
//     qDebug("gotoCustomDir(const QString &dir) " +dir );
//  QString curDir = dir;
//  QDir *thisDir = CurrentDir();
//     if( curDir.isEmpty()) {
//     }
   if( dir == s_addBookmark) {
      addCustomDir();
   }
   if( dir == s_removeBookmark) {
      removeCustomDir( );
   } else {
      gotoDirectory( dir);
//       if(QDir( curDir).exists() )
//         {
//           thisDir->setPath( curDir );
//           chdir( curDir.latin1() );
//           thisDir->cd( curDir, TRUE);
//           populateView();
//         }
   }
}

QDir *AdvancedFm::CurrentDir() {
   
   if ( whichTab == 1) {
      return &currentDir;
   } else {
      return &currentRemoteDir;
   }
}

QDir *AdvancedFm::OtherDir() {
   if ( whichTab == 1) {
      return &currentRemoteDir;
   } else {
      return &currentDir;
   }
}

QListView * AdvancedFm::CurrentView() {
   if ( whichTab == 1) {
//      qWarning("CurrentView Tab 1");
      return Local_View;
   } else {
//      qWarning("CurrentView Tab 2");
      return Remote_View;
   }
}

QListView * AdvancedFm::OtherView() {
   if ( whichTab == 1)
      return Remote_View;
   else
      return Local_View;
}

void AdvancedFm::setOtherTabCurrent() {
   qWarning("setOtherTabCurrent() %d", whichTab);
   if ( whichTab == 1) {
      TabWidget->setCurrentWidget(1);
   } else {
      TabWidget->setCurrentWidget(0);
   }
   OtherView()->setFocus();
   OtherView()->setSelected( CurrentView()->firstChild(), true);      
}

void AdvancedFm::qcopReceive(const QCString &msg, const QByteArray &data) {
//   qDebug("qcop message "+msg );
   QDataStream stream ( data, IO_ReadOnly );
   if ( msg == "openDirectory(QString)" ) {
//      qDebug("received");
      QString file;
      stream >> file;
      gotoDirectory( (const QString &) file);
   }
}

void AdvancedFm::setDocument(const QString &file) {
   gotoDirectory( file);

}

void AdvancedFm::gotoDirectory(const QString &file) {
//   qWarning("goto dir "+file);
   QString curDir = file;
   QDir *thisDir = CurrentDir();
   if(QDir( curDir).exists() )  {
      thisDir->setPath( curDir );
      chdir( curDir.latin1() );
      thisDir->cd( curDir, TRUE);
      populateView();
   }
   else if(QFileInfo(curDir).exists()) {
      QFileInfo fileInfo(curDir);
      curDir=fileInfo.dirPath();
      if(QDir( curDir).exists() )  {
         thisDir->setPath( curDir );
         chdir( curDir.latin1() );
         thisDir->cd( curDir, TRUE);
         populateView();
      }
      findFile(file);
   }
  
}

void AdvancedFm::findFile(const QString &fileName) {
   QFileInfo fi(fileName);
   QListView *thisView = CurrentView();
   QListViewItemIterator it( thisView );
   for ( ; it.current(); ++it ) {
      if(it.current()->text(0) == fi.fileName()) {
         it.current()->setSelected(true);
         thisView->ensureItemVisible(it.current());
      }
   }
}

void AdvancedFm::slotSwitchMenu(int ) {
//   qDebug("Switch %d", item);
   //   viewMenu->setItemChecked(item, true);
}

void AdvancedFm::dealWithSchmooSchmaa(QWidget *w) {
   tabChanged( w);
   if( w == Local_View) {
      Remote_View->clearFocus();
   } else {
      Local_View->clearFocus();
   }
}

void AdvancedFm::navigateToSelected() {
   if( !CurrentView()->currentItem()) return;
   doDirChange();
}
