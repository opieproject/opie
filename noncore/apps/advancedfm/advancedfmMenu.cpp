/***************************************************************************
   AdvancedFm.cpp
  -------------------
  ** Created: Sat Mar 9 23:33:09 2002
    copyright            : (C) 2002 by ljp
    email               : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/
#include "advancedfm.h"
#include "output.h"
#include "filePermissions.h"

#include <qpe/lnkproperties.h>
#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>

#include <qmessagebox.h>


#include <qpopupmenu.h>
#include <qlistview.h>

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <fcntl.h>


using namespace Opie::Core;
using namespace Opie::Core;
void AdvancedFm::doDirChange() {
   QString pathItem = CurrentView()->currentItem()->text(0);
   if( pathItem == "../") {
      ListClicked( CurrentView()->currentItem());
   } else {
      if( pathItem.find(" -> ",0,TRUE) != -1)
         pathItem = dealWithSymName((const QString&)pathItem)+"/";
//      qWarning(pathItem);
      gotoDirectory( CurrentDir()->path()+"/"+pathItem.left( pathItem.length() - 1) );
   }
}

void AdvancedFm::showMenuHidden() {
  if (b) {
      CurrentDir()->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
      OtherDir()->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
      fileMenu->setItemChecked( fileMenu->idAt(0),TRUE);
    } else {
      CurrentDir()->setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
      OtherDir()->setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
      fileMenu->setItemChecked( fileMenu->idAt(0),FALSE);
    }
     b = !b;
         populateView();
}

void AdvancedFm::showHidden() {
  if (b) {
      CurrentDir()->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
      OtherDir()->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
    }  else {
      CurrentDir()->setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
      OtherDir()->setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    }
         populateView();
}

QString AdvancedFm::dealWithSymName(const QString &fileName) {
   QString strItem = fileName;
   return  strItem.right( (strItem.length() - strItem.find("->",0,TRUE)) - 4);
}

void AdvancedFm::runThis() {
   if( !CurrentView()->currentItem()) return;
  QString fs;
  QDir *thisDir = CurrentDir();

  QString curFile = CurrentView()->currentItem()->text(0);
  QString path = thisDir->canonicalPath();

  if( curFile.find("@",0,TRUE) !=-1 || curFile.find("->",0,TRUE) !=-1 )  //if symlink

    curFile = dealWithSymName((const QString&)curFile);

  if(curFile != "../") {

      fs = getFileSystemType((const QString &) path);
      QFileInfo fileInfo( path  + "/" + curFile);
//      qDebug( fileInfo.owner());

      if( (fileInfo.permission( QFileInfo::ExeUser)
           | fileInfo.permission( QFileInfo::ExeGroup)
           | fileInfo.permission( QFileInfo::ExeOther)) // & fs.find("vfat",0,TRUE) == -1) {
          | fs == "vfat" && fileInfo.filePath().contains("/bin") ) {
        QCopEnvelope e("QPE/System", "execute(QString)" );
        e << curFile;
      } else {
          curFile =  path + "/" + curFile;
          DocLnk nf(curFile);
          QString execStr = nf.exec();
//          qDebug( execStr);
          if( execStr.isEmpty() ) {
            } else {
              nf.execute();
            }
        }
    }
}

void AdvancedFm::runText() {
   if( !CurrentView()->currentItem()) return;
  QString curFile = CurrentView()->currentItem()->text(0);
  if(curFile != "../") {
      if( curFile.find("@",0,TRUE) !=-1 || curFile.find("->",0,TRUE) !=-1 )  //if symlink
        curFile = dealWithSymName((const QString&)curFile);
      curFile =  CurrentDir()->canonicalPath()+"/"+curFile;
      QCopEnvelope e("QPE/Application/textedit", "setDocument(QString)" );
      e << curFile;
    }
}

void AdvancedFm::makeDir() {
  InputDialog *fileDlg;
  fileDlg = new InputDialog(this,tr("Make Directory"),TRUE, 0);
  fileDlg->exec();
  if( fileDlg->result() == 1 ) {
      QDir *thisDir = CurrentDir();
      QString  filename = fileDlg->LineEdit1->text();
      thisDir->mkdir( thisDir->canonicalPath()+"/"+filename);
  }
    populateView();
}

void AdvancedFm::doDelete() {
   QStringList curFileList = getPath();
   bool doMsg=true;
   int count = curFileList.count();
   if( count > 0)  {
      if(count > 1 ) {
         QString msg;
         msg=tr("Really delete\n%1 files?").arg(count);
         switch ( QMessageBox::warning(this,tr("Delete"),msg
                                       ,tr("Yes"),tr("No"),0,0,1) )
         {
         case 0:
            doMsg=false;
            break;
         case 1:
            return;
            break;
         };
      }

      QString myFile;

      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
         myFile = (*it);
         if( myFile.find(" -> ",0,TRUE) != -1)
            myFile = myFile.left( myFile.find(" -> ",0,TRUE));

         QString f = CurrentDir()->canonicalPath();
         if(f.right(1).find("/",0,TRUE) == -1)
            f += "/";
         f += myFile;
         if(QDir(f).exists() && !QFileInfo(f).isSymLink() ) {
            //if file is a directory

            switch ( QMessageBox::warning( this, tr("Delete Directory?"),
                                           tr("Really delete %1\nand all it's contents ?" ).arg( f ) ,
                                           tr("Yes"), tr("No"), 0, 0, 1) ) {
            case 0:
            {
               f=f.left(f.length()-1);
               QString cmd="rm -rf "+f;
               startProcess( (const QString)cmd.latin1() );
                             populateView();
            }
            break;
            case 1:
               // exit
               break;
            };

         } else {
            if(doMsg) {
               switch ( QMessageBox::warning(this,tr("Delete"),
                                             tr("Really delete\n%1?").arg( myFile ),
                                             tr("Yes"), tr("No"), 0, 0, 1) ) {
               case 1:
                  return;
                  break;
               };
            }

            QString cmd="rm "+f;
            QFile file(f);
            QFileInfo fi(myFile);
            if( fi.fileName().find("../",0,TRUE)==-1) {
//               qDebug("remove link files "+myFile);

//               DocLnk lnk(f);
                 DocLnk *lnk;
                 lnk = new DocLnk(f);
//                 qDebug("Deleting doclnk " + lnk->linkFile());
                 if(lnk->isValid())
                    lnk->removeLinkFile();
               // delete lnk;
               file.remove();
            }
         }
      }
   }
    populateView();
}

void AdvancedFm::filePerms() {
   QStringList curFileList = getPath();
   QString filePath;

   filePath = CurrentDir()->canonicalPath()+"/";

   for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
      filePermissions *filePerm;
      filePerm = new filePermissions(this, "Permissions",true,0,(const QString &)(filePath+*it));
      QPEApplication::execDialog( filePerm );
      if( filePerm )
         delete  filePerm;
   }
    populateView();
}

void AdvancedFm::doProperties() {
#if defined(QT_QWS_OPIE)

   QStringList curFileList = getPath();

   QString filePath;
   filePath = CurrentDir()->canonicalPath()+"/";

//   qDebug("%d",curFileList.count());

   for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
//      qDebug((filePath+*it));
      DocLnk lnk( (filePath+*it));
      LnkProperties prop( &lnk );
      QPEApplication::execDialog( &prop );
   }
#endif

}

void AdvancedFm::upDir() {
  QDir *thisDir = CurrentDir();
  QString current = thisDir->canonicalPath();
  QDir dir(current);
  dir.cdUp();
  current = dir.canonicalPath();
  chdir( current.latin1() );
  thisDir->cd(  current, TRUE);

    populateView();
  update();
}

void AdvancedFm::copy() {
   qApp->processEvents();
   QStringList curFileList = getPath();

   QDir *thisDir = CurrentDir();
   QDir *thatDir = OtherDir();

   bool doMsg=true;
   int count=curFileList.count();
   if( count > 0) {
      if(count > 1 ){
         QString msg;
         msg=tr("Really copy\n%1 files?").arg(count);
         switch ( QMessageBox::warning(this,tr("Copy"),msg
                                       ,tr("Yes"),tr("No"),0,0,1) )
         {
         case 0:
            doMsg=false;
            break;
         case 1:
            return;
            break;
         };
      }

      QString curFile, item, destFile;
      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
         item=(*it);
         if(item.find("->",0,TRUE)) //symlink
            item = item.left(item.find("->",0,TRUE));

         curFile = thisDir->canonicalPath()+"/"+ item;
         destFile = thatDir->canonicalPath()+"/"+ item;

//          qDebug("Destination file is "+destFile);
//          qDebug("CurrentFile file is " + curFile);

         QFile f(destFile);
         if( f.exists()) {
            if(doMsg) {
               switch ( QMessageBox::warning(this,tr("File Exists!"),
                                             tr("%1 exists. Ok to overwrite?").arg( item ),
                                             tr("Yes"),tr("No"),0,0,1) ) {
               case 1:
                  return;
                  break;
               };
            }
            f.remove();
         }

         if( !copyFile( curFile, destFile) )  {
            QMessageBox::message("AdvancedFm",
                                 tr( "Could not copy %1 to %2").arg( curFile ).arg( destFile ) );
            return;
         }
      }
            setOtherTabCurrent();
            rePopulate();
   }
}

void AdvancedFm::copyAs() {
   qApp->processEvents();

   QStringList curFileList = getPath();
   QString curFile, item;
   InputDialog *fileDlg;

   QDir *thisDir = CurrentDir();
   QDir *thatDir = OtherDir();

   for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
      QString destFile;
      item=(*it);
      curFile = thisDir->canonicalPath()+"/"+(*it);
      fileDlg = new InputDialog( this, tr("Copy "+curFile+" As"), TRUE, 0);

      fileDlg->setInputText((const QString &) destFile );
      fileDlg->exec();

      if( fileDlg->result() == 1 ) {
         QString  filename = fileDlg->LineEdit1->text();
         destFile = thatDir->canonicalPath()+"/"+filename;

         QFile f( destFile);
         if( f.exists()) {
            switch (QMessageBox::warning(this,tr("File Exists!"),
                                         item+tr("\nexists. Ok to overwrite?"),
                                         tr("Yes"),tr("No"),0,0,1) ) {
            case 0:
               f.remove();
               break;
            case 1:
               return;
               break;
            };
         }
         if( !copyFile( curFile, destFile) ) {
            QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                                 +curFile +tr("to\n")+destFile);
            return;
         }
      }
      delete fileDlg;

   }
            rePopulate();
            setOtherTabCurrent();
}

void AdvancedFm::copySameDir() {
  qApp->processEvents();
  QStringList curFileList = getPath();
  QString curFile, item, destFile;
  InputDialog *fileDlg;

  QDir *thisDir = CurrentDir();

  for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
      item=(*it);
      curFile = thisDir->canonicalPath()+"/"+ item;

      fileDlg = new InputDialog(this,tr("Copy ")+curFile+tr(" As"),TRUE, 0);
      fileDlg->setInputText((const QString &) destFile );
      fileDlg->exec();

      if( fileDlg->result() == 1 ) {

          QString filename = fileDlg->LineEdit1->text();
          destFile = thisDir->canonicalPath()+"/"+filename;

          QFile f(destFile);
          if( f.exists()) {
              switch (QMessageBox::warning(this,tr("Delete"),
                                           destFile+tr(" already exists.\nDo you really want to delete it?"),
                                           tr("Yes"),tr("No"),0,0,1) ) {
              case 0:

                f.remove();
                break;
              case 1:
                return;
                break;
              };
            }
          if(!copyFile( curFile,destFile) )  {
              QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                                   +curFile +tr("to\n")+destFile);
              return;
            }

//          qDebug("copy "+curFile+" as "+destFile);
        }
      delete fileDlg;
    }
            rePopulate();
}

void AdvancedFm::move() {
  qApp->processEvents();

  QStringList curFileList = getPath();
  if( curFileList.count() > 0) {
      QString curFile, destFile, item;

      QDir *thisDir = CurrentDir();
      QDir *thatDir = OtherDir();
      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
          item=(*it);
          QString destFile =  thatDir->canonicalPath();

          if(destFile.right(1).find("/",0,TRUE) == -1)
            destFile+="/";
          destFile += item;
//          qDebug("Destination file is "+destFile);

          curFile = thisDir->canonicalPath();
          if(curFile.right(1).find("/",0,TRUE) == -1)
                            curFile +="/";
          curFile+= item;
//          qDebug("CurrentFile file is " + curFile);

          if(QFileInfo(curFile).isDir()) {
                            moveDirectory( curFile, destFile );
                            rePopulate();
                            return;
          }

                    QFile f( curFile);
          if( f.exists()) {
                            if( !copyFile(  curFile, destFile) )  {
                                    QMessageBox::message(tr("Note"),tr("Could not move\n")+curFile);
                                    return;
              } else
                                    QFile::remove(curFile);
          }
            }

  }
            rePopulate();
            setOtherTabCurrent();
}

bool AdvancedFm::moveDirectory( const QString & src, const QString & dest ) {
        int err = 0;
        if( copyDirectory( src, dest ) ) {      QString cmd = "rm -rf " + src;
        err = system((const char*)cmd);
        } else
                err = -1;

        if(err!=0) {
                QMessageBox::message(tr("Note"),tr("Could not move\n") + src);
                return false;
        }
        return true;
}

bool AdvancedFm::copyDirectory( const QString & src, const QString & dest ) {

        QString    cmd = "/bin/cp -fpR " + src + " " + dest;
    qWarning(cmd);
        int    err = system( (const char *) cmd );
        if ( err != 0 ) {
                QMessageBox::message("AdvancedFm",
                                                         tr( "Could not copy \n%1 \nto \n%2").arg( src ).arg( dest ) );
                return false;
        }

        return true;
}


bool AdvancedFm::copyFile( const QString & src, const QString & dest ) {


        if(QFileInfo(src).isDir()) {
                if( copyDirectory( src, dest )) {
                        setOtherTabCurrent();
                        populateView();
                        return true;
                }
                else
                        return false;
        }


        bool success = true;
        struct stat status;
        QFile srcFile(src);
        QFile destFile(dest);
        int err=0;
        int read_fd=0;
   int write_fd=0;
   struct stat stat_buf;
   off_t offset = 0;
      if(!srcFile.open( IO_ReadOnly|IO_Raw)) {
//         qWarning("open failed");
         return success = false;
   }
   read_fd = srcFile.handle();
   if(read_fd != -1) {
      fstat (read_fd, &stat_buf);
      if( !destFile.open( IO_WriteOnly|IO_Raw ) ) {
//       qWarning("destfile open failed");
       return success = false;
      }
      write_fd = destFile.handle();
      if(write_fd != -1) {
         err = sendfile(write_fd, read_fd, &offset, stat_buf.st_size);
           if( err == -1) {
              QString msg;
              switch(err) {
              case EBADF : msg = "The input file was not opened for reading or the output file was not opened for writing. ";
              case EINVAL: msg = "Descriptor is not valid or locked. ";
              case ENOMEM: msg = "Insufficient memory to read from in_fd.";
              case EIO: msg = "Unspecified error while reading from in_fd.";
              };
              success = false;
//              qWarning(msg);
         }
      } else {
         success = false;
      }
   } else {
      success = false;
   }
   srcFile.close();
   destFile.close();
    // Set file permissions
  if( stat( (const char *) src, &status ) == 0 )  {
      chmod( (const char *) dest, status.st_mode );
    }

  return success;
}

void AdvancedFm::runCommand() {
   if( !CurrentView()->currentItem()) return;
   QDir *thisDir = CurrentDir();

   QString curFile;
   curFile = thisDir->canonicalPath() +"/"+ CurrentView()->currentItem()->text(0);

   InputDialog *fileDlg;
   fileDlg = new InputDialog(this,tr("Run Command"),TRUE, 0);
   fileDlg->setInputText(curFile);
   fileDlg->exec();
   //QString command;

   if( fileDlg->result() == 1 ) {
//      qDebug(fileDlg->LineEdit1->text());
      QStringList command;

      command << "/bin/sh";
      command << "-c";
      command <<  fileDlg->LineEdit1->text();
      Output *outDlg;
      outDlg = new Output( command, this, tr("AdvancedFm Output"), true);
      QPEApplication::execDialog( outDlg );
      qApp->processEvents();

   }
}

void AdvancedFm::runCommandStd() {
   if( !CurrentView()->currentItem()) return;
   QString curFile;
   QDir *thisDir = CurrentDir();
   QListView *thisView = CurrentView();
   if( thisView->currentItem())
      curFile = thisDir->canonicalPath() +"/"+  thisView->currentItem()->text(0);

   InputDialog *fileDlg;
   fileDlg = new InputDialog(this,tr("Run Command"),TRUE, 0);
   fileDlg->setInputText(curFile);
   fileDlg->exec();

   if( fileDlg->result() == 1 ) {
      qApp->processEvents();
      startProcess( (const QString)fileDlg->LineEdit1->text().latin1());
   }
}

void AdvancedFm::fileStatus() {
   if( !CurrentView()->currentItem()) return;
   QString curFile;
   curFile = CurrentView()->currentItem()->text(0);

   QStringList command;
   command << "/bin/sh";
   command << "-c";
   command << "stat -l "+ curFile;

   Output *outDlg;
   outDlg = new Output( command, this, tr("AdvancedFm Output"), true);
   QPEApplication::execDialog( outDlg );
   qApp->processEvents();
}


void AdvancedFm::mkDir() {
    makeDir();
}

void AdvancedFm::rn() {
  renameIt();
}

void AdvancedFm::del() {
    doDelete();
}

void AdvancedFm::mkSym() {
  QString cmd;
  QStringList curFileList = getPath();
  if( curFileList.count() > 0) {
      QDir *thisDir = CurrentDir();
      QDir * thatDir = OtherDir();

      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

          QString destName = thatDir->canonicalPath()+"/"+(*it);
          if(destName.right(1) == "/") {
              destName = destName.left( destName.length() -1);
            }

          QString curFile =  thisDir->canonicalPath()+"/"+(*it);

          if( curFile.right(1) == "/") {
              curFile = curFile.left( curFile.length() -1);
            }

          cmd = "ln -s "+curFile+" "+destName;
//          qDebug(cmd);
          startProcess( (const QString)cmd );
        }
            rePopulate();
            setOtherTabCurrent();
    }
}

void AdvancedFm::doBeam() {
   Ir ir;
   if(!ir.supported())  {
   } else {
      QStringList curFileList = getPath();
      if( curFileList.count() > 0)  {
         for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
            QString curFile = (*it);
            QString curFilePath =  CurrentDir()->canonicalPath()+"/"+curFile;
            if( curFilePath.right(1) == "/") {
               curFilePath = curFilePath.left( curFilePath.length() -1);
            }
            Ir *file = new Ir(this, "IR");
            connect(file, SIGNAL(done(Ir*)), this, SLOT( fileBeamFinished(Ir*)));
            file->send( curFilePath, curFile );
         }
      }
   }
}

void AdvancedFm::fileBeamFinished( Ir *) {
  QMessageBox::message( tr("Advancedfm Beam out"), tr("Ir sent.") ,tr("Ok") );
}

void AdvancedFm::selectAll() {
  QListView *thisView = CurrentView();
  thisView->selectAll(true);
  thisView->setSelected( thisView->firstChild(),false);
}

void AdvancedFm::startProcess(const QString & cmd) {
  QStringList command;
  OProcess *process;
  process = new OProcess();
  connect(process, SIGNAL(processExited(Opie::Core::OProcess*)),
          this, SLOT( processEnded(OProcess*)));

  connect(process, SIGNAL( receivedStderr(Opie::Core::OProcess*,char*,int)),
            this, SLOT( oprocessStderr(OProcess*,char*,int)));

  command << "/bin/sh";
  command << "-c";
  command << cmd.latin1();
  *process << command;
  if(!process->start(OProcess::NotifyOnExit, OProcess::All) )
    qDebug("could not start process");
}

void AdvancedFm::processEnded(OProcess *) {
            rePopulate();
}

void AdvancedFm::oprocessStderr(OProcess*, char *buffer, int ) {
//    qWarning("received stderrt %d bytes", buflen);

    QString lineStr = buffer;
    QMessageBox::warning( this, tr("Error"), lineStr ,tr("Ok") );
}

bool AdvancedFm::eventFilter( QObject * o, QEvent * e ) {
  if ( o->inherits( "QLineEdit" ) )  {
      if ( e->type() == QEvent::KeyPress ) {
          QKeyEvent *ke = (QKeyEvent*)e;
          if ( ke->key() == Key_Return ||
               ke->key() == Key_Enter )  {
              okRename();
              return true;
            }
          else if ( ke->key() == Key_Escape ) {
                cancelRename();
                return true;
              }
        }
      else if ( e->type() == QEvent::FocusOut ) {
            cancelRename();
            return true;
          }
    }
   if ( o->inherits( "QListView" ) ) {
       if ( e->type() == QEvent::FocusIn ) {
          if( o == Local_View) { //keep track of which view
             whichTab=1;
          }
          else {
             whichTab=2;
          }
       }
       OtherView()->setSelected( OtherView()->currentItem(), FALSE );//make sure there's correct selection
     }

  return QWidget::eventFilter( o, e );
}


void AdvancedFm::cancelRename() {
//  qDebug("cancel rename");
  QListView * view;
  view = CurrentView();

  bool resetFocus = view->viewport()->focusProxy() == renameBox;
  delete renameBox;
  renameBox = 0;
  if ( resetFocus ) {
      view->viewport()->setFocusProxy( view);
      view->setFocus();
    }
}

void AdvancedFm::doRename(QListView * view) {
   if( !CurrentView()->currentItem()) return;

    QRect r = view->itemRect( view->currentItem( ));
    r = QRect( view->viewportToContents( r.topLeft() ), r.size() );
    r.setX( view->contentsX() );

        if ( r.width() > view->visibleWidth() )
        r.setWidth( view->visibleWidth() );

    renameBox = new QLineEdit( view->viewport(), "qt_renamebox" );
    renameBox->setFrame(true);

    renameBox->setText(  view->currentItem()->text(0) );

    renameBox->selectAll();
    renameBox->installEventFilter( this );

    view->addChild( renameBox, r.x(), r.y() );

        renameBox->resize( r.size() );

        view->viewport()->setFocusProxy( renameBox );

        renameBox->setFocus();
    renameBox->show();
}


void AdvancedFm::renameIt() {
        if( !CurrentView()->currentItem()) return;

        QListView *thisView = CurrentView();
    oldName = thisView->currentItem()->text(0);
    doRename( thisView );
}

void AdvancedFm::okRename() {
   if( !CurrentView()->currentItem()) return;

   QString newName = renameBox->text();
     cancelRename();
     QListView * view = CurrentView();
   QString path =  CurrentDir()->canonicalPath() + "/";
   oldName = path + oldName;
   newName = path + newName;
   if( rename( oldName.latin1(), newName.latin1())== -1)
      QMessageBox::message(tr("Note"),tr("Could not rename"));
   else
      oldName = "";
   view->takeItem( view->currentItem() );
   delete view->currentItem();
     rePopulate();
}

void AdvancedFm::openSearch() {
    QMessageBox::message(tr("Note"),tr("Not Yet Implemented"));
}
