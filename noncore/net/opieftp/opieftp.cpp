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
//#define DEVELOPERS_VERSION

#include "opieftp.h"

extern "C" {
#include "ftplib.h"
}

#include "inputDialog.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>
#include <qpe/mimetype.h>
#include <qpe/qpemessagebox.h>

#include <qstringlist.h>
#include <qtextstream.h>
#include <qpushbutton.h>
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
//     QPEToolBar *menuBar = new QPEToolBar(this);
//     menuBar->setHorizontalStretchable( TRUE );

    connectionMenu  = new QPopupMenu( this );
    localMenu  = new QPopupMenu( this );
    remoteMenu  = new QPopupMenu( this );
    tabMenu = new QPopupMenu( this );
    
    layout->addMultiCellWidget( menuBar, 0, 0, 0, 2 );

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
    localMenu->setCheckable(TRUE);

    remoteMenu->insertItem( tr( "Download" ), this, SLOT( remoteDownload() ));
    remoteMenu->insertItem( tr( "Make Directory" ), this, SLOT( remoteMakDir() ));
    remoteMenu->insertItem( tr( "Rename" ), this, SLOT( remoteRename() ));
    remoteMenu->insertSeparator();
    remoteMenu->insertItem( tr( "Delete" ), this, SLOT( remoteDelete() ));

    tabMenu->insertItem( tr( "Switch to Local" ), this, SLOT( switchToLocalTab() ));
    tabMenu->insertItem( tr( "Switch to Remote" ), this, SLOT( switchToRemoteTab() ));
    tabMenu->insertItem( tr( "Switch to Config" ), this, SLOT( switchToConfigTab() ));
    tabMenu->insertSeparator();
    tabMenu->insertItem( tr( "About" ), this, SLOT( doAbout() ));
    tabMenu->setCheckable(TRUE);



    cdUpButton = new QPushButton(Resource::loadIconSet("up"),"",this,"cdUpButton");
    cdUpButton ->setFixedSize( QSize( 20, 20 ) );
    connect( cdUpButton ,SIGNAL(released()),this,SLOT( upDir()) );
    cdUpButton ->setFlat(TRUE);
    layout->addMultiCellWidget( cdUpButton, 0, 0, 3, 3 );
     cdUpButton->hide();

//     docButton = new QPushButton(Resource::loadIconSet("DocsIcon"),"",this,"docsButton");
//     docButton->setFixedSize( QSize( 20, 20 ) );
//     connect( docButton,SIGNAL(released()),this,SLOT( docButtonPushed()) );
//     docButton->setFlat(TRUE);
//     layout->addMultiCellWidget( docButton, 0, 0, 6, 6 );

    homeButton = new QPushButton( Resource::loadIconSet("home"),"",this,"homeButton");
    homeButton->setFixedSize( QSize( 20, 20 ) );
    connect(homeButton,SIGNAL(released()),this,SLOT(homeButtonPushed()) );
    homeButton->setFlat(TRUE);
    layout->addMultiCellWidget( homeButton, 0, 0, 4, 4);
     homeButton->hide();

    TabWidget = new QTabWidget( this, "TabWidget" );
    layout->addMultiCellWidget( TabWidget, 1, 1, 0, 4 );

//     TabWidget->setTabShape(QTabWidget::Triangular);

    tab = new QWidget( TabWidget, "tab" );
    tabLayout = new QGridLayout( tab );
    tabLayout->setSpacing( 2);
    tabLayout->setMargin( 2);

    Local_View = new QListView( tab, "Local_View" );
//    Local_View->setResizePolicy( QListView::AutoOneFit );
    Local_View->addColumn( tr("File"),150);
    Local_View->addColumn( tr("Date"),-1);
    Local_View->setColumnAlignment(1,QListView::AlignRight);
    Local_View->addColumn( tr("Size"),-1);
    Local_View->setColumnAlignment(2,QListView::AlignRight);
    Local_View->setAllColumnsShowFocus(TRUE);

    Local_View->setMultiSelection( TRUE);
    Local_View->setSelectionMode(QListView::Extended);
    Local_View->setFocusPolicy(QWidget::ClickFocus);

    QPEApplication::setStylusOperation( Local_View->viewport(),QPEApplication::RightOnHold);

    tabLayout->addWidget( Local_View, 0, 0 );

     connect( Local_View, SIGNAL( clicked( QListViewItem*)),
              this,SLOT( localListClicked(QListViewItem *)) );
//     connect( Local_View, SIGNAL( doubleClicked( QListViewItem*)),
//              this,SLOT( localListClicked(QListViewItem *)) );
     connect( Local_View, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
              this,SLOT( ListPressed(int, QListViewItem *, const QPoint&, int)) );

    TabWidget->insertTab( tab, tr( "Local" ) );

    tab_2 = new QWidget( TabWidget, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2 );
    tabLayout_2->setSpacing( 2);
    tabLayout_2->setMargin( 2);

    Remote_View = new QListView( tab_2, "Remote_View" );
    Remote_View->addColumn( tr("File"),150);
    Remote_View->addColumn( tr("Date"),-1);
    Remote_View->setColumnAlignment(1,QListView::AlignRight);
    Remote_View->addColumn( tr("Size"),-1);
    Remote_View->setColumnAlignment(2,QListView::AlignRight);
    Remote_View->addColumn( tr("Dir"),-1);
    Remote_View->setColumnAlignment(4,QListView::AlignRight);
    Remote_View->setAllColumnsShowFocus(TRUE);

    Remote_View->setMultiSelection( FALSE);
    Remote_View->setSelectionMode(QListView::Extended);
    Remote_View->setFocusPolicy(QWidget::ClickFocus);

    QPEApplication::setStylusOperation( Remote_View->viewport(),QPEApplication::RightOnHold);

    connect( Remote_View, SIGNAL( clicked( QListViewItem*)),
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
    tabLayout_3->addMultiCellWidget( UsernameComboBox, 1, 1, 0, 1 );

    TextLabel2 = new QLabel( tab_3, "TextLabel2" );
    TextLabel2->setText( tr( "Password" ) );
    tabLayout_3->addMultiCellWidget( TextLabel2, 0, 0, 2, 3 );

    PasswordEdit = new QLineEdit( "", tab_3, "PasswordComboBox" );
    PasswordEdit->setEchoMode(QLineEdit::Password);
    tabLayout_3->addMultiCellWidget( PasswordEdit, 1, 1, 2, 3 );

    TextLabel3 = new QLabel( tab_3, "TextLabel3" );
    TextLabel3->setText( tr( "Remote server" ) );
    tabLayout_3->addMultiCellWidget( TextLabel3, 2, 2, 0, 1 );

    ServerComboBox = new QComboBox( FALSE, tab_3, "ServerComboBox" );
    ServerComboBox->setEditable(TRUE);
    tabLayout_3->addMultiCellWidget( ServerComboBox, 3, 3, 0, 1 );

    connect(ServerComboBox,SIGNAL(activated(int)),this,SLOT(serverComboSelected(int ) ));
    connect(ServerComboBox,SIGNAL(textChanged(const QString &)),this,SLOT(serverComboEdited(const QString & ) ));

    QLabel *TextLabel5 = new QLabel( tab_3, "TextLabel5" );
    TextLabel5->setText( tr( "Remote path" ) );
    tabLayout_3->addMultiCellWidget( TextLabel5, 2, 2, 2, 3 );


    remotePath = new QLineEdit( "/", tab_3, "remotePath" );
    tabLayout_3->addMultiCellWidget( remotePath, 3, 3, 2, 3 );

    TextLabel4 = new QLabel( tab_3, "TextLabel4" );
    TextLabel4->setText( tr( "Port" ) );
    tabLayout_3->addMultiCellWidget( TextLabel4, 4, 4, 0, 1 );

    PortSpinBox = new QSpinBox( tab_3, "PortSpinBox" );
    PortSpinBox->setButtonSymbols( QSpinBox::UpDownArrows );
    PortSpinBox->setMaxValue(32786);
    tabLayout_3->addMultiCellWidget( PortSpinBox, 4, 4, 1, 1);

    QPushButton *deleteServerBtn;
    deleteServerBtn = new QPushButton( "Delete Server", tab_3 , "OpenButton" );
    tabLayout_3->addMultiCellWidget( deleteServerBtn, 5, 5, 2, 3);

    connect(deleteServerBtn,SIGNAL(clicked()),SLOT(deleteServer()));

    connectServerBtn = new QPushButton( "Connect", tab_3 , "ConnectButton" );
    tabLayout_3->addMultiCellWidget( connectServerBtn, 5, 5, 0, 1);
    connectServerBtn->setToggleButton(TRUE);
    connect(connectServerBtn,SIGNAL( toggled( bool)),SLOT( connectorBtnToggled(bool) ));

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_3->addItem( spacer, 5, 0 );

    TabWidget->insertTab( tab_3, tr( "Config" ) );

    connect(TabWidget,SIGNAL(currentChanged(QWidget *)),
            this,SLOT(tabChanged(QWidget*)));

    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    currentDir.setPath( QDir::currentDirPath());
//      currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);

    currentPathCombo = new QComboBox( FALSE, this, "currentPathCombo" );
    layout->addMultiCellWidget( currentPathCombo, 3, 3, 0, 4);

    currentPathCombo->setEditable(TRUE);
    currentPathCombo->lineEdit()->setText( currentDir.canonicalPath());

    connect( currentPathCombo, SIGNAL( activated( const QString & ) ),
              this, SLOT(  currentPathComboActivated( const QString & ) ) );

    connect( currentPathCombo->lineEdit(),SIGNAL(returnPressed()),
             this,SLOT(currentPathComboChanged()));

    ProgressBar = new QProgressBar( this, "ProgressBar" );
    layout->addMultiCellWidget( ProgressBar, 4, 4, 0, 4);

//     fillCombos();

    filterStr="*";
    b=FALSE;
    populateLocalView();
    readConfig();
    ServerComboBox->setCurrentItem(currentServerConfig);
    
    TabWidget->setCurrentPage(2);
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
    exit(0);
}

void OpieFtp::tabChanged(QWidget *w)
{
    if (TabWidget->currentPageIndex() == 0) {
        currentPathCombo->lineEdit()->setText( currentDir.canonicalPath());
        tabMenu->setItemChecked(tabMenu->idAt(0),TRUE);
        tabMenu->setItemChecked(tabMenu->idAt(1),FALSE);
        tabMenu->setItemChecked(tabMenu->idAt(2),FALSE);
        if(cdUpButton->isHidden())
            cdUpButton->show();
        if(homeButton->isHidden())
            homeButton->show();
            
    }
    if (TabWidget->currentPageIndex() == 1) {
        currentPathCombo->lineEdit()->setText( currentRemoteDir );
        tabMenu->setItemChecked(tabMenu->idAt(1),TRUE);
        tabMenu->setItemChecked(tabMenu->idAt(0),FALSE);
        tabMenu->setItemChecked(tabMenu->idAt(2),FALSE);
        if(cdUpButton->isHidden())
            cdUpButton->show();
        homeButton->hide();

    }
    if (TabWidget->currentPageIndex() == 2) {
        tabMenu->setItemChecked(tabMenu->idAt(2),TRUE);
        tabMenu->setItemChecked(tabMenu->idAt(0),FALSE);
        tabMenu->setItemChecked(tabMenu->idAt(1),FALSE);
        cdUpButton->hide();
        homeButton->hide();
    }
}

void OpieFtp::newConnection()
{
   UsernameComboBox->lineEdit()->setText("");
   PasswordEdit->setText( "" );
   ServerComboBox->lineEdit()->setText( "");
   remotePath->setText( currentRemoteDir = "/");
   PortSpinBox->setValue( 21);
   TabWidget->setCurrentPage(2);
}

void OpieFtp::serverComboEdited(const QString & edit)
{
 if(  ServerComboBox->text(currentServerConfig) != edit /*edit.isEmpty() */) {
     qDebug("ServerComboEdited");
        currentServerConfig = -1;
    }
}

void OpieFtp::connectorBtnToggled(bool On)
{
    if(On) {
        connector();
    } else {
        disConnector();
    }

}

void OpieFtp::connector()
{
//    QCopEnvelope ( "QPE/System", "busy()" );
//    qApp->processEvents();
    currentRemoteDir=remotePath->text();
    if(ServerComboBox->currentText().isEmpty()) {
        QMessageBox::warning(this,tr("Ftp"),tr("Please set the server info"),tr("Ok"),0,0);
        TabWidget->setCurrentPage(2);
        ServerComboBox->setFocus();
        connectServerBtn->setOn(FALSE);
        connectServerBtn->setText( tr("Connect"));
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
        QMessageBox::message(tr("Note"),tr("Unable to connect to\n")+ftp_host);
        connectServerBtn->setOn(FALSE);
        connectServerBtn->setText( tr("Connect"));
        return ;
    }
    if (!FtpLogin( ftp_user.latin1(), ftp_pass.latin1(),conn )) {
        QString msg;
        msg.sprintf(tr("Unable to log in\n")+"%s",FtpLastResponse(conn));
        msg.replace(QRegExp(":"),"\n");
        QMessageBox::message(tr("Note"),msg);
        if(conn)
            FtpQuit(conn);
        connectServerBtn->setOn(FALSE);
        connectServerBtn->setText( tr("Connect"));
        return ;
    }
    remoteDirList("/") ;
    setCaption(ftp_host);
    writeConfig();
    connectServerBtn->setText( tr("Disconnect"));
//     QCopEnvelope ( "QPE/System", "notBusy()" );
}

void OpieFtp::disConnector()
{
    if(conn)
        FtpQuit(conn);
    setCaption("OpieFtp");
    currentRemoteDir="/";
    Remote_View->clear();
    connectServerBtn->setText( tr("Connect"));
    connectServerBtn->setOn(FALSE);
    setCaption("OpieFtp");
}

void OpieFtp::localUpload()
{
    int fsz;
//    QCopEnvelope ( "QPE/System", "busy()" );
//    qApp->processEvents();
    QList<QListViewItem> * getSelectedItems( QListView * Local_View );
    QListViewItemIterator it( Local_View );
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() ) {
            QString strItem = it.current()->text(0);
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
                    msg.sprintf(tr("Unable to upload\n")+"%s",FtpLastResponse(conn));
                    msg.replace(QRegExp(":"),"\n");
                    QMessageBox::message(tr("Note"),msg);
                }
            } else {
                QMessageBox::message(tr("Note"),tr("Cannot upload directories"));
            }
            ProgressBar->reset();
            nullifyCallBack();
        it.current()->setSelected(FALSE);
        } //end currentSelected
    }
    for ( ; it.current(); ++it ) {
        Local_View->clearSelection();
    }
    Local_View->clearFocus();
    TabWidget->setCurrentPage(1);
    remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
//    QCopEnvelope ( "QPE/System", "notBusy()" );
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
//    qApp->processEvents();
    int fsz;
//    QCopEnvelope ( "QPE/System", "busy()" );

    QList<QListViewItem> * getSelectedItems( QListView * Remote_View );
    QListViewItemIterator it( Remote_View );
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() ) {
            QString strItem = it.current()->text(0);
//      strItem=strItem.right(strItem.length()-1);
            QString localFile = currentDir.canonicalPath();
            if(localFile.right(1).find("/",0,TRUE) == -1)
                localFile += "/";
            localFile += strItem;
//    QString localFile = currentDir.canonicalPath()+"/"+strItem;
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
                msg.sprintf(tr("Unable to download \n")+"%s",FtpLastResponse(conn));
                msg.replace(QRegExp(":"),"\n");
                QMessageBox::message(tr("Note"),msg);
            }
            ProgressBar->reset();
            nullifyCallBack();
        it.current()->setSelected(FALSE);
        }
    }
    for ( ; it.current(); ++it ) {
        Remote_View->clearSelection();
    }
    Remote_View->setFocus();    
    TabWidget->setCurrentPage(0);
    populateLocalView();
//    QCopEnvelope ( "QPE/System", "notBusy()" );
}

bool OpieFtp::remoteDirList(const QString &dir)
{
    QString tmp = QDir::homeDirPath();
    if(tmp.right(1) != "/")
        tmp+="/._temp";
    else
        tmp+="._temp";
//    qDebug("Listing remote dir "+tmp);
//    QCopEnvelope ( "QPE/System", "busy()" );
    if (!FtpDir( tmp.latin1(), dir.latin1(), conn) ) {
        QString msg;
        msg.sprintf(tr("Unable to list the directory\n")+dir+"\n%s",FtpLastResponse(conn) );
        msg.replace(QRegExp(":"),"\n");
        QMessageBox::message(tr("Note"),msg);
        return false;
    }
    populateRemoteView() ;
//    QCopEnvelope ( "QPE/System", "notBusy()" );
    return true;
}

bool OpieFtp::remoteChDir(const QString &dir)
{
//    QCopEnvelope ( "QPE/System", "busy()" );
    if (!FtpChdir( dir.latin1(), conn )) {
        QString msg;
        msg.sprintf(tr("Unable to change directories\n")+dir+"\n%s",FtpLastResponse(conn));
        msg.replace(QRegExp(":"),"\n");
        QMessageBox::message(tr("Note"),msg);
//        qDebug(msg);
//        QCopEnvelope ( "QPE/System", "notBusy()" );
        return FALSE;
    }
//    QCopEnvelope ( "QPE/System", "notBusy()" );
    return TRUE;
}

void OpieFtp::populateLocalView()
{
    Local_View->clear();
    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    currentDir.setMatchAllDirs(TRUE);
    currentDir.setNameFilter(filterStr);
    QString fileL, fileS, fileDate;
    bool isDir=FALSE;
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
                isDir=TRUE;
//     qDebug( fileL);
            }
        }
        if(fileL !="./" && fi->exists()) {
            item= new QListViewItem( Local_View,fileL, fileDate, fileS );
            QPixmap pm;
         
            if(isDir || fileL.find("/",0,TRUE) != -1) {
                if( !QDir( fi->filePath() ).isReadable())
                    pm = Resource::loadPixmap( "lockedfolder" );
                else
                    pm= Resource::loadPixmap( "folder" );
                item->setPixmap( 0,pm );
            } else {
                if( !fi->isReadable() )
                    pm = Resource::loadPixmap( "locked" );
                else {
                    MimeType mt(fi->filePath());
                    pm=mt.pixmap(); //sets the correct pixmap for mimetype
                    if(pm.isNull())
                        pm =  Resource::loadPixmap( "UnknownDocument-14" );
                    item->setPixmap( 0,pm);
                }
            }
            if(  fileL.find("->",0,TRUE) != -1) {
                  // overlay link image
                pm= Resource::loadPixmap( "folder" );
                QPixmap lnk = Resource::loadPixmap( "opie/symlink" );
                QPainter painter( &pm );
                painter.drawPixmap( pm.width()-lnk.width(), pm.height()-lnk.height(), lnk );
                pm.setMask( pm.createHeuristicMask( FALSE ) );
                item->setPixmap( 0, pm);
            }
        }
        isDir=FALSE;
        ++it;
    }
    Local_View->setSorting( 3,FALSE);
    currentPathCombo->lineEdit()->setText( currentDir.canonicalPath() );
    fillCombo( (const QString &)currentDir);
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
    QListViewItem *itemDir=NULL, *itemFile=NULL;
    QString fileL, fileS, fileDate;
    if ( file.open(IO_ReadOnly)) {
        QTextStream t( &file );   // use a text stream
        while ( !t.eof()) {
            s = t.readLine();
            fileL = s.right(s.length()-55);
            fileL = fileL.stripWhiteSpace();
            if(s.left(1) == "d") 
                fileL = fileL+"/";
//                  fileL = "/"+fileL+"/";
            fileS = s.mid( 30, 42-30);
            fileS = fileS.stripWhiteSpace();
            fileDate = s.mid( 42, 55-42);
            fileDate = fileDate.stripWhiteSpace();
            if(fileL.find("total",0,TRUE) == -1) {
                if(s.left(1) == "d" || fileL.find("/",0,TRUE) != -1) {
                QListViewItem * item = new QListViewItem( Remote_View, fileL, fileDate, fileS,"d");
                    item->setPixmap( 0, Resource::loadPixmap( "folder" ));
//                      if(itemDir)
                          item->moveItem(itemDir);
                      itemDir=item;
                } else {
                QListViewItem * item = new QListViewItem( Remote_View, fileL, fileDate, fileS,"f");
                    item->setPixmap( 0, Resource::loadPixmap( "fileopen" ));
//                      if(itemFile)
                          item->moveItem(itemDir);
                          item->moveItem(itemFile);
                      itemFile=item;
                }
            }
        }
        QListViewItem * item1 = new QListViewItem( Remote_View, "../");
        item1->setPixmap( 0, Resource::loadPixmap( "folder" ));
        file.close();
        if( file.exists())
            file. remove();
    } else
        qDebug("temp file not opened successfullly "+sfile);
    Remote_View->setSorting( 4,TRUE);
    return true;
}

void OpieFtp::remoteListClicked(QListViewItem *selectedItem)
{
    if( selectedItem) {
//         QCopEnvelope ( "QPE/System", "busy()" );
        QString  oldRemoteCurrentDir =  currentRemoteDir;
        QString strItem=selectedItem->text(0);
        strItem=strItem.simplifyWhiteSpace();
        if(strItem == "../") { // the user wants to go ^
            if( FtpCDUp( conn) == 0) {
                QString msg;
                msg.sprintf(tr("Unable to cd up\n")+"%s",FtpLastResponse(conn));
                msg.replace(QRegExp(":"),"\n");
                QMessageBox::message(tr("Note"),msg);
//                qDebug(msg);
            }
            char path[256];
            if( FtpPwd( path,sizeof(path),conn) == 0) { //this is easier than fudging the string
                QString msg;
                msg.sprintf(tr("Unable to get working dir\n")+"%s",FtpLastResponse(conn));
                msg.replace(QRegExp(":"),"\n");
                QMessageBox::message(tr("Note"),msg);
//                qDebug(msg);
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
//                     qDebug("RemoteCurrentDir1 "+oldRemoteCurrentDir);
                }
            } else if(strItem.find("/",0,TRUE) != -1) { // this is a directory
                if( !remoteChDir( (const QString &)currentRemoteDir + strItem)) {
                    currentRemoteDir = oldRemoteCurrentDir;
                    strItem="";
//                     qDebug("RemoteCurrentDir1 "+oldRemoteCurrentDir);

                } else {
                    currentRemoteDir = currentRemoteDir+strItem;
                }
            } else {
//                QCopEnvelope ( "QPE/System", "notBusy()" );
                return;
            }
        }
        remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
        if(currentRemoteDir.right(1) !="/")
            currentRemoteDir +="/";
        currentPathCombo->lineEdit()->setText( currentRemoteDir);
        fillRemoteCombo( (const QString &)currentRemoteDir);
//        QCopEnvelope ( "QPE/System", "notBusy()" );
    }
}

void OpieFtp::localListClicked(QListViewItem *selectedItem)
{
    if(selectedItem!= NULL) {
    
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
                      //  qDebug("upload "+strItem);
                    return;
                }
            } //end not symlink
            chdir(strItem.latin1());
        }
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
    localMenu->setItemChecked(localMenu->idAt(0),TRUE);
//    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    b=TRUE;

    }  else {
    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden*/ | QDir::All);
    localMenu->setItemChecked(localMenu->idAt(0),FALSE);
//    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    b=FALSE;
    }
    populateLocalView();
}

void OpieFtp::ListPressed( int mouse, QListViewItem *item, const QPoint &point, int i)
{
// if(item)
    if (mouse == 2) {
        showLocalMenu(item);
    }
}

void OpieFtp::RemoteListPressed( int mouse, QListViewItem *item, const QPoint &point, int i)
{
    if(mouse == 2) {
        showRemoteMenu(item);
    }
}

void OpieFtp::showRemoteMenu(QListViewItem * item)
{
    QPopupMenu * m;// = new QPopupMenu( Local_View );
    m = new QPopupMenu(this);
    if( /*item->text(0).right(1) == "/" ||*/ item->text(0).find("/",0,TRUE) != -1)
        m->insertItem( tr( "Change Directory" ), this, SLOT( doRemoteCd() ));
    else
    m->insertItem( tr( "Download" ), this, SLOT( remoteDownload() ));
    m->insertItem( tr( "Make Directory" ), this, SLOT( remoteMakDir() ));
    m->insertItem( tr("Rescan"), this, SLOT( populateLocalView() ));
    m->insertItem( tr( "Rename" ), this, SLOT( remoteRename() ));
    m->insertSeparator();
    m->insertItem( tr( "Delete" ), this, SLOT( remoteDelete() ));
    m->exec( QCursor::pos() );
    delete m;
}

void OpieFtp::showLocalMenu(QListViewItem * item)
{

 QPopupMenu  *m;
 m = new QPopupMenu( this);
    m->insertItem(  tr( "Show Hidden Files" ), this,  SLOT( showHidden() ));
    m->insertSeparator();
    if( /*item->text(0).right(1) == "/" ||*/ item->text(0).find("/",0,TRUE) !=-1)
    m->insertItem( tr( "Change Directory" ), this, SLOT( doLocalCd() ));
    else
    m->insertItem( tr( "Upload" ), this, SLOT( localUpload() ));
    m->insertItem( tr( "Make Directory" ), this, SLOT( localMakDir() ));
    m->insertItem( tr("Rescan"), this, SLOT( populateRemoteView() ));
    m->insertItem( tr( "Rename" ), this, SLOT( localRename() ));
    m->insertSeparator();
    m->insertItem( tr( "Delete" ), this, SLOT( localDelete() ));
    m->setCheckable(TRUE);
    if (b)
        m->setItemChecked(m->idAt(0),TRUE);
    else
        m->setItemChecked(m->idAt(0),FALSE);

    m->exec( QCursor::pos() );
    delete m;
}

void OpieFtp::localMakDir()
{
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Make Directory"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
       QString  filename = fileDlg->LineEdit1->text();
       currentDir.mkdir( currentDir.canonicalPath()+"/"+filename);
    }
    populateLocalView();
}

void OpieFtp::localDelete()
{
    QList<QListViewItem> * getSelectedItems( QListView * Local_View );
    QListViewItemIterator it( Local_View );
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() ) {
            QString f = it.current()->text(0);
            it.current()->setSelected(FALSE);
            
//    QString f = Local_View->currentItem()->text(0);
            if(QDir(f).exists() ) {
                switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f+
                                              tr(" ?\nIt must be empty"),tr("Yes"),tr("No"),0,0,1) ) {
                  case 0: {
                      f=currentDir.canonicalPath()+"/"+f;
                      QString cmd="rmdir "+f;
                      system( cmd.latin1());
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
                      f=currentDir.canonicalPath()+"/"+f;
                      QString cmd="rm "+f;
                      system( cmd.latin1());
                  }
                      break;
                  case 1: 
                        // exit
                      break;
                };
            }
        }
    }
    populateLocalView();
    
}

void OpieFtp::remoteMakDir()
{
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Make Directory"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
       QString  filename = fileDlg->LineEdit1->text();//+".playlist";
       QString tmp=currentRemoteDir+filename;
//       QCopEnvelope ( "QPE/System", "busy()" );
       if(FtpMkdir( tmp.latin1(), conn) == 0) {
           QString msg;
           msg.sprintf(tr("Unable to make directory\n")+"%s",FtpLastResponse(conn));
           msg.replace(QRegExp(":"),"\n");
           QMessageBox::message(tr("Note"),msg);
       }
//       QCopEnvelope ( "QPE/System", "notBusy()" );
    remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
    }
}

void OpieFtp::remoteDelete()
{
    QList<QListViewItem> * getSelectedItems( QListView * Remote_View );
    QListViewItemIterator it( Remote_View );
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() ) {
            QString f = it.current()->text(0);
//    QString f = Remote_View->currentItem()->text(0);
//            QCopEnvelope ( "QPE/System", "busy()" );
            if( f.right(1) =="/") {
                QString path= currentRemoteDir+f;
                switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f+"?"
                                              ,tr("Yes"),tr("No"),0,0,1) ) {
                  case 0: {
                      f=currentDir.canonicalPath()+"/"+f;
                      if(FtpRmdir( path.latin1(), conn) ==0) {
                          QString msg;
                          msg.sprintf(tr("Unable to remove directory\n")+"%s",FtpLastResponse(conn));
                          msg.replace(QRegExp(":"),"\n");
                          QMessageBox::message(tr("Note"),msg);
                      }
                      remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
                  }
                      break;
                };
            } else {
                switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f+"?"
                                              ,tr("Yes"),tr("No"),0,0,1) ) {
                  case 0: {
                      QString path= currentRemoteDir+f;
                      if(FtpDelete( path.latin1(), conn)==0) {
                          QString msg;
                          msg.sprintf(tr("Unable to delete file\n")+"%s",FtpLastResponse(conn));
                          msg.replace(QRegExp(":"),"\n");
                          QMessageBox::message(tr("Note"),msg);
                      }
                      remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
                  }
                      break;
                };
            }
        }
    }
//    QCopEnvelope ( "QPE/System", "notBusy()" );
}

void OpieFtp::remoteRename()
{
    QString curFile = Remote_View->currentItem()->text(0);
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Rename"),TRUE, 0);
    fileDlg->setTextEdit((const QString &)curFile);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        QString oldName = currentRemoteDir +"/"+ curFile;
        QString newName = currentRemoteDir  +"/"+ fileDlg->LineEdit1->text();//+".playlist";
//        QCopEnvelope ( "QPE/System", "busy()" );
        if(FtpRename( oldName.latin1(), newName.latin1(),conn) == 0) {
            QString msg;
            msg.sprintf(tr("Unable to rename file\n")+"%s",FtpLastResponse(conn));
            msg.replace(QRegExp(":"),"\n");
            QMessageBox::message(tr("Note"),msg);
        }
//        QCopEnvelope ( "QPE/System", "notBusy()" );
    remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
    }
}

void OpieFtp::localRename()
{
    QString curFile = Local_View->currentItem()->text(0);
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Rename"),TRUE, 0);
    fileDlg->setTextEdit((const QString &)curFile);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        QString oldname =  currentDir.canonicalPath() + "/" + curFile;
        QString newName =  currentDir.canonicalPath() + "/" + fileDlg->LineEdit1->text();//+".playlist";
        if( rename(oldname.latin1(), newName.latin1())== -1)
            QMessageBox::message(tr("Note"),tr("Could not rename"));
    }
    populateLocalView();
}

void OpieFtp::currentPathComboActivated(const QString & currentPath) {
    if (TabWidget->currentPageIndex() == 0) {
    chdir( currentPath.latin1() );
    currentDir.cd( currentPath, TRUE);
    populateLocalView();
    update();
    } else {
//     chdir( currentPath.latin1() );
//     currentDir.cd( currentPath, TRUE);
//     populateList();
//     update();

    }
}

void OpieFtp::fillCombo(const QString &currentPath) {

        currentPathCombo->lineEdit()->setText(currentPath);
        if( localDirPathStringList.grep(currentPath,TRUE).isEmpty() ) {
            currentPathCombo->clear();
            localDirPathStringList.prepend(currentPath );
            currentPathCombo->insertStringList( localDirPathStringList,-1);
        }
        currentPathCombo->lineEdit()->setText(currentPath);
        if( remoteDirPathStringList.grep(currentPath,TRUE).isEmpty() ) {
            currentPathCombo->clear();
            remoteDirPathStringList.prepend(currentPath );
            currentPathCombo->insertStringList( remoteDirPathStringList,-1);
        }
}

void OpieFtp::fillRemoteCombo(const QString &currentPath) {

        currentPathCombo->lineEdit()->setText(currentPath);
        if( remoteDirPathStringList.grep(currentPath,TRUE).isEmpty() ) {
            currentPathCombo->clear();
            remoteDirPathStringList.prepend(currentPath );
            currentPathCombo->insertStringList( remoteDirPathStringList,-1);
        }
}

void OpieFtp::currentPathComboChanged()
{
    QString  oldRemoteCurrentDir =  currentRemoteDir;
//    qDebug("oldRemoteCurrentDir "+oldRemoteCurrentDir);
    if (TabWidget->currentPageIndex() == 0) {
        if(QDir( currentPathCombo->lineEdit()->text()).exists()) {
            currentDir.setPath( currentPathCombo->lineEdit()->text() );
            populateLocalView();
        } else {
            QMessageBox::message(tr("Note"),tr("That directory does not exist"));
        }
    }
    if (TabWidget->currentPageIndex() == 1) {
        currentRemoteDir = currentPathCombo->lineEdit()->text();
        if(currentRemoteDir.right(1) !="/") {
            currentRemoteDir = currentRemoteDir +"/";
            currentPathCombo->lineEdit()->setText( currentRemoteDir );
        }
            if( !remoteChDir( (const QString &)currentRemoteDir) ) {
                currentRemoteDir = oldRemoteCurrentDir;
                currentPathCombo->lineEdit()->setText( currentRemoteDir );
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

void OpieFtp::readConfig()
{
    fillCombos();
    Config cfg("opieftp");
    cfg.setGroup("Server");
    currentServerConfig = cfg.readNumEntry("currentServer", -1);
//    qDebug("Reading %d", currentServerConfig);
    serverComboSelected( currentServerConfig);
}

void OpieFtp::writeConfig()
{
    Config cfg("opieftp");
    cfg.setGroup("Server");
    QString username, remoteServerStr, remotePathStr, password, port, temp;
    int numberOfEntries = cfg.readNumEntry("numberOfEntries",0);
    if( currentServerConfig == -1) {
    for (int i = 1; i <= numberOfEntries; i++) {
        temp.setNum(i);
        cfg.setGroup("Server");
        QString tempStr = cfg.readEntry( temp,"");
    }
        temp.setNum( numberOfEntries + 1);
        cfg.setGroup("Server");
        remoteServerStr = cfg.readEntry( temp,"");
        int divider = remoteServerStr.length() - remoteServerStr.find(":",0,TRUE);
        remoteServerStr = remoteServerStr.left(remoteServerStr.length()-divider);

        temp.setNum(numberOfEntries+1);
        cfg.setGroup("Server");
        cfg.writeEntry( temp, ServerComboBox->currentText() +":"+PortSpinBox->cleanText() );
        cfg.setGroup(temp);
        cfg.writeEntry("RemotePath", remotePath->text());
        cfg.writeEntry("Username", UsernameComboBox->currentText());
        cfg.writeEntryCrypt( UsernameComboBox->currentText(), PasswordEdit->text());
        cfg.setGroup("Server");
        cfg.writeEntry("numberOfEntries", QString::number(numberOfEntries + 1 ));
    }
}

void OpieFtp::fillCombos()
{
    Config cfg("opieftp");
    cfg.setGroup("Server");
    QString username, remoteServerStr, remotePathStr, password, port, temp;
    int numberOfEntries = cfg.readNumEntry("numberOfEntries",0);
    for (int i = 1; i <= numberOfEntries; i++) {
        temp.setNum(i);
        cfg.setGroup("Server");
        remoteServerStr = cfg.readEntry( temp,"");
        int divider = remoteServerStr.length() - remoteServerStr.find(":",0,TRUE);
        port = remoteServerStr.right( divider - 1);
        bool ok;
        PortSpinBox->setValue( port.toInt(&ok,10));

        remoteServerStr = remoteServerStr.left(remoteServerStr.length()-divider);
        ServerComboBox->insertItem( remoteServerStr );
//          cfg.setGroup(temp);

//          remotePathStr = cfg.readEntry(remoteServer,"");
//          int divider = remoteServer.length() - remoteServer.find(":",0,TRUE);
//          port = remoteServer.right( divider+1);
//          PortSpinBox->setValue( port);

//          remoteServer = remoteServer.left(divider - 1);
//          remotePath->setText( remotePathStr);

//          username = cfg.readEntry(temp);
//          UsernameComboBox->insertItem(username);
//          password = cfg.readEntryCrypt(username,"");
//          PasswordEdit->setText(password);
    }
}


void OpieFtp::serverComboSelected(int index)
{
    currentServerConfig = index;
    QString username, remoteServerStr, remotePathStr, password, port, temp;
//    remoteServerStr = ServerComboBox->text(index);
    Config cfg("opieftp");
    cfg.setGroup("Server");
    int numberOfEntries = cfg.readNumEntry("numberOfEntries",0);

    temp.setNum(index+1);
    remoteServerStr = cfg.readEntry( temp,"");
    cfg.setGroup(temp);
//    qDebug(temp);
    int divider = remoteServerStr.length() - remoteServerStr.find(":",0,TRUE);
    port = remoteServerStr.right( divider - 1);
    bool ok;
    int portInt = port.toInt(&ok,10);
    if( portInt == 0) portInt = 21;

    PortSpinBox->setValue( portInt);

    remotePath->setText(cfg.readEntry("RemotePath", "/"));

    username =  cfg.readEntry("Username", "anonymous");
    UsernameComboBox->lineEdit()->setText(username);

//    qDebug("Password is "+cfg.readEntryCrypt(username, "me@opieftp.org"));
    PasswordEdit->setText(cfg.readEntryCrypt(username, "me@opieftp.org"));

    cfg.setGroup("Server");
    temp.sprintf("%d",currentServerConfig);
    cfg.writeEntry("currentServer", temp);    
    update();
}
//    UsernameComboBox->lineEdit()->setText("root");
//    PasswordEdit->setText( tr( "" ) );
//    ServerComboBox->lineEdit()->setText( tr( "192.168.129.201" ) );
//    remotePath->setText( currentRemoteDir = "/home/llornkcor/");
//    PortSpinBox->setValue( 4242);

void OpieFtp::deleteServer()
{
    QString username, remoteServerStr, remotePathStr, password, port, temp;
    remoteServerStr = ServerComboBox->currentText( );
    username = UsernameComboBox->currentText();
    Config cfg("opieftp");
    cfg.setGroup("Server");
    int numberOfEntries = cfg.readNumEntry("numberOfEntries",0);
    for (int i = 0; i <= numberOfEntries; i++) {
        temp.setNum(i+1);
        cfg.setGroup("Server");
        if( cfg.readEntry( temp,"").find( remoteServerStr,0,TRUE)  != -1
            && cfg.readEntry(temp).find(username,0,TRUE) != -1
            && !remoteServerStr.isEmpty()) {
            qDebug(temp);
        }
    }
}

void OpieFtp::upDir()
{
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
        if( FtpCDUp( conn) == 0) {
            QString msg;
            msg.sprintf(tr("Unable to cd up\n")+"%s",FtpLastResponse(conn));
            msg.replace(QRegExp(":"),"\n");
            QMessageBox::message(tr("Note"),msg);
//                qDebug(msg);
        }
        char path[256];
        if( FtpPwd( path,sizeof(path),conn) == 0) { //this is easier than fudging the string
            QString msg;
            msg.sprintf(tr("Unable to get working dir\n")+"%s",FtpLastResponse(conn));
            msg.replace(QRegExp(":"),"\n");
            QMessageBox::message(tr("Note"),msg);
//                qDebug(msg);
        }
        currentRemoteDir=path;
        remoteDirList( (const QString &)currentRemoteDir); //this also calls populate
        if(currentRemoteDir.right(1) !="/")
            currentRemoteDir +="/";
        currentPathCombo->lineEdit()->setText( currentRemoteDir);
        fillRemoteCombo( (const QString &)currentRemoteDir);

    }
}

void OpieFtp::docButtonPushed() {
    QString current = QPEApplication::documentDir();
    chdir( current.latin1() );
    currentDir.cd( current, TRUE);
     populateLocalView();
    update();

}

void OpieFtp::homeButtonPushed() {
    QString current = QDir::homeDirPath();
    chdir( current.latin1() );
    currentDir.cd(  current, TRUE);
     populateLocalView();
    update();
}

void OpieFtp::doAbout() {
    QMessageBox::message("OpieFtp","Opie ftp client is copyright 2002 by\n"
                         "L.J.Potter<llornkcor@handhelds.org>\n"
                         "and uses ftplib copyright 1996-2000\n"
                         "by Thomas Pfau, pfau@cnj.digex.net\n\n"
                         "and is licensed by the GPL");
}
