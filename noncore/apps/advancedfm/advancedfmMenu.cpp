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


void AdvancedFm::doDirChange()
{
  ListClicked( CurrentView()->currentItem());
}

void AdvancedFm::showMenuHidden()
{
  if (b)
		{
			CurrentDir()->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
			OtherDir()->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
			fileMenu->setItemChecked( fileMenu->idAt(0),TRUE);
//    b=FALSE;

		}
	else
		{
			CurrentDir()->setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
			OtherDir()->setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
			fileMenu->setItemChecked( fileMenu->idAt(0),FALSE);
//    b=TRUE;
		}
  rePopulate();
//    if(b) qDebug("<<<<<<<<<<<<<<<<<<<<<<<<<<<< true");
    if(b) b = false; else b = true;
}

void AdvancedFm::showHidden()
{
  if (b)
		{
			CurrentDir()->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
			OtherDir()->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
//     fileMenu->setItemChecked( fileMenu->idAt(0),TRUE);
//    b=FALSE;

		}
	else
		{
			CurrentDir()->setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
			OtherDir()->setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
//     fileMenu->setItemChecked( fileMenu->idAt(0),FALSE);
//    b=TRUE;
		}
  rePopulate();
}

QString AdvancedFm::dealWithSymName(const QString &fileName)
{
        QString strItem = fileName;
        return  strItem.right( (strItem.length() - strItem.find("->",0,TRUE)) - 4);
}

void AdvancedFm::runThis()
{
	QString fs;
	QDir *thisDir = CurrentDir();

	QString curFile = CurrentView()->currentItem()->text(0);
	QString path = thisDir->canonicalPath();

	if( curFile.find("@",0,TRUE) !=-1 || curFile.find("->",0,TRUE) !=-1 )  //if symlink

		curFile = dealWithSymName((const QString&)curFile);

	if(curFile != "../")
		{

			fs = getFileSystemType((const QString &) path);
			QFileInfo fileInfo( path  + "/" + curFile);
			qDebug( fileInfo.owner());

			if( (fileInfo.permission( QFileInfo::ExeUser)
					 | fileInfo.permission( QFileInfo::ExeGroup)
					 | fileInfo.permission( QFileInfo::ExeOther)) // & fs.find("vfat",0,TRUE) == -1) {
					| fs == "vfat" && fileInfo.filePath().contains("/bin") ) {
				QCopEnvelope e("QPE/System", "execute(QString)" );
				e << curFile;
			}
			else
				{
					curFile =  path + "/" + curFile;
					DocLnk nf(curFile);
					QString execStr = nf.exec();
					qDebug( execStr);
					if( execStr.isEmpty() )
						{
						}
					else
						{
							nf.execute();
						}
				}
		}
}

void AdvancedFm::runText()
{
	QString curFile = CurrentView()->currentItem()->text(0);
	if(curFile != "../")
		{
			if( curFile.find("@",0,TRUE) !=-1 || curFile.find("->",0,TRUE) !=-1 )  //if symlink
        curFile = dealWithSymName((const QString&)curFile);
      curFile =  CurrentDir()->canonicalPath()+"/"+curFile;
      QCopEnvelope e("QPE/Application/textedit", "setDocument(QString)" );
      e << curFile;
    }
}

void AdvancedFm::makeDir()
{
  InputDialog *fileDlg;
  fileDlg = new InputDialog(this,tr("Make Directory"),TRUE, 0);
  fileDlg->exec();
  if( fileDlg->result() == 1 )
		{
			QDir *thisDir = CurrentDir();
			QString  filename = fileDlg->LineEdit1->text();
			thisDir->mkdir( thisDir->canonicalPath()+"/"+filename);
  }
  populateView();
}

void AdvancedFm::doDelete()
{

  QStringList curFileList = getPath();
  bool doMsg=true;
  int count = curFileList.count();
  if( count > 0)
		{
      if(count > 1 )
				{
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
    for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
			{
      myFile = (*it);
      if( myFile.find(" -> ",0,TRUE) != -1)
        myFile = myFile.left( myFile.find(" -> ",0,TRUE));

      QString f = CurrentDir()->canonicalPath();
      if(f.right(1).find("/",0,TRUE) == -1)
        f += "/";
      f += myFile;
      if(QDir(f).exists() && !QFileInfo(f).isSymLink() )
				{
        switch ( QMessageBox::warning( this, tr("Delete Directory?"), tr("Really delete\n") + f +
                                      "\nand all it's contents ?"
                                      ,tr("Yes"),tr("No"),0,0,1) )
					{
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
  populateView();
}

void AdvancedFm::filePerms()
{
	QStringList curFileList = getPath();
  QString filePath;

	filePath = CurrentDir()->canonicalPath()+"/";

  for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
		{
			filePermissions *filePerm;
			filePerm = new filePermissions(this, "Permissions",true,0,(const QString &)(filePath+*it));
			filePerm->showMaximized();
			filePerm->exec();
			if( filePerm)
				delete  filePerm;
		}
	populateView();
}

void AdvancedFm::doProperties()
{
#if defined(QT_QWS_OPIE)

  QStringList curFileList = getPath();

  QString filePath;
    filePath = CurrentDir()->canonicalPath()+"/";

		qDebug("%d",curFileList.count());

   for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
		 {
    qDebug((filePath+*it));
    DocLnk lnk( (filePath+*it));
    LnkProperties prop( &lnk );
    prop.showMaximized();
    prop.exec();
  }
#endif

}

void AdvancedFm::upDir()
{
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

void AdvancedFm::copy()
{
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

    QString curFile, item, destFile;
		for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
			{
				item=(*it);
				if(item.find("->",0,TRUE)) //symlink
					item = item.left(item.find("->",0,TRUE));

				curFile = thisDir->canonicalPath()+"/"+ item;
				destFile = thatDir->canonicalPath()+"/"+ item;

				qDebug("Destination file is "+destFile);
				qDebug("CurrentFile file is " + curFile);

				QFile f(destFile);
				if( f.exists())
					{
						if(doMsg)
							{
								switch ( QMessageBox::warning(this,tr("File Exists!"),
																							tr("%1 exists. Ok to overwrite?").arg( item ),
																							tr("Yes"),tr("No"),0,0,1) )
									{
									case 1:
										return;
										break;
                  };
              }
						f.remove();
          }

				if( !copyFile( curFile, destFile) )
					{
						QMessageBox::message("AdvancedFm",
																 tr( "Could not copy %1 to %2").arg( curFile ).arg( destFile ) );
						return;
					}
      }
		setOtherTabCurrent();
		populateView();
   }
}

void AdvancedFm::copyAs()
{
  qApp->processEvents();

  QStringList curFileList = getPath();
  QString curFile, item;
  InputDialog *fileDlg;

	QDir *thisDir = CurrentDir();
	QDir *thatDir = OtherDir();

	for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
		{
			QString destFile;
      item=(*it);
      curFile = thisDir->canonicalPath()+"/"+(*it);
      fileDlg = new InputDialog( this, tr("Copy "+curFile+" As"), TRUE, 0);

      fileDlg->setInputText((const QString &) destFile );
      fileDlg->exec();

      if( fileDlg->result() == 1 )
				{
					QString  filename = fileDlg->LineEdit1->text();
					destFile = thatDir->canonicalPath()+"/"+filename;

					QFile f( destFile);
					if( f.exists())
						{
							switch (QMessageBox::warning(this,tr("File Exists!"),
																					 item+tr("\nexists. Ok to overwrite?"),
																					 tr("Yes"),tr("No"),0,0,1) )
								{
								case 0:
									f.remove();
									break;
								case 1:
									return;
									break;
								};
						}
					if( !copyFile( curFile, destFile) )
						{
							QMessageBox::message("AdvancedFm",tr("Could not copy\n")
																	 +curFile +tr("to\n")+destFile);
							return;
						}
				}
      delete fileDlg;

    }
		setOtherTabCurrent();
	populateView();
}

void AdvancedFm::copySameDir()
{
  qApp->processEvents();
  QStringList curFileList = getPath();
  QString curFile, item, destFile;
  InputDialog *fileDlg;

	QDir *thisDir = CurrentDir();

	for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
		{
			item=(*it);
      curFile = thisDir->canonicalPath()+"/"+ item;

      fileDlg = new InputDialog(this,tr("Copy ")+curFile+tr(" As"),TRUE, 0);
      fileDlg->setInputText((const QString &) destFile );
      fileDlg->exec();

      if( fileDlg->result() == 1 )
				{

					QString filename = fileDlg->LineEdit1->text();
					destFile = thisDir->canonicalPath()+"/"+filename;

					QFile f(destFile);
					if( f.exists())
						{
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
					if(!copyFile( curFile,destFile) )
						{
							QMessageBox::message("AdvancedFm",tr("Could not copy\n")
																	 +curFile +tr("to\n")+destFile);
							return;
						}

					qDebug("copy "+curFile+" as "+destFile);
				}
      delete fileDlg;
    }
	populateView();
}

void AdvancedFm::move()
{
  qApp->processEvents();

  QStringList curFileList = getPath();
  if( curFileList.count() > 0)
		{
			QString curFile, destFile, item;

			QDir *thisDir = CurrentDir();
			QDir *thatDir = OtherDir();
			for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
				{
					item=(*it);
					QString destFile =  thatDir->canonicalPath();

					if(destFile.right(1).find("/",0,TRUE) == -1)
						destFile+="/";
					destFile += item;
					qDebug("Destination file is "+destFile);

					curFile = thisDir->canonicalPath();
					if(curFile.right(1).find("/",0,TRUE) == -1)
						curFile +="/";
					curFile+= item;
					qDebug("CurrentFile file is " + curFile);

					QFile f( curFile);
					if( f.exists()) {
						if( !copyFile(  curFile, destFile) )
							{
								QMessageBox::message(tr("Note"),tr("Could not move\n")+curFile);
								return;
							} else
								QFile::remove(curFile);
					}
				}

		}
		setOtherTabCurrent();
			populateView();
//			populateLocalView();
}

bool AdvancedFm::copyFile( const QString & src, const QString & dest )
{
  char bf[ 50000 ];
  int  bytesRead;
  bool success = TRUE;
  struct stat status;

  QFile s( src );
  QFile d( dest );

  if( s.open( IO_ReadOnly | IO_Raw ) &&  d.open( IO_WriteOnly | IO_Raw ) )
		{
			while( (bytesRead = s.readBlock( bf, sizeof( bf ) )) == sizeof( bf ) )
				{
				if( d.writeBlock( bf, sizeof( bf ) ) != sizeof( bf ) ){
					success = FALSE;
					break;
				}
			}
			if( success && (bytesRead > 0) )
				{
					d.writeBlock( bf, bytesRead );
				}
		}
	else
		{
			success = FALSE;
		}

		// Set file permissions
  if( stat( (const char *) src, &status ) == 0 )
		{
			chmod( (const char *) dest, status.st_mode );
		}

  return success;
}

void AdvancedFm::runCommand()
{
	QDir *thisDir = CurrentDir();

	QString curFile;
	curFile = thisDir->canonicalPath() +"/"+ CurrentView()->currentItem()->text(0);

	InputDialog *fileDlg;
	fileDlg = new InputDialog(this,tr("Run Command"),TRUE, 0);
	fileDlg->setInputText(curFile);
	fileDlg->exec();
		//QString command;

	if( fileDlg->result() == 1 )
		{
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

void AdvancedFm::runCommandStd()
{
  QString curFile;
	QDir *thisDir = CurrentDir();
	QListView *thisView = CurrentView();
	if( thisView->currentItem())
		curFile = thisDir->canonicalPath() +"/"+  thisView->currentItem()->text(0);

  InputDialog *fileDlg;
  fileDlg = new InputDialog(this,tr("Run Command"),TRUE, 0);
  fileDlg->setInputText(curFile);
  fileDlg->exec();

  if( fileDlg->result() == 1 )
		{
      qApp->processEvents();
      startProcess( (const QString)fileDlg->LineEdit1->text().latin1());
		}
}

void AdvancedFm::fileStatus()
{
  QString curFile;
	curFile = CurrentView()->currentItem()->text(0);

	QStringList command;
	command << "/bin/sh";
	command << "-c";
	command << "stat -l "+ curFile;

	Output *outDlg;
	outDlg = new Output( command, this, tr("AdvancedFm Output"), true);
	outDlg->showMaximized();
	outDlg->exec();
	qApp->processEvents();
}


void AdvancedFm::mkDir()
{
    makeDir();
}

void AdvancedFm::rn()
{
	renameIt();
}

void AdvancedFm::del()
{
    doDelete();
}

void AdvancedFm::mkSym()
{
	QString cmd;
	QStringList curFileList = getPath();
	if( curFileList.count() > 0)
		{
			QDir *thisDir = CurrentDir();
			QDir * thatDir = OtherDir();

			for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
				{

					QString destName = thatDir->canonicalPath()+"/"+(*it);
					if(destName.right(1) == "/")
						{
							destName = destName.left( destName.length() -1);
						}

					QString curFile =  thisDir->canonicalPath()+"/"+(*it);

					if( curFile.right(1) == "/")
						{
							curFile = curFile.left( curFile.length() -1);
						}

					cmd = "ln -s "+curFile+" "+destName;
					qDebug(cmd);
					startProcess( (const QString)cmd );
				}
			setOtherTabCurrent();
			populateView();
		}
}

void AdvancedFm::doBeam()
{
  Ir ir;
  if(!ir.supported())
		{
		}
	else
		{
			QStringList curFileList = getPath();
			if( curFileList.count() > 0)
				{
					for ( QStringList::Iterator it = curFileList.begin(); it != curFileList.end(); ++it )
						{

							QString curFile =  CurrentDir()->canonicalPath()+"/"+(*it);
							if( curFile.right(1) == "/")
								{
									curFile = curFile.left( curFile.length() -1);
								}
							Ir *file = new Ir(this, "IR");
							connect(file, SIGNAL(done(Ir*)), this, SLOT( fileBeamFinished( Ir * )));
							file->send( curFile, curFile );
						}
				}
		}

}

void AdvancedFm::fileBeamFinished( Ir *)
{
  QMessageBox::message( tr("Advancedfm Beam out"), tr("Ir sent.") ,tr("Ok") );
}

void AdvancedFm::selectAll()
{
//    if (TabWidget->getCurrentTab() == 0) {
	QListView *thisView = CurrentView();
	thisView->selectAll(true);
	thisView->setSelected( thisView->firstChild(),false);
//     } else {
//         Remote_View->selectAll(true);
//         Remote_View->setSelected( Remote_View->firstChild(),false);
//     }
}

void AdvancedFm::startProcess(const QString & cmd)
{
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

void AdvancedFm::processEnded()
{
//  populateLocalView();
  populateView();
}

bool AdvancedFm::eventFilter( QObject * o, QEvent * e )
{
	if ( o->inherits( "QLineEdit" ) )
		{
			if ( e->type() == QEvent::KeyPress )
				{
					QKeyEvent *ke = (QKeyEvent*)e;
					if ( ke->key() == Key_Return ||
							 ke->key() == Key_Enter )
						{
							okRename();
							return true;
            }
					else if ( ke->key() == Key_Escape )
							{
								cancelRename();
								return true;
							}
        }
			else if ( e->type() == QEvent::FocusOut )
					{
						cancelRename();
						return true;
					}
    }
	if ( o->inherits( "QListView" ) )
		{
			if ( e->type() == QEvent::FocusOut )
				{
					printf("focusIn\n");

        }
    }

	return QWidget::eventFilter( o, e );
}


void AdvancedFm::cancelRename()
{
	qDebug("cancel rename");
	QListView * view;
	view = CurrentView();

	bool resetFocus = view->viewport()->focusProxy() == renameBox;
	delete renameBox;
	renameBox = 0;
	if ( resetFocus )
		{
			view->viewport()->setFocusProxy( view);
			view->setFocus();
    }
}

void AdvancedFm::doRename(QListView * view)
{

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


void AdvancedFm::renameIt()
{
	QListView *thisView = CurrentView();
    oldName = thisView->currentItem()->text(0);
    doRename( thisView );
    populateView();
}

void AdvancedFm::okRename()
{
	QString newName = renameBox->text();
	cancelRename();
//	int tabs=0;
	QListView * view = CurrentView();
	QString path =  CurrentDir()->canonicalPath() + "/";
	oldName = path + oldName;
	newName = path + newName;

	if(  view->currentItem() == NULL)
		return;
	if( rename( oldName.latin1(), newName.latin1())== -1)
		QMessageBox::message(tr("Note"),tr("Could not rename"));
	else
		oldName = "";

	view->takeItem( view->currentItem() );
	delete view->currentItem();
	populateView();
}
