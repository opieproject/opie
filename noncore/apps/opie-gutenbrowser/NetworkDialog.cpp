// /***************************************************************************
//                           NetworkDialog.cpp  -  description
//     begin                : Sun Aug 27 2000
//     copyright            : (C) 2000 - 2004 by L.J. Potter
//     email                : ljp@llornkcor.com
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  ***************************************************************************/
// // half-assed attempt at providing a network dialog.
// /* Created: Sun Aug 27 15:24:52 2000*/


#include "NetworkDialog.h"
#include "gutenbrowser.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qprogressbar.h>
#include <qstringlist.h>
#include <qpe/config.h>
#include <qvaluelist.h>
#include <qapplication.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>

/* STD */
#include <unistd.h>
extern "C" {
#include <ftplib.h>
}

QProgressBar* ProgressBar1;
QPushButton* buttonCancel;
static netbuf *conn = NULL;

static int log_progress(netbuf *ctl, int xfered, void *arg) {
	int fsz = *(int *)arg;
	int pct = (xfered * 100) / fsz;
	printf("%3d%%\r", pct);
	fflush(stdout);
	ProgressBar1->setProgress(xfered);
	qApp->processEvents();

	return 1;
}

NetworkDialog::NetworkDialog( QWidget* parent,  const char* name, bool modal, WFlags fl, const QStringList netL)
		: QDialog( parent, name, modal, fl )
{

	ftp_host = netL[0];
	networkUrl = strUrl =  netL[0];

	dir = ftp_base_dir = netL[1];
	localFileName = netL[2];
	s_partialFileName = netL[3];

	resize(240,120);

	local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
//      autoOk = autoDownload;
//      if( networkUrl.find("ftp",0,false)== -1 ) {
//         if ( !name )
//              setName( "HTTP NetworkDialog" );
//          setCaption( tr( "HTTP Download  ) );
//      qInitNetworkProtocols();  // registers ftp protocol // for now
//      QNetworkProtocol::registerNetworkProtocol( "http", new QNetworkProtocolFactory<Http> );
//      } else {
	if ( !name )
		setName( "FTP NetworkDialog" );
	setCaption(tr("FTP Download"));
//    }
	initDialog(); //opens file to be written
}

NetworkDialog::~NetworkDialog() {
}

void NetworkDialog::initDialog() {

	QGridLayout *layout = new QGridLayout( this );
	totalBytesDownloaded=0;
	warnLabel = new QLabel( this, "TextLabel" );
	warnLabel ->setText( tr( "Download should start shortly" ) );
	TextLabel3 = new QLabel( this, "TextLabel3" );
	TextLabel3->setText( "");
	layout->addMultiCellWidget( warnLabel, 0, 0, 0, 5 );
	layout->addMultiCellWidget( TextLabel3, 1, 1, 0, 5 );

	ProgressBar1 = new QProgressBar(  this, "ProgressBar1" );
	ProgressBar1->setProgress(0);
	layout->addMultiCellWidget( ProgressBar1, 3, 3, 0, 4 );

		//  hbox->addStretch(1);
	buttonCancel = new QPushButton( this, "buttonCancel" );
	buttonCancel->setText( tr( "&Cancel"  ) );
	buttonCancel->setAutoDefault( TRUE );
	buttonCancel->setDefault( TRUE );
	layout->addMultiCellWidget( buttonCancel, 3, 3, 5, 5 );

	connect(buttonCancel,SIGNAL(clicked()),this,SLOT(reject()));
	QTimer::singleShot( 1000, this, SLOT(  doOk()  ));

}

/*
	downloads the file networkUrl */
bool NetworkDialog::downloadFile( QString networkUrl )
{
	int fsz;
	warnLabel ->setText( "");
	qApp->processEvents();
	odebug << "Downloading: " << networkUrl << "" << oendl;
	odebug << "Into: " << localFileName << "" << oendl;
	if( networkUrl.length() > 5) {
		QString ftp_user = "anonymous";
		QString ftp_pass = "zaurus@gutenbrowser.com";
//    ftp_host= networkUrl.mid(networkUrl.find("ftp://",0, TRUE),
		if(ftp_host.length() < 2) {
			odebug << "Please select an ftp host" << oendl;
			successDownload=false;
			QMessageBox::message("Note","You need to select an ftp host");
			return false;
		}
		QString msg;
		odebug << ftp_host << oendl;
		odebug << "Opening ftp connection." << oendl;
		warnLabel->setText("connecting to: "+ftp_host );
		qApp->processEvents();
			/////////// Open FTP connection
			if (!FtpConnect( ftp_host.latin1(), &conn)) {
				i=0;
				successDownload=false;
				QMessageBox::message("Note","Unable to connect to\n"+ftp_host);
				return false;
			}
			TextLabel3->setText("Opening ftp connection.");
			qApp->processEvents();
			if (!FtpLogin( ftp_user.latin1(), ftp_pass.latin1(),conn )) {
				FtpQuit(conn);
				successDownload=false;
				msg.sprintf("Unable to log in\n%s",FtpLastResponse(conn));
				QMessageBox::message("Note",msg);
				return  false;
			}
			odebug << "Changing directories." << oendl;
			TextLabel3->setText("Changing directories.");
			qApp->processEvents();
			if (!FtpChdir( dir.latin1(), conn )) {
				successDownload=false;
				msg.sprintf("Unable to change directories "+dir+"\n%s",FtpLastResponse(conn));
				QMessageBox::message("Note",msg);
				FtpQuit(conn);
				return false;
			}
//      if (!FtpNlst( "./.guten_temp", dir, conn)) {
//              successDownload=false;
//              msg.sprintf("Unable to change directories "+dir+"\n%s",FtpLastResponse(conn));
//              QMessageBox::message("Note",msg);
//              FtpQuit(conn);
//              return false;
//      }
			odebug << "Requesting directory list." << oendl;
			TextLabel3->setText("Getting directory list.");
			qApp->processEvents();
			if (!FtpDir( "./.guten_temp", dir.latin1(), conn) ) {
				msg.sprintf("Unable to list the directory\n"+dir+"\n%s",FtpLastResponse(conn));
				QMessageBox::message("Note",msg);
				successDownload=false;
				FtpQuit(conn);
				return false;
			}
			QFile tmp("./.guten_temp");
			QString s, File_Name;

			if (tmp.open(IO_ReadOnly)) {
				QTextStream t( &tmp );   // use a text stream
				odebug << "Finding partial filename "+s_partialFileName << oendl;
				while ( !t.eof()) {
					s = t.readLine();

					if (s.contains(s_partialFileName, FALSE)) {
						QString str = s.right( (s.length()) - (s.find(s_partialFileName, FALSE)) );

						if (str.contains(".txt")) {
							File_Name = str;
							odebug << "Found file_name "+ File_Name << oendl;
							break;
						}
//                      if (str.contains(".zip")) {
//                          File_Name = str;
//                          odebug << "Found file_name "+ File_Name << oendl;
//                          break;
//                      }

					}
				}  //end of while loop
				tmp.close();
//        tmp.remove(); ///TODO this is for release version Zaurus
			}
			else
				odebug << "Error opening temp file." << oendl;

			Config cfg("Gutenbrowser");
			cfg.setGroup("General");
			QString temp=cfg.readEntry("DownloadDirectory",local_library);

			localFileName = temp+File_Name;
			odebug << "Requesting file "+ File_Name << oendl;
			odebug << "Saving as "+localFileName << oendl;
			msg="Requesting file "+ File_Name;
			TextLabel3->setText(msg);
			qApp->processEvents();
			if( File_Name.length()>3) {
				if (!FtpSize( File_Name.latin1(), &fsz, FTPLIB_ASCII, conn))
					fsz = 0;
				QString temp;
				temp.sprintf( File_Name+" "+" %dkb", fsz);
				TextLabel3->setText(temp);

				ProgressBar1->setTotalSteps(fsz);
				FtpOptions(FTPLIB_CALLBACK, (long) log_progress, conn);
				FtpOptions(FTPLIB_IDLETIME, (long) 1000, conn);
				FtpOptions(FTPLIB_CALLBACKARG, (long) &fsz, conn);
				FtpOptions(FTPLIB_CALLBACKBYTES, (long) fsz/10, conn);

				if (!FtpGet( localFileName.latin1(), File_Name.latin1(), FTPLIB_ASCII, conn)) {
					msg.sprintf("Could not download "+ File_Name+"\n%s",FtpLastResponse(conn));
					successDownload=false;
					QMessageBox::message("Note",msg);
					update();
				}
			} else {
				QMessageBox::message("Note","Could not download file "+ File_Name);
				successDownload=false;
				FtpQuit(conn);
				return false;
			}
			odebug << "Ftp session successful" << oendl;
			successDownload=TRUE;
			FtpQuit(conn);
			return true;
	} //no network url
	return false;
}

void NetworkDialog::doOk() {
	owarn << "Do OK" << oendl;
	QString loginStr;
	loginStr = "gutenbrowser";
	if ( !ftp_host.isEmpty() )   {
		if( ftp_host.find( "/", ftp_host.length() - 1,TRUE) != -1  &&  ftp_host.find("ftp://",0, TRUE) != -1) {
			TextLabel3->setText( tr( "List remote dir:\n" + ftp_host) );
//    TextLabel2->setText( tr( "local file to download into: " +localFileName ) );

			if( downloadFile(ftp_host))
				successDownload = true;
			else {
				successDownload = false;
				reject();
			}
		} else {
			if(downloadFile(ftp_host))
				successDownload = true;
			else {
				successDownload = false;
				reject();
			}
		}
	}
	accept();
}
