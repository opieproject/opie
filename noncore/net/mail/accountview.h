#ifndef ACCOUNTVIEW_H
#define ACCOUNTVIEW_H

#include <libmailwrapper/mailtypes.h>
#include <opie2/osmartpointer.h>
#include <qlistview.h>
#include <qlist.h>
#include <qmap.h>

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
    virtual RecBodyP fetchBody(const Opie::Core::OSmartPointer<RecMail>&aMail);
    virtual void downloadMails(const Opie::Core::OSmartPointer<Folder>&fromFolder,AbstractMail*fromWrapper);
    virtual bool currentisDraft();
    QMap<int,QString> currentServerMenu()const;
    QMap<int,QString> currentFolderMenu()const;

public slots:
    virtual void refreshAll();
    virtual void refresh(QListViewItem *item);
    virtual void refreshCurrent();
    virtual void slotContextMenu(int id);
    virtual void readSettings();

    void setupFolderselect(Selectstore*sels);

signals:
    void refreshMailview(const QValueList<RecMailP>& );
    void serverSelected(int);
    void refreshMenues(int);

protected:
    QListViewItem* m_currentItem;
    QValueList<IMAPviewItem*> imapAccounts;
    QValueList<MHviewItem*> mhAccounts;
    bool m_rightPressed:1;
    bool m_clickopens:1;

protected slots:
    virtual void slotRightButton(int, QListViewItem *,const QPoint&,int);
    virtual void slotLeftButton(int, QListViewItem *,const QPoint&,int);
    virtual void slotMouseButton(int, QListViewItem *,const QPoint&,int);
    virtual void slotMouseClicked(QListViewItem*);
    virtual void slotSelectionChanged(QListViewItem*);
};

#endif
