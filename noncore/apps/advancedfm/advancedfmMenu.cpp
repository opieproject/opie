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
#include "advancedfm.h"
#include "output.h"
#include "filePermissions.h"

#include <opie/otabwidget.h>
#include <opie/oprocess.h>

#include <qpe/lnkproperties.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/applnk.h>
#include <qpe/ir.h>

#include <qmessagebox.h>
#include <qmultilineedit.h>

#include <qstring.h>

#include <qlayout.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qlistview.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>


void AdvancedFm::doLocalCd() {
  localListClicked( Local_View->currentItem());
}

void AdvancedFm::doRemoteCd() {
  localListClicked( Remote_View->currentItem());
}

void AdvancedFm::showMenuHidden() {
  if (b) {
    currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
    currentRemoteDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
    fileMenu->setItemChecked( fileMenu->idAt(0),TRUE);
//    b=FALSE;

  }  else {
    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    currentRemoteDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    fileMenu->setItemChecked( fileMenu->idAt(0),FALSE);
//    b=TRUE;
  }
  populateLocalView();
  populateRemoteView();
//     if(TabWidget->getCurrentTab() == 0)
//         showHidden();
//     else
//         showRemoteHidden();
//    if(b) qDebug("<<<<<<<<<<<<<<<<<<<<<<<<<<<< true");
    if(b) b = false; else b = true;
}

void AdvancedFm::showHidden() {
  if (b) {
    currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
//     fileMenu->setItemChecked( fileMenu->idAt(0),TRUE);
//    b=FALSE;

  }  else {
    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
//     fileMenu->setItemChecked( fileMenu->idAt(0),FALSE);
//    b=TRUE;
  }
  populateLocalView();
}

void AdvancedFm::showRemoteHidden() {
  if (b) {
    currentRemoteDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
//    b=TRUE;

  }  else {
    currentRemoteDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
//    b=FALSE;
  }
  populateRemoteView();
}

QString AdvancedFm::dealWithSymName(const QString &fileName) {
        QString strItem = fileName;
        return  strItem.right( (strItem.length() - strItem.find("->",0,TRUE)) - 4);
}

void AdvancedFm::runThis() {
  QString fs;
  if (TabWidget->getCurrentTab() == 0) {
    QString curFile = Local_View->currentItem()->text(0);
    if( curFile.find("@",0,TRUE) !=-1 || curFile.find("->",0,TRUE) !=-1 )  //if symlink
        curFile = dealWithSymName((const QString&)curFile);

    if(curFile != "../") {

      fs = getFileSystemType((const QString &) currentDir.canonicalPath());
      QFileInfo fileInfo( currentDir.canonicalPath()+"/"+curFile);
      qDebug( fileInfo.owner());
      if( (fileInfo.permission( QFileInfo::ExeUser)
           | fileInfo.permission( QFileInfo::ExeGroup)
           | fileInfo.permission( QFileInfo::ExeOther)) // & fs.find("vfat",0,TRUE) == -1) {
          | fs == "vfat" && fileInfo.filePath().contains("/bin") ) {
        QCopEnvelope e("QPE/System", "execute(QString)" );
        e << curFile;
      } else {
        curFile =  currentDir.canonicalPath()+"/"+curFile;
        DocLnk nf(curFile);
        QString execStr = nf.exec();
        qDebug( execStr);
        if( execStr.isEmpty() ) {
        } else {
          nf.execute();
        }
      }
    }
  } else {
    QString curFile = Remote_View->currentItem()->text(0);
    if(curFile != "../") {
    if( curFile.find("@",0,TRUE) !=-1 || curFile.find("->",0,TRUE) !=-1 )  //if symlink
        curFile = dealWithSymName((const QString&)curFile);

      fs= getFileSystemType((const QString &) currentRemoteDir.canonicalPath());
      qDebug("Filesystemtype is "+fs);
      QFileInfo fileInfo( currentRemoteDir.canonicalPath()+"/"+curFile);
      if( (fileInfo.permission( QFileInfo::ExeUser)
           | fileInfo.permission( QFileInfo::ExeGroup)
           | fileInfo.permission( QFileInfo::ExeOther)) // & fs.find("vfat",0,TRUE) == -1) {
          | fs == "vfat" && fileInfo.filePath().contains("/bin") ) {
        QCopEnvelope e("QPE/System", "execute(QString)" );
        e << curFile;
      } else {
        curFile =  currentRemoteDir.canonicalPath()+"/"+curFile;
        DocLnk nf(curFile);
        QString execStr = nf.exec();
        qDebug(execStr);
        if( execStr.isEmpty() ) {
        } else {
          nf.execute();
        }
      }
    }
  }
}

void AdvancedFm::runText() {
  if (TabWidget->getCurrentTab() == 0) {
    QString curFile = Local_View->currentItem()->text(0);
    if(curFile != "../") {
    if( curFile.find("@",0,TRUE) !=-1 || curFile.find("->",0,TRUE) !=-1 )  //if symlink
        curFile = dealWithSymName((const QString&)curFile);
      curFile =  currentDir.canonicalPath()+"/"+curFile;
      QCopEnvelope e("QPE/Application/textedit", "setDocument(QString)" );
      e << curFile;
    }
  } else {
    QString curFile = Remote_View->currentItem()->text(0);
    if(curFile != "../") {
      curFile =  currentRemoteDir.canonicalPath()+"/"+curFile;
    if( curFile.find("@",0,TRUE) !=-1 || curFile.find("->",0,TRUE) !=-1 )  //if symlink
        curFile = dealWithSymName((const QString&)curFile);
      DocLnk nf(curFile);
      QCopEnvelope e("QPE/Application/textedit", "setDocument(QString)" );
      e << curFile;
    }
  }
}

void AdvancedFm::localMakDir() {
  InputDialog *fileDlg;
  fileDlg = new InputDialog(this,tr("Make Directory"),TRUE, 0);
  fileDlg->exec();
  if( fileDlg->result() == 1 ) {
    QString  filename = fileDlg->LineEdit1->text();
    currentDir.mkdir( currentDir.canonicalPath()+"/"+filename);
  }
  populateLocalView();
}

void AdvancedFm::remoteMakDir() {
  InputDialog *fileDlg;
  fileDlg = new InputDialog(this,tr("Make Directory"),TRUE, 0);
  fileDlg->exec();
  if( fileDlg->result() == 1 ) {
    QString  filename = fileDlg->LineEdit1->text();
    currentRemoteDir.mkdir(  currentRemoteDir.canonicalPath()+"/"+filename);
  }
  populateRemoteView();
}

void AdvancedFm::localDelete() {
  QStringList curFileList = getPath();
  bool doMsg=true;
  int count=curFileList.count();
  if( count > 0) {
      if(count > 1 ){
          QString msg;
          msg=tr("Really delete\n%1 files?").arg(count);
          switch ( QMessageBox::warning(this,tr("Delete"),msg
                                        ,tr("Yes"),tr("No"),0,0,1) ) {
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

      QString f = currentDir.canonicalPath();
      if(f.right(1).find("/",0,TRUE) == -1)
        f+="/";
      f+=myFile;
      if(QDir(f).exists() && !QFileInfo(f).isSymLink() ) {
        switch ( QMessageBox::warning(this,tr("Delete Directory?"),tr("Really delete\n")+f+
                                      "\nand all it's contents ?"
                                      ,tr("Yes"),tr("No"),0,0,1) ) {
        case 0: {
          f=f.left(f.length()-1);
          QString cmd="rm -rf "+f;
          startProcess( (const QString)cmd.latin1() );
          populateLocalView();
        }
          break;
        case 1:
          // exit
          break;
        };

      } else {
          if(doMsg) {
        switch ( QMessageBox::warning(this,tr("Delete"),tr("Really delete\n")+f
                 +" ?",tr("Yes"),tr("No"),0,0,1) ) {
          case 1:
              return;
              break;
        };
          }
        QString cmd="rm "+f;
        QFile file(f);
        if(QFileInfo(myFile).fileName().find("../",0,TRUE)==-1)
            file.remove();
      }
    }
  }
  populateLocalView();
}

void AdvancedFm::remoteDelete() {
    QStringList curFileList = getPath();
    bool doMsg=true;
    int count=curFileList.count();
    if( count > 0) {
        if(count > 1 ){
            QString msg;
            msg=tr("Really delete\n%1 files?").arg(count);
            switch ( QMessageBox::warning(this,tr("Delete"),msg
                                          ,tr("Yes"),tr("No"),0,0,1) ) {
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
            if(myFile.find(" -> ",0,TRUE) != -1)
                myFile = myFile.left(myFile.find(" -> ",0,TRUE));
            QString f = currentRemoteDir.canonicalPath();
            if(f.right(1).find("/",0,TRUE) == -1)
                f+="/";
            f+=myFile;
            if(QDir(f).exists() && !QFileInfo(f).isSymLink()  ) {
                switch ( QMessageBox::warning(this,tr("Delete Directory"),tr("Really delete\n")+f+
                                              "\nand all it's contents ?",
                                              tr("Yes"),tr("No"),0,0,1) ) {
                  case 0: {
                      f=f.left(f.length()-1);
                      QString cmd="rm -rf "+f;
                      startProcess( (const QString)cmd );
                      populateRemoteView();
                  }
                      break;
                  case 1:
                        // exit
                      break;
                };

            } else {
                if(doMsg) {
                    switch ( QMessageBox::warning(this,tr("Delete"),tr("Really delete\n")+f
                                                  +" ?",tr("Yes"),tr("No"),0,0,1) ) {
                      case 1:
                          return;
                          break;
                    };
                }
                QString cmd="rm "+f;
                QFile file(f);
                if(QFileInfo(myFile).fileName().find("../",0,TRUE)==-1)
                    file.remove();
            }
        }
    }
    populateRemoteView();
}

void AdvancedFm::localRename() {
  QString curFile = Local_View->currentItem()->text(0);
  qDebug("currentItem "+curFile);
  if( curFile !="../") {
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Rename"),TRUE, 0);
    fileDlg->setInputText((const QString &)curFile);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
      QString oldname = currentDir.canonicalPath() + "/" + curFile;
      QString newName = currentDir.canonicalPath() + "/" + fileDlg->LineEdit1->text();
//+".playlist";
      if( rename(oldname.latin1(), newName.latin1())== -1)
        QMessageBox::message(tr("Note"),tr("Could not rename"));
    }
    populateLocalView();
  }
}

void AdvancedFm::remoteRename()
{
  QString curFile = Remote_View->currentItem()->text(0);
  if( curFile !="../") {
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Rename"),TRUE, 0);
    fileDlg->setInputText((const QString &)curFile);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
      QString oldname = currentRemoteDir.canonicalPath() + "/" + curFile;
      QString newName = currentRemoteDir.canonicalPath() + "/" + fileDlg->LineEdit1->text();
//+".playlist";
      if( rename(oldname.latin1(), newName.latin1())== -1)
        QMessageBox::message(tr("Note"),tr("Could not rename"));
    }
    populateRemoteView();
  }
}


void AdvancedFm::filePerms() {

  QStringList curFileList = getPath();
  QString filePath;

  if (TabWidget->getCurrentTab() == 0) {
    filePath = currentDir.canonicalPath()+"/";
  } else {
    filePath= currentRemoteDir.canonicalPath()+"/";
  }

  for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
    filePermissions *filePerm;
    filePerm = new filePermissions(this, "Permissions",true,0,(const QString &)(filePath+*it));
    filePerm->showMaximized();
    filePerm->exec();
    if( filePerm)
      delete  filePerm;
  }
  if (TabWidget->getCurrentTab() == 0) {
    populateLocalView();
  } else {
    populateRemoteView();
  }
}

void AdvancedFm::doProperties() {
#if defined(QT_QWS_OPIE)
    
  QStringList curFileList = getPath();
  
  QString filePath;
  if (TabWidget->getCurrentTab() == 0) {
    filePath = currentDir.canonicalPath()+"/";
  } else {
    filePath= currentRemoteDir.canonicalPath()+"/";
  }
   qDebug("%d",curFileList.count());

   for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
    qDebug((filePath+*it));
    DocLnk lnk( (filePath+*it));
    LnkProperties prop( &lnk );
    prop.showMaximized();
    prop.exec();
  }
#endif    
  
}

void AdvancedFm::upDir() {
  if (TabWidget->getCurrentTab() == 0) {
    QString current = currentDir.canonicalPath();
    QDir dir(current);
    dir.cdUp();
    current = dir.canonicalPath();
    chdir( current.latin1() );
    currentDir.cd(  current, TRUE);
    populateLocalView();
    update();
  } else {
    QString current = currentRemoteDir.canonicalPath();
    QDir dir(current);
    dir.cdUp();
    current = dir.canonicalPath();
    chdir( current.latin1() );
    currentRemoteDir.cd(  current, TRUE);
    populateRemoteView();
    update();
  }
}

void AdvancedFm::copy() {
  qApp->processEvents();
  QStringList curFileList = getPath();
    bool doMsg=true;
    int count=curFileList.count();
    if( count > 0) {
        if(count > 1 ){
            QString msg;
            msg=tr("Really copy\n%1 files?").arg(count);
            switch ( QMessageBox::warning(this,tr("Delete"),msg
                                          ,tr("Yes"),tr("No"),0,0,1) ) {
              case 0:
                  doMsg=false;
                  break;
              case 1:
                  return;
                  break;
            };
        }

    QString curFile, item, destFile;
    if (TabWidget->getCurrentTab() == 0) {
        for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
          item=(*it);
          if(item.find("->",0,TRUE)) //symlink
              item = item.left(item.find("->",0,TRUE));

          destFile = currentRemoteDir.canonicalPath()+"/"+ item;
          qDebug("Destination file is "+destFile);

          curFile = currentDir.canonicalPath()+"/"+ item;
          qDebug("CurrentFile file is " + curFile);

          QFile f(destFile);
          if( f.exists()) {
              if(doMsg) {
                  switch ( QMessageBox::warning(this,tr("File Exists!"), tr("%1 exists. Ok to overwrite?").arg( item ), tr("Yes"),tr("No"),0,0,1) ) {
                    case 1:
                        return;
                        break;
                  };
              }
              f.remove();                 
          }
          if(!copyFile( curFile, destFile) ) {
              QMessageBox::message("AdvancedFm",tr( "Could not copy %1 to %2").arg( curFile ).arg( destFile ) );
          return;
        }
      }
      populateRemoteView();
      TabWidget->setCurrentTab(1);

    } else {
      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
          item= (*it);
          if(item.find("->",0,TRUE)) //symlink
              item = item.left(item.find("->",0,TRUE));

          destFile = currentDir.canonicalPath()+"/"+ item;
          qDebug("Destination file is "+destFile);

          curFile = currentRemoteDir.canonicalPath()+"/"+ item;;
          qDebug("CurrentFile file is " + curFile);

          QFile f(destFile);
          if( f.exists()) {
              switch ( QMessageBox::warning(this,tr("File Exists!"),
                   item+tr("\nexists. Ok to overwrite?"),
                   tr("Yes"),tr("No"),0,0,1) ) {
          case 1:
            return;
            break;
          };
            f.remove();
        }
        if(!copyFile( curFile, destFile) ) {
          QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                               +curFile +tr("to\n")+destFile);
          return;

        }
      }
      populateLocalView();
      TabWidget->setCurrentTab(0);
    }

  }
}

void AdvancedFm::copyAs() {
  qApp->processEvents();

  QStringList curFileList = getPath();
  QString curFile, item;
  InputDialog *fileDlg;
  if (TabWidget->getCurrentTab() == 0) {
    qDebug("tab 1");
    for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
      QString destFile;
      item=(*it);
      curFile = currentDir.canonicalPath()+"/"+(*it);
      fileDlg = new InputDialog( this,tr("Copy "+curFile+" As"), TRUE, 0);

      fileDlg->setInputText((const QString &) destFile );
      fileDlg->exec();

      if( fileDlg->result() == 1 ) {
        QString  filename = fileDlg->LineEdit1->text();
        destFile = currentRemoteDir.canonicalPath()+"/"+filename;

        QFile f(destFile);
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
        if(!copyFile( curFile,destFile) ) {
          QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                               +curFile +tr("to\n")+destFile);
          return;
        }
      }
      delete fileDlg;

    }
    populateRemoteView();
    TabWidget->setCurrentTab(1);

  } else {
    for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

      item=(*it);
      curFile = currentRemoteDir.canonicalPath()+"/"+(*it);
      fileDlg = new InputDialog( this,tr("Copy "+curFile+" As"), TRUE, 0);

      QString destFile;
      fileDlg->setInputText((const QString &) destFile);
      fileDlg->exec();

      if( fileDlg->result() == 1 ) {
        QString  filename = fileDlg->LineEdit1->text();
        destFile = currentDir.canonicalPath()+"/"+filename;

        QFile f( destFile);
        if( f.exists()) {
          switch ( QMessageBox::warning(this,tr("File Exists!"),
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
        if(!copyFile( curFile,destFile) ) {
          QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                               +curFile +tr("to\n")+destFile);
          return;
        }
                        
      }
      delete fileDlg;

    }
    populateLocalView();
    TabWidget->setCurrentTab(0);
  }
}

void AdvancedFm::copySameDir() {
  qApp->processEvents();
  QStringList curFileList = getPath();
  QString curFile, item, destFile;
  InputDialog *fileDlg;

  if (TabWidget->getCurrentTab() == 0) {

    for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
        item=(*it);
      curFile = currentDir.canonicalPath()+"/"+ item;

      fileDlg = new InputDialog(this,tr("Copy ")+curFile+tr(" As"),TRUE, 0);
      fileDlg->setInputText((const QString &) destFile );
      fileDlg->exec();

      if( fileDlg->result() == 1 ) {

        QString filename = fileDlg->LineEdit1->text();
        destFile = currentDir.canonicalPath()+"/"+filename;

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
        if(!copyFile( curFile,destFile) ) {
          QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                               +curFile +tr("to\n")+destFile);
          return;
        }
                    
        qDebug("copy "+curFile+" as "+destFile);
      }
      delete fileDlg;
    }
    populateLocalView();

  } else {
    for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
        item=(*it);
      curFile = currentRemoteDir.canonicalPath()+"/"+ item;

      fileDlg = new InputDialog(this,tr("Copy ")+curFile+tr(" As"),TRUE, 0);
      fileDlg->setInputText((const QString &) destFile);
      fileDlg->exec();
      if( fileDlg->result() == 1 ) {
        QString  filename = fileDlg->LineEdit1->text();

        destFile = currentRemoteDir.canonicalPath()+"/"+filename;

        QFile f(destFile);
        if( f.exists()) {
          switch ( QMessageBox::warning(this,tr("Delete"),
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
        if(!copyFile( curFile,destFile) ) {
          QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                               +curFile +tr("to\n")+destFile);
          return;
        }
        qDebug("copy "+curFile+" as "+destFile);
      }
      delete fileDlg;
    }
    populateRemoteView();
  }
}
 
void AdvancedFm::move() {
  qApp->processEvents();

  QStringList curFileList = getPath();
  if( curFileList.count() > 0) {
    QString curFile, destFile, item;

    if (TabWidget->getCurrentTab() == 0) {

      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
          item=(*it);
        QString destFile =  currentRemoteDir.canonicalPath();

        if(destFile.right(1).find("/",0,TRUE) == -1)
          destFile+="/";
        destFile += item;
        curFile = currentDir.canonicalPath();

        qDebug("Destination file is "+destFile);

        if(curFile.right(1).find("/",0,TRUE) == -1)
          curFile +="/";

        curFile+= item;
        qDebug("CurrentFile file is " + curFile);

        QFile f( curFile);
        if( f.exists()) {
          if(!copyFile(  curFile,destFile) ) {
            QMessageBox::message(tr("Note"),tr("Could not move\n")+curFile);
            return;
          } else
            QFile::remove(curFile);
        }
      }

      TabWidget->setCurrentTab(1);

    } else { //view 2

      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
          item = (*it);
        QString destFile = currentDir.canonicalPath();

        if(destFile.right(1).find("/",0,TRUE) == -1)
          destFile+="/";

        destFile += item;

        qDebug("Destination file is "+destFile);

        curFile = currentRemoteDir.canonicalPath();

        if(curFile.right(1).find("/",0,TRUE) == -1)
          curFile +="/";
        curFile+= item;
        qDebug("CurrentFile file is " + curFile);

        QFile f( curFile);
        if( f.exists()) {
          if(!copyFile( curFile, destFile) ) {
            QMessageBox::message(tr("Note"),tr("Could not move\n") + curFile);
            return;
          } else
            QFile::remove( curFile);
        }
        TabWidget->setCurrentTab(0);
      }
    }
    populateRemoteView();
    populateLocalView();
  }
}

bool AdvancedFm::copyFile( const QString & src, const QString & dest ) {
  char bf[ 50000 ];
  int  bytesRead;
  bool success = TRUE;
  struct stat status;

  QFile s( src );
  QFile d( dest );

  if( s.open( IO_ReadOnly | IO_Raw ) &&  d.open( IO_WriteOnly | IO_Raw ) )  {
    while( (bytesRead = s.readBlock( bf, sizeof( bf ) )) == sizeof( bf ) ) {
      if( d.writeBlock( bf, sizeof( bf ) ) != sizeof( bf ) ){
        success = FALSE;
        break;
      }
    }
    if( success && (bytesRead > 0) ){
      d.writeBlock( bf, bytesRead );
    }
  } else {
    success = FALSE;
  }

  // Set file permissions
  if( stat( (const char *) src, &status ) == 0 ){
    chmod( (const char *) dest, status.st_mode );
  }

  return success;
}

void AdvancedFm::runCommand() {
    QString curFile;
    if (TabWidget->getCurrentTab() == 0) {
        if( Local_View->currentItem())
            curFile = currentDir.canonicalPath() +"/"+ Local_View->currentItem()->text(0);
    } else {
        if(Remote_View->currentItem())
            curFile = currentRemoteDir.canonicalPath() + "/"+Remote_View->currentItem()->text(0);
    }

    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Run Command"),TRUE, 0);
    fileDlg->setInputText(curFile);
    fileDlg->exec();
      //QString command;

    if( fileDlg->result() == 1 ) {
        qDebug(fileDlg->LineEdit1->text());
        QStringList command;

        command << "/bin/sh";
        command << "-c";
        command <<  fileDlg->LineEdit1->text();
        Output *outDlg;
        outDlg = new Output( command, this, tr("AdvancedFm Output"), true);
        outDlg->showMaximized();
        outDlg->exec();
        qApp->processEvents();

    }
}

void AdvancedFm::runCommandStd() {
  QString curFile;
  if (TabWidget->getCurrentTab() == 0) {
    if( Local_View->currentItem())
      curFile = currentDir.canonicalPath() +"/"+  Local_View->currentItem()->text(0);
  } else {
    if(Remote_View->currentItem())
      curFile = currentRemoteDir.canonicalPath() +"/"
          +  Remote_View->currentItem()->text(0);
  }

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
  QString curFile;
  if (TabWidget->getCurrentTab() == 0) {
    curFile = Local_View->currentItem()->text(0);
  } else {
    curFile = Remote_View->currentItem()->text(0);
  }

    QStringList command;
    command << "/bin/sh";
    command << "-c";
    command << "stat -l "+ curFile;

    Output *outDlg;
    outDlg = new Output( command, this, tr("AdvancedFm Output"), true);
    outDlg->showMaximized();
    outDlg->exec();
    qApp->processEvents();


//   Output *outDlg;
//   outDlg = new Output(this, tr("AdvancedFm Output"),FALSE);
//   outDlg->showMaximized();
//   outDlg->show();
//   qApp->processEvents();

//   FILE *fp;
//   char line[130];
//   sleep(1);
//   fp = popen(  (const char *) command, "r");
//   if ( !fp ) {
//     qDebug("Could not execute '" + command + "'! err=%d", fp);
//     QMessageBox::warning( this, "AdvancedFm", tr("command failed!"), tr("&OK") );
//     pclose(fp);
//     return;
//   } else {
//     while ( fgets( line, sizeof line, fp)) {
//       outDlg->OutputEdit->append(line);
//       outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);

//     }

//   }
//   pclose(fp);
}


void AdvancedFm::mkDir() {
  if (TabWidget->getCurrentTab() == 0)
    localMakDir();
  else
    remoteMakDir();

}

void AdvancedFm::rn() {
  if (TabWidget->getCurrentTab() == 0)
    localRename();
  else
    remoteRename();

}

void AdvancedFm::del() {
  if (TabWidget->getCurrentTab() == 0)
    localDelete();
  else
    remoteDelete();
}

void AdvancedFm::mkSym() {
    QString cmd;
    QStringList curFileList = getPath();
    if( curFileList.count() > 0) {

        if (TabWidget->getCurrentTab() == 0) {
            for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

                QString destName = currentRemoteDir.canonicalPath()+"/"+(*it);
                if(destName.right(1) == "/") destName = destName.left( destName.length() -1);
                QString curFile =  currentDir.canonicalPath()+"/"+(*it);
                if( curFile.right(1) == "/") curFile = curFile.left( curFile.length() -1);
                cmd = "ln -s "+curFile+" "+destName;
                qDebug(cmd);
                startProcess( (const QString)cmd );
            }
            populateRemoteView();
            TabWidget->setCurrentTab(1);
        } else {
            for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

                QString destName = currentDir.canonicalPath()+"/"+(*it);
                if(destName.right(1) == "/") destName = destName.left( destName.length() -1);
                QString curFile =  currentRemoteDir.canonicalPath()+"/"+(*it);
                if( curFile.right(1) == "/") curFile = curFile.left( curFile.length() -1);

                cmd = "ln -s "+curFile+" "+destName;
                qDebug(cmd);
                startProcess( (const QString)cmd );
            }
            populateLocalView();
            TabWidget->setCurrentTab(0);
        }
    }
}

void AdvancedFm::doBeam() {
  Ir ir;
  if(!ir.supported()){
  } else {

    QStringList curFileList = getPath();
    if( curFileList.count() > 0) {

      if (TabWidget->getCurrentTab() == 0) {
        for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

          QString curFile =  currentDir.canonicalPath()+"/"+(*it);
          if( curFile.right(1) == "/") curFile = curFile.left( curFile.length() -1);
          Ir *file = new Ir(this, "IR");
          connect(file, SIGNAL(done(Ir*)), this, SLOT( fileBeamFinished( Ir * )));
          file->send( curFile, curFile );
        }

      }  else {
        for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

          QString curFile =  currentRemoteDir.canonicalPath()+"/"+(*it);
          if( curFile.right(1) == "/") curFile = curFile.left( curFile.length() -1);
          Ir *file = new Ir(this, "IR");
          connect(file, SIGNAL(done(Ir*)), this, SLOT( fileBeamFinished( Ir * )));
          file->send( curFile, curFile );

        }
      }
    }
  }

}

void AdvancedFm::fileBeamFinished( Ir *) {
  QMessageBox::message( tr("Advancedfm Beam out"), tr("Ir sent.") ,tr("Ok") );

}

void AdvancedFm::selectAll() {
    if (TabWidget->getCurrentTab() == 0) {
        Local_View->selectAll(true);
        Local_View->setSelected( Local_View->firstChild(),false);
    } else {
        Remote_View->selectAll(true);
        Remote_View->setSelected( Remote_View->firstChild(),false);
    }
}

void AdvancedFm::startProcess(const QString & cmd) {
      QStringList command;
      OProcess *process;
      process = new OProcess();
      connect(process, SIGNAL(processExited(OProcess *)),
              this, SLOT( processEnded()));

      command << "/bin/sh";
      command << "-c";
      command << cmd.latin1();
      *process << command;
      if(!process->start(OProcess::NotifyOnExit) )
          qDebug("could not start process");
}

void AdvancedFm::processEnded() {
  populateLocalView();
  populateRemoteView();
}
