#include "opiemail.h"
#include "editaccounts.h"
#include "composemail.h"

OpieMail::OpieMail( QWidget *parent, const char *name, WFlags flags )
    : MainWindow( parent, name, flags )
{
    settings = new Settings();
 
    folderView->populate( settings->getAccounts() );
    
    connect( composeMail, SIGNAL( activated() ), SLOT( slotComposeMail() ) );
    connect( sendQueued, SIGNAL( activated() ), SLOT( slotSendQueued() ) );
    connect( searchMails, SIGNAL( activated() ), SLOT( slotSearchMails() ) );
    connect( editSettings, SIGNAL( activated() ), SLOT( slotEditSettings() ) );
    connect( editAccounts, SIGNAL( activated() ), SLOT( slotEditAccounts() ) );
}

void OpieMail::slotComposeMail()
{
    qDebug( "Compose Mail" );
    ComposeMail compose( settings, this, 0 , true );
    compose.showMaximized();
    compose.slotAdjustColumns();
    compose.exec();
}

void OpieMail::slotSendQueued()
{
    qDebug( "Send Queued" );
}

void OpieMail::slotSearchMails()
{
    qDebug( "Search Mails" );
}

void OpieMail::slotEditSettings()
{
    qDebug( "Edit Settings" );
}

void OpieMail::slotEditAccounts()
{
    qDebug( "Edit Accounts" );
    EditAccounts eaDialog( settings, this, 0, true );
    eaDialog.showMaximized();
    eaDialog.slotAdjustColumns();
    eaDialog.exec();
}
