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
    setCaption( tr( "OpieFtp" ) );

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 2);
    layout->setMargin( 2);

    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    QPEMenuBar *menuBar = new QPEMenuBar(this);
    connectionMenu  = new QPopupMenu( this );
    localMenu  = new QPopupMenu( this );
    remoteMenu  = new QPopupMenu( this );
    tabMenu = new QPopupMenu( this );

    layout->addMultiCellWidget( menuBar, 0, 0, 0, 3 );

    menuBar->insertItem( tr( "Connection" ), connectionMenu);
    menuBar->insertItem( tr( "Local" ), localMenu);
    menuBar->insertItem( tr( "Remote" ), remoteMenu);
    menuBar->insertItem( tr( "View" ), tabMenu);

    connectionMenu->insertItem( tr( "New" ), this,  SLOT( newConnection() ));
    connectionMenu->insertItem( tr( "Connect" ), this,  SLOT( connector() ));
    connectionMenu->insertItem( tr( "Disconnect" ), this,  SLOT( disConnector() ));

    localMenu->insertItem( tr( "Show Hidden Files" ), this,  SLOT( showHidden() ));
    localMenu->insertSeparator();
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

    tabMenu->insertItem( tr( "Switch to Local" ), this, SLOT( switchToLocalTab() ));
    tabMenu->insertItem( tr( "Switch to Remote" ), this, SLOT( switchToRemoteTab() ));
    tabMenu->insertItem( tr( "Switch to Config" ), this, SLOT( switchToConfigTab() ));
    tabMenu->setCheckable(TRUE);

    TabWidget = new QTabWidget( this, "TabWidget" );
    layout->addMultiCellWidget( TabWidget, 1, 1, 0, 3 );

    TabWidget->setTabShape(QTabWidget::Triangular);

    tab = new QWidget( TabWidget, "tab" );
    tabLayout = new QGridLayout( tab );
    tabLayout->setSpacing( 2);
    tabLayout->setMargin( 2);

    Local_View = new QListView( tab, "Local_View" );
//    Local_View->setResizePolicy( QListView::AutoOneFit );
    Local_View->addColumn( "File",120);
    Local_View->addColumn( "Size",-1);
    Local_View->setColumnAlignment(1,QListView::AlignRight);
    Local_View->addColumn( "Date",-1);
    Local_View->setColumnAlignment(2,QListView::AlignRight);
    Local_View->setAllColumnsShowFocus(TRUE);
    QPEApplication::setStylusOperation( Local_View->viewport(),QPEApplication::RightOnHold);

    tabLayout->addWidget( Local_View, 0, 0 );

    connect( Local_View, SIGNAL( doubleClicked( QListViewItem*)),
             this,SLOT( localListClicked(QListViewItem *)) );
    connect( Local_View, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
             this,SLOT( ListPressed(int, QListViewItem *, const QPoint&, int)) );

    TabWidget->insertTab( tab, tr( "Local" ) );

    tab_2 = new QWidget( TabWidget, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2 );
    tabLayout_2->setSpacing( 2);
    tabLayout_2->setMargin( 2);

    Remote_View = new QListView( tab_2, "Remote_View" );
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

    tabLayout_2->addWidget( Remote_View, 0, 0 );

    TabWidget->insertTab( tab_2, tr( "Remote" ) );

    tab_3 = new QWidget( TabWidget, "tab_3" );
    tabLayout_3 = new QGridLayout( tab_3 );
    tabLayout_3->setSpacing( 2);
    tabLayout_3->setMargin( 2);

    TextLabel1 = new QLabel( tab_3, "TextLabel1" );
    TextLabel1->setText( tr( "Username" ) );
    tabLayout_3->addMultiCellWidget( TextLabel1, 0, 0, 0, 1 );

    UsernameComboBox = new QComboBox( FALSE, tab_3, "UsernameComboBox" );
    UsernameComboBox->setEditable(TRUE);
//    UsernameComboBox->lineEdit()->setText("anonymous");
    UsernameComboBox->lineEdit()->setText("root");
//    UsernameComboBox->lineEdit()->setText("llornkcor");
    tabLayout_3->addMultiCellWidget( UsernameComboBox, 1, 1, 0, 1 );

    TextLabel2 = new QLabel( tab_3, "TextLabel2" );
    TextLabel2->setText( tr( "Password" ) );
    tabLayout_3->addMultiCellWidget( TextLabel2, 0, 0, 2, 3 );

    PasswordEdit = new QLineEdit( "", tab_3, "PasswordComboBox" );
    PasswordEdit->setEchoMode(QLineEdit::Password);
//    PasswordEdit->setText( tr( "me@opieftp.org" ) );
//    PasswordEdit->setText( tr( "" ) );
    tabLayout_3->addMultiCellWidget( PasswordEdit, 1, 1, 2, 3 );

    TextLabel3 = new QLabel( tab_3, "TextLabel3" );
    TextLabel3->setText( tr( "Remote server" ) );
    tabLayout_3->addMultiCellWidget( TextLabel3, 2, 2, 0, 1 );

    ServerComboBox = new QComboBox( FALSE, tab_3, "ServerComboBox" );
    ServerComboBox->setEditable(TRUE);
    ServerComboBox->lineEdit()->setText( tr( "" ) );
    tabLayout_3->addMultiCellWidget( ServerComboBox, 3, 3, 0, 1 );
//    ServerComboBox->lineEdit()->setText( tr( "llornkcor.com" ) );
    ServerComboBox->lineEdit()->setText( tr( "192.168.129.201" ) );

    QLabel *TextLabel5 = new QLabel( tab_3, "TextLabel5" );
    TextLabel5->setText( tr( "Remote path" ) );
    tabLayout_3->addMultiCellWidget( TextLabel5, 2, 2, 2, 3 );

    remotePath = new QLineEdit( "/", tab_3, "remotePath" );
    remotePath->setText( currentRemoteDir = "/");
    tabLayout_3->addMultiCellWidget( remotePath, 3, 3, 2, 3 );
//    remotePath->setText( currentRemoteDir = "/home/llornkcor/");

    TextLabel4 = new QLabel( tab_3, "TextLabel4" );
    TextLabel4->setText( tr( "Port" ) );
    tabLayout_3->addMultiCellWidget( TextLabel4, 4, 4, 0, 1 );

    PortSpinBox = new QSpinBox( tab_3, "PortSpinBox" );
    PortSpinBox->setButtonSymbols( QSpinBox::UpDownArrows );
    PortSpinBox->setMaxValue(32786);
    PortSpinBox->setValue( 4242);
//    PortSpinBox->setValue( 21);
    tabLayout_3->addMultiCellWidget( PortSpinBox, 4, 4, 1, 1);

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_3->addItem( spacer, 5, 0 );

    TabWidget->insertTab( tab_3, tr( "Config" ) );

    connect(TabWidget,SIGNAL(currentChanged(QWidget *)),
            this,SLOT(tabChanged(QWidget*)));

    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    currentDir.setPath( QDir::currentDirPath());
//      currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    currentPathEdit = new QLineEdit( "/", this, "currentPathEdit" );
    layout->addMultiCellWidget( currentPathEdit, 3, 3, 0, 3 );

    currentPathEdit->setText( currentDir.canonicalPath());
    connect( currentPathEdit,SIGNAL(returnPressed()),this,SLOT(currentPathEditChanged()));

    ProgressBar = new QProgressBar( this, "ProgressBar" );
    layout->addMultiCellWidget( ProgressBar, 4, 4, 0, 3 );

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
    QString sfile=QDir::homeDirPath();
    if(sfile.right(1) != "/")
        sfile+="/._temp";
    else
        sfile+="._temp";
    QFile file( sfile);
    if(file.exists())
        file.remove();
}

void OpieFtp::tabChanged(QWidget *w)
{
    if (TabWidget->currentPageIndex() == 0) {
            currentPathEdit->setText( currentDir.canonicalPath());
            tabMenu->setItemChecked(tabMenu->idAt(0),TRUE);
            tabMenu->setItemChecked(tabMenu->idAt(1),FALSE);
            tabMenu->setItemChecked(tabMenu->idAt(2),FALSE);
    }
  if (TabWidget->currentPageIndex() == 1) {
            currentPathEdit->setText( currentRemoteDir );
            tabMenu->setItemChecked(tabMenu->idAt(1),TRUE);
            tabMenu->setItemChecked(tabMenu->idAt(0),FALSE);
            tabMenu->setItemChecked(tabMenu->idAt(2),FALSE);
    }
  if (TabWidget->currentPageIndex() == 2) {
            tabMenu->setItemChecked(tabMenu->idAt(2),TRUE);
            tabMenu->setItemChecked(tabMenu->idAt(0),FALSE);
            tabMenu->setItemChecked(tabMenu->idAt(1),FALSE);
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
    remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
    QCopEnvelope ( "QPE/System", "notBusy()" );
}

void OpieFtp::nullifyCallBack()
{
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
    QString tmp = QDir::homeDirPath();
    if(tmp.right(1) != "/")
        tmp+="/._temp";
    else
        tmp+="._temp";
//    qDebug("Listing remote dir "+tmp);
    QCopEnvelope ( "QPE/System", "busy()" );
    if (!FtpDir( tmp.latin1(), dir.latin1(), conn) ) {
        QString msg;
        msg.sprintf("Unable to list the directory\n"+dir+"\n%s",FtpLastResponse(conn) );
        msg.replace(QRegExp(":"),"\n");
        QMessageBox::message("Note",msg);
//        FtpQuit(conn);
        return false;
    }
    populateRemoteView() ;
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

bool OpieFtp::populateRemoteView( )
{
//    qDebug("populate remoteview");
    QString sfile=QDir::homeDirPath();
    if(sfile.right(1) != "/")
        sfile+="/._temp";
    else
        sfile+="._temp";
    QFile file( sfile);
    Remote_View->clear();
    QString s, File_Name;
    QString fileL, fileS, fileDate;
    new QListViewItem( Remote_View, "../");
    if ( file.open(IO_ReadOnly)) {
        QTextStream t( &file );   // use a text stream
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
        file.close();
        if( file.exists())
            file. remove();
    } else
        qDebug("temp file not opened successfullly "+sfile);
    
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
                qDebug("RemoteCurrentDir1 "+oldRemoteCurrentDir);
            }
        } else if(strItem.find("/",0,TRUE) != -1) { // this is a directory
            qDebug("trying directory");
            if( !remoteChDir( (const QString &)currentRemoteDir + strItem)) {
                currentRemoteDir = oldRemoteCurrentDir;
                strItem="";
                qDebug("RemoteCurrentDir1 "+oldRemoteCurrentDir);

            } else {
            currentRemoteDir = currentRemoteDir+strItem;
            }
        } else {
            qDebug("download "+strItem);
        }
    }
    remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
    if(currentRemoteDir.right(1) !="/")
        currentRemoteDir +="/";
    currentPathEdit->setText( currentRemoteDir );
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

void OpieFtp::doLocalCd()
{
    localListClicked( Local_View->currentItem());
}

void OpieFtp:: doRemoteCd()
{
    remoteListClicked( Remote_View->currentItem());

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
          showLocalMenu(item);
    break;
    };
}

void OpieFtp::RemoteListPressed( int mouse, QListViewItem *item, const QPoint &point, int i)
{
 switch (mouse) {
      case 1:
          break;
      case 2:
          showRemoteMenu(item);
    break;
    };
}

void OpieFtp::showRemoteMenu(QListViewItem * item)
{
    QPopupMenu  m;// = new QPopupMenu( Local_View );
    if(item->text(0).right(1) == "/")
    m.insertItem( tr( "Change Directory" ), this, SLOT( doRemoteCd() ));
    else
    m.insertItem( tr( "Download" ), this, SLOT( remoteDownload() ));
    m.insertItem( tr( "Make Directory" ), this, SLOT( remoteMakDir() ));
    m.insertItem( tr( "Rename" ), this, SLOT( remoteRename() ));
    m.insertSeparator();
    m.insertItem( tr( "Delete" ), this, SLOT( remoteDelete() ));
    m.exec( QCursor::pos() );
}

void OpieFtp::showLocalMenu(QListViewItem * item)
{
    QPopupMenu  m;
    m.insertItem(  tr( "Show Hidden Files" ), this,  SLOT( showHidden() ));
    m.insertSeparator();
    if(item->text(0).right(1) == "/")
    m.insertItem( tr( "Change Directory" ), this, SLOT( doLocalCd() ));
    else
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
    remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
    }
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
              remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
              
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
              remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
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
    remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
    }
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

void OpieFtp::switchToLocalTab()
{
    TabWidget->setCurrentPage(0);
}

void OpieFtp::switchToRemoteTab()
{
    TabWidget->setCurrentPage(1);
}

void OpieFtp::switchToConfigTab()
{
    TabWidget->setCurrentPage(2);
}
