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
#include "inputDialog.h"
#include "output.h"
#include "filePermissions.h"

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
    showHidden();
    if(b) b= false; else b=true;
    showRemoteHidden();
}

void AdvancedFm::showHidden() {
  if (b) {
    currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
    fileMenu->setItemChecked( fileMenu->idAt(0),TRUE);
    b=FALSE;

  }  else {
    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    fileMenu->setItemChecked( fileMenu->idAt(0),FALSE);
    b=TRUE;
  }
  populateLocalView();

}

void AdvancedFm::showRemoteHidden() {
  if (b) {
    currentRemoteDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
    b=TRUE;

  }  else {
    currentRemoteDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    b=FALSE;
  }
  populateRemoteView();
}

void AdvancedFm::runThis() {
  QString fs;
  if (TabWidget->currentPageIndex() == 0) {
    QString curFile = Local_View->currentItem()->text(0);
    if(curFile != "../") {

      fs= getFileSystemType((const QString &) currentDir.canonicalPath());
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
  if (TabWidget->currentPageIndex() == 0) {
    QString curFile = Local_View->currentItem()->text(0);
    if(curFile != "../") {
      curFile =  currentDir.canonicalPath()+"/"+curFile;
      QCopEnvelope e("QPE/Application/textedit", "setDocument(QString)" );
      e << curFile;
    }
  } else {
    QString curFile = Remote_View->currentItem()->text(0);
    if(curFile != "../") {
      curFile =  currentRemoteDir.canonicalPath()+"/"+curFile;
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
  if(curFileList.count() > 0) {
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
        switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f+
                                      "\nand all it's contents ?"
                                      ,tr("Yes"),tr("No"),0,0,1) ) {
        case 0: {
          f=f.left(f.length()-1);
          QString cmd="rm -rf "+f;
          system( cmd.latin1());
          populateLocalView();
        }
          break;
        case 1:
          // exit
          break;
        };

      } else {
        switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f
                 +" ?",tr("Yes"),tr("No"),0,0,1) ) {
        case 0: {
          QString cmd="rm "+f;
          QFile file(f);
          file.remove();
          //                   system( cmd.latin1());
          populateLocalView();
        }
          break;
        case 1:
          // exit
          break;
        };
      }
    }
  }
}

void AdvancedFm::remoteDelete() {
  QStringList curFileList = getPath();
  if( curFileList.count() > 0) {
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
        switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f+
                 "\nand all it's contents ?",
                 tr("Yes"),tr("No"),0,0,1) ) {
        case 0: {
          f=f.left(f.length()-1);
          QString cmd="rm -rf "+f;
          system( cmd.latin1());
          populateRemoteView();
        }
          break;
        case 1:
          // exit
          break;
        };

      } else {
        switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f
                 +" ?",tr("Yes"),tr("No"),0,0,1) ) {
        case 0: {
          QString cmd="rm "+f;
          QFile file(f);
          file.remove();
          //                   system( cmd.latin1());
          populateRemoteView();
        }
          break;
        case 1:
          // exit
          break;
        };
      }
    }
  }
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

  if (TabWidget->currentPageIndex() == 0) {
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
  if (TabWidget->currentPageIndex() == 0) {
    populateLocalView();
  } else {
    populateRemoteView();
  }
}

void AdvancedFm::doProperties() {
  QStringList curFileList = getPath();
  QString filePath;
  if (TabWidget->currentPageIndex() == 0) {
    filePath = currentDir.canonicalPath()+"/";
  } else {
    filePath= currentRemoteDir.canonicalPath()+"/";
  }
  //    qDebug("%d",curFileList.count());
  for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
    qDebug((filePath+*it));
    DocLnk lnk( (filePath+*it));
    LnkProperties prop( &lnk );
    prop.showMaximized();
    prop.exec();
  }
}

void AdvancedFm::upDir() {
  if (TabWidget->currentPageIndex() == 0) {
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
  if( curFileList.count() > 0) {
    QString curFile, item, destFile;
    if (TabWidget->currentPageIndex() == 0) {
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
              switch ( QMessageBox::warning(this,tr("Delete"),
                       destFile+tr(" already exists\nDo you really want to delete it?"),
                       tr("Yes"),tr("No"),0,0,1) ) {
          case 0: 
            f.remove();
            break;
          case 1:
            return;
            break;
          };
        }
        if(!copyFile(destFile, curFile) ) {
          QMessageBox::message("AdvancedFm","Could not copy\n"+curFile +"to\n"+destFile);
          return;
        }
      }
      populateRemoteView();
      TabWidget->setCurrentPage(1);

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
              switch ( QMessageBox::warning(this,tr("Delete"),
                   destFile+tr(" already exists\nDo you really want to delete it?"),
                   tr("Yes"),tr("No"),0,0,1) ) {
          case 0: 
            f.remove();
            break;
          case 1:
            return;
            break;
          };
        }
        if(!copyFile(destFile, curFile) ) {
          QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                               +curFile +tr("to\n")+destFile);
          return;

        }
      }
      populateLocalView();
      TabWidget->setCurrentPage(0);
    }

  }
}

void AdvancedFm::copyAs() {
  qApp->processEvents();

  QStringList curFileList = getPath();
  QString curFile;
  InputDialog *fileDlg;
  if (TabWidget->currentPageIndex() == 0) {
    qDebug("tab 1");
    for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
      QString destFile;
      curFile = currentDir.canonicalPath()+"/"+(*it);
      fileDlg = new InputDialog( this,tr("Copy "+curFile+" As"), TRUE, 0);

      fileDlg->setInputText((const QString &) destFile );
      fileDlg->exec();

      if( fileDlg->result() == 1 ) {
        QString  filename = fileDlg->LineEdit1->text();
        destFile = currentRemoteDir.canonicalPath()+"/"+filename;

        QFile f(destFile);
        if( f.exists()) {
          switch (QMessageBox::warning(this,tr("Delete"),
                  destFile+tr(" already exists\nDo you really want to delete it?"),
                  tr("Yes"),tr("No"),0,0,1) ) {
          case 0: 
            f.remove();
            break;
          case 1:
            return;
            break;
          };
        }
        if(!copyFile(destFile, curFile) ) {
          QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                               +curFile +tr("to\n")+destFile);
          return;
        }
      }
      delete fileDlg;

    }
    populateRemoteView();
    TabWidget->setCurrentPage(1);

  } else {
    for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

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
          switch ( QMessageBox::warning(this,tr("Delete"),
                   destFile+tr(" already exists\nDo you really want to delete it?"),
                   tr("Yes"),tr("No"),0,0,1) ) {
          case 0: 
            f.remove();
            break;
          case 1:
            return;
            break;
          };
        }
        if(!copyFile(destFile, curFile) ) {
          QMessageBox::message("AdvancedFm",tr("Could not copy\n")
                               +curFile +tr("to\n")+destFile);
          return;
        }
                        
      }
      delete fileDlg;

    }
    populateLocalView();
    TabWidget->setCurrentPage(0);
  }
}

void AdvancedFm::copySameDir() {
  qApp->processEvents();
  QStringList curFileList = getPath();
  QString curFile, item, destFile;
  InputDialog *fileDlg;

  if (TabWidget->currentPageIndex() == 0) {

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
                  destFile+tr(" already exists\nDo you really want to delete it?"),
                  tr("Yes"),tr("No"),0,0,1) ) {
          case 0:

            f.remove();
            break;
          case 1:
            return;
            break;
          };
        }
        if(!copyFile(destFile, curFile) ) {
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
                   destFile+tr(" already exists\nDo you really want to delete it?"),
                   tr("Yes"),tr("No"),0,0,1) ) {
          case 0: 
            f.remove();
            break;
          case 1:
            return;
            break;
          };
        }
        if(!copyFile(destFile, curFile) ) {
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

    if (TabWidget->currentPageIndex() == 0) {

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
          if(!copyFile( destFile, curFile) ) {
            QMessageBox::message(tr("Note"),tr("Could not move\n")+curFile);
            return;
          } else
            QFile::remove(curFile);
        }
      }

      TabWidget->setCurrentPage(1);

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
          if(!copyFile( destFile, curFile) ) {
            QMessageBox::message(tr("Note"),tr("Could not move\n") + curFile);
            return;
          } else
            QFile::remove( curFile);
        }
        TabWidget->setCurrentPage(0);
      }
    }
    populateRemoteView();
    populateLocalView();
  }
}

bool AdvancedFm::copyFile( const QString & dest, const QString & src ) {
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
  if (TabWidget->currentPageIndex() == 0) {
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
  QString command;
  if( fileDlg->result() == 1 ) {
    command = fileDlg->LineEdit1->text();

    Output *outDlg;
    outDlg = new Output(this, tr("AdvancedFm Output"),FALSE);
    outDlg->showMaximized();
    outDlg->show();
    qApp->processEvents();
    FILE *fp;
    char line[130];
    sleep(1);
    command +=" 2>&1";
    fp = popen(  (const char *) command, "r");
    if ( !fp ) {
      qDebug("Could not execute '" + command + "'! err=%d", fp);
      QMessageBox::warning( this, "AdvancedFm", tr("command failed!"), tr("&OK") );
      pclose(fp);
      return;
    } else {
      while ( fgets( line, sizeof line, fp)) {
        QString lineStr = line;
        lineStr=lineStr.left(lineStr.length()-1);
        outDlg->OutputEdit->append(lineStr);
        outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
      }
    }
    pclose(fp);

  }
}

void AdvancedFm::runCommandStd() {
  QString curFile;
  if (TabWidget->currentPageIndex() == 0) {
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
  QString command;
  if( fileDlg->result() == 1 ) {
    qApp->processEvents();
    command = fileDlg->LineEdit1->text() + " &";
    system(command.latin1());
  }
}

void AdvancedFm::fileStatus() {
  QString curFile;
  if (TabWidget->currentPageIndex() == 0) {
    curFile = Local_View->currentItem()->text(0);
  } else {
    curFile = Remote_View->currentItem()->text(0);
  }
  QString command = " stat -l "+ curFile +" 2>&1";
  Output *outDlg;
  outDlg = new Output(this, tr("AdvancedFm Output"),FALSE);
  outDlg->showMaximized();
  outDlg->show();
  qApp->processEvents();
  FILE *fp;
  char line[130];
  sleep(1);
  fp = popen(  (const char *) command, "r");
  if ( !fp ) {
    qDebug("Could not execute '" + command + "'! err=%d", fp);
    QMessageBox::warning( this, "AdvancedFm", tr("command failed!"), tr("&OK") );
    pclose(fp);
    return;
  } else {
    while ( fgets( line, sizeof line, fp)) {
      outDlg->OutputEdit->append(line);
      outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);

    }

  }
  pclose(fp);
}

void AdvancedFm::mkDir() {
  if (TabWidget->currentPageIndex() == 0)
    localMakDir();
  else
    remoteMakDir();

}

void AdvancedFm::rn() {
  if (TabWidget->currentPageIndex() == 0)
    localRename();
  else
    remoteRename();

}

void AdvancedFm::del() {
  if (TabWidget->currentPageIndex() == 0)
    localDelete();
  else
    remoteDelete();
}

void AdvancedFm::mkSym() {
  QString cmd;
  QStringList curFileList = getPath();
  if( curFileList.count() > 0) {

    if (TabWidget->currentPageIndex() == 0) {
      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

        QString destName = currentRemoteDir.canonicalPath()+"/"+(*it);
        if(destName.right(1) == "/") destName = destName.left( destName.length() -1);
        QString curFile =  currentDir.canonicalPath()+"/"+(*it);
        if( curFile.right(1) == "/") curFile = curFile.left( curFile.length() -1);
        cmd = "ln -s "+curFile+" "+destName;
        qDebug(cmd);
        system(cmd.latin1() );
      }
      populateRemoteView();
      TabWidget->setCurrentPage(1);
    } else {
      for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {

        QString destName = currentDir.canonicalPath()+"/"+(*it);
        if(destName.right(1) == "/") destName = destName.left( destName.length() -1);
        QString curFile =  currentRemoteDir.canonicalPath()+"/"+(*it);
        if( curFile.right(1) == "/") curFile = curFile.left( curFile.length() -1);

        cmd = "ln -s "+curFile+" "+destName;
        qDebug(cmd);
        system(cmd.latin1() );
      }
      populateLocalView();
      TabWidget->setCurrentPage(0);
    }
  }
}

void AdvancedFm::doBeam() {
  Ir ir;
  if(!ir.supported()){
  } else {

    QStringList curFileList = getPath();
    if( curFileList.count() > 0) {

      if (TabWidget->currentPageIndex() == 0) {
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
