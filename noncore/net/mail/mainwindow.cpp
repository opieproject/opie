#include <qlabel.h>
#include <qvbox.h>
#include <qheader.h>
#include <qtimer.h>
#include <qlayout.h>

#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>

#include "defines.h"
#include "mainwindow.h"

using namespace Opie::Core;

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags flags )
    : QMainWindow( parent, name, flags )
{

    setCaption( tr( "Mail" ) );
    setToolBarsMovable( false );

    toolBar = new QToolBar( this );
    menuBar = new QMenuBar( toolBar );

    mailMenu = new QPopupMenu( menuBar );
    menuBar->insertItem( tr( "Mail" ), mailMenu );

    settingsMenu = new QPopupMenu( menuBar );
    menuBar->insertItem( tr( "Settings" ), settingsMenu );

    if (QApplication::desktop()->width()<330) {
        serverMenu = new QPopupMenu( mailMenu );
        folderMenu = new QPopupMenu( mailMenu );
        m_ServerMenuId = mailMenu->insertItem( tr( "Server" ), serverMenu );
        m_FolderMenuId = mailMenu->insertItem( tr( "Folder" ), folderMenu );
    } else {
        serverMenu = new QPopupMenu( menuBar );
        folderMenu = new QPopupMenu( menuBar );
        m_FolderMenuId = menuBar->insertItem( tr( "Folder" ), folderMenu );
        m_ServerMenuId = menuBar->insertItem( tr( "Server" ), serverMenu );
    }
    serverMenu->insertItem(tr("Disconnect"),SERVER_MENU_DISCONNECT);
    serverMenu->insertItem(tr("Set on/offline"),SERVER_MENU_OFFLINE);
    serverMenu->insertSeparator();
    serverMenu->insertItem(tr("Refresh folder list"),SERVER_MENU_REFRESH_FOLDER);
    serverMenu->insertItem(tr("Create new folder"),SERVER_MENU_CREATE_FOLDER);
    serverMenu->insertSeparator();
    serverMenu->insertItem(tr("(Un-)Subscribe groups"),SERVER_MENU_SUBSCRIBE);

    folderMenu->insertItem(tr("Refresh headerlist"),FOLDER_MENU_REFRESH_HEADER);
    folderMenu->insertItem(tr("Delete all mails"),FOLDER_MENU_DELETE_ALL_MAILS);
    folderMenu->insertItem(tr("New subfolder"),FOLDER_MENU_NEW_SUBFOLDER);
    folderMenu->insertItem(tr("Delete folder"),FOLDER_MENU_DELETE_FOLDER);
    folderMenu->insertItem(tr("Move/Copie all mails"),FOLDER_MENU_MOVE_MAILS);
    menuBar->setItemEnabled(m_ServerMenuId,false);
    menuBar->setItemEnabled(m_FolderMenuId,false);

    addToolBar( toolBar );
    toolBar->setHorizontalStretchable( true );

    QLabel *spacer = new QLabel( toolBar );
    spacer->setBackgroundMode( QWidget::PaletteButton );
    toolBar->setStretchableWidget( spacer );

    composeMail = new QAction( tr( "Compose new mail" ), ICON_COMPOSEMAIL,
                               0, 0, this );
    composeMail->addTo( toolBar );
    composeMail->addTo( mailMenu );

    sendQueued = new QAction( tr( "Send queued mails" ), ICON_SENDQUEUED,
                              0, 0, this );
    sendQueued->addTo( toolBar );
    sendQueued->addTo( mailMenu );

    /*
    syncFolders = new QAction( tr( "Sync mailfolders" ), ICON_SYNC,
                               0, 0, this );
    syncFolders->addTo( toolBar );
    syncFolders->addTo( mailMenu );
    */

    showFolders = new QAction( tr( "Show/Hide folders" ), ICON_SHOWFOLDERS,
                               0, 0, this, 0, true );
    showFolders->addTo( toolBar );
    showFolders->addTo( mailMenu );
    showFolders->setOn( true );
    connect(showFolders, SIGNAL( toggled(bool) ),
            SLOT( slotShowFolders(bool) ) );

    /*
      searchMails = new QAction( tr( "Search mails" ), QIconSet( Resource::loadPixmap("find") ),
                               0, 0, this );
    searchMails->addTo( toolBar );
    searchMails->addTo( mailMenu );
    */

    deleteMails = new QAction(tr("Delete Mail"), QIconSet( Resource::loadPixmap("trash")), 0, 0, this);
    deleteMails->addTo( toolBar );
    deleteMails->addTo( mailMenu );
    connect( deleteMails, SIGNAL( activated() ),
             SLOT( slotDeleteMail() ) );

    editSettings = new QAction( tr( "Edit settings" ), QIconSet( Resource::loadPixmap("SettingsIcon") ) ,
                                0, 0, this );
    editSettings->addTo( settingsMenu );
    connect( editSettings, SIGNAL( activated() ),
             SLOT( slotEditSettings() ) );
    editAccounts = new QAction( tr( "Configure accounts" ), QIconSet( Resource::loadPixmap("mail/editaccounts") ) ,
                                0, 0, this );
    editAccounts->addTo( settingsMenu );


    QWidget *view = new QWidget( this );
    setCentralWidget( view );
    layout = new QBoxLayout ( view, QBoxLayout::LeftToRight );

    folderView = new AccountView( view );
    folderView->header()->hide();
    folderView->setRootIsDecorated( true );
    folderView->addColumn( tr( "Mailbox" ) );
    connect(folderView,SIGNAL(serverSelected(int)),this,SLOT(serverSelected(int)));
    connect(serverMenu,SIGNAL(activated(int)),folderView,SLOT(slotContextMenu(int)));
    connect(folderMenu,SIGNAL(activated(int)),folderView,SLOT(slotContextMenu(int)));


    layout->addWidget( folderView );

    mailView = new QListView( view );
    mailView->addColumn( "" );
    mailView->addColumn( tr( "Subject" ),QListView::Manual );
    mailView->addColumn( tr( "Sender" ),QListView::Manual );
    mailView->addColumn( tr( "Size" ),QListView::Manual);
    mailView->addColumn( tr( "Date" ));
    mailView->setAllColumnsShowFocus(true);
    mailView->setShowSortIndicator(true);
    mailView->setSorting(4,false);

    layout->addWidget( mailView );
    layout->setStretchFactor( folderView, 1 );
    layout->setStretchFactor( mailView, 2 );

    m_Rotate = (QApplication::desktop()->width() > QApplication::desktop()->height()?0:90);

    slotAdjustLayout();

    QPEApplication::setStylusOperation( mailView->viewport(),QPEApplication::RightOnHold);
    QPEApplication::setStylusOperation( folderView->viewport(),QPEApplication::RightOnHold);

    connect( mailView, SIGNAL( mouseButtonClicked(int,QListViewItem*,const QPoint&,int) ),this,
             SLOT( mailLeftClicked(int,QListViewItem*,const QPoint&,int) ) );
    connect( mailView, SIGNAL( mouseButtonPressed(int,QListViewItem*,const QPoint&,int) ),this,
             SLOT( mailHold(int,QListViewItem*,const QPoint&,int) ) );

    connect(folderView, SIGNAL(refreshMailview(const QValueList<RecMailP>&)),
            this,SLOT(refreshMailView(const QValueList<RecMailP>&)));

    connect( composeMail, SIGNAL( activated() ), SLOT( slotComposeMail() ) );
    connect( sendQueued, SIGNAL( activated() ), SLOT( slotSendQueued() ) );

//    connect( searchMails, SIGNAL( activated() ), SLOT( slotSearchMails() ) );
    connect( editAccounts, SIGNAL( activated() ), SLOT( slotEditAccounts() ) );
    // Added by Stefan Eilers to allow starting by addressbook..
    // copied from old mail2

#if !defined(QT_NO_COP)
    connect( qApp, SIGNAL( appMessage(const QCString&,const QByteArray&) ),
             this, SLOT( appMessage(const QCString&,const QByteArray&) ) );
    m_sysChannel = new QCopChannel( "QPE/System", this );
    connect( m_sysChannel, SIGNAL( received(const QCString&,const QByteArray&) ),
        this, SLOT( systemMessage(const QCString&,const QByteArray&) ) );
#endif

    QTimer::singleShot( 10, this, SLOT( slotAdjustColumns() ) );
}

MainWindow::~MainWindow()
{
}

void MainWindow::serverSelected(int)
{
    odebug << "slotShowFolders not reached" << oendl;
}

void MainWindow::systemMessage( const QCString& msg, const QByteArray& data )
{
    int _newrotation;
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "setCurrentRotation(int)" )
    {
        stream >> _newrotation;
        if (m_Rotate != _newrotation) {
            slotAdjustLayout();
            m_Rotate = _newrotation;
        }
    }
}

void MainWindow::appMessage(const QCString &, const QByteArray &)
{
    odebug << "appMessage not reached" << oendl;
}

void MainWindow::slotAdjustLayout() {

    QWidget *d = QApplication::desktop();
    if ( d->width() < d->height() ) {
        layout->setDirection( QBoxLayout::TopToBottom );
    } else {
        layout->setDirection( QBoxLayout::LeftToRight );
    }
}

void MainWindow::slotAdjustColumns()
{
    bool hidden = folderView->isHidden();
    if ( hidden ) folderView->show();
    folderView->setColumnWidth( 0, folderView->visibleWidth() );
    if ( hidden ) folderView->hide();

    mailView->setColumnWidth( 0, 10 );
    mailView->setColumnWidth( 1, mailView->visibleWidth() - 130 );
    mailView->setColumnWidth( 2, 80 );
    mailView->setColumnWidth( 3, 50 );
    mailView->setColumnWidth( 4, 50 );
}

void MainWindow::slotEditSettings()
{
}

void MainWindow::slotShowFolders( bool )
{
   odebug << "slotShowFolders not reached" << oendl;
}

void MainWindow::refreshMailView(const QValueList<RecMailP>&)
{
   odebug << "refreshMailView not reached" << oendl;
}

void MainWindow::mailLeftClicked(int, QListViewItem *,const QPoint&,int )
{
    odebug << "mailLeftClicked not reached" << oendl;
}

void MainWindow::displayMail()
{
    odebug << "displayMail not reached" << oendl;
}

void MainWindow::slotDeleteMail()
{
    odebug << "deleteMail not reached" << oendl;
}

void MainWindow::mailHold(int, QListViewItem *,const QPoint&,int  )
{
    odebug << "mailHold not reached" << oendl;
}

void MainWindow::slotSendQueued()
{
}

void MainWindow::slotEditAccounts()
{
}

void MainWindow::slotComposeMail()
{
}
