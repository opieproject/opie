#include <qmessagebox.h>
#include "settingsdialog.h"
#include "opiemail.h"
#include "editaccounts.h"
#include "composemail.h"
#include "smtpwrapper.h"
#include <qpe/qcopenvelope_qws.h>
#include <qaction.h>
#include <qapplication.h>

OpieMail::OpieMail( QWidget *parent, const char *name, WFlags flags )
    : MainWindow( parent, name, flags )
{
    settings = new Settings();

    folderView->populate( settings->getAccounts() );

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
}

void OpieMail::appMessage(const QCString &msg, const QByteArray &data)
{
    // copied from old mail2
    if (msg == "writeMail(QString,QString)") {
        QDataStream stream(data,IO_ReadOnly);
        QString name, email;
        stream >> name >> email;
        // removing the whitespaces at beginning and end is needed!
        slotwriteMail(name.stripWhiteSpace(),email.stripWhiteSpace());
    } else if (msg == "newMail()") {
        slotComposeMail();
    }
}

void OpieMail::slotwriteMail(const QString&name,const QString&email)
{
    ComposeMail compose( settings, this, 0 , true );
    if (!email.isEmpty()) {
        if (!name.isEmpty()) {
            compose.setTo("\"" + name + "\"" + " " + "<"+ email + ">");
        } else {
            compose.setTo(email);
        }
    }
    compose.showMaximized();
    compose.slotAdjustColumns();
    compose.exec();
}

void OpieMail::slotComposeMail()
{
    qDebug( "Compose Mail" );
    slotwriteMail(0l,0l);
}

void OpieMail::slotSendQueued()
{
    qDebug( "Send Queued" );
    SMTPaccount *smtp = 0;

    QList<Account> list = settings->getAccounts();
    Account *it;
//    if (list.count()==1) {
        for ( it = list.first(); it; it = list.next() ) {
            if ( it->getType().compare( "SMTP" ) == 0 ) {
                smtp = static_cast<SMTPaccount *>(it);
                break;
            }
        }
//    }
    if (smtp) {
        SMTPwrapper * wrap = new SMTPwrapper(settings);
        if ( wrap->flushOutbox(smtp) ) {
            QMessageBox::information(0,tr("Info"),tr("Mail queue flushed"));
        }
    }
}

void OpieMail::slotSearchMails()
{
    qDebug( "Search Mails" );
}

void OpieMail::slotEditSettings()
{
    SettingsDialog settingsDialog( this,  0, true );
    settingsDialog.showMaximized();
    settingsDialog.exec();
}

void OpieMail::slotEditAccounts()
{
    qDebug( "Edit Accounts" );
    EditAccounts eaDialog( settings, this, 0, true );
    eaDialog.showMaximized();
    eaDialog.slotAdjustColumns();
    eaDialog.exec();
    if ( settings ) delete settings;
    settings = new Settings();

    folderView->populate( settings->getAccounts() );
}

