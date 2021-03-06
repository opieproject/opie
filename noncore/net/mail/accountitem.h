#ifndef __ACCOUNT_ITEM
#define __ACCOUNT_ITEM

#include <opie2/osmartpointer.h>

#include <qlistview.h>
#include <qlist.h>
#include <qmap.h>

class POP3wrapper;
class RecMail;
class RecBody;
class QPopupMenu;
class Selectstore;
class AccountView;
class POP3account;
class NNTPaccount;
class IMAPaccount;
class AbstractMail;
class Folder;

#define RECBODYP Opie::Core::OSmartPointer<RecBody>

class AccountViewItem : public QListViewItem
{

public:
    AccountViewItem( AccountView *parent );
    AccountViewItem( QListViewItem *parent);
    AccountViewItem( QListViewItem *parent , QListViewItem*after  );
    AccountViewItem( const Opie::Core::OSmartPointer<Folder>&folderInit,QListViewItem *parent , QListViewItem*after  );

    virtual ~AccountViewItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&)=0;
    virtual RECBODYP fetchBody(const Opie::Core::OSmartPointer<RecMail>&)=0;
    virtual QPopupMenu * getContextMenu();
    virtual bool contextMenuSelected(int){return false;}
    virtual AccountView*accountView();
    virtual bool matchName(const QString&name)const;
    virtual bool isDraftfolder();
    /* 1 - server
     * 2 - folder
     * 3 - both
     */
    virtual int isServer()const=0;
    virtual QMap<int,QString> serverMenu();
    virtual QMap<int,QString> folderMenu();

protected:
    AccountViewItem*findSubItem(const QString&path,AccountViewItem*start=0);
    virtual void init();
    virtual void removeChilds();
    virtual void deleteAllMail(AbstractMail*wrapper,const Opie::Core::OSmartPointer<Folder>&f);
    static const QString contextName;
    AccountView*m_Backlink;
    Opie::Core::OSmartPointer<Folder> folder;
};

class POP3viewItem : public AccountViewItem
{

public:
    POP3viewItem( POP3account *a, AccountView *parent );
    virtual ~POP3viewItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&target );
    virtual RECBODYP fetchBody( const Opie::Core::OSmartPointer<RecMail> &mail );
    AbstractMail *getWrapper();
    virtual bool contextMenuSelected(int);
    virtual int isServer()const{return 1;}
    virtual QMap<int,QString> serverMenu();

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
    POP3folderItem( const Opie::Core::OSmartPointer<Folder>&folder, POP3viewItem *parent , QListViewItem*after  );
    virtual ~POP3folderItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&);
    virtual RECBODYP fetchBody(const Opie::Core::OSmartPointer<RecMail>&);
    virtual bool contextMenuSelected(int);
    virtual int isServer()const{return 2;}
    virtual QMap<int,QString> folderMenu();

protected:
    void downloadMails();
    POP3viewItem *pop3;
};


class NNTPviewItem : public AccountViewItem
{

public:
    NNTPviewItem( NNTPaccount *a, AccountView *parent );
    virtual ~NNTPviewItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&target );
    virtual RECBODYP fetchBody( const Opie::Core::OSmartPointer<RecMail> &mail );
    AbstractMail *getWrapper();
    virtual bool contextMenuSelected(int);
    virtual int isServer()const{return 1;}
    virtual QMap<int,QString> serverMenu();

protected:
    NNTPaccount *account;
    virtual void refresh();
    AbstractMail *wrapper;
    void disconnect();
    void setOnOffline();
    void subscribeGroups();
};

class NNTPfolderItem : public AccountViewItem
{

public:
    NNTPfolderItem(const Opie::Core::OSmartPointer<Folder>&folder, NNTPviewItem *parent , QListViewItem*after  );
    virtual ~NNTPfolderItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&);
    virtual RECBODYP fetchBody(const Opie::Core::OSmartPointer<RecMail>&);
    virtual bool contextMenuSelected(int);
    virtual int isServer()const{return 2;}
    virtual QMap<int,QString> folderMenu();
protected:
    void downloadMails();
    NNTPviewItem *nntp;
};



class IMAPviewItem : public AccountViewItem
{
    friend class IMAPfolderItem;
public:
    IMAPviewItem( IMAPaccount *a, AccountView *parent );
    virtual ~IMAPviewItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&);
    virtual RECBODYP fetchBody(const Opie::Core::OSmartPointer<RecMail>&);
    AbstractMail *getWrapper();
    virtual bool contextMenuSelected(int);
    const QStringList&subFolders();
    virtual void refreshFolders(bool force=false);
    virtual int isServer()const{return 1;}
    bool offline();
    virtual QMap<int,QString> serverMenu();

protected:
    virtual void createNewFolder();
    IMAPaccount *account;
    AbstractMail *wrapper;
    QStringList currentFolders;
};

class IMAPfolderItem : public AccountViewItem
{

public:
    IMAPfolderItem( const Opie::Core::OSmartPointer<Folder>&folder, IMAPviewItem *parent , QListViewItem*after  );
    IMAPfolderItem( const Opie::Core::OSmartPointer<Folder>&folder, IMAPfolderItem *parent , QListViewItem*after, IMAPviewItem *master  );
    virtual ~IMAPfolderItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&);
    virtual RECBODYP fetchBody(const Opie::Core::OSmartPointer<RecMail>&);
    virtual bool contextMenuSelected(int);
    virtual const QString& Delemiter()const;
    virtual int isServer()const{return 2;}
    virtual QMap<int,QString> folderMenu();
protected:
    virtual void createNewFolder();
    virtual bool deleteFolder();
    virtual void downloadMails();
    IMAPviewItem *imap;
};

class MHviewItem : public AccountViewItem
{
    friend class MHfolderItem;

public:
    MHviewItem( const QString&aMboxPath, AccountView *parent );
    virtual ~MHviewItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&target );
    virtual RECBODYP fetchBody( const Opie::Core::OSmartPointer<RecMail> &mail );
    AbstractMail *getWrapper();
    virtual bool contextMenuSelected(int);
    QStringList subFolders();
    virtual void refresh(bool force=false);
    virtual int isServer()const{return 3;}
    virtual QMap<int,QString> serverMenu();
    virtual QMap<int,QString> folderMenu();

protected:
    void downloadMails();
    virtual void createFolder();
    QString m_Path;
    AbstractMail *wrapper;
    QStringList currentFolders;
};

class MHfolderItem : public AccountViewItem
{

public:
    MHfolderItem( const Opie::Core::OSmartPointer<Folder>&folder, MHviewItem *parent , QListViewItem*after  );
    MHfolderItem( const Opie::Core::OSmartPointer<Folder>&folder, MHfolderItem *parent, QListViewItem*after, MHviewItem*master);
    virtual ~MHfolderItem();
    virtual void refresh(QValueList<Opie::Core::OSmartPointer<RecMail> >&);
    virtual RECBODYP fetchBody(const Opie::Core::OSmartPointer<RecMail>&);
    virtual bool contextMenuSelected(int);
    virtual const Opie::Core::OSmartPointer<Folder>&getFolder()const;
    virtual bool isDraftfolder();
    virtual int isServer()const{return 2;}
    virtual QMap<int,QString> folderMenu();

protected:
    void downloadMails();
    virtual void createFolder();
    virtual bool deleteFolder();
    void initName();
    MHviewItem *mbox;
};

#endif
