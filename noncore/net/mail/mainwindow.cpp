#include <qlabel.h>
#include <qvbox.h>
#include <qheader.h>
#include <qtimer.h>


#include "defines.h"
#include "mainwindow.h"

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags flags )
    : QMainWindow( parent, name, flags )
{
    setCaption( tr( "Opie-Mail" ) );
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

    syncFolders = new QAction( tr( "Sync mailfolders" ), ICON_SYNC,
                               0, 0, this );
    syncFolders->addTo( toolBar );
    syncFolders->addTo( mailMenu );

    showFolders = new QAction( tr( "Show/Hide folders" ), ICON_SHOWFOLDERS,
                               0, 0, this, 0, true );
    showFolders->addTo( toolBar );
    showFolders->addTo( mailMenu );
    connect(showFolders, SIGNAL( toggled( bool ) ),
            SLOT( slotShowFolders( bool ) ) );

    searchMails = new QAction( tr( "Search mails" ), ICON_SEARCHMAILS,
                               0, 0, this );
    searchMails->addTo( toolBar );
    searchMails->addTo( mailMenu );


    editSettings = new QAction( tr( "Edit settings" ), ICON_EDITSETTINGS,
                                0, 0, this );
    editSettings->addTo( settingsMenu );

    editAccounts = new QAction( tr( "Configure accounts" ), ICON_EDITACCOUNTS,
                                0, 0, this );
    editAccounts->addTo( settingsMenu );

    QVBox *view = new QVBox( this );
    setCentralWidget( view );

    folderView = new AccountView( view );
    folderView->header()->hide();
    folderView->setMinimumHeight( 90 );
    folderView->setMaximumHeight( 90 );
    folderView->addColumn( tr( "Mailbox" ) );
    folderView->hide();

    mailView = new QListView( view );
    mailView->setMinimumHeight( 50 );
    mailView->addColumn( tr( "Subject" ),QListView::Manual );
    mailView->addColumn( tr( "Sender" ),QListView::Manual );
    mailView->addColumn( tr( "Date" ));
    mailView->setAllColumnsShowFocus(true);
    mailView->setSorting(-1);
    connect( mailView, SIGNAL( clicked( QListViewItem * ) ),this,
             SLOT( displayMail( QListViewItem * ) ) );

    connect(folderView,SIGNAL(refreshMailview(Maillist*)),this,SLOT(refreshMailView(Maillist*)));

   QTimer::singleShot( 1000, this, SLOT(  slotAdjustColumns() ) );
}

void MainWindow::slotAdjustColumns()
{
    bool hidden = folderView->isHidden();
    if ( hidden ) folderView->show();
    folderView->setColumnWidth( 0, folderView->visibleWidth() );
    if ( hidden ) folderView->hide();

    mailView->setColumnWidth( 0, mailView->visibleWidth() - 130 );
    mailView->setColumnWidth( 1, 80 );
    mailView->setColumnWidth( 2, 50 );
}

void MainWindow::slotShowFolders( bool show )
{
    qDebug( "Show Folders" );
    if ( show && folderView->isHidden() ) {
        qDebug( "-> showing" );
        folderView->show();
    } else if ( !show && !folderView->isHidden() ) {
        qDebug( "-> hiding" );
        folderView->hide();
    }
}

void MainWindow::refreshMailView(Maillist*list)
{
    MailListViewItem*item = 0;
    mailView->clear();
#if 0
    QFont f = mailView->getFont();
    QFont bf = f;
#endif
    for (unsigned int i = 0; i < list->count();++i) {
        item = new MailListViewItem(mailView,item);
        item->storeData(*(list->at(i)));
        item->showEntry();
#if 0
        if (!list->at(i)->getFlags().testBit(FLAG_SEEN)) {
            item->setFont(bf);
        }
#endif
    }
}
void MainWindow::displayMail(QListViewItem*item)
{
    if (!item) return;
    qDebug("View mail");
    RecMail mail = ((MailListViewItem*)item)->data();
    QString body = folderView->fetchBody(mail);

    qDebug(body);
}

void MailListViewItem::showEntry()
{
    setText(0,mail_data.getSubject());
    setText(1,mail_data.getFrom());
    setText(2,mail_data.getDate());
}
