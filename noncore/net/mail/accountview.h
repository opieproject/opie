#ifndef ACCOUNTVIEW_H
#define ACCOUNTVIEW_H

#include <qlistview.h>
#include <qlist.h>

#include "settings.h"
#include "mailwrapper.h"

class IMAPwrapper;
class POP3wrapper;
class RecMail;
class RecBody;

class AccountViewItem : public QListViewItem
{
        
public:
    AccountViewItem( QListView *parent ) : QListViewItem( parent ) {}
    AccountViewItem( QListViewItem *parent ) : QListViewItem( parent ) {}
    virtual void refresh(QList<RecMail>&)=0;
    virtual RecBody fetchBody(const RecMail&)=0;
};

class POP3viewItem : public AccountViewItem
{

public:
    POP3viewItem( POP3account *a, QListView *parent );
    ~POP3viewItem();
    virtual void refresh( QList<RecMail> &target );
    virtual RecBody fetchBody( const RecMail &mail );

private:
    POP3account *account;
    POP3wrapper *wrapper;

};

class IMAPviewItem : public AccountViewItem
{

public:
    IMAPviewItem( IMAPaccount *a, QListView *parent );
    ~IMAPviewItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    IMAPwrapper *getWrapper();

private:
    IMAPaccount *account;
    IMAPwrapper *wrapper;
    
};

class IMAPfolderItem : public AccountViewItem
{

public:
    IMAPfolderItem( IMAPFolder *folder, IMAPviewItem *parent );
    ~IMAPfolderItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);

private:
    IMAPFolder *folder;
    IMAPviewItem *imap;
    
};

class AccountView : public QListView
{
    Q_OBJECT

public:
    AccountView( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    void populate( QList<Account> list );
    RecBody fetchBody(const RecMail&aMail);

public slots:
    void refreshAll();
    void refresh(QListViewItem *item);
    
signals:
    void refreshMailview(QList<RecMail>*);    
};

#endif
