#include <stdlib.h>
#include <qdir.h>

#include <qpe/config.h>

#include "settings.h"
#include "defines.h"

Settings::Settings()
    : QObject()
{
   updateAccounts();
}

void Settings::checkDirectory()
{
    if ( !QDir( (QString) getenv( "HOME" ) + "/Applications/opiemail/" ).exists() ) {
        system( "mkdir -p $HOME/Applications/opiemail" );
        qDebug( "$HOME/Applications/opiemail created" );
    }
}

QList<Account> Settings::getAccounts()
{
    return accounts;
}

void Settings::addAccount( Account *account )
{
    accounts.append( account );
}

void Settings::delAccount( Account *account )
{
    accounts.remove( account );
    account->remove();
}

void Settings::updateAccounts()
{
    accounts.clear();
    QDir dir( (QString) getenv( "HOME" ) + "/Applications/opiemail" );
    QStringList::Iterator it;

    QStringList imap = dir.entryList( "imap-*" );
    for ( it = imap.begin(); it != imap.end(); it++ ) {
        qDebug( "Added IMAP account" );
        IMAPaccount *account = new IMAPaccount( (*it).replace(0, 5, "") );
        accounts.append( account );
    }

    QStringList pop3 = dir.entryList( "pop3-*" );
    for ( it = pop3.begin(); it != pop3.end(); it++ ) {
        qDebug( "Added POP account" );
        POP3account *account = new POP3account( (*it).replace(0, 5, "") );
        accounts.append( account );
    }

    QStringList smtp = dir.entryList( "smtp-*" );
    for ( it = smtp.begin(); it != smtp.end(); it++ ) {
        qDebug( "Added SMTP account" );
        SMTPaccount *account = new SMTPaccount( (*it).replace(0, 5, "") );
        accounts.append( account );
    }
    
    QStringList nntp = dir.entryList( "nntp-*" );
    for ( it = nntp.begin(); it != nntp.end(); it++ ) {
        qDebug( "Added NNTP account" );
        NNTPaccount *account = new NNTPaccount( (*it).replace(0, 5, "") );
        accounts.append( account );
    }
    
    readAccounts();
}

void Settings::saveAccounts()
{
    checkDirectory(); 
    Account *it;

    for ( it = accounts.first(); it; it = accounts.next() ) {
        it->save();
    }
}

void Settings::readAccounts()
{
    checkDirectory(); 
    Account *it;

    for ( it = accounts.first(); it; it = accounts.next() ) {
        it->read();
    }
}

Account::Account()
{
    accountName = "changeMe";
    type = "changeMe";
    ssl = false;
}

void Account::remove()
{
    QFile file( getFileName() );
    file.remove();
}

IMAPaccount::IMAPaccount()
    : Account()
{
    file = IMAPaccount::getUniqueFileName();
    accountName = "New IMAP Account";
    ssl = false;
    type = "IMAP";
    port = IMAP_PORT;
}

IMAPaccount::IMAPaccount( QString filename )
    : Account()
{
    file = filename;
    accountName = "New IMAP Account";
    ssl = false;
    type = "IMAP";
    port = IMAP_PORT;
}

QString IMAPaccount::getUniqueFileName()
{
    int num = 0;
    QString unique;
    
    QDir dir( (QString) getenv( "HOME" ) + "/Applications/opiemail" );
    QStringList::Iterator it;

    QStringList imap = dir.entryList( "imap-*" );
    do {
        unique.setNum( num++ );
    } while ( imap.contains( "imap-" + unique ) > 0 );

    return unique;
}

void IMAPaccount::read()
{
    Config *conf = new Config( getFileName(), Config::File );
    conf->setGroup( "IMAP Account" );
    accountName = conf->readEntry( "Account","" );
    if (accountName.isNull()) accountName = "";
    server = conf->readEntry( "Server","" );
    if (server.isNull()) server="";
    port = conf->readEntry( "Port","" );
    if (port.isNull()) port="143";
    ssl = conf->readBoolEntry( "SSL",false );
    user = conf->readEntry( "User","" );
    if (user.isNull()) user = "";
    password = conf->readEntryCrypt( "Password","" );
    if (password.isNull()) password = "";
    prefix = conf->readEntry("MailPrefix","");
    if (prefix.isNull()) prefix = "";
}

void IMAPaccount::save()
{
    qDebug( "saving " + getFileName() );
    Settings::checkDirectory();
    
    Config *conf = new Config( getFileName(), Config::File );
    conf->setGroup( "IMAP Account" );
    conf->writeEntry( "Account", accountName );
    conf->writeEntry( "Server", server );
    conf->writeEntry( "Port", port );
    conf->writeEntry( "SSL", ssl );
    conf->writeEntry( "User", user );
    conf->writeEntryCrypt( "Password", password );
    conf->writeEntry( "MailPrefix",prefix);
    conf->write();
}


QString IMAPaccount::getFileName()
{
    return (QString) getenv( "HOME" ) + "/Applications/opiemail/imap-" + file;
}

POP3account::POP3account()
    : Account()
{
    file = POP3account::getUniqueFileName();
    accountName = "New POP3 Account";
    ssl = false;
    type = "POP3";
    port = POP3_PORT;
}

POP3account::POP3account( QString filename )
    : Account()
{
    file = filename;
    accountName = "New POP3 Account";
    ssl = false;
    type = "POP3";
    port = POP3_PORT;
}

QString POP3account::getUniqueFileName()
{
    int num = 0;
    QString unique;
    
    QDir dir( (QString) getenv( "HOME" ) + "/Applications/opiemail" );
    QStringList::Iterator it;

    QStringList imap = dir.entryList( "pop3-*" );
    do {
        unique.setNum( num++ );
    } while ( imap.contains( "pop3-" + unique ) > 0 );

    return unique;
}

void POP3account::read()
{
    Config *conf = new Config( getFileName(), Config::File );
    conf->setGroup( "POP3 Account" );
    accountName = conf->readEntry( "Account" );
    server = conf->readEntry( "Server" );
    port = conf->readEntry( "Port" );
    ssl = conf->readBoolEntry( "SSL" );
    user = conf->readEntry( "User" );
    password = conf->readEntryCrypt( "Password" );
}

void POP3account::save()
{
    qDebug( "saving " + getFileName() );
    Settings::checkDirectory();
    
    Config *conf = new Config( getFileName(), Config::File );
    conf->setGroup( "POP3 Account" );
    conf->writeEntry( "Account", accountName );
    conf->writeEntry( "Server", server );
    conf->writeEntry( "Port", port );
    conf->writeEntry( "SSL", ssl );
    conf->writeEntry( "User", user );
    conf->writeEntryCrypt( "Password", password );
    conf->write();
}


QString POP3account::getFileName()
{
    return (QString) getenv( "HOME" ) + "/Applications/opiemail/pop3-" + file;
}

SMTPaccount::SMTPaccount()
    : Account()
{
    file = SMTPaccount::getUniqueFileName();
    accountName = "New SMTP Account";
    ssl = false;
    login = false;
    useCC = false;
    useBCC = false;
    useReply = false;
    type = "SMTP";
    port = SMTP_PORT;
}

SMTPaccount::SMTPaccount( QString filename )
    : Account()
{
    file = filename;
    accountName = "New SMTP Account";
    ssl = false;
    login = false;
    useCC = false;
    useBCC = false;
    useReply = false;
    type = "SMTP";
    port = SMTP_PORT;
}

QString SMTPaccount::getUniqueFileName()
{
    int num = 0;
    QString unique;
    
    QDir dir( (QString) getenv( "HOME" ) + "/Applications/opiemail" );
    QStringList::Iterator it;

    QStringList imap = dir.entryList( "smtp-*" );
    do {
        unique.setNum( num++ );
    } while ( imap.contains( "smtp-" + unique ) > 0 );

    return unique;
}

void SMTPaccount::read()
{
    Config *conf = new Config( getFileName(), Config::File );
    conf->setGroup( "SMTP Account" );
    accountName = conf->readEntry( "Account" );
    server = conf->readEntry( "Server" );
    port = conf->readEntry( "Port" );
    ssl = conf->readBoolEntry( "SSL" );
    login = conf->readBoolEntry( "Login" );
    user = conf->readEntry( "User" );
    password = conf->readEntryCrypt( "Password" );
    useCC = conf->readBoolEntry( "useCC" );
    useBCC = conf->readBoolEntry( "useBCC" );
    useReply = conf->readBoolEntry( "useReply" );
    name = conf->readEntry( "Name" );
    mail = conf->readEntry( "Mail" );
    org = conf->readEntry( "Org" );
    cc = conf->readEntry( "CC" );
    bcc = conf->readEntry( "BCC" );
    reply = conf->readEntry( "Reply" );
    signature = conf->readEntry( "Signature" );
    signature = signature.replace( QRegExp( "<br>" ), "\n" );
}

void SMTPaccount::save()
{
    qDebug( "saving " + getFileName() );
    Settings::checkDirectory();
    
    Config *conf = new Config( getFileName(), Config::File );
    conf->setGroup( "SMTP Account" );
    conf->writeEntry( "Account", accountName );
    conf->writeEntry( "Server", server );
    conf->writeEntry( "Port", port );
    conf->writeEntry( "SSL", ssl );
    conf->writeEntry( "Login", login );
    conf->writeEntry( "User", user );
    conf->writeEntryCrypt( "Password", password );
    conf->writeEntry( "useCC", useCC );
    conf->writeEntry( "useBCC", useBCC );
    conf->writeEntry( "useReply", useReply );
    conf->writeEntry( "Name", name );
    conf->writeEntry( "Mail", mail );
    conf->writeEntry( "Org", org );
    conf->writeEntry( "CC", cc );
    conf->writeEntry( "BCC", bcc );
    conf->writeEntry( "Reply", reply );
    conf->writeEntry( "Signature", 
                      signature.replace( QRegExp( "\\n" ), "<br>" ) );
    conf->write();
}


QString SMTPaccount::getFileName()
{
    return (QString) getenv( "HOME" ) + "/Applications/opiemail/smtp-" + file;
}

NNTPaccount::NNTPaccount()
    : Account()
{
    file = NNTPaccount::getUniqueFileName();
    accountName = "New NNTP Account";
    ssl = false;
    login = false;
    type = "NNTP";
    port = NNTP_PORT;
}

NNTPaccount::NNTPaccount( QString filename )
    : Account()
{
    file = filename;
    accountName = "New NNTP Account";
    ssl = false;
    login = false;
    type = "NNTP";
    port = NNTP_PORT;
}

QString NNTPaccount::getUniqueFileName()
{
    int num = 0;
    QString unique;
    
    QDir dir( (QString) getenv( "HOME" ) + "/Applications/opiemail" );
    QStringList::Iterator it;

    QStringList imap = dir.entryList( "nntp-*" );
    do {
        unique.setNum( num++ );
    } while ( imap.contains( "nntp-" + unique ) > 0 );

    return unique;
}

void NNTPaccount::read()
{
    Config *conf = new Config( getFileName(), Config::File );
    conf->setGroup( "NNTP Account" );
    accountName = conf->readEntry( "Account" );
    server = conf->readEntry( "Server" );
    port = conf->readEntry( "Port" );
    ssl = conf->readBoolEntry( "SSL" );
    login = conf->readBoolEntry( "Login" );
    user = conf->readEntry( "User" );
    password = conf->readEntryCrypt( "Password" );
}

void NNTPaccount::save()
{
    qDebug( "saving " + getFileName() );
    Settings::checkDirectory();
    
    Config *conf = new Config( getFileName(), Config::File );
    conf->setGroup( "NNTP Account" );
    conf->writeEntry( "Account", accountName );
    conf->writeEntry( "Server", server );
    conf->writeEntry( "Port", port );
    conf->writeEntry( "SSL", ssl );
    conf->writeEntry( "Login", login );
    conf->writeEntry( "User", user );
    conf->writeEntryCrypt( "Password", password );
    conf->write();
}


QString NNTPaccount::getFileName()
{
    return (QString) getenv( "HOME" ) + "/Applications/opiemail/nntp-" + file;
}

