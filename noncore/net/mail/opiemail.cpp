
#include "settingsdialog.h"
#include "opiemail.h"
#include "editaccounts.h"
#include "composemail.h"
#include "mailistviewitem.h"
#include "viewmail.h"
#include "selectstore.h"
#include "selectsmtp.h"

#include <libmailwrapper/smtpwrapper.h>
#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/abstractmail.h>
/* OPIE */
#include <opie2/odebug.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

/* QT */

using namespace Opie::Core;

OpieMail::OpieMail( QWidget *parent, const char *name, WFlags flags )
        : MainWindow( parent, name, WStyle_ContextHelp )
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
    if (msg == "writeMail(QString,QString)")
    {
        QDataStream stream(data,IO_ReadOnly);
        QString name, email;
        stream >> name >> email;
        // removing the whitespaces at beginning and end is needed!
        slotwriteMail(name.stripWhiteSpace(),email.stripWhiteSpace());
    }
    else if (msg == "newMail()")
    {
        slotComposeMail();
    }
}

void OpieMail::slotwriteMail(const QString&name,const QString&email)
{
    ComposeMail compose( settings, this, 0, true , WStyle_ContextHelp );
    if (!email.isEmpty())
    {
        if (!name.isEmpty())
        {
            compose.setTo("\"" + name + "\"" + " " + "<"+ email + ">");
        }
        else
        {
            compose.setTo(email);
        }
    }
    compose.slotAdjustColumns();
    QPEApplication::execDialog( &compose );
}

void OpieMail::slotComposeMail()
{
    odebug << "Compose Mail" << oendl; 
    slotwriteMail(0l,0l);
}

void OpieMail::slotSendQueued()
{
    odebug << "Send Queued" << oendl; 
    SMTPaccount *smtp = 0;

    QList<Account> list = settings->getAccounts();
    QList<SMTPaccount> smtpList;
    smtpList.setAutoDelete(false);
    Account *it;
    for ( it = list.first(); it; it = list.next() )
    {
        if ( it->getType() == MAILLIB::A_SMTP )
        {
            smtp = static_cast<SMTPaccount *>(it);
            smtpList.append(smtp);
        }
    }
    if (smtpList.count()==0)
    {
        QMessageBox::information(0,tr("Info"),tr("Define a smtp account first"));
        return;
    }
    if (smtpList.count()==1)
    {
        smtp = smtpList.at(0);
    }
    else
    {
        smtp = 0;
        selectsmtp selsmtp;
        selsmtp.setSelectionlist(&smtpList);
        if ( QPEApplication::execDialog( &selsmtp ) == QDialog::Accepted )
        {
            smtp = selsmtp.selected_smtp();
        }
    }
    if (smtp)
    {
        SMTPwrapper * wrap = new SMTPwrapper(smtp);
        if ( wrap->flushOutbox() )
        {
            QMessageBox::information(0,tr("Info"),tr("Mail queue flushed"));
        }
        delete wrap;
    }
}

void OpieMail::slotSearchMails()
{
    odebug << "Search Mails" << oendl; 
}

void OpieMail::slotEditSettings()
{
    SettingsDialog settingsDialog( this,  0, true,  WStyle_ContextHelp );
    QPEApplication::execDialog( &settingsDialog );
}

void OpieMail::slotEditAccounts()
{
    odebug << "Edit Accounts" << oendl; 
    EditAccounts eaDialog( settings, this, 0, true,  WStyle_ContextHelp );
    eaDialog.slotAdjustColumns();
    QPEApplication::execDialog( &eaDialog );
    if ( settings ) delete settings;
    settings = new Settings();

    folderView->populate( settings->getAccounts() );
}

void OpieMail::displayMail()
{
    QListViewItem*item = mailView->currentItem();
    if (!item) return;
    RecMailP mail = ((MailListViewItem*)item)->data();
    RecBodyP body = folderView->fetchBody(mail);
    ViewMail readMail( this,"", Qt::WType_Modal | WStyle_ContextHelp  );
    readMail.setBody( body );
    readMail.setMail( mail );
    readMail.showMaximized();
    readMail.exec();

    if (  readMail.deleted )
    {
        folderView->refreshCurrent();
    }
    else
    {
        ( (MailListViewItem*)item )->setPixmap( 0, Resource::loadPixmap( "" ) );
    }
}

void OpieMail::slotDeleteMail()
{
    if (!mailView->currentItem()) return;
    RecMailP mail = ((MailListViewItem*)mailView->currentItem() )->data();
    if ( QMessageBox::warning(this, tr("Delete Mail"), QString( tr("<p>Do you really want to delete this mail? <br><br>" ) + mail->getFrom() + " - " + mail->getSubject() ) , QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
    {
        mail->Wrapper()->deleteMail( mail );
        folderView->refreshCurrent();
    }
}

void OpieMail::mailHold(int button, QListViewItem *item,const QPoint&,int  )
{
    if (!mailView->currentItem()) return;
    MAILLIB::ATYPE mailtype = ((MailListViewItem*)mailView->currentItem() )->wrapperType();
    /* just the RIGHT button - or hold on pda */
    if (button!=2) {return;}
    odebug << "Event right/hold" << oendl; 
    if (!item) return;
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (mailtype==MAILLIB::A_NNTP) {
            m->insertItem(tr("Read this posting"),this,SLOT(displayMail()));
//            m->insertItem(tr("Copy this posting"),this,SLOT(slotMoveCopyMail()));
        } else {
            if (folderView->currentisDraft()) {
                m->insertItem(tr("Edit this mail"),this,SLOT(reEditMail()));
            }
            m->insertItem(tr("Read this mail"),this,SLOT(displayMail()));
            m->insertItem(tr("Delete this mail"),this,SLOT(slotDeleteMail()));
            m->insertItem(tr("Copy/Move this mail"),this,SLOT(slotMoveCopyMail()));
        }
        m->setFocus();
        m->exec( QPoint( QCursor::pos().x(), QCursor::pos().y()) );
        delete m;
    }
}

void OpieMail::slotShowFolders( bool show )
{
    odebug << "Show Folders" << oendl; 
    if ( show && folderView->isHidden() )
    {
        odebug << "-> showing" << oendl; 
        folderView->show();
    }
    else if ( !show && !folderView->isHidden() )
    {
        odebug << "-> hiding" << oendl; 
        folderView->hide();
    }
}

void OpieMail::refreshMailView(const QValueList<RecMailP>&list)
{
    MailListViewItem*item = 0;
    mailView->clear();

    QValueList<RecMailP>::ConstIterator it;
    for (it = list.begin(); it != list.end();++it)
    {
        item = new MailListViewItem(mailView,item);
        item->storeData((*it));
        item->showEntry();
    }
}

void OpieMail::mailLeftClicked(int button, QListViewItem *item,const QPoint&,int )
{
    /* just LEFT button - or tap with stylus on pda */
    if (button!=1) return;
    if (!item) return;
    if (folderView->currentisDraft()) {
        reEditMail();
    } else {
        displayMail();
    }
}

void OpieMail::slotMoveCopyMail()
{
    if (!mailView->currentItem()) return;
    RecMailP mail = ((MailListViewItem*)mailView->currentItem() )->data();
    AbstractMail*targetMail = 0;
    QString targetFolder = "";
    Selectstore sels;
    folderView->setupFolderselect(&sels);
    if (!sels.exec()) return;
    targetMail = sels.currentMail();
    targetFolder = sels.currentFolder();
    if ( (mail->Wrapper()==targetMail && mail->getMbox()==targetFolder) ||
            targetFolder.isEmpty())
    {
        return;
    }
    if (sels.newFolder() && !targetMail->createMbox(targetFolder))
    {
        QMessageBox::critical(0,tr("Error creating new Folder"),
                              tr("<center>Error while creating<br>new folder - breaking.</center>"));
        return;
    }
    mail->Wrapper()->mvcpMail(mail,targetFolder,targetMail,sels.moveMails());
    folderView->refreshCurrent();
}

void OpieMail::reEditMail()
{
    if (!mailView->currentItem()) return;

    ComposeMail compose( settings, this, 0, true , WStyle_ContextHelp );
    compose.reEditMail(((MailListViewItem*)mailView->currentItem() )->data());
    compose.slotAdjustColumns();
    QPEApplication::execDialog( &compose );
}
