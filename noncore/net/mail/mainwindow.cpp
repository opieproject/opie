#include <qlabel.h>
#include <qvbox.h>
#include <qheader.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtextstream.h>

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>


#include "defines.h"
#include "mainwindow.h"
#include "viewmail.h"
#include <libmailwrapper/mailtypes.h>
#include "mailistviewitem.h"


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
      searchMails = new QAction( tr( "Search mails" ), ICON_SEARCHMAILS,
                               0, 0, this );
    searchMails->addTo( toolBar );
    searchMails->addTo( mailMenu );
    */

    deleteMails = new QAction(tr("Delete Mail"), QIconSet(Resource::loadPixmap("mail/delete")), 0, 0, this);
    deleteMails->addTo( toolBar );
    deleteMails->addTo( mailMenu );
    connect( deleteMails, SIGNAL( activated() ),
             SLOT( slotDeleteMail() ) );

    editSettings = new QAction( tr( "Edit settings" ), ICON_EDITSETTINGS,
                                0, 0, this );
    editSettings->addTo( settingsMenu );
    connect( editSettings, SIGNAL( activated() ),
             SLOT( slotEditSettings() ) );
    editAccounts = new QAction( tr( "Configure accounts" ), ICON_EDITACCOUNTS,
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

    QTimer::singleShot( 1000, this, SLOT( slotAdjustColumns() ) );
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

void MainWindow::refreshMailView(QList<RecMail>*list)
{
    MailListViewItem*item = 0;
    mailView->clear();
    for (unsigned int i = 0; i < list->count();++i) {
        item = new MailListViewItem(mailView,item);
        item->storeData(*(list->at(i)));
        item->showEntry();
    }
}
void MainWindow::mailLeftClicked(int button, QListViewItem *item,const QPoint&,int )
{
    /* just LEFT button - or tap with stylus on pda */
    if (button!=1) return;
    if (!item) return;
    displayMail();
}

void MainWindow::displayMail()
{
    QListViewItem*item = mailView->currentItem();
    if (!item) return;
    RecMail mail = ((MailListViewItem*)item)->data();
    RecBody body = folderView->fetchBody(mail);
    ViewMail readMail( this );
    readMail.setBody( body );
    readMail.setMail( mail );
    readMail.showMaximized();
    readMail.exec();

    if (  readMail.deleted ) {
         folderView->refreshCurrent();
    } else {
        ( (MailListViewItem*)item )->setPixmap( 0, Resource::loadPixmap( "") );
    }
}

void MainWindow::slotDeleteMail()
{
    if (!mailView->currentItem()) return;
    RecMail mail = ((MailListViewItem*)mailView->currentItem() )->data();
    if ( QMessageBox::warning(this, tr("Delete Mail"), QString( tr("<p>Do you really want to delete this mail? <br><br>" ) + mail.getFrom() + " - " + mail.getSubject() ) , QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes ) {
       mail.Wrapper()->deleteMail( mail );
       folderView->refreshCurrent();
    }
}

void MainWindow::mailHold(int button, QListViewItem *item,const QPoint&,int  )
{
    /* just the RIGHT button - or hold on pda */
    if (button!=2) {return;}
    qDebug("Event right/hold");
    if (!item) return;
    QPopupMenu *m = new QPopupMenu(0);
    if (m) {
        m->insertItem(tr("Read this mail"),this,SLOT(displayMail()));
        m->insertItem(tr("Delete this mail"),this,SLOT(slotDeleteMail()));
        m->setFocus();
        m->exec( QPoint( QCursor::pos().x(), QCursor::pos().y()) );
        delete m;
    }
}

MailListViewItem::MailListViewItem(QListView * parent, MailListViewItem * item )
        :QListViewItem(parent,item),mail_data()
{
}

void MailListViewItem::showEntry()
{
    if ( mail_data.getFlags().testBit( FLAG_ANSWERED ) == true) {
        setPixmap( 0, Resource::loadPixmap( "mail/kmmsgreplied") );
    } else if ( mail_data.getFlags().testBit( FLAG_SEEN ) == true )  {
        /* I think it looks nicer if there are not such a log of icons but only on mails
           replied or new - Alwin*/
        //setPixmap( 0, Resource::loadPixmap( "mail/kmmsgunseen") );
    } else  {
        setPixmap( 0, Resource::loadPixmap( "mail/kmmsgnew") );
    }
    double s = mail_data.Msgsize();
    int w;
    w=0;

    while (s>1024) {
        s/=1024;
        ++w;
        if (w>=2) break;
    }

    QString q="";
    QString fsize="";
    switch(w) {
    case 1:
        q="k";
        break;
    case 2:
        q="M";
        break;
    default:
        break;
    }

    {
        QTextOStream o(&fsize);
        if (w>0) o.precision(2); else o.precision(0);
        o.setf(QTextStream::fixed);
        o << s << " " << q << "Byte";
    }

    setText(1,mail_data.getSubject());
    setText(2,mail_data.getFrom());
    setText(3,fsize);
    setText(4,mail_data.getDate());
}

void MailListViewItem::storeData(const RecMail&data)
{
    mail_data = data;
}

const RecMail& MailListViewItem::data()const
{
    return mail_data;
}


