#include <qt.h>

#include <opie/ofiledialog.h>
#include <qpe/resource.h>

#include "composemail.h"

ComposeMail::ComposeMail( Settings *s, QWidget *parent, const char *name, bool modal, WFlags flags )
    : ComposeMailUI( parent, name, modal, flags )
{
    settings = s;

    attList->addColumn( tr( "Name" ) );
    attList->addColumn( tr( "Size" ) );

    QList<Account> accounts = settings->getAccounts();
    Account *it;
    for ( it = accounts.first(); it; it = accounts.next() ) {
        if ( it->getType().compare( "SMTP" ) == 0 ) {
           SMTPaccount *smtp = static_cast<SMTPaccount *>(it);
           fromBox->insertItem( smtp->getMail() );
           smtpAccounts.append( smtp );
        }
    }

    if ( smtpAccounts.count() > 0 ) {
        fillValues( fromBox->currentItem() );
    } else {
        QMessageBox::information( this, tr( "Problem" ),
                                  tr( "<p>Please create an SMTP account first.</p>" ),
                                  tr( "Ok" ) );
    }

    connect( fromBox, SIGNAL( activated( int ) ), SLOT( fillValues( int ) ) );
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
/*  QString toline;
  QStringList toEntry = to;
  for ( QStringList::Iterator it = toEntry.begin(); it != toEntry.end(); ++it ) {
     toline += (*it);
  }
  toLine->setText( toline );
*/
toLine->setText( to );
}

void ComposeMail::setSubject( const QString & subject )
{
 subjectLine->setText( subject );
}

void ComposeMail::setInReplyTo( const QString & messageId )
{

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

void ComposeMail::fillValues( int current )
{
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
    qDebug( "Sending Mail with " +
            smtpAccounts.at( fromBox->currentItem() )->getAccountName() );
    Mail *mail = new Mail();
    SMTPaccount *smtp = smtpAccounts.at( fromBox->currentItem() );
    mail->setMail( smtp->getMail() );
    mail->setName( smtp->getName() );

    if ( !toLine->text().isEmpty() ) {
        mail->setTo( toLine->text() );
    } else {
        qDebug( "No Reciever spezified -> returning" );
        return;
    }

    mail->setCC( ccLine->text() );
    mail->setBCC( bccLine->text() );
    mail->setReply( replyLine->text() );
    mail->setSubject( subjectLine->text() );
    QString txt = message->text();
    if ( !sigMultiLine->text().isEmpty() ) {
        txt.append( "\n--\n" );
        txt.append( sigMultiLine->text() );
    }
    mail->setMessage( txt );
    AttachViewItem *it = (AttachViewItem *) attList->firstChild();
    while ( it != NULL ) {
        mail->addAttachment( it->getAttachment() );
        it = (AttachViewItem *) it->itemBelow();
    }

    MailWrapper wrapper( settings );
    wrapper.sendMail( *mail );

    QDialog::accept();
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

