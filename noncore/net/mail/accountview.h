#ifndef ACCOUNTVIEW_H
#define ACCOUNTVIEW_H

#include <qlistview.h>
#include <qlist.h>

#include "settings.h"
#include "mailwrapper.h"
#include "abstractmail.h"

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
    AbstractMail *wrapper;

};

class IMAPviewItem : public AccountViewItem
{

public:
    IMAPviewItem( IMAPaccount *a, QListView *parent );
    ~IMAPviewItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    AbstractMail *getWrapper();
private:
    IMAPaccount *account;
    AbstractMail *wrapper;
};

class IMAPfolderItem : public AccountViewItem
{

public:
    IMAPfolderItem( Folder *folder, IMAPviewItem *parent );
    ~IMAPfolderItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
private:
    Folder *folder;
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
    void refreshCurrent();
    
signals:
    void refreshMailview(QList<RecMail>*);  

private: 
    QListViewItem* m_currentItem; 
};

#endif
