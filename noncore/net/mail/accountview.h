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
    AccountViewItem( QListViewItem *parent) : QListViewItem( parent) {}
    AccountViewItem( QListViewItem *parent , QListViewItem*after  ) : QListViewItem( parent,after ) {}
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
    AbstractMail *getWrapper();
private:
    POP3account *account;
    AbstractMail *wrapper;

};

class POP3folderItem : public AccountViewItem
{

public:
    POP3folderItem( Folder *folder, POP3viewItem *parent , QListViewItem*after  );
    ~POP3folderItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
private:
    Folder *folder;
    POP3viewItem *pop3;
};

class IMAPfolderItem;

class IMAPviewItem : public AccountViewItem
{

public:
    IMAPviewItem( IMAPaccount *a, QListView *parent );
    ~IMAPviewItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    AbstractMail *getWrapper();

protected:
    IMAPfolderItem*findSubItem(const QString&path,IMAPfolderItem*start=0);

private:
    IMAPaccount *account;
    AbstractMail *wrapper;
};

class IMAPfolderItem : public AccountViewItem
{

public:
    IMAPfolderItem( Folder *folder, IMAPviewItem *parent , QListViewItem*after  );
    IMAPfolderItem( Folder *folder, IMAPfolderItem *parent , QListViewItem*after, IMAPviewItem *master  );
    ~IMAPfolderItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    bool matchName(const QString&name)const;
private:
    Folder *folder;
    IMAPviewItem *imap;
};

class MBOXviewItem : public AccountViewItem
{

public:
//    MBOXviewItem( MBOXaccount *a, QListView *parent );
    MBOXviewItem( const QString&aMboxPath, QListView *parent );
    ~MBOXviewItem();
    virtual void refresh( QList<RecMail> &target );
    virtual RecBody fetchBody( const RecMail &mail );
    AbstractMail *getWrapper();
private:
//    MBOXaccount *account;
    QString m_Path;
    AbstractMail *wrapper;

};

class MBOXfolderItem : public AccountViewItem
{

public:
    MBOXfolderItem( Folder *folder, MBOXviewItem *parent , QListViewItem*after  );
    ~MBOXfolderItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
private:
    Folder *folder;
    MBOXviewItem *mbox;
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
