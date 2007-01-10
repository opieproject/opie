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
#include "fileInfoDialog.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/lnkproperties.h>
#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
using namespace Opie::Core;

/* QT*/

#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qlistview.h>
#include <qlabel.h>

/* STD */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <fcntl.h>

void AdvancedFm::doDirChange() {
   QString pathItem = CurrentView()->currentItem()->text(0);
   if( pathItem == "../") {
      ListClicked( CurrentView()->currentItem());
   } else {
      if( pathItem.find(" -> ",0,TRUE) != -1)
         pathItem = dealWithSymName((const QString&)pathItem)+"/";
//      owarn << pathItem << oendl;
      changeTo( CurrentDir()->path()+"/"+pathItem.left( pathItem.length() - 1) );
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
//      odebug << fileInfo.owner() << oendl;

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
//          odebug << execStr << oendl;
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
         msg=tr("<p>Really delete %1 files?</p>").arg(count);
         switch ( QMessageBox::warning(this,tr("Delete"),msg
                                       ,tr("Yes"),tr("No"),0,0,1) )
         {
         case 0:
            doMsg=false;
            break;
         case 1:
            return;
            break;
				 default:
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
                                           tr("<p>Really delete %1 and all it's contents?</p>" ).arg( f ) ,
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
						default:
							break;
            };

         } else {
					 if(doMsg) {
						 switch ( QMessageBox::warning(this,tr("Delete"),
									 tr("<p>Really delete %1?</p>").arg( myFile ),
									 tr("Yes"), tr("No"), 0, 0, 1) ) {
						 case 0:
							 break;
						 case 1:
							 return;
							 break;
						 default:
							 return;
							 break;
						 };
					 }

            QString cmd="rm "+f;
            QFile file(f);
            QFileInfo fi(myFile);
            if( fi.fileName().find("../",0,TRUE)==-1) {
//               odebug << "remove link files "+myFile << oendl;

//               DocLnk lnk(f);
                 DocLnk *lnk;
                 lnk = new DocLnk(f);
//                 odebug << "Deleting doclnk " + lnk->linkFile() << oendl;
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

//   odebug << "" << curFileList.count() << "" << oendl;

   for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
//      odebug << (filePath+*it) << oendl;
      DocLnk lnk( (filePath+*it));
      LnkProperties prop( &lnk );
      QPEApplication::execDialog( &prop );
   }
#endif

}

void AdvancedFm::upDir() {

  QDir dir( CurrentDir()->canonicalPath());
  dir.cdUp();
	changeTo(dir.canonicalPath());
}

void AdvancedFm::copyTimer() {
		QTimer::singleShot(125,this,SLOT(copy()));
}

void AdvancedFm::copy() {
		QStringList curFileList = getPath();

		QDir *thisDir = CurrentDir();
		QDir *thatDir = OtherDir();

		bool doMsg = true;
		int count = curFileList.count();
		if( count > 0) {
				if(count > 1 ){
						QString msg;
						msg=tr("<p>Really copy %1 files?</p>").arg(count);
						switch ( QMessageBox::warning(this,tr("Copy"),msg
																					,tr("Yes"),tr("No"),0,0,1) )
							{
							case 0:
								doMsg=false;
								break;
							case 1:
								return;
								break;
							default:
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

//          odebug << "Destination file is "+destFile << oendl;
//          odebug << "CurrentFile file is " + curFile << oendl;

						QFile f(destFile);
						if( f.exists()) {
								if(doMsg) {
									switch ( QMessageBox::warning(this,tr("File Exists!"),
																								tr("<p>%1 already  exists. Ok to overwrite?</P>").arg(item),
																								tr("Yes"),tr("No"),0,0,1)) {
										case 0:
										break;
									case 1:
										return;
										break;
									default:
										return;
										break;
									};
								}
								f.remove();
						}

						if( !copyFile( curFile, destFile) )  {
								QMessageBox::message("AdvancedFm",
																		 tr( "<P>Could not copy %1 to %2</P>").arg(curFile).arg(destFile));
								return;
						}
				}
				rePopulate();
		}
}

void AdvancedFm::copyAsTimer() {
		QTimer::singleShot(125,this,SLOT(copyAs()));
}

void AdvancedFm::copyAs() {

   QStringList curFileList = getPath();
   QString curFile, item;
   InputDialog *fileDlg;

   QDir *thisDir = CurrentDir();
   QDir *thatDir = OtherDir();

   for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it ) {
      QString destFile;
      item=(*it);
      curFile = thisDir->canonicalPath()+"/"+(*it);
      fileDlg = new InputDialog( this, tr("Copy %1 As").arg(curFile), TRUE, 0);

      fileDlg->setInputText((const QString &) destFile );
      fileDlg->exec();

      if( fileDlg->result() == 1 ) {
         QString  filename = fileDlg->LineEdit1->text();
         destFile = thatDir->canonicalPath()+"/"+filename;

         QFile f( destFile);
         if( f.exists()) {
					 switch (QMessageBox::warning(this,tr("File Exists!"),
																				tr("<P> %1 already exists. Ok to overwrite?</p>").arg(item),
																				tr("Yes"),tr("No"),0,0,1) ) {
					 case 0:
						 f.remove();
						 break;
					 case 1:
						 return;
						 break;
					 default:
						 return;
             break;
					 };
         }
         if( !copyFile( curFile, destFile) ) {
            QMessageBox::message("AdvancedFm",tr("<p>Could not copy %1 to %2</P>").arg(curFile).arg(destFile));
            return;
         }
      }
      delete fileDlg;

   }
	 rePopulate();
//	 setOtherTabCurrent();
   qApp->processEvents();

}

void AdvancedFm::copySameDirTimer() {
		QTimer::singleShot(125,this,SLOT(copySameDir()));
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
																				 tr("<p> %1 already exists. Do you really want to delete it?</P>").arg(destFile),
																				 tr("Yes"),tr("No"),0,0,1) ) {
						case 0:
							f.remove();
							break;
						case 1:
							return;
							break;
						default:
							return;
							break;
						};
            }
          if(!copyFile( curFile,destFile) )  {
              QMessageBox::message("AdvancedFm",tr("<P>Could not copy %1 to %2</P>").arg(curFile).arg(destFile));
              return;
            }

//          odebug << "copy "+curFile+" as "+destFile << oendl;
        }
      delete fileDlg;
    }
            rePopulate();
}

void AdvancedFm::moveTimer() {
		QTimer::singleShot(125,this,SLOT(move()));
}

void AdvancedFm::move() {

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
//          odebug << "Destination file is "+destFile << oendl;

						curFile = thisDir->canonicalPath();
						if(curFile.right(1).find("/",0,TRUE) == -1)
								curFile +="/";
						curFile+= item;
//           odebug << "CurrentFile file is " + curFile << oendl;

						if(QFileInfo(curFile).isDir()) {
								moveDirectory( curFile, destFile );
								rePopulate();
								return;
						}
						QFile f( destFile);
						if( f.exists()) {
							switch ( QMessageBox::warning(this,tr("File Exists!"),
																						tr("<p>%1 already  exists. Ok to overwrite?</P>").arg(destFile),
																						tr("Yes"),tr("No"),0,0,1)) {
							case 0:
								break;
							case 1:
								return;
								break;
							default:
								return;
								break;
							};
						}
								if( !copyFile( curFile, destFile) )  {
										QMessageBox::message(tr("Note"),tr("<p>Could not move %1</p>").arg(curFile));
										return;
								} else
										QFile::remove(curFile);
				}
		}
		rePopulate();
//	setOtherTabCurrent();
}

bool AdvancedFm::moveDirectory( const QString & src, const QString & dest ) {
		int err = 0;
		if( copyDirectory( src, dest ) ) {
				QString cmd = "rm -rf " + src;
				err = system((const char*)cmd);
		} else
				err = -1;

		if(err!=0) {
				QMessageBox::message(tr("Note"),tr("<p>Could not move %1</p>").arg( src));
				return false;
		}
		return true;
}

bool AdvancedFm::copyDirectory( const QString & src, const QString & dest ) {

		QString cmd = "/bin/cp -fpR " + src + " " + dest;
    owarn << cmd << oendl;
		int    err = system( (const char *) cmd );
		if ( err != 0 ) {
				QMessageBox::message("AdvancedFm", tr( "<p>Could not copy %1 to %2</p>").arg( src ).arg( dest ) );
				return false;
		}

		return true;
}


bool AdvancedFm::copyFile( const QString & src, const QString & dest ) {
		if(QFileInfo(src).isDir()) {
				if( copyDirectory( src, dest )) {
//						setOtherTabCurrent();
						rePopulate();
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
//         owarn << "open failed" << oendl;
				return success = false;
		}
		read_fd = srcFile.handle();
		if(read_fd != -1) {
				fstat (read_fd, &stat_buf);
				if( !destFile.open( IO_WriteOnly|IO_Raw ) ) {
//       owarn << "destfile open failed" << oendl;
						return success = false;
				}
				write_fd = destFile.handle();
				if(write_fd != -1) {
						err = sendfile(write_fd, read_fd, &offset, stat_buf.st_size);
						if( err == -1) {
								QString msg;
								switch(errno) {
									case EBADF : msg = "The input file was not opened for reading or the output file was not opened for writing. ";
									case EINVAL: msg = "Descriptor is not valid or locked. ";
									case ENOMEM: msg = "Insufficient memory to read from in_fd.";
									case EIO: msg = "Unspecified error while reading from in_fd.";
								};
								success = false;
//              owarn << msg << oendl;
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
		if( stat( QFile::encodeName(src), &status ) == 0 )  {
				chmod( QFile::encodeName(dest), status.st_mode );
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
//      odebug << fileDlg->LineEdit1->text() << oendl;
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
	
	QFileInfo curFileInfo(curFile);
	
	FileInfoDialog *infoDlg = new FileInfoDialog(this);
	infoDlg->setCaption(tr("Info for %1").arg(curFile));
	
	uint size = curFileInfo.size();
	QString sizestr;
	if( size > 1048576 )
		sizestr = tr("%1MB (%2 bytes)").arg(QString().sprintf("%.0f", size / 1048576.0)).arg(size);
	else if( size > 1024 )
		sizestr = tr("%1kB (%2 bytes)").arg(QString().sprintf("%.0f", size / 1024.0)).arg(size);
	else
		sizestr = tr("%1 bytes").arg(size);
			
	infoDlg->sizeLabel->setText(sizestr);
	
	if(curFileInfo.isSymLink())
		infoDlg->typeLabel->setText(tr("symbolic link"));
	else if(curFileInfo.isFile()) {
		if(curFileInfo.isExecutable())
			infoDlg->typeLabel->setText(tr("executable file"));
		else
			infoDlg->typeLabel->setText(tr("file"));
	}
	else if(curFileInfo.isDir())
		infoDlg->typeLabel->setText(tr("directory"));
	else
		infoDlg->typeLabel->setText(tr("unknown"));
	
	infoDlg->ownerLabel->setText( QString("%1 (%2)").arg(curFileInfo.owner()).arg(curFileInfo.ownerId()) );
	infoDlg->groupLabel->setText( QString("%1 (%2)").arg(curFileInfo.group()).arg(curFileInfo.groupId()) );
	
	infoDlg->lastReadLabel->setText( curFileInfo.lastRead().toString() );
	infoDlg->lastModifiedLabel->setText( curFileInfo.lastModified().toString() );
	
	QString perms;
	// User
	if(curFileInfo.permission(QFileInfo::ReadUser))
		perms += "r";
	else
		perms += "-";
	if(curFileInfo.permission(QFileInfo::WriteUser))
		perms += "w";
	else
		perms += "-";
	if(curFileInfo.permission(QFileInfo::ExeUser))
		perms += "x";
	else
		perms += "-";
	// Group
	if(curFileInfo.permission(QFileInfo::ReadGroup))
		perms += "r";
	else
		perms += "-";
	if(curFileInfo.permission(QFileInfo::WriteGroup))
		perms += "w";
	else
		perms += "-";
	if(curFileInfo.permission(QFileInfo::ExeGroup))
		perms += "x";
	else
		perms += "-";
	// Other
	if(curFileInfo.permission(QFileInfo::ReadOther))
		perms += "r";
	else
		perms += "-";
	if(curFileInfo.permission(QFileInfo::WriteOther))
		perms += "w";
	else
		perms += "-";
	if(curFileInfo.permission(QFileInfo::ExeOther))
		perms += "x";
	else
		perms += "-";
	infoDlg->permsLabel->setText( perms );
	
	QPEApplication::execDialog( infoDlg );

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
//          odebug << cmd << oendl;
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
  connect(process,SIGNAL(processExited(Opie::Core::OProcess*)),this,SLOT(processEnded(Opie::Core::OProcess*)));
  connect(process,SIGNAL(receivedStderr(Opie::Core::OProcess*,char*,int)),this,SLOT(oprocessStderr(Opie::Core::OProcess*,char*,int)));

  command << "/bin/sh";
  command << "-c";
  command << cmd.latin1();
  *process << command;
  if(!process->start(OProcess::NotifyOnExit, OProcess::All) )
    odebug << "could not start process" << oendl;
}

void AdvancedFm::processEnded(OProcess *) {
            rePopulate();
}

void AdvancedFm::oprocessStderr(OProcess*, char *buffer, int ) {
//    owarn << "received stderrt " << buflen << " bytes" << oendl;

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
               whichTab = 1;
 				       viewMenu->setItemChecked(viewMenu->idAt(0), true);
 				       viewMenu->setItemChecked(viewMenu->idAt(1), false);
            } else {
               whichTab = 2;
 				       viewMenu->setItemChecked(viewMenu->idAt(0), false);
 				       viewMenu->setItemChecked(viewMenu->idAt(1), true);
            }
       }
       OtherView()->setSelected( OtherView()->currentItem(), FALSE );//make sure there's correct selection
     }

  return QWidget::eventFilter( o, e );
}


void AdvancedFm::cancelRename() {
//  odebug << "cancel rename" << oendl;
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
		qDebug("okrename");
		if( !renameBox) return;

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
		QListViewItem *item = view->currentItem();
		view->takeItem( item );
		delete item;
		populateView();
		
}

void AdvancedFm::openSearch() {
    QMessageBox::message(tr("Note"),tr("Not Yet Implemented"));
}
