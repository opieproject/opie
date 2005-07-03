
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
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
using namespace Opie::Core;

/* QT */
#include <qmap.h>
#include <qvaluelist.h>

/* UNIX */
#include <signal.h>

typedef QMapNode<QString,QString> tkeyvalues;
typedef QValueList<tkeyvalues> tvaluelist;

class ValueExplode
{
protected:
    //! what was parsed last
    tvaluelist m_LastParsed;
    //! the delemiter to use
    QString mDelemiter;
    //! the inner delemiter
    QString m2Delemiter;
    //! the real split routine
    void splitit();
    //! the content
    QString m_Command;
    //! constructor
    ValueExplode(){}
public:
    //! constructor
    /*!
     * \param aCommand the string to be splitted
     * \param aDelemiter which sign will be the delemiter character
     * \param a2Delemiter which sign will delemiter the key-value-pairs between other delemiters
     */
    ValueExplode(const QString&aCommand,const char aDelemiter = '&',const char a2Delemiter='=');
    //! destructor
    virtual ~ValueExplode();
    //! assigen operator
    /*!
     * \return a list of substrings
     */
    operator const tvaluelist& (){return m_LastParsed;}
};

ValueExplode::~ValueExplode()
{
}

ValueExplode::ValueExplode(const QString&aCommand,const char aDelemiter,const char a2Delemiter)
    :m_LastParsed(),m_Command(aCommand)
{
    mDelemiter = aDelemiter;
    m2Delemiter = a2Delemiter;
    splitit();
}

void ValueExplode::splitit()
{
    QString iLine;
    m_LastParsed.clear();
    if (mDelemiter.isEmpty()||m2Delemiter.isEmpty()) {
        m_LastParsed.append(tkeyvalues(m_Command,""));
        return;
    }
    int pos,pos2;
    unsigned startpos = 0;
    iLine = m_Command;
    while ( (pos = iLine.find(mDelemiter,startpos))!=-1) {
        pos2 = iLine.find(m2Delemiter,startpos);
        if (pos2==-1||pos2>pos) {
            m_LastParsed.append(tkeyvalues(iLine.mid(startpos,pos-startpos),""));
        } else {
            m_LastParsed.append(tkeyvalues(iLine.mid(startpos,pos2-startpos),iLine.mid(pos2+1,pos-pos2-1)));
        }
        startpos = pos+1;
    }
    if (startpos<iLine.length()) {
        pos2 = iLine.find(m2Delemiter,startpos);
        if (pos2==-1) {
            m_LastParsed.append(tkeyvalues(iLine.mid(startpos),""));
        } else {
            m_LastParsed.append(tkeyvalues(iLine.mid(startpos,pos2-startpos),iLine.mid(pos2+1)));
        }
    }
}

OpieMail::OpieMail( QWidget *parent, const char *name, WFlags  )
        : MainWindow( parent, name, WStyle_ContextHelp )
{
    setup_signalblocking();
    Config cfg("mail");
    cfg.setGroup( "Settings" );
    m_clickopens = cfg.readBoolEntry("clickOpensMail",true);

    settings = new Settings();
    folderView->populate( settings->getAccounts() );
    connect(folderView,SIGNAL(refreshMenues(int)),this,SLOT(refreshMenu(int)));
}

OpieMail::~OpieMail()
{
    if (settings) delete settings;
}

void OpieMail::setup_signalblocking()
{
    /* for networking we must block SIGPIPE and Co. */
    struct sigaction blocking_action,temp_action;
    blocking_action.sa_handler = SIG_IGN;
    sigemptyset(&(blocking_action.sa_mask));
    blocking_action.sa_flags = 0;
    sigaction(SIGPIPE,&blocking_action,&temp_action);
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

/**
 * Konqueror calls us with the mailto:name@address
 */
void OpieMail::setDocument(const QString& mail)
{
    /*
     * It looks like a mailto address, lets try it
     */
    if( mail.startsWith(QString::fromLatin1("mailto:")) )
        slotwriteMail(QString::null, mail.mid(7));
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
    if (QPEApplication::execDialog( &settingsDialog )) {
        Config cfg("mail");
        cfg.setGroup( "Settings" );
        m_clickopens = cfg.readBoolEntry("clickOpensMail",true);
        emit settingsChanged();
    }
}

void OpieMail::slotEditAccounts()
{
    odebug << "Edit Accounts" << oendl;
    EditAccounts eaDialog( settings, this, 0, true,  WStyle_ContextHelp );
    eaDialog.slotAdjustColumns();
    if (QPEApplication::execDialog( &eaDialog )==QDialog::Rejected);// return;

    if ( settings ) delete settings;
    settings = new Settings();
    mailView->clear();
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
        ( (MailListViewItem*)item )->setPixmap( 0, QPixmap() );
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
    if (!m_clickopens) return;
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

void OpieMail::refreshMenu(int m_isFolder)
{
    if (QApplication::desktop()->width()<330) {
        mailMenu->setItemEnabled(m_ServerMenuId,m_isFolder&1);
        mailMenu->setItemEnabled(m_FolderMenuId,m_isFolder&2);
    } else {
        menuBar->setItemEnabled(m_ServerMenuId,m_isFolder&1);
        menuBar->setItemEnabled(m_FolderMenuId,m_isFolder&2);
    }

    QMap<int,QString>::ConstIterator it;
    QMap<int,QString> server_entries=folderView->currentServerMenu();
    QMap<int,QString> folder_entries=folderView->currentFolderMenu();

    int id;
    unsigned int i;
    for (i=0; i<folderMenu->count();++i) {
        id = folderMenu->idAt(i);
        folderMenu->setItemEnabled(id,false);
    }
    for (it=folder_entries.begin();it!=folder_entries.end();++it) {
        folderMenu->changeItem(it.key(),it.data());
        folderMenu->setItemEnabled(it.key(),true);
    }
    for (i=0; i<serverMenu->count();++i) {
        id = serverMenu->idAt(i);
        serverMenu->setItemEnabled(id,false);
    }
    for (it=server_entries.begin();it!=server_entries.end();++it) {
        serverMenu->changeItem(it.key(),it.data());
        serverMenu->setItemEnabled(it.key(),true);
    }
}

void OpieMail::serverSelected(int m_isFolder)
{
    mailView->clear();
    refreshMenu(m_isFolder);
}
