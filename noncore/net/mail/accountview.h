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
class Selectstore;
class AccountView;

class AccountViewItem : public QListViewItem
{
        
public:
    AccountViewItem( AccountView *parent );
    AccountViewItem( QListViewItem *parent);
    AccountViewItem( QListViewItem *parent , QListViewItem*after  );
    virtual ~AccountViewItem();
    virtual void refresh(QList<RecMail>&)=0;
    virtual RecBody fetchBody(const RecMail&)=0;
    virtual QPopupMenu * getContextMenu(){return 0;};
    virtual void contextMenuSelected(int){}
    virtual AccountView*accountView();
protected:
    virtual void deleteAllMail(AbstractMail*wrapper,Folder*f);
    static const QString contextName;
    AccountView*m_Backlink;
};

class POP3viewItem : public AccountViewItem
{

public:
    POP3viewItem( POP3account *a, AccountView *parent );
    virtual ~POP3viewItem();
    virtual void refresh( QList<RecMail> &target );
    virtual RecBody fetchBody( const RecMail &mail );
    AbstractMail *getWrapper();
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);

protected:
    POP3account *account;
    virtual void refresh();
    AbstractMail *wrapper;
    void disconnect();
    void setOnOffline();
};

class POP3folderItem : public AccountViewItem
{

public:
    POP3folderItem( Folder *folder, POP3viewItem *parent , QListViewItem*after  );
    virtual ~POP3folderItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);

protected:
    void downloadMails();
    Folder *folder;
    POP3viewItem *pop3;
};

class IMAPfolderItem;

class IMAPviewItem : public AccountViewItem
{
    friend class IMAPfolderItem;
public:
    IMAPviewItem( IMAPaccount *a, AccountView *parent );
    virtual ~IMAPviewItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    AbstractMail *getWrapper();
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);
    const QStringList&subFolders();
    virtual void refreshFolders(bool force=false);
    bool offline();

protected:
    IMAPfolderItem*findSubItem(const QString&path,IMAPfolderItem*start=0);
    virtual void createNewFolder();
    virtual void removeChilds();
    IMAPaccount *account;
    AbstractMail *wrapper;
    QStringList currentFolders;
};

class IMAPfolderItem : public AccountViewItem
{

public:
    IMAPfolderItem( Folder *folder, IMAPviewItem *parent , QListViewItem*after  );
    IMAPfolderItem( Folder *folder, IMAPfolderItem *parent , QListViewItem*after, IMAPviewItem *master  );
    virtual ~IMAPfolderItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    bool matchName(const QString&name)const;
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);
    virtual const QString& Delemiter()const;
protected:
    virtual void createNewFolder();
    virtual void deleteFolder();
    virtual void downloadMails();
    Folder *folder;
    IMAPviewItem *imap;
};

class MBOXviewItem : public AccountViewItem
{
    friend class MBOXfolderItem;

public:
    MBOXviewItem( const QString&aMboxPath, AccountView *parent );
    virtual ~MBOXviewItem();
    virtual void refresh( QList<RecMail> &target );
    virtual RecBody fetchBody( const RecMail &mail );
    AbstractMail *getWrapper();
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);
    QStringList subFolders();
    virtual void refresh(bool force=false);

protected:
    virtual void createFolder();
    QString m_Path;
    AbstractMail *wrapper;

};

class MBOXfolderItem : public AccountViewItem
{

public:
    MBOXfolderItem( Folder *folder, MBOXviewItem *parent , QListViewItem*after  );
    virtual ~MBOXfolderItem();
    virtual void refresh(QList<RecMail>&);
    virtual RecBody fetchBody(const RecMail&);
    virtual QPopupMenu * getContextMenu();
    virtual void contextMenuSelected(int);
    virtual Folder*getFolder();

protected:
    virtual void deleteFolder();
    Folder *folder;
    MBOXviewItem *mbox;
};

class AccountView : public QListView
{
    Q_OBJECT

public:
    AccountView( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    virtual ~AccountView();
    virtual void populate( QList<Account> list );
    virtual RecBody fetchBody(const RecMail&aMail);
    virtual void downloadMails(Folder*fromFolder,AbstractMail*fromWrapper);

public slots:
    virtual void refreshAll();
    virtual void refresh(QListViewItem *item);
    virtual void refreshCurrent();
    virtual void slotHold(int, QListViewItem *,const QPoint&,int);
    virtual void slotContextMenu(int id);
    void setupFolderselect(Selectstore*sels);

signals:
    void refreshMailview(QList<RecMail>*);  

protected: 
    QListViewItem* m_currentItem;
    QValueList<IMAPviewItem*> imapAccounts;
    QValueList<MBOXviewItem*> mboxAccounts;
};

#endif
