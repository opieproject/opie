#include <qt.h>

#include "defines.h"
#include "editaccounts.h"

AccountListItem::AccountListItem( QListView *parent, Account *a)
    : QListViewItem( parent )
{
    account = a;
    setText( 0, account->getAccountName() );
    setText( 1, account->getType() );
}

EditAccounts::EditAccounts( Settings *s, QWidget *parent, const char *name, bool modal, WFlags flags )
    : EditAccountsUI( parent, name, modal, flags )
{
    qDebug( "New Account Configuration Widget" );
    settings = s;

    mailList->addColumn( tr( "Account" ) );
    mailList->addColumn( tr( "Type" ) );

    newsList->addColumn( tr( "Account" ) );

    connect( newMail, SIGNAL( clicked() ), SLOT( slotNewMail() ) );
    connect( editMail, SIGNAL( clicked() ), SLOT( slotEditMail() ) );
    connect( deleteMail, SIGNAL( clicked() ), SLOT( slotDeleteMail() ) );
    connect( newNews, SIGNAL( clicked() ), SLOT( slotNewNews() ) );
    connect( editNews, SIGNAL( clicked() ), SLOT( slotEditNews() ) );
    connect( deleteNews, SIGNAL( clicked() ), SLOT( slotDeleteNews() ) );

    slotFillLists();
}

void EditAccounts::slotFillLists()
{
    mailList->clear();
    newsList->clear();

    QList<Account> accounts = settings->getAccounts();
    Account *it;
    for ( it = accounts.first(); it; it = accounts.next() ) {
        if ( it->getType().compare( "NNTP" ) == 0 ) {
            (void) new AccountListItem( newsList, it );
        } else {
            (void) new AccountListItem( mailList, it );
        }
    }
}

void EditAccounts::slotNewMail()
{
    qDebug( "New Mail Account" );
    QString *selection = new QString();
    SelectMailType selType( selection, this, 0, true );
    selType.show();
    if ( QDialog::Accepted == selType.exec() ) {
        slotNewAccount( *selection );
    }
}

void EditAccounts::slotNewAccount( const QString &type )
{
    if ( type.compare( "IMAP" ) == 0 ) {
        qDebug( "-> config IMAP" );
        IMAPaccount *account = new IMAPaccount();
        IMAPconfig imap( account, this, 0, true );
        imap.showMaximized();
        if ( QDialog::Accepted == imap.exec() ) {
            settings->addAccount( account );
            account->save();
            slotFillLists();
          } else {
            account->remove();
        }
    } else if ( type.compare( "POP3" ) == 0 ) {
        qDebug( "-> config POP3" );
        POP3account *account = new POP3account();
        POP3config pop3( account, this, 0, true );
        pop3.showMaximized();
        if ( QDialog::Accepted == pop3.exec() ) {
            settings->addAccount( account );
            account->save();
            slotFillLists();
        } else {
            account->remove();
        }
    } else if ( type.compare( "SMTP" ) == 0 ) {
        qDebug( "-> config SMTP" );
        SMTPaccount *account = new SMTPaccount();
        SMTPconfig smtp( account, this, 0, true );
        smtp.showMaximized();
        if ( QDialog::Accepted == smtp.exec() ) {
            settings->addAccount( account );
            account->save();
            slotFillLists();

        } else {
            account->remove();
        }
    } else if ( type.compare( "NNTP" ) == 0 ) {
        qDebug( "-> config NNTP" );
        NNTPaccount *account = new NNTPaccount();
        NNTPconfig nntp( account, this, 0, true );
        nntp.showMaximized();
        if ( QDialog::Accepted == nntp.exec() ) {
            settings->addAccount( account );
            account->save();
            slotFillLists();
        } else {
            account->remove();
        }
    }
}

void EditAccounts::slotEditAccount( Account *account )
{
    if ( account->getType().compare( "IMAP" ) == 0 ) {
        IMAPaccount *imapAcc = static_cast<IMAPaccount *>(account);
        IMAPconfig imap( imapAcc, this, 0, true );
        imap.showMaximized();
        if ( QDialog::Accepted == imap.exec() ) {
           slotFillLists();
        }
    } else if ( account->getType().compare( "POP3" ) == 0 ) {
        POP3account *pop3Acc = static_cast<POP3account *>(account);
        POP3config pop3( pop3Acc, this, 0, true );
        pop3.showMaximized();
        if ( QDialog::Accepted == pop3.exec() ) {
           slotFillLists();
        }
    } else if ( account->getType().compare( "SMTP" ) == 0 ) {
        SMTPaccount *smtpAcc = static_cast<SMTPaccount *>(account);
        SMTPconfig smtp( smtpAcc, this, 0, true );
        smtp.showMaximized();
        if ( QDialog::Accepted == smtp.exec() ) {
           slotFillLists();
        }
    } else if ( account->getType().compare( "NNTP" ) == 0 ) {
        NNTPaccount *nntpAcc = static_cast<NNTPaccount *>(account);
        NNTPconfig nntp( nntpAcc, this, 0, true );
        nntp.showMaximized();
        if ( QDialog::Accepted == nntp.exec() ) {
           slotFillLists();
        }
    }
}

void EditAccounts::slotDeleteAccount( Account *account )
{
    if ( QMessageBox::information( this, tr( "Question" ),
         tr( "<p>Do you really want to delete the selected Account?</p>" ),
         tr( "Yes" ), tr( "No" ) ) == 0 ) {
        settings->delAccount( account );
        slotFillLists();
    }
}

void EditAccounts::slotEditMail()
{
    qDebug( "Edit Mail Account" );
    if ( !mailList->currentItem() ) {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select an account.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) mailList->currentItem())->getAccount();
    slotEditAccount( a );
}

void EditAccounts::slotDeleteMail()
{
    if ( !mailList->currentItem() ) {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select an account.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) mailList->currentItem())->getAccount();
    slotDeleteAccount( a );
}

void EditAccounts::slotNewNews()
{
    qDebug( "New News Account" );
    slotNewAccount( "NNTP" );
}

void EditAccounts::slotEditNews()
{
    qDebug( "Edit News Account" );
    if ( !newsList->currentItem() ) {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select an account.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) newsList->currentItem())->getAccount();
    slotEditAccount( a );
}

void EditAccounts::slotDeleteNews()
{
    qDebug( "Delete News Account" );
    if ( !newsList->currentItem() ) {
        QMessageBox::information( this, tr( "Error" ),
                                  tr( "<p>Please select an account.</p>" ),
                                  tr( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) newsList->currentItem())->getAccount();
    slotDeleteAccount( a );
}

void EditAccounts::slotAdjustColumns()
{
    int currPage = configTab->currentPageIndex();

    configTab->showPage( mailTab );
    mailList->setColumnWidth( 0, mailList->visibleWidth() - 50 );
    mailList->setColumnWidth( 1, 50 );

    configTab->showPage( newsTab );
    newsList->setColumnWidth( 0, newsList->visibleWidth() );

    configTab->setCurrentPage( currPage );
}

void EditAccounts::accept()
{
    settings->saveAccounts();

    QDialog::accept();
}

/**
 * SelectMailType
 */

SelectMailType::SelectMailType( QString *selection, QWidget *parent, const char *name, bool modal, WFlags flags )
    : SelectMailTypeUI( parent, name, modal, flags )
{
    selected = selection;
    selected->replace( 0, selected->length(), typeBox->currentText() );
    connect( typeBox, SIGNAL( activated( const QString & ) ), SLOT( slotSelection( const QString & ) ) );
}

void SelectMailType::slotSelection( const QString &sel )
{
    selected->replace( 0, selected->length(), sel );
}

/**
 * IMAPconfig
 */

IMAPconfig::IMAPconfig( IMAPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
    : IMAPconfigUI( parent, name, modal, flags )
{
    data = account;

    fillValues();

    connect( sslBox, SIGNAL( toggled( bool ) ), SLOT( slotSSL( bool ) ) );
}

void IMAPconfig::slotSSL( bool enabled )
{
    if ( enabled ) {
        portLine->setText( IMAP_SSL_PORT );
    } else {
        portLine->setText( IMAP_PORT );
    }
}

void IMAPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    sslBox->setChecked( data->getSSL() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
    prefixLine->setText(data->getPrefix());
}

void IMAPconfig::accept()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setSSL( sslBox->isChecked() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );
    data->setPrefix(prefixLine->text());

    QDialog::accept();
}

/**
 * POP3config
 */

POP3config::POP3config( POP3account *account, QWidget *parent, const char *name, bool modal, WFlags flags )
    : POP3configUI( parent, name, modal, flags )
{
    data = account;
    fillValues();

    connect( ComboBox1, SIGNAL( activated( int ) ), SLOT( slotConnectionToggle( int ) ) );
    ComboBox1->insertItem( "Only if available", 0 );
    ComboBox1->insertItem( "Always, Negotiated", 1 );
    ComboBox1->insertItem( "Connect on secure port", 2 );
    ComboBox1->insertItem( "Run command instead", 3 );
    CommandEdit->hide();
    ComboBox1->setCurrentItem( data->ConnectionType() );
}

void POP3config::slotConnectionToggle( int index )
{
    // 2 is ssl connection
    if ( index == 2 ) {
       portLine->setText( POP3_SSL_PORT );
    } else if (  index == 3 ) {
        portLine->setText( POP3_PORT );
        CommandEdit->show();
    } else {
        portLine->setText( POP3_PORT );
    }
}

void POP3config::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    ComboBox1->setCurrentItem( data->ConnectionType() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
}

void POP3config::accept()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setConnectionType( ComboBox1->currentItem() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );

    QDialog::accept();
}

/**
 * SMTPconfig
 */

SMTPconfig::SMTPconfig( SMTPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
    : SMTPconfigUI( parent, name, modal, flags )
{
    data = account;

    connect( loginBox, SIGNAL( toggled( bool ) ), userLine, SLOT( setEnabled( bool ) ) );
    connect( loginBox, SIGNAL( toggled( bool ) ), passLine, SLOT( setEnabled( bool ) ) );

    fillValues();

    connect( sslBox, SIGNAL( toggled( bool ) ), SLOT( slotSSL( bool ) ) );
}

void SMTPconfig::slotSSL( bool enabled )
{
    if ( enabled ) {
        portLine->setText( SMTP_SSL_PORT );
    } else {
        portLine->setText( SMTP_PORT );
    }
}

void SMTPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    sslBox->setChecked( data->getSSL() );
    loginBox->setChecked( data->getLogin() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
}

void SMTPconfig::accept()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setSSL( sslBox->isChecked() );
    data->setLogin( loginBox->isChecked() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );

    QDialog::accept();
}

/**
 * NNTPconfig
 */

NNTPconfig::NNTPconfig( NNTPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
    : NNTPconfigUI( parent, name, modal, flags )
{
    data = account;

    connect( loginBox, SIGNAL( toggled( bool ) ), userLine, SLOT( setEnabled( bool ) ) );
    connect( loginBox, SIGNAL( toggled( bool ) ), passLine, SLOT( setEnabled( bool ) ) );

    fillValues();

    connect( sslBox, SIGNAL( toggled( bool ) ), SLOT( slotSSL( bool ) ) );
}

void NNTPconfig::slotSSL( bool enabled )
{
    if ( enabled ) {
        portLine->setText( NNTP_SSL_PORT );
    } else {
        portLine->setText( NNTP_PORT );
    }
}

void NNTPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    sslBox->setChecked( data->getSSL() );
    loginBox->setChecked( data->getLogin() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
}

void NNTPconfig::accept()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setSSL( sslBox->isChecked() );
    data->setLogin( loginBox->isChecked() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );

    QDialog::accept();
}

