#ifndef ACCOUNTVIEW_H
#define ACCOUNTVIEW_H

#include <qlistview.h>
#include <qlist.h>
#include <opie2/osmartpointer.h>
#include <libmailwrapper/mailtypes.h>

class Selectstore;
class Folder;
class AbstractMail;
class Account;
class IMAPviewItem;
class MHviewItem;

class AccountView : public QListView
{
    Q_OBJECT

public:
    AccountView( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    virtual ~AccountView();
    virtual void populate( QList<Account> list );
    virtual RecBody fetchBody(const Opie::OSmartPointer<RecMail>&aMail);
    virtual void downloadMails(const Opie::OSmartPointer<Folder>&fromFolder,AbstractMail*fromWrapper);
    virtual bool currentisDraft();

public slots:
    virtual void refreshAll();
    virtual void refresh(QListViewItem *item);
    virtual void refreshCurrent();
    virtual void slotHold(int, QListViewItem *,const QPoint&,int);
    virtual void slotContextMenu(int id);
    void setupFolderselect(Selectstore*sels);

signals:
    void refreshMailview(const QValueList<RecMailP>& );

protected:
    QListViewItem* m_currentItem;
    QValueList<IMAPviewItem*> imapAccounts;
    QValueList<MHviewItem*> mhAccounts;
};

#endif
