#ifndef SETTINGS_H
#define SETTINGS_H

#include "maildefines.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qobject.h>
#include <qlist.h>

class Account
{

public:
    Account();
    virtual ~Account() {}

    void remove();
    void setAccountName( QString name ) { accountName = name; }
    const QString&getAccountName()const{ return accountName; }
    MAILLIB::ATYPE getType()const{ return type; }

    void setServer(const QString&str){ server = str; }
    const QString&getServer()const{ return server; }

    void setPort(const QString&str) { port = str; }
    const QString&getPort()const{ return port; }

    void setUser(const QString&str){ user = str; }
    const QString&getUser()const{ return user; }

    void setPassword(const QString&str) { password = str; }
    const QString&getPassword()const { return password; }

    void setSSL( bool b ) { ssl = b; }
    bool getSSL() { return ssl; }

    void setConnectionType( int x ) { connectionType = x; }
    int ConnectionType() { return connectionType; }


    void setOffline(bool b) {offline = b;}
    bool getOffline()const{return offline;}

    virtual QString getFileName() { return accountName; }
    virtual void read() { odebug << "base reading..." << oendl; }
    virtual void save() { odebug << "base saving..." << oendl; }

protected:
    QString accountName, server, port, user, password;
    bool ssl;
    int connectionType;
    bool offline;
    MAILLIB::ATYPE type;
};

class IMAPaccount : public Account
{

public:
    IMAPaccount();
    IMAPaccount( QString filename );

    static QString getUniqueFileName();

    virtual void read();
    virtual void save();
    virtual QString getFileName();

    void setPrefix(const QString&str) {prefix=str;}
    const QString&getPrefix()const{return prefix;}

private:
    QString file,prefix;

};

class POP3account : public Account
{

public:
    POP3account();
    POP3account( QString filename );

    static QString getUniqueFileName();

    virtual void read();
    virtual void save();
    virtual QString getFileName();

private:
    QString file;

};

class SMTPaccount : public Account
{

public:
    SMTPaccount();
    SMTPaccount( QString filename );

    static QString getUniqueFileName();

    virtual void read();
    virtual void save();
    virtual QString getFileName();

    void setLogin( bool b ) { login = b; }
    bool getLogin() { return login; }

private:
    QString file, name, mail, org, cc, bcc, reply, signature;
    bool useCC, useBCC, useReply, login;

};

class NNTPaccount : public Account
{

public:
    NNTPaccount();
    NNTPaccount( QString filename );

    static QString getUniqueFileName();

    virtual void read();
    virtual void save();
    virtual QString getFileName();

    void setLogin( bool b ) { login = b; }
    bool getLogin() { return login; }

    void setGroups( QStringList list )  { subscribedGroups = list; }
    QStringList getGroups()  { return subscribedGroups; }

private:
    QString file;
    bool login;
    QStringList subscribedGroups;

};

class Settings : public QObject
{
    Q_OBJECT

public:
    Settings();
    QList<Account> getAccounts();
    void addAccount(Account *account);
    void delAccount(Account *account);
    void saveAccounts();
    void readAccounts();
    static void checkDirectory();

private:
    void updateAccounts();
    QList<Account> accounts;

};

#endif
