#ifndef EDITACCOUNTS_H
#define EDITACCOUNTS_H

#include <qstring.h>
#include <qlistview.h>

#include "editaccountsui.h"
#include "selectmailtypeui.h"
#include "imapconfigui.h"
#include "pop3configui.h"
#include "smtpconfigui.h"
#include "nntpconfigui.h"
#include "settings.h"

class AccountListItem : public QListViewItem
{

public:
    AccountListItem( QListView *parent, Account *a);
    Account *getAccount() { return account; }

private:
    Account *account;

};

class EditAccounts : public EditAccountsUI
{
    Q_OBJECT

public: 
    EditAccounts( Settings *s, QWidget *parent = 0, const char *name = 0, bool modal = 0, WFlags flags = 0 );
    
public slots:
    void slotNewAccount( const QString &type );
    void slotEditAccount( Account *account );
    void slotDeleteAccount( Account * account );
    void slotAdjustColumns();

protected slots:
    void slotFillLists();
    void slotNewMail();
    void slotEditMail();
    void slotDeleteMail();
    void slotNewNews();
    void slotEditNews();
    void slotDeleteNews();
    void accept();

private:
    Settings *settings;
    
};

class SelectMailType : public SelectMailTypeUI
{
    Q_OBJECT

public:
    SelectMailType( QString *selection = 0, QWidget *parent = 0, const char *name = 0, bool modal = 0, WFlags flags = 0 );

private slots:
    void slotSelection( const QString &sel );

private:
    QString *selected;

};

class IMAPconfig : public IMAPconfigUI
{
    Q_OBJECT

public:
    IMAPconfig( IMAPaccount *account, QWidget *parent = 0, const char *name = 0, bool modal = 0, WFlags flags = 0 );

public slots:
    void fillValues();

protected slots:
    void slotSSL( bool enabled );
    void accept();

private:
    IMAPaccount *data;

};

class POP3config : public POP3configUI
{
    Q_OBJECT

public:
    POP3config( POP3account *account, QWidget *parent = 0, const char *name = 0, bool modal = 0, WFlags flags = 0 );
    
public slots:
    void fillValues();

protected slots:
    void slotSSL( bool enabled );
    void accept();

private:
    POP3account *data;

};

class SMTPconfig : public SMTPconfigUI
{
    Q_OBJECT

public:
    SMTPconfig( SMTPaccount *account, QWidget *parent = 0, const char *name = 0, bool modal = 0, WFlags flags = 0 );

public slots:
    void slotSSL( bool enabled );
    void fillValues();

protected slots:
    void accept();

private:
    SMTPaccount *data;

};

class NNTPconfig : public NNTPconfigUI
{
    Q_OBJECT

public:
    NNTPconfig( NNTPaccount *account, QWidget *parent = 0, const char *name = 0, bool modal = 0, WFlags flags = 0 );

public slots:
    void fillValues();

protected slots:
    void slotSSL( bool enabled );
    void accept();

private:
    NNTPaccount *data;

};

#endif
