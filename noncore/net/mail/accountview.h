#ifndef ACCOUNTVIEW_H
#define ACCOUNTVIEW_H

#include <qlistview.h>
#include <qlist.h>

#include <libmailwrapper/settings.h>
#include <libmailwrapper/mailwrapper.h>
#include <libmailwrapper/abstractmail.h>

class POP3wrapper;
class RecMail;
class RecBody;
class QPopupMenu;

class AccountViewItem : public QListViewItem
{
        
public:
    AccountViewItem( QListView *parent ) : QListViewItem( parent ) {}
    AccountViewItem( QListViewItem *parent) : QListViewItem( parent) {}
    AccountViewItem( QListViewItem *parent , QListViewItem*after  ) : QListViewItem( parent,after ) {}
    virtual void refresh(QList<RecMail>&)=0;
    virtual RecBody fetchBody(const RecMail&)=0;
    virtual QPopupMenu * getContextMenu(){return 0;};
    virtual void contextMenuSelected(int){}
protected:
    virtual void deleteAllMail(AbstractMail*wrapper,Folder*f);
    static const QString contextName;
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
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);

private:
    Folder *folder;
    POP3viewItem *pop3;
};

class IMAPfolderItem;

class IMAPviewItem : public AccountViewItem
{
    friend class IMAPfolderItem;
public:
    IMAPviewItem( IMAPaccount *a, QListView *parent );
    ~IMAPviewItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    AbstractMail *getWrapper();
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);

protected:
    IMAPfolderItem*findSubItem(const QString&path,IMAPfolderItem*start=0);
    virtual void refreshFolders(bool force=false);
    virtual void createNewFolder();

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
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);
    virtual const QString& Delemiter()const;
protected:
    virtual void createNewFolder();
    virtual void deleteFolder();
    
private:
    Folder *folder;
    IMAPviewItem *imap;
};

class MBOXviewItem : public AccountViewItem
{
    friend class MBOXfolderItem;

public:
//    MBOXviewItem( MBOXaccount *a, QListView *parent );
    MBOXviewItem( const QString&aMboxPath, QListView *parent );
    ~MBOXviewItem();
    virtual void refresh( QList<RecMail> &target );
    virtual RecBody fetchBody( const RecMail &mail );
    AbstractMail *getWrapper();
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);

protected:
    virtual void refresh(bool force=false);
    virtual void createFolder();

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
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);

protected:
    virtual void deleteFolder();
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
    virtual void refreshAll();
    virtual void refresh(QListViewItem *item);
    virtual void refreshCurrent();
    virtual void slotHold(int, QListViewItem *,const QPoint&,int);
    virtual void slotContextMenu(int id);

signals:
    void refreshMailview(QList<RecMail>*);  

private: 
    QListViewItem* m_currentItem; 
};

#endif
