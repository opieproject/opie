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
#include <unistd.h>
extern "C" {
#include "../../net/ftplib/ftplib.h"
}


#include "NetworkDialog.h"
#include "gutenbrowser.h"

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

		resize(240,110);

		local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
//		autoOk = autoDownload;
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

    totalBytesDownloaded=0;
    warnLabel = new QLabel( this, "TextLabel" );
    warnLabel ->setText( tr( "Push Ok to download file...." ) );
    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setText( "");
    QWidget* Layout1 = new QWidget( this, "Layout1" );

    hbox = new QHBoxLayout(Layout1);
    hbox->setMargin(4);

    ProgressBar1 = new QProgressBar( Layout1, "ProgressBar1" );
    ProgressBar1->setProgress(0);
    hbox->addWidget(ProgressBar1,10,AlignCenter);

    hbox->addStretch(1);
    buttonOk = new QPushButton( Layout1, "buttonOk" );
    buttonOk->setText( tr( "&OK"  ) );
    hbox->addWidget(buttonOk,0,AlignRight);
    hbox->addSpacing(5);
    buttonCancel = new QPushButton( Layout1, "buttonCancel" );
    buttonCancel->setText( tr( "&Cancel"  ) );
    buttonCancel->setAutoDefault( TRUE );
    buttonCancel->setDefault( TRUE );
    hbox->addWidget(buttonCancel,0,AlignRight);

     ProgressBar1->setFixedSize(140,22);
    buttonOk->setFixedSize(35,22);
    buttonCancel->setFixedSize(35,22);
    warnLabel ->setGeometry( QRect( 5,1,230,25));
    TextLabel3->setGeometry( QRect( 5,20,230,25));
    Layout1->setGeometry( QRect(1,60,235,50)); //TODO check these!!

//        timer= new QTimer(this,"vu timer");
//        connectionTimer=new QTimer(this,"connectionTimeout");

    connect(buttonOk,SIGNAL(clicked()),this,SLOT(doOk()));
    connect(buttonCancel,SIGNAL(clicked()),this,SLOT(reject()));
//    connect( timer, SIGNAL(timeout()), this , SLOT(timeSlot()));
//    connect( connectionTimer,SIGNAL( timeout()),this,SLOT( connectionTimeSlot()));
		if(autoOk) {
				qWarning("XXXXXXXXXXXXXXXXXXXXXXXX");
				buttonOk->setDown(true);
				doOk();
		}
}

void NetworkDialog::timeSlot() {

//      if(timerProgess < 19 && posTimer) {
//      ProgressBar1->setProgress(timerProgess);
//      timerProgess++;
//      } else if(timerProgess > 19 && posTimer) {
//      ProgressBar1->setProgress(timerProgess);
//      timerProgess++;
//          posTimer=FALSE;
//      }
//      if(timerProgess > 1 &&!posTimer) {
//      ProgressBar1->setProgress(timerProgess);
//      timerProgess--;
//      } else if(timerProgess > 1 &&!posTimer){
//      ProgressBar1->setProgress(timerProgess);
//      timerProgess--;
//          posTimer=TRUE;
//      }
//  //    qDebug("timer event");
//      qApp->processEvents();
//      repaint();
}

void NetworkDialog::connectionTimeSlot() {
//      qDebug("Connections timed out");
//      ftpQuit();
//      qApp->processEvents();
//      repaint();
//      reject();
}


/*
downloads the file networkUrl */
bool NetworkDialog::downloadFile( QString networkUrl )
{
    int fsz;
//        timer->start( 250 , FALSE);
//        posTimer=TRUE;
//    connectionTimer->start( 600 , FALSE);
    warnLabel ->setText( "");
    qApp->processEvents();
    qDebug("Downloading: %s",networkUrl.latin1());
    qDebug("Into: %s",localFileName.latin1());
    if( networkUrl.length() > 5) {
        QString ftp_user = "anonymous";
        QString ftp_pass = "zaurus@gutenbrowser.com";
//    ftp_host= networkUrl.mid(networkUrl.find("ftp://",0, TRUE),
        if(ftp_host.length() < 2) {
            qDebug("Please select an ftp host" );
            successDownload=false;
            QMessageBox::message("Note","You need to select an ftp host");
            return false;
        }
        QString msg;
        qDebug(ftp_host);
        qDebug("Opening ftp connection.");
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
        qDebug("Changing directories.");
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
        qDebug("Requesting directory list.");
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
            qDebug("Finding partial filename "+s_partialFileName);
            while ( !t.eof()) {
                s = t.readLine();

                if (s.contains(s_partialFileName, FALSE)) {
                    QString str = s.right( (s.length()) - (s.find(s_partialFileName, FALSE)) );

                    if (str.contains(".txt")) {
                        File_Name = str;
                        qDebug("Found file_name "+ File_Name);
                        break;
                    }
//                      if (str.contains(".zip")) {
//                          File_Name = str;
//                          qDebug("Found file_name "+ File_Name);
//                          break;
//                      }
                    
                }
            }  //end of while loop
            tmp.close();
//        tmp.remove(); ///TODO this is for release version Zaurus
        }
        else
            qDebug("Error opening temp file.");

        Config cfg("Gutenbrowser");
        cfg.setGroup("General");
        QString temp=cfg.readEntry("DownloadDirectory",local_library);

        localFileName = temp+File_Name;
        qDebug("Requesting file "+ File_Name);
        qDebug( "Saving as "+localFileName);
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
        qDebug("Ftp session successful");
        successDownload=TRUE;
        FtpQuit(conn);
        return true;
    } //no network url
    return false;
}

void NetworkDialog::doOk() {
		qWarning("Do OK");
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
