/***************************************************************************
   opieftp.cpp  
                             -------------------
** Created: Sat Mar 9 23:33:09 2002
    copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "opieftp.h"
#include "ftplib.h"
#include "inputDialog.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qtextstream.h>
#include <qtoolbutton.h>
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
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qregexp.h>

#include <unistd.h>
#include <stdlib.h>

QProgressBar *ProgressBar;
static netbuf *conn=NULL;

static int log_progress(netbuf *ctl, int xfered, void *arg)
{
    int fsz = *(int *)arg;
    int pct = (xfered * 100) / fsz;
//      printf("%3d%%\r", pct);
//      fflush(stdout);
    ProgressBar->setProgress(xfered);
    qApp->processEvents();
    return 1;
}

OpieFtp::OpieFtp( )
        : QMainWindow( )
{
    resize( 236, 290 ); 
    setMaximumSize( QSize( 240, 320 ) );
    setCaption( tr( "OpieFtp" ) );

    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    QPEMenuBar *menuBar = new QPEMenuBar( this );
    connectionMenu  = new QPopupMenu( this );
    localMenu  = new QPopupMenu( this );
    remoteMenu  = new QPopupMenu( this );
    menuBar->insertItem( tr( "Connection" ), connectionMenu);
    menuBar->insertItem( tr( "Local" ), localMenu);
    menuBar->insertItem( tr( "Remote" ), remoteMenu);

    connectionMenu->insertItem( tr( "New" ), this,  SLOT( newConnection() ));
    connectionMenu->insertItem( tr( "Connect" ), this,  SLOT( connector() ));
    connectionMenu->insertItem( tr( "Disconnect" ), this,  SLOT( disConnector() ));

    localMenu->insertItem( tr( "Show Hidden Files" ), this,  SLOT( showHidden() ));
    localMenu->insertItem( tr( "Upload" ), this, SLOT( localUpload() ));
    localMenu->insertItem( tr( "Make Directory" ), this, SLOT( localMakDir() ));
    localMenu->insertItem( tr( "Rename" ), this, SLOT( localRename() ));
    localMenu->insertSeparator();
    localMenu->insertItem( tr( "Delete" ), this, SLOT( localDelete() ));

    remoteMenu->insertItem( tr( "Download" ), this, SLOT( remoteDownload() ));
    remoteMenu->insertItem( tr( "Make Directory" ), this, SLOT( remoteMakDir() ));
    remoteMenu->insertItem( tr( "Rename" ), this, SLOT( remoteRename() ));
    remoteMenu->insertSeparator();
    remoteMenu->insertItem( tr( "Delete" ), this, SLOT( remoteDelete() ));

    ProgressBar = new QProgressBar( this, "ProgressBar" );
    ProgressBar->setGeometry( QRect( 5, 268, 231, 15 ) ); 

    TabWidget = new QTabWidget( this, "TabWidget2" );
    TabWidget->setGeometry( QRect( 3, 25, 240, 220 ) );
    TabWidget->setTabShape(QTabWidget::Triangular);

    tab = new QWidget( TabWidget, "tab" );

    Local_View = new QListView( tab, "Local_View" );
    Local_View->setGeometry( QRect( 3, 2, 225, 195 ) );
    Local_View->addColumn( "File",120);
    Local_View->addColumn( "Size",-1);
    Local_View->setColumnAlignment(1,QListView::AlignRight);
    Local_View->addColumn( "Date",-1);
    Local_View->setColumnAlignment(2,QListView::AlignRight);
    Local_View->setAllColumnsShowFocus(TRUE);
    QPEApplication::setStylusOperation( Local_View->viewport(),QPEApplication::RightOnHold);

    connect( Local_View, SIGNAL( doubleClicked( QListViewItem*)),
             this,SLOT( localListClicked(QListViewItem *)) );
    connect( Local_View, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
             this,SLOT( ListPressed(int, QListViewItem *, const QPoint&, int)) );

    TabWidget->insertTab( tab, tr( "Local" ) );

    tab_2 = new QWidget( TabWidget, "tab_2" );

    Remote_View = new QListView( tab_2, "Remote_View" );
    Remote_View->setGeometry( QRect( 3, 2, 225, 195 ) ); 
    Remote_View->addColumn( "File",120);
    Remote_View->addColumn( "Size",-1);
    Remote_View->setColumnAlignment(1,QListView::AlignRight);
    Remote_View->addColumn( "Date",-1);
    Remote_View->setColumnAlignment(2,QListView::AlignRight);
    Remote_View->setAllColumnsShowFocus(TRUE);
    QPEApplication::setStylusOperation( Remote_View->viewport(),QPEApplication::RightOnHold);

    connect( Remote_View, SIGNAL( doubleClicked( QListViewItem*)),
             this,SLOT( remoteListClicked(QListViewItem *)) );
    connect( Remote_View, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
             this,SLOT( RemoteListPressed(int, QListViewItem *, const QPoint&, int)) );

    TabWidget->insertTab( tab_2, tr( "Remote" ) );

    tab_3 = new QWidget( TabWidget, "tab_3" );

    TextLabel1 = new QLabel( tab_3, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 10, 60, 16 ) ); 
    TextLabel1->setText( tr( "Username" ) );
    UsernameComboBox = new QComboBox( FALSE, tab_3, "UsernameComboBox" );
    UsernameComboBox->setGeometry( QRect( 10, 25, 196, 21 ) );
    UsernameComboBox->setEditable(TRUE);
    UsernameComboBox->lineEdit()->setText("anonymous");
//    UsernameComboBox->lineEdit()->setText("llornkcor");

    TextLabel2 = new QLabel( tab_3, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 10, 50, 65, 16 ) ); 
    TextLabel2->setText( tr( "Password" ) );
    PasswordEdit = new QLineEdit( "", tab_3, "PasswordComboBox" );
    PasswordEdit->setGeometry( QRect( 10, 65, 195, 16 ) );
    PasswordEdit->setEchoMode(QLineEdit::Password);
    PasswordEdit->setText( tr( "me@opieftp.org" ) );

    TextLabel3 = new QLabel( tab_3, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 10, 90, 95, 16 ) ); 
    TextLabel3->setText( tr( "Remote server" ) );
    ServerComboBox = new QComboBox( FALSE, tab_3, "ServerComboBox" );
    ServerComboBox->setGeometry( QRect( 10, 105, 195, 21 ) );
    ServerComboBox->setEditable(TRUE);
    ServerComboBox->lineEdit()->setText( tr( "" ) );
//    ServerComboBox->lineEdit()->setText( tr( "llornkcor.com" ) );

    QLabel *TextLabel5 = new QLabel( tab_3, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 10, 130, 95, 16 ) ); 
    TextLabel5->setText( tr( "Remote path" ) );
    remotePath = new QLineEdit( "/", tab_3, "remotePath" );
    remotePath->setGeometry( QRect( 10, 145, 195, 16 ) );
    remotePath->setText( currentRemoteDir = "/");
//    remotePath->setText( currentRemoteDir = "/home/llornkcor/");
    
    TextLabel4 = new QLabel( tab_3, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 10, 170, 30, 21 ) ); 
    TextLabel4->setText( tr( "Port" ) );
    PortSpinBox = new QSpinBox( tab_3, "PortSpinBox" );
    PortSpinBox->setGeometry( QRect( 40, 175, 75, 20 ) ); 
    PortSpinBox->setButtonSymbols( QSpinBox::UpDownArrows );
    PortSpinBox->setMaxValue(32786);
    PortSpinBox->setValue( 21);

    TabWidget->insertTab( tab_3, tr( "Config" ) );

    connect(TabWidget,SIGNAL(currentChanged(QWidget *)),
            this,SLOT(tabChanged(QWidget*)));

    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    currentDir.setPath( QDir::currentDirPath());
//      currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    currentPathEdit = new QLineEdit( "/", this, "currentPathEdit" );
    currentPathEdit->setGeometry( QRect( 5, 248, 220, 18 ) );
    currentPathEdit->setText( currentDir.canonicalPath());
    connect( currentPathEdit,SIGNAL(returnPressed()),this,SLOT(currentPathEditChanged()));
    
    filterStr="*";
    populateLocalView();
}

OpieFtp::~OpieFtp()
{
}

void OpieFtp::cleanUp()
{
    if(conn)
        FtpQuit(conn);
   QFile f("./._temp");
   if(f.exists())
       f. remove();
}

void OpieFtp::tabChanged(QWidget *w)
{
    if (TabWidget->currentPageIndex() == 0) {
            currentPathEdit->setText( currentDir.canonicalPath());
    } else if (TabWidget->currentPageIndex() == 1) {
            currentPathEdit->setText( currentRemoteDir );
    }
}

void OpieFtp::localUpload()
{
    int fsz;
    QCopEnvelope ( "QPE/System", "busy()" );
    qApp->processEvents();
    QString strItem = Local_View->currentItem()->text(0);
    QString localFile = currentDir.canonicalPath()+"/"+strItem;
    QString remoteFile= currentRemoteDir+strItem;
    QFileInfo fi(localFile);
    if( !fi.isDir()) {
        fsz=fi.size();
        ProgressBar->setTotalSteps(fsz);

        FtpOptions(FTPLIB_CALLBACK, (long) log_progress, conn);
        FtpOptions(FTPLIB_IDLETIME, (long) 1000, conn);
        FtpOptions(FTPLIB_CALLBACKARG, (long) &fsz, conn);
        FtpOptions(FTPLIB_CALLBACKBYTES, (long) fsz/10, conn);
        qDebug("Put: %s, %s",localFile.latin1(),remoteFile.latin1());

        if( !FtpPut( localFile.latin1(), remoteFile.latin1(),FTPLIB_IMAGE, conn ) ) {
            QString msg;
            msg.sprintf("Unable to upload\n%s",FtpLastResponse(conn));
            msg.replace(QRegExp(":"),"\n");
            QMessageBox::message("Note",msg);
//            FtpQuit(conn);
        }
        ProgressBar->reset();
        nullifyCallBack();
    } else {
        QMessageBox::message("Note","Cannot upload directories");
    }
    TabWidget->setCurrentPage(1);
    populateRemoteView();
    QCopEnvelope ( "QPE/System", "notBusy()" );
}

void OpieFtp::nullifyCallBack() {
        FtpOptions(FTPLIB_CALLBACK, NULL, conn);
        FtpOptions(FTPLIB_IDLETIME, NULL, conn);
        FtpOptions(FTPLIB_CALLBACKARG, NULL, conn);
        FtpOptions(FTPLIB_CALLBACKBYTES, NULL, conn);

}

void OpieFtp::remoteDownload()
{
    int fsz;
    QCopEnvelope ( "QPE/System", "busy()" );
    qApp->processEvents();
    QString strItem = Remote_View->currentItem()->text(0);
    QString localFile = currentDir.canonicalPath()+"/"+strItem;
    QString remoteFile= currentRemoteDir+strItem;
    if (!FtpSize( remoteFile.latin1(), &fsz, FTPLIB_ASCII, conn))
        fsz = 0;
    QString temp;
    temp.sprintf( remoteFile+" "+" %dkb", fsz); 
            
    ProgressBar->setTotalSteps(fsz);
    FtpOptions(FTPLIB_CALLBACK, (long) log_progress, conn);
    FtpOptions(FTPLIB_IDLETIME, (long) 1000, conn);
    FtpOptions(FTPLIB_CALLBACKARG, (long) &fsz, conn);
    FtpOptions(FTPLIB_CALLBACKBYTES, (long) fsz/10, conn);
    qDebug("Get: %s, %s",localFile.latin1(),remoteFile.latin1());

    if(!FtpGet( localFile.latin1(), remoteFile.latin1(),FTPLIB_IMAGE, conn ) ) {
        QString msg;
        msg.sprintf("Unable to download \n%s",FtpLastResponse(conn));
        msg.replace(QRegExp(":"),"\n");
        QMessageBox::message("Note",msg);
//        FtpQuit(conn);
    }
    ProgressBar->reset();
    nullifyCallBack();
    TabWidget->setCurrentPage(0);
    populateLocalView();
    QCopEnvelope ( "QPE/System", "notBusy()" );
}


void OpieFtp::newConnection()
{
    TabWidget->setCurrentPage(2);
}

void OpieFtp::connector()
{
    QCopEnvelope ( "QPE/System", "busy()" );
     qApp->processEvents();
     currentRemoteDir=remotePath->text();
    if(ServerComboBox->currentText().isEmpty()) {
        QMessageBox::warning(this,"Ftp","Please set the server info","Ok",0,0);
        TabWidget->setCurrentPage(2);
        ServerComboBox->setFocus();
        return;
    }
    FtpInit();
    TabWidget->setCurrentPage(1);
    QString ftp_host = ServerComboBox->currentText();
    QString ftp_user = UsernameComboBox->currentText();
    QString ftp_pass = PasswordEdit->text();
    QString port=PortSpinBox->cleanText();
    port.stripWhiteSpace();
    
    if(ftp_host.find("ftp://",0, TRUE) != -1 )
        ftp_host=ftp_host.right(ftp_host.length()-6);
    ftp_host+=":"+port;
    if (!FtpConnect( ftp_host.latin1(), &conn)) {
            QMessageBox::message("Note","Unable to connect to\n"+ftp_host);
            return ;
        }
        if (!FtpLogin( ftp_user.latin1(), ftp_pass.latin1(),conn )) {
            QString msg;
            msg.sprintf("Unable to log in\n%s",FtpLastResponse(conn));
            msg.replace(QRegExp(":"),"\n");
            QMessageBox::message("Note",msg);
            FtpQuit(conn);
            return ;
        }
        remoteDirList("/") ;
        setCaption(ftp_host);
    QCopEnvelope ( "QPE/System", "notBusy()" );
}

void OpieFtp::disConnector()
{
    FtpQuit(conn);
    setCaption("OpieFtp");
    currentRemoteDir="/";
    Remote_View->clear();
}

bool OpieFtp::remoteDirList(const QString &dir)
{
    QCopEnvelope ( "QPE/System", "busy()" );
    if (!FtpDir( "./._temp", dir.latin1(), conn) ) {
        QString msg;
        msg.sprintf("Unable to list the directory\n"+dir+"\n%s",FtpLastResponse(conn) );
        msg.replace(QRegExp(":"),"\n");
        QMessageBox::message("Note",msg);
//        FtpQuit(conn);
        return false;
    }
    populateRemoteView();
    QCopEnvelope ( "QPE/System", "notBusy()" );
    return true;
}

bool OpieFtp::remoteChDir(const QString &dir)
{
    QCopEnvelope ( "QPE/System", "busy()" );
    if (!FtpChdir( dir.latin1(), conn )) {
        QString msg;
        msg.sprintf("Unable to change directories\n"+dir+"\n%s",FtpLastResponse(conn));
        msg.replace(QRegExp(":"),"\n");
        QMessageBox::message("Note",msg);
            qDebug(msg);
//        FtpQuit(conn);
        QCopEnvelope ( "QPE/System", "notBusy()" );
        return FALSE;
    }
    QCopEnvelope ( "QPE/System", "notBusy()" );
    return TRUE;
}

void OpieFtp::populateLocalView()
{
    Local_View->clear();
    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    currentDir.setMatchAllDirs(TRUE);
    currentDir.setNameFilter(filterStr);
    QString fileL, fileS, fileDate;
    const QFileInfoList *list = currentDir.entryInfoList( /*QDir::All*/ /*, QDir::SortByMask*/);
    QFileInfoListIterator it(*list);
    QFileInfo *fi;
    while ( (fi=it.current()) ) {
        if (fi->isSymLink() ){
            QString symLink=fi->readLink();
//         qDebug("Symlink detected "+symLink);
            QFileInfo sym( symLink);
            fileS.sprintf( "%10li", sym.size() );
            fileL.sprintf( "%s ->  %s",  sym.fileName().data(),sym.absFilePath().data() );
            fileDate = sym.lastModified().toString(); 
        } else {
//        qDebug("Not a dir: "+currentDir.canonicalPath()+fileL);
            fileS.sprintf( "%10li", fi->size() );
            fileL.sprintf( "%s",fi->fileName().data() );
            fileDate= fi->lastModified().toString(); 
            if( QDir(QDir::cleanDirPath(currentDir.canonicalPath()+"/"+fileL)).exists() ) {
                fileL+="/";
//     qDebug( fileL);
            }
        }
        item= new QListViewItem( Local_View,fileL,fileS, fileDate);
        ++it;
     }
    Local_View->setSorting( 3,FALSE);
    currentPathEdit->setText( currentDir.canonicalPath() );
}

bool OpieFtp::populateRemoteView()
{
 Remote_View->clear();
    QFile tmp("./._temp");
    QString s, File_Name;
    QString fileL, fileS, fileDate;
    new QListViewItem( Remote_View, "../");
    if (tmp.open(IO_ReadOnly)) {
        QTextStream t( &tmp );   // use a text stream
        while ( !t.eof()) {
            s = t.readLine();
            fileL = s.right(s.length()-55);
            fileL = fileL.stripWhiteSpace();
            if(s.left(1) == "d")
                fileL = fileL+"/";
            fileS = s.mid( 30, 42-30);
            fileS = fileS.stripWhiteSpace();
            fileDate = s.mid( 42, 55-42);
            fileDate = fileDate.stripWhiteSpace();
            if(fileL.find("total",0,TRUE) == -1)          
             new QListViewItem( Remote_View, fileL, fileS, fileDate);
        }
        tmp.close();
    }
    return true;        
}

void OpieFtp::remoteListClicked(QListViewItem *selectedItem)
{
    QCopEnvelope ( "QPE/System", "busy()" );
    QString  oldRemoteCurrentDir =  currentRemoteDir;
    QString strItem=selectedItem->text(0);
    strItem=strItem.simplifyWhiteSpace();
    if(strItem == "../") { // the user wants to go ^
        if( FtpCDUp( conn) == 0) {
            QString msg;
            msg.sprintf("Unable to cd up\n%s",FtpLastResponse(conn));
            msg.replace(QRegExp(":"),"\n");
            QMessageBox::message("Note",msg);
            qDebug(msg);
        }
        char path[256];
        if( FtpPwd( path,sizeof(path),conn) == 0) { //this is easier than fudging the string
            QString msg;
            msg.sprintf("Unable to get working dir\n%s",FtpLastResponse(conn));
            msg.replace(QRegExp(":"),"\n");
            QMessageBox::message("Note",msg);
            qDebug(msg);
        }
        currentRemoteDir=path;
    } else {
        if(strItem.find("->",0,TRUE) != -1) { //symlink on some servers
            strItem=strItem.right( strItem.length() - strItem.find("->",0,TRUE) - 2 );
            strItem = strItem.stripWhiteSpace();
            currentRemoteDir = strItem;
            if( !remoteChDir( (const QString &)strItem)) {
                currentRemoteDir = oldRemoteCurrentDir;
                strItem="";
                populateRemoteView();
                qDebug("RemoteCurrentDir1 "+oldRemoteCurrentDir);
            }
        } else if(strItem.find("/",0,TRUE) != -1) { // this is a directory
            qDebug("trying directory");
            if( !remoteChDir( (const QString &)currentRemoteDir + strItem)) {
                currentRemoteDir = oldRemoteCurrentDir;
                strItem="";
                qDebug("RemoteCurrentDir1 "+oldRemoteCurrentDir);
                
                populateRemoteView();
            } else {
            currentRemoteDir = currentRemoteDir+strItem;
            }
        } else {
            qDebug("download "+strItem);
        }
    }
    if(currentRemoteDir.right(1) !="/")
        currentRemoteDir +="/";
    currentPathEdit->setText( currentRemoteDir );
    remoteDirList( (const QString &)currentRemoteDir);
    QCopEnvelope ( "QPE/System", "notBusy()" );
}

 void OpieFtp::localListClicked(QListViewItem *selectedItem)
{
    QString strItem=selectedItem->text(0);
    QString strSize=selectedItem->text(1);
    strSize=strSize.stripWhiteSpace();
    if(strItem.find("@",0,TRUE) !=-1 || strItem.find("->",0,TRUE) !=-1 ) { //if symlink
          // is symlink
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
            } else {
                currentDir.cdUp();
                populateLocalView();
            }
            if(QDir(strItem).exists()){
                currentDir.cd(strItem, TRUE);
                populateLocalView();
            }
        } else {
            strItem=QDir::cleanDirPath(currentDir.canonicalPath()+"/"+strItem);
            if( QFile::exists(strItem ) ) {
                qDebug("upload "+strItem);
            }
        } //end not symlink
        chdir(strItem.latin1());
    }
}

void OpieFtp::showHidden()
{
    if (!b) {
    currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
//    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    b=TRUE;
    
    }  else {
    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
//    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    b=FALSE;
    }
    populateLocalView();
}

void OpieFtp::ListPressed( int mouse, QListViewItem *item, const QPoint &point, int i)
{
    switch (mouse) {
      case 1:
          break;
      case 2: 
    showLocalMenu();
    break;
    };
}

void OpieFtp::RemoteListPressed( int mouse, QListViewItem *item, const QPoint &point, int i)
{
 switch (mouse) {
      case 1:
          break;
      case 2: 
    showRemoteMenu();
    break;
    };
}

void OpieFtp::showRemoteMenu()
{
    QPopupMenu  m;// = new QPopupMenu( Local_View );
    m.insertItem( tr( "Download" ), this, SLOT( remoteDownload() ));
    m.insertItem( tr( "Make Directory" ), this, SLOT( remoteMakDir() ));
    m.insertItem( tr( "Rename" ), this, SLOT( remoteRename() ));
    m.insertSeparator();
    m.insertItem( tr( "Delete" ), this, SLOT( remoteDelete() ));
    m.exec( QCursor::pos() );
}

void OpieFtp::showLocalMenu()
{
    QPopupMenu  m;
    m.insertItem(  tr( "Show Hidden Files" ), this,  SLOT( showHidden() ));
    m.insertItem( tr( "Upload" ), this, SLOT( localUpload() ));
    m.insertItem( tr( "Make Directory" ), this, SLOT( localMakDir() ));
    m.insertItem( tr( "Rename" ), this, SLOT( localRename() ));
    m.insertSeparator();
    m.insertItem( tr( "Delete" ), this, SLOT( localDelete() ));
    m.exec( QCursor::pos() );
}

void OpieFtp::localMakDir()
{
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,"Make Directory",TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
       QString  filename = fileDlg->LineEdit1->text();
       currentDir.mkdir( currentDir.canonicalPath()+"/"+filename);
    }
    populateLocalView();
}

void OpieFtp::localDelete()
{
    QString f = Local_View->currentItem()->text(0);
    if(QDir(f).exists() ) {
        switch ( QMessageBox::warning(this,"Delete","Do you really want to delete\n"+f+
                                      " ?\nIt must be empty","Yes","No",0,0,1) ) {
          case 0: {
              f=currentDir.canonicalPath()+"/"+f;
              QString cmd="rmdir "+f;
              system( cmd.latin1());
              populateLocalView();
          }
              break;
          case 1: 
                // exit
              break;
        };
        
    } else {
        switch ( QMessageBox::warning(this,"Delete","Do you really want to delete\n"+f
                                      +" ?","Yes","No",0,0,1) ) {
          case 0: {
              f=currentDir.canonicalPath()+"/"+f;
              QString cmd="rm "+f;
              system( cmd.latin1());
              populateLocalView();
          }
              break;
          case 1: 
                // exit
              break;
        };
    }
}

void OpieFtp::remoteMakDir()
{
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,"Make Directory",TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
       QString  filename = fileDlg->LineEdit1->text();//+".playlist";
       QString tmp=currentRemoteDir+filename;
       QCopEnvelope ( "QPE/System", "busy()" );
       if(FtpMkdir( tmp.latin1(), conn) == 0) {
           QString msg;
           msg.sprintf("Unable to make directory\n%s",FtpLastResponse(conn));
           msg.replace(QRegExp(":"),"\n");
           QMessageBox::message("Note",msg);
       }
       QCopEnvelope ( "QPE/System", "notBusy()" );
    }
    populateRemoteView();
}

void OpieFtp::remoteDelete()
{
    QString f = Remote_View->currentItem()->text(0);
    QCopEnvelope ( "QPE/System", "busy()" );
    if( f.right(1) =="/") {
        QString path= currentRemoteDir+f;
        switch ( QMessageBox::warning(this,"Delete","Do you really want to delete\n"+f+"?"
                                      ,"Yes","No",0,0,1) ) {
          case 0: {
              f=currentDir.canonicalPath()+"/"+f;
              if(FtpRmdir( path.latin1(), conn) ==0) {
                  QString msg;
                  msg.sprintf("Unable to remove directory\n%s",FtpLastResponse(conn));
                  msg.replace(QRegExp(":"),"\n");
                  QMessageBox::message("Note",msg);
              }
          }
          break;
        };
    } else {
        switch ( QMessageBox::warning(this,"Delete","Do you really want to delete\n"+f+"?"
                                      ,"Yes","No",0,0,1) ) {
          case 0: {
              QString path= currentRemoteDir+f;
              if(FtpDelete( path.latin1(), conn)==0) {
                  QString msg;
                  msg.sprintf("Unable to delete file\n%s",FtpLastResponse(conn));
                  msg.replace(QRegExp(":"),"\n");
                  QMessageBox::message("Note",msg);
              }
          }
          break;
        };
    }
    QCopEnvelope ( "QPE/System", "notBusy()" );
}

void OpieFtp::remoteRename()
{
    QString curFile = Remote_View->currentItem()->text(0);
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,"Rename",TRUE, 0);
    fileDlg->inputText = curFile;
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        QString oldName = currentRemoteDir +"/"+ curFile;
        QString newName = currentRemoteDir  +"/"+ fileDlg->LineEdit1->text();//+".playlist";
        QCopEnvelope ( "QPE/System", "busy()" );
        if(FtpRename( oldName.latin1(), newName.latin1(),conn) == 0) {
            QString msg;
            msg.sprintf("Unable to rename file\n%s",FtpLastResponse(conn));
            msg.replace(QRegExp(":"),"\n");
            QMessageBox::message("Note",msg);
        }
        QCopEnvelope ( "QPE/System", "notBusy()" );
    }
    populateRemoteView();
}

void OpieFtp::localRename()
{
    QString curFile = Local_View->currentItem()->text(0);
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,"Rename",TRUE, 0);
    fileDlg->inputText = curFile;
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        QString oldname =  currentDir.canonicalPath() + "/" + curFile;
        QString newName =  currentDir.canonicalPath() + "/" + fileDlg->LineEdit1->text();//+".playlist";
        if( rename(oldname.latin1(), newName.latin1())== -1)
            QMessageBox::message("Note","Could not rename");
    }
    populateLocalView();
}

void OpieFtp::currentPathEditChanged()
{
    QString  oldRemoteCurrentDir =  currentRemoteDir;
 qDebug("oldRemoteCurrentDir "+oldRemoteCurrentDir);
    if (TabWidget->currentPageIndex() == 0) {
        if(QDir( currentPathEdit->text()).exists()) {
            currentDir.setPath( currentPathEdit->text() );
            populateLocalView();
        } else {
            QMessageBox::message("Note","That directory does not exist");
        }
    }
    if (TabWidget->currentPageIndex() == 1) {
        currentRemoteDir = currentPathEdit->text();
        if(currentRemoteDir.right(1) !="/") {
            currentRemoteDir = currentRemoteDir +"/";
            currentPathEdit->setText( currentRemoteDir );
        }
            if( !remoteChDir( (const QString &)currentRemoteDir) ) {
                currentRemoteDir = oldRemoteCurrentDir;
                currentPathEdit->setText( currentRemoteDir );
            }
        
        remoteDirList( (const QString &)currentRemoteDir);
    }
}
