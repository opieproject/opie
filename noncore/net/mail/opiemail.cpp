#include <qmessagebox.h>
#include "settingsdialog.h"
#include "opiemail.h"
#include "editaccounts.h"
#include "composemail.h"
#include <libmailwrapper/smtpwrapper.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qaction.h>
#include <qapplication.h>
#include <libmailwrapper/mailtypes.h>
#include "mailistviewitem.h"
#include "viewmail.h"
#include "selectstore.h"

OpieMail::OpieMail( QWidget *parent, const char *name, WFlags flags )
    : MainWindow( parent, name, flags )
{
    settings = new Settings();

    folderView->populate( settings->getAccounts() );
}

OpieMail::~OpieMail()
{
    if (settings) delete settings;
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

void OpieMail::displayMail()
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

void OpieMail::slotDeleteMail()
{
    if (!mailView->currentItem()) return;
    RecMail mail = ((MailListViewItem*)mailView->currentItem() )->data();
    if ( QMessageBox::warning(this, tr("Delete Mail"), QString( tr("<p>Do you really want to delete this mail? <br><br>" ) + mail.getFrom() + " - " + mail.getSubject() ) , QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes ) {
       mail.Wrapper()->deleteMail( mail );
       folderView->refreshCurrent();
    }
}

void OpieMail::mailHold(int button, QListViewItem *item,const QPoint&,int  )
{
    /* just the RIGHT button - or hold on pda */
    if (button!=2) {return;}
    qDebug("Event right/hold");
    if (!item) return;
    QPopupMenu *m = new QPopupMenu(0);
    if (m) {
        m->insertItem(tr("Read this mail"),this,SLOT(displayMail()));
        m->insertItem(tr("Delete this mail"),this,SLOT(slotDeleteMail()));
        m->insertItem(tr("Copy/Move this mail"),this,SLOT(slotMoveCopyMail()));
        m->setFocus();
        m->exec( QPoint( QCursor::pos().x(), QCursor::pos().y()) );
        delete m;
    }
}

void OpieMail::slotShowFolders( bool show )
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

void OpieMail::refreshMailView(QList<RecMail>*list)
{
    MailListViewItem*item = 0;
    mailView->clear();
    for (unsigned int i = 0; i < list->count();++i) {
        item = new MailListViewItem(mailView,item);
        item->storeData(*(list->at(i)));
        item->showEntry();
    }
}

void OpieMail::mailLeftClicked(int button, QListViewItem *item,const QPoint&,int )
{
    /* just LEFT button - or tap with stylus on pda */
    if (button!=1) return;
    if (!item) return;
    displayMail();
}

void OpieMail::slotMoveCopyMail()
{
    if (!mailView->currentItem()) return;
    RecMail mail = ((MailListViewItem*)mailView->currentItem() )->data();
    AbstractMail*targetMail = 0;
    QString targetFolder = "";
    Selectstore sels;
    folderView->setupFolderselect(&sels);
    if (!sels.exec()) return;
    targetMail = sels.currentMail();
    targetFolder = sels.currentFolder();
    if ( (mail.Wrapper()==targetMail && mail.getMbox()==targetFolder) ||
        targetFolder.isEmpty()) {
        return;
    }
    if (sels.newFolder() && !targetMail->createMbox(targetFolder)) {
        QMessageBox::critical(0,tr("Error creating new Folder"),
            tr("<center>Error while creating<br>new folder - breaking.</center>"));
        return;
    }
    mail.Wrapper()->mvcpMail(mail,targetFolder,targetMail,sels.moveMails());
    folderView->refreshCurrent();
}
