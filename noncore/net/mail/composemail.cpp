#include <qt.h>

#include <opie2/ofiledialog.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/contact.h>

#include "composemail.h"

#include <libmailwrapper/smtpwrapper.h>
#include <libmailwrapper/storemail.h>
#include <libmailwrapper/abstractmail.h>
#include <libmailwrapper/mailtypes.h>

using namespace Opie::Core;
using namespace Opie::Ui;
ComposeMail::ComposeMail( Settings *s, QWidget *parent, const char *name, bool modal, WFlags flags )
    : ComposeMailUI( parent, name, modal, flags )
{
    settings = s;
    m_replyid = "";

    QString vfilename = Global::applicationFileName("addressbook",
                "businesscard.vcf");
    Contact c;
    if (QFile::exists(vfilename)) {
        c = Contact::readVCard( vfilename )[0];
    }

    QStringList mails = c.emailList();
    QString defmail = c.defaultEmail();

    if (defmail.length()!=0) {
        fromBox->insertItem(defmail);
    }
    QStringList::ConstIterator sit = mails.begin();
    for (;sit!=mails.end();++sit) {
        if ( (*sit)==defmail)
            continue;
        fromBox->insertItem((*sit));
    }
    senderNameEdit->setText(c.firstName()+" "+c.lastName());
    Config cfg( "mail" );
    cfg.setGroup( "Compose" );
    checkBoxLater->setChecked( cfg.readBoolEntry( "sendLater", false ) );

    attList->addColumn( tr( "Name" ) );
    attList->addColumn( tr( "Size" ) );

    QList<Account> accounts = settings->getAccounts();

    Account *it;
    for ( it = accounts.first(); it; it = accounts.next() ) {
        if ( it->getType()==MAILLIB::A_SMTP ) {
           SMTPaccount *smtp = static_cast<SMTPaccount *>(it);
           smtpAccountBox->insertItem( smtp->getAccountName() );
           smtpAccounts.append( smtp );
        }
    }

    if ( smtpAccounts.count() > 0 ) {
        fillValues( smtpAccountBox->currentItem() );
    } else {
        QMessageBox::information( this, tr( "Problem" ),
                                  tr( "<p>Please create an SMTP account first.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    connect( smtpAccountBox, SIGNAL( activated(int) ), SLOT( fillValues(int) ) );
    connect( toButton, SIGNAL( clicked() ), SLOT( pickAddressTo() ) );
    connect( ccButton, SIGNAL( clicked() ), SLOT( pickAddressCC() ) );
    connect( bccButton, SIGNAL( clicked() ), SLOT( pickAddressBCC() ) );
    connect( replyButton, SIGNAL( clicked() ), SLOT( pickAddressReply() ) );
    connect( addButton, SIGNAL( clicked() ), SLOT( addAttachment() ) );
    connect( deleteButton, SIGNAL( clicked() ), SLOT( removeAttachment() ) );
}

void ComposeMail::pickAddress( QLineEdit *line )
{
    QString names = AddressPicker::getNames();
    if ( line->text().isEmpty() ) {
        line->setText( names );
    } else if ( !names.isEmpty() ) {
        line->setText( line->text() + ", " + names );
    }
}


void ComposeMail::setTo( const QString & to )
{
    toLine->setText( to );
}

void ComposeMail::setSubject( const QString & subject )
{
 subjectLine->setText( subject );
}

void ComposeMail::setInReplyTo( const QString & messageId )
{
    m_replyid = messageId;
}

void ComposeMail::setMessage( const QString & text )
{
  message->setText( text );
}


void ComposeMail::pickAddressTo()
{
    pickAddress( toLine );
}

void ComposeMail::pickAddressCC()
{
    pickAddress( ccLine );
}

void ComposeMail::pickAddressBCC()
{
    pickAddress( bccLine );
}

void ComposeMail::pickAddressReply()
{
    pickAddress( replyLine );
}

void ComposeMail::fillValues( int )
{
#if 0
    SMTPaccount *smtp = smtpAccounts.at( current );
    ccLine->clear();
    if ( smtp->getUseCC() ) {
        ccLine->setText( smtp->getCC() );
    }
    bccLine->clear();
    if ( smtp->getUseBCC() ) {
        bccLine->setText( smtp->getBCC() );
    }
    replyLine->clear();
    if ( smtp->getUseReply() ) {
        replyLine->setText( smtp->getReply() );
    }
    sigMultiLine->setText( smtp->getSignature() );
#endif
}

void ComposeMail::slotAdjustColumns()
{
    int currPage = tabWidget->currentPageIndex();

    tabWidget->showPage( attachTab );
    attList->setColumnWidth( 0, attList->visibleWidth() - 80 );
    attList->setColumnWidth( 1, 80 );

    tabWidget->setCurrentPage( currPage );
}

void ComposeMail::addAttachment()
{
    DocLnk lnk = OFileDialog::getOpenFileName( 1, "/" );
    if ( !lnk.name().isEmpty() ) {
        Attachment *att = new Attachment( lnk );
        (void) new AttachViewItem( attList, att );
    }
}

void ComposeMail::removeAttachment()
{
    if ( !attList->currentItem() ) {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select a File.</p>" ),
                                  tr( "Ok" ) );
    } else {
        attList->takeItem( attList->currentItem() );
    }
}

void ComposeMail::accept()
{
    if ( checkBoxLater->isChecked() ) {
        qDebug(  "Send later" );
    }

#if 0
    qDebug( "Sending Mail with " +
            smtpAccounts.at( smtpAccountBox->currentItem() )->getAccountName() );
#endif
    Opie::Core::OSmartPointer<Mail> mail=new Mail;

    SMTPaccount *smtp = smtpAccounts.at( smtpAccountBox->currentItem() );
    mail->setMail(fromBox->currentText());

    if ( !toLine->text().isEmpty() ) {
        mail->setTo( toLine->text() );
    } else {
        QMessageBox::warning(0,tr("Sending mail"),
            tr("No Receiver spezified" ) );
        return;
    }
    mail->setName(senderNameEdit->text());
    mail->setCC( ccLine->text() );
    mail->setBCC( bccLine->text() );
    mail->setReply( replyLine->text() );
    mail->setSubject( subjectLine->text() );
    if (!m_replyid.isEmpty()) {
        QStringList ids;
        ids.append(m_replyid);
        mail->setInreply(ids);
    }
    QString txt = message->text();
    if ( !sigMultiLine->text().isEmpty() ) {
        txt.append( "\n--\n" );
        txt.append( sigMultiLine->text() );
    }
    mail->setMessage( txt );
    AttachViewItem *it = (AttachViewItem *) attList->firstChild();
    while ( it != NULL ) {
        mail->addAttachment( it->getAttachment() );
        it = (AttachViewItem *) it->nextSibling();
    }

    SMTPwrapper wrapper( smtp );
    wrapper.sendMail( mail,checkBoxLater->isChecked() );

    QDialog::accept();
}

void ComposeMail::reject()
{
    int yesno = QMessageBox::warning(0,tr("Store message"),
                                     tr("Store message into drafts?"),
                                     tr("Yes"),
                                     tr("No"),QString::null,0,1);

    if (yesno == 0) {
        Opie::Core::OSmartPointer<Mail> mail=new Mail();
        mail->setMail(fromBox->currentText());
        mail->setTo( toLine->text() );
        mail->setName(senderNameEdit->text());
        mail->setCC( ccLine->text() );
        mail->setBCC( bccLine->text() );
        mail->setReply( replyLine->text() );
        mail->setSubject( subjectLine->text() );
        if (!m_replyid.isEmpty()) {
            QStringList ids;
            ids.append(m_replyid);
            mail->setInreply(ids);
        }
        QString txt = message->text();
        if ( !sigMultiLine->text().isEmpty() ) {
            txt.append( "\n--\n" );
            txt.append( sigMultiLine->text() );
        }
        qDebug(txt);
        mail->setMessage( txt );

        /* only use the default drafts folder name! */
        Storemail wrapper(AbstractMail::draftFolder());
        wrapper.storeMail(mail);

        AttachViewItem *it = (AttachViewItem *) attList->firstChild();
        /* attachments we will ignore! */
        if ( it != NULL ) {
            QMessageBox::warning(0,tr("Store message"),
                tr("<center>Attachments will not be stored in \"Draft\" folder</center>"));
        }
    }
    QDialog::reject();
}

ComposeMail::~ComposeMail()
{
}

void ComposeMail::reEditMail(const RecMailP&current)
{
    RecMailP data = current;
    message->setText(data->Wrapper()->fetchBody(current)->Bodytext());
    subjectLine->setText( data->getSubject());
    toLine->setText(data->To().join(","));
    ccLine->setText(data->CC().join(","));
    bccLine->setText(data->Bcc().join(","));
    replyLine->setText(data->Replyto());
}

AttachViewItem::AttachViewItem( QListView *parent, Attachment *att )
    : QListViewItem( parent )
{
    attachment = att;
    qDebug( att->getMimeType() );
    setPixmap( 0, attachment->getDocLnk().pixmap().isNull() ?
                  Resource::loadPixmap( "UnknownDocument-14" ) :
                  attachment->getDocLnk().pixmap() );
    setText( 0, att->getName().isEmpty() ? att->getFileName() : att->getName() );
    setText( 1, QString::number( att->getSize() ) );
}

