#include <qlabel.h>
#include <qvbox.h>
#include <qheader.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qmessagebox.h>

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

#include "defines.h"
#include "mainwindow.h"


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
    connect(showFolders, SIGNAL( toggled( bool ) ),
            SLOT( slotShowFolders( bool ) ) );

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

    //setCentralWidget( view );

    QVBox* wrapperBox = new QVBox( this );
    setCentralWidget( wrapperBox );

    QWidget *view = new QWidget( wrapperBox );

    layout = new QBoxLayout ( view, QBoxLayout::LeftToRight );

    folderView = new AccountView( view );
    folderView->header()->hide();
    folderView->setRootIsDecorated( true );
    folderView->addColumn( tr( "Mailbox" ) );

    layout->addWidget( folderView );

    mailView = new QListView( view );
    mailView->addColumn( tr( "" ) );
    mailView->addColumn( tr( "Subject" ),QListView::Manual );
    mailView->addColumn( tr( "Sender" ),QListView::Manual );
    mailView->addColumn( tr( "Size" ),QListView::Manual);
    mailView->addColumn( tr( "Date" ));
    mailView->setAllColumnsShowFocus(true);
    mailView->setSorting(-1);

    statusWidget = new StatusWidget( wrapperBox );
    statusWidget->hide();

    layout->addWidget( mailView );
    layout->setStretchFactor( folderView, 1 );
    layout->setStretchFactor( mailView, 2 );

    slotAdjustLayout();

    QPEApplication::setStylusOperation( mailView->viewport(),QPEApplication::RightOnHold);
    QPEApplication::setStylusOperation( folderView->viewport(),QPEApplication::RightOnHold);

    connect( mailView, SIGNAL( mouseButtonClicked(int, QListViewItem *,const QPoint&,int  ) ),this,
             SLOT( mailLeftClicked( int, QListViewItem *,const QPoint&,int  ) ) );
    connect( mailView, SIGNAL( mouseButtonPressed(int, QListViewItem *,const QPoint&,int  ) ),this,
             SLOT( mailHold( int, QListViewItem *,const QPoint&,int  ) ) );
    connect(folderView, SIGNAL(refreshMailview(QList<RecMail>*)),this,SLOT(refreshMailView(QList<RecMail>*)));
    connect( composeMail, SIGNAL( activated() ), SLOT( slotComposeMail() ) );
    connect( sendQueued, SIGNAL( activated() ), SLOT( slotSendQueued() ) );
//    connect( searchMails, SIGNAL( activated() ), SLOT( slotSearchMails() ) );
    connect( editAccounts, SIGNAL( activated() ), SLOT( slotEditAccounts() ) );
    // Added by Stefan Eilers to allow starting by addressbook..
    // copied from old mail2
#if !defined(QT_NO_COP)
    connect( qApp, SIGNAL( appMessage( const QCString&, const QByteArray& ) ),
             this, SLOT( appMessage( const QCString&, const QByteArray& ) ) );
#endif

    QTimer::singleShot( 1000, this, SLOT( slotAdjustColumns() ) );
}

MainWindow::~MainWindow()
{
}

void MainWindow::appMessage(const QCString &, const QByteArray &)
{
    qDebug("appMessage not reached");
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
   qDebug( "slotShowFolders not reached" );
}

void MainWindow::refreshMailView(QList<RecMail>*)
{
   qDebug( "refreshMailView not reached" );
}

void MainWindow::mailLeftClicked(int, QListViewItem *,const QPoint&,int )
{
    qDebug( "mailLeftClicked not reached" );
}

void MainWindow::displayMail()
{
    qDebug("displayMail not reached");
}

void MainWindow::slotDeleteMail()
{
    qDebug("deleteMail not reached");
}

void MainWindow::mailHold(int, QListViewItem *,const QPoint&,int  )
{
    qDebug("mailHold not reached");
}
