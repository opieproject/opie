
#include "accountitem.h"
#include "accountview.h"
#include "newmaildir.h"
#include "nntpgroupsdlg.h"
#include "defines.h"

/* OPIE */
#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/abstractmail.h>
#include <libmailwrapper/mailwrapper.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qpopupmenu.h>
#include <qmessagebox.h>

#define SETPIX(x) if (!account->getOffline()) {setPixmap( 0,x);} else {setPixmap( 0, PIXMAP_OFFLINE );}
/**
 * POP3 Account stuff
 */
POP3viewItem::POP3viewItem( POP3account *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    SETPIX(PIXMAP_POP3FOLDER);
#if 0
    if (!account->getOffline())
    {
        setPixmap( 0,  );
    }
    else
    {
        setPixmap( 0, PIXMAP_OFFLINE );
    }
#endif
    setText( 0, account->getAccountName() );
    setOpen( true );
}

POP3viewItem::~POP3viewItem()
{
    delete wrapper;
}

AbstractMail *POP3viewItem::getWrapper()
{
    return wrapper;
}

void POP3viewItem::refresh( QList<RecMail> & )
{
    refresh();
}

void POP3viewItem::refresh()
{
    if (account->getOffline()) return;
    QValueList<FolderP> *folders = wrapper->listFolders();
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    QValueList<FolderP>::ConstIterator it;
    QListViewItem*item = 0;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        item = new POP3folderItem( (*it), this , item );
        item->setSelectable( (*it)->may_select());
    }
    delete folders;
}

RecBody POP3viewItem::fetchBody( const RecMail &mail )
{
    qDebug( "POP3 fetchBody" );
    return wrapper->fetchBody( mail );
}

QPopupMenu * POP3viewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (!account->getOffline())
        {
            m->insertItem(QObject::tr("Disconnect",contextName),0);
            m->insertItem(QObject::tr("Set offline",contextName),1);
        }
        else
        {
            m->insertItem(QObject::tr("Set online",contextName),1);
        }
    }
    return m;
}

void POP3viewItem::disconnect()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    wrapper->logout();
}

void POP3viewItem::setOnOffline()
{
    if (!account->getOffline())
    {
        disconnect();
    }
    account->setOffline(!account->getOffline());
    account->save();
    SETPIX(PIXMAP_POP3FOLDER);
    refresh();
}

void POP3viewItem::contextMenuSelected(int which)
{
    switch (which)
    {
    case 0:
        disconnect();
        break;
    case 1:
        setOnOffline();
        break;
    }
}

POP3folderItem::~POP3folderItem()
{}

POP3folderItem::POP3folderItem( const FolderP&folderInit, POP3viewItem *parent , QListViewItem*after  )
        : AccountViewItem(folderInit,parent,after )
{
    pop3 = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_POP3FOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

void POP3folderItem::refresh(QList<RecMail>&target)
{
    if (folder->may_select())
        pop3->getWrapper()->listMessages( folder->getName(),target );
}

RecBody POP3folderItem::fetchBody(const RecMail&aMail)
{
    return pop3->getWrapper()->fetchBody(aMail);
}

QPopupMenu * POP3folderItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        m->insertItem(QObject::tr("Refresh header list",contextName),0);
        m->insertItem(QObject::tr("Delete all mails",contextName),1);
        m->insertItem(QObject::tr("Move/Copie all mails",contextName),2);
    }
    return m;
}

void POP3folderItem::downloadMails()
{
    AccountView*bl = pop3->accountView();
    if (!bl) return;
    bl->downloadMails(folder,pop3->getWrapper());
}

void POP3folderItem::contextMenuSelected(int which)
{
    AccountView * view = (AccountView*)listView();
    switch (which)
    {
    case 0:
        /* must be 'cause pop3 lists are cached */
        pop3->getWrapper()->logout();
        view->refreshCurrent();
        break;
    case 1:
        deleteAllMail(pop3->getWrapper(),folder);
        break;
    case 2:
        downloadMails();
        break;
    default:
        break;
    }
}

/**
 * NNTP Account stuff
 */
NNTPviewItem::NNTPviewItem( NNTPaccount *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    //FIXME
    SETPIX(PIXMAP_POP3FOLDER);
#if 0
    if (!account->getOffline())
    {
        setPixmap( 0,  );
    }
    else
    {
        setPixmap( 0, PIXMAP_OFFLINE );
    }
#endif
    setText( 0, account->getAccountName() );
    setOpen( true );
}

NNTPviewItem::~NNTPviewItem()
{
    delete wrapper;
}

AbstractMail *NNTPviewItem::getWrapper()
{
    return wrapper;
}

void NNTPviewItem::refresh( QList<RecMail> & )
{
    refresh();
}

void NNTPviewItem::refresh()
{
    if (account->getOffline()) return;
    QValueList<FolderP> *folders = wrapper->listFolders();

    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    QValueList<FolderP>::ConstIterator it;
    QListViewItem*item = 0;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        item = new NNTPfolderItem( (*it), this , item );
        item->setSelectable( (*it)->may_select());
    }
    delete folders;
}

RecBody NNTPviewItem::fetchBody( const RecMail &mail )
{
    qDebug( "NNTP fetchBody" );
    return wrapper->fetchBody( mail );
}

QPopupMenu * NNTPviewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (!account->getOffline())
        {
            m->insertItem(QObject::tr("Disconnect",contextName),0);
            m->insertItem(QObject::tr("Set offline",contextName),1);
            m->insertItem(QObject::tr("(Un-)Subscribe groups",contextName),2);
        }
        else
        {
            m->insertItem(QObject::tr("Set online",contextName),1);
        }
    }
    return m;
}

void NNTPviewItem::subscribeGroups()
{
    NNTPGroupsDlg dlg(account);
    if (QPEApplication::execDialog(&dlg)== QDialog::Accepted ){
        refresh();
    }
}

void NNTPviewItem::disconnect()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    wrapper->logout();
}

void NNTPviewItem::setOnOffline()
{
    if (!account->getOffline())
    {
        disconnect();
    }
    account->setOffline(!account->getOffline());
    account->save();
   //FIXME
    SETPIX(PIXMAP_POP3FOLDER);
    refresh();
}

void NNTPviewItem::contextMenuSelected(int which)
{
    switch (which)
    {
    case 0:
        disconnect();
        break;
    case 1:
        setOnOffline();
        break;
    case 2:
        subscribeGroups();
        break;
    }
}

NNTPfolderItem::~NNTPfolderItem()
{}

NNTPfolderItem::NNTPfolderItem( const FolderP &folderInit, NNTPviewItem *parent , QListViewItem*after  )
        : AccountViewItem( folderInit, parent,after )
{
    nntp = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_POP3FOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

void NNTPfolderItem::refresh(QList<RecMail>&target)
{
    if (folder->may_select())
        nntp->getWrapper()->listMessages( folder->getName(),target );
}

RecBody NNTPfolderItem::fetchBody(const RecMail&aMail)
{
    return nntp->getWrapper()->fetchBody(aMail);
}

QPopupMenu * NNTPfolderItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        m->insertItem(QObject::tr("Refresh header list",contextName),0);
        m->insertItem(QObject::tr("Copy all postings",contextName),1);
    }
    return m;
}

void NNTPfolderItem::downloadMails()
{
    AccountView*bl = nntp->accountView();
    if (!bl) return;
    bl->downloadMails(folder,nntp->getWrapper());
}

void NNTPfolderItem::contextMenuSelected(int which)
{
    AccountView * view = (AccountView*)listView();
    switch (which)
    {
    case 0:
        /* must be 'cause pop3 lists are cached */
        nntp->getWrapper()->logout();
        view->refreshCurrent();
        break;
    case 1:
        downloadMails();
        break;
    default:
        break;
    }
}

/**
 * IMAP Account stuff
 */
IMAPviewItem::IMAPviewItem( IMAPaccount *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    SETPIX(PIXMAP_IMAPFOLDER);
    setText( 0, account->getAccountName() );
    setOpen( true );
}

IMAPviewItem::~IMAPviewItem()
{
    delete wrapper;
}

AbstractMail *IMAPviewItem::getWrapper()
{
    return wrapper;
}

void IMAPviewItem::refresh(QList<RecMail>&)
{
    refreshFolders(false);
}

const QStringList&IMAPviewItem::subFolders()
{
    return currentFolders;
}

void IMAPviewItem::refreshFolders(bool force)
{
    if (childCount()>0 && force==false) return;
    if (account->getOffline()) return;

    removeChilds();
    currentFolders.clear();
    QValueList<FolderP> * folders = wrapper->listFolders();

    QValueList<FolderP>::Iterator it;
    QListViewItem*item = 0;
    QListViewItem*titem = 0;
    QString fname,del,search;
    int pos;

    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        if ((*it)->getDisplayName().lower()=="inbox")
        {
            item = new IMAPfolderItem( (*it), this , item );
            folders->remove(it);
            qDebug("inbox found");
            break;
        }
    }
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        fname = (*it)->getDisplayName();
        currentFolders.append((*it)->getName());
        pos = fname.findRev((*it)->Separator());
        if (pos != -1)
        {
            fname = fname.left(pos);
        }
        IMAPfolderItem*pitem = (IMAPfolderItem*)findSubItem(fname);
        if (pitem)
        {
            titem = item;
            item = new IMAPfolderItem( (*it),pitem,pitem->firstChild(),this);
            /* setup the short name */
            item->setText(0,(*it)->getDisplayName().right((*it)->getDisplayName().length()-pos-1));
            item = titem;
        }
        else
        {
            item = new IMAPfolderItem( (*it), this , item );
        }
    }
    delete folders;
}

QPopupMenu * IMAPviewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (!account->getOffline())
        {
            m->insertItem(QObject::tr("Refresh folder list",contextName),0);
            m->insertItem(QObject::tr("Create new folder",contextName),1);
            m->insertSeparator();
            m->insertItem(QObject::tr("Disconnect",contextName),2);
            m->insertItem(QObject::tr("Set offline",contextName),3);
        }
        else
        {
            m->insertItem(QObject::tr("Set online",contextName),3);
        }
    }
    return m;
}

void IMAPviewItem::createNewFolder()
{
    Newmdirdlg ndirdlg;
    if ( QPEApplication::execDialog( &ndirdlg ))
    {
        QString ndir = ndirdlg.Newdir();
        bool makesubs = ndirdlg.subpossible();
        QString delemiter = "/";
        IMAPfolderItem*item = (IMAPfolderItem*)firstChild();
        if (item)
        {
            delemiter = item->Delemiter();
        }
        if (wrapper->createMbox(ndir,0,delemiter,makesubs))
        {
            refreshFolders(true);
        }
    }
}

void IMAPviewItem::contextMenuSelected(int id)
{
    qDebug("Id selected: %i",id);
    switch (id)
    {
    case 0:
        refreshFolders(true);
        break;
    case 1:
        createNewFolder();
        break;
    case 2:
        removeChilds();
        wrapper->logout();
        break;
    case 3:
        if (account->getOffline()==false)
        {
            removeChilds();
            wrapper->logout();
        }
        account->setOffline(!account->getOffline());
        account->save();
        SETPIX(PIXMAP_IMAPFOLDER);
        refreshFolders(false);
        break;
    default:
        break;
    }
}

RecBody IMAPviewItem::fetchBody(const RecMail&)
{
    return RecBody();
}

bool IMAPviewItem::offline()
{
    return account->getOffline();
}

IMAPfolderItem::IMAPfolderItem( const FolderP& folderInit, IMAPviewItem *parent , QListViewItem*after )
        : AccountViewItem( folderInit, parent , after )
{
    imap = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_IMAPFOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

IMAPfolderItem::IMAPfolderItem(const FolderP &folderInit, IMAPfolderItem *parent , QListViewItem*after, IMAPviewItem *master  )
        : AccountViewItem(folderInit, parent,after )
{
    imap = master;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_IMAPFOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

IMAPfolderItem::~IMAPfolderItem()
{}

const QString& IMAPfolderItem::Delemiter()const
{
    return folder->Separator();
}

void IMAPfolderItem::refresh(QList<RecMail>&target)
{
    if (folder->may_select())
    {
        imap->getWrapper()->listMessages( folder->getName(),target );
    }
    else
    {
        target.clear();
    }
}

RecBody IMAPfolderItem::fetchBody(const RecMail&aMail)
{
    return imap->getWrapper()->fetchBody(aMail);
}

QPopupMenu * IMAPfolderItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (folder->may_select())
        {
            m->insertItem(QObject::tr("Refresh header list",contextName),0);
            m->insertItem(QObject::tr("Move/Copie all mails",contextName),4);
            m->insertItem(QObject::tr("Delete all mails",contextName),1);
        }
        if (folder->no_inferior()==false)
        {
            m->insertItem(QObject::tr("Create new subfolder",contextName),2);
        }
        if (folder->getDisplayName().lower()!="inbox")
        {
            m->insertItem(QObject::tr("Delete folder",contextName),3);
        }
    }
    return m;
}

void IMAPfolderItem::createNewFolder()
{
    Newmdirdlg ndirdlg;
    if ( QPEApplication::execDialog( &ndirdlg ) )
    {
        QString ndir = ndirdlg.Newdir();
        bool makesubs = ndirdlg.subpossible();
        QString delemiter = Delemiter();
        if (imap->wrapper->createMbox(ndir,folder,delemiter,makesubs))
        {
            imap->refreshFolders(true);
        }
    }
}

void IMAPfolderItem::deleteFolder()
{
    int yesno = QMessageBox::warning(0,QObject::tr("Delete folder",contextName),
                                     QObject::tr("<center>Realy delete folder <br><b>%1</b><br>and all if it content?</center>",contextName).arg(folder->getDisplayName()),
                                     QObject::tr("Yes",contextName),
                                     QObject::tr("No",contextName),QString::null,1,1);
    qDebug("Auswahl: %i",yesno);
    if (yesno == 0)
    {
        if (imap->getWrapper()->deleteMbox(folder))
        {
            QListView*v=listView();
            IMAPviewItem * box = imap;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            imap->refreshFolders(true);
            if (v)
            {
                v->setSelected(box,true);
            }
        }
    }
}

void IMAPfolderItem::downloadMails()
{
    AccountView*bl = imap->accountView();
    if (!bl) return;
    bl->downloadMails(folder,imap->getWrapper());
}

void IMAPfolderItem::contextMenuSelected(int id)
{
    qDebug("Selected id: %i",id);
    AccountView * view = (AccountView*)listView();
    switch(id)
    {
    case 0:
        view->refreshCurrent();
        break;
    case 1:
        deleteAllMail(imap->getWrapper(),folder);
        break;
    case 2:
        createNewFolder();
        break;
    case 3:
        deleteFolder();
        break;
    case 4:
        downloadMails();
        break;
    default:
        break;
    }
}

/**
 * MH Account stuff
 */
/* MH is a little bit different - the top folder can contains messages other than in IMAP and
   POP3 and MBOX */
MHviewItem::MHviewItem( const QString&aPath, AccountView *parent )
        : AccountViewItem( parent )
{
    m_Path = aPath;
    /* be carefull - the space within settext is wanted - thats why the string twice */
    wrapper = AbstractMail::getWrapper( m_Path,"Local Folders");
    setPixmap( 0, PIXMAP_LOCALFOLDER );
    setText( 0, " Local Folders" );
    setOpen( true );
    folder = 0;
}

MHviewItem::~MHviewItem()
{
    delete wrapper;
}

AbstractMail *MHviewItem::getWrapper()
{
    return wrapper;
}

void MHviewItem::refresh( QList<RecMail> & target)
{
    refresh(false);
    getWrapper()->listMessages( "",target );
}

void MHviewItem::refresh(bool force)
{
    if (childCount()>0 && force==false) return;
    removeChilds();
    currentFolders.clear();
    QValueList<FolderP> *folders = wrapper->listFolders();
    QValueList<FolderP>::ConstIterator it;
    MHfolderItem*item = 0;
    MHfolderItem*pmaster = 0;
    QString fname = "";
    int pos;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        fname = (*it)->getDisplayName();
        /* this folder itself */
        if (fname=="/")
        {
            currentFolders.append(fname);
            folder = (*it);
            continue;
        }
        currentFolders.append(fname);
        pos = fname.findRev("/");
        if (pos > 0)
        {
            fname = fname.left(pos);
            pmaster = (MHfolderItem*)findSubItem(fname);
        }
        else
        {
            pmaster = 0;
        }
        if (pmaster)
        {
            item = new MHfolderItem( (*it), pmaster, item, this );
        }
        else
        {
            item = new MHfolderItem( (*it), this , item );
        }
        item->setSelectable((*it)->may_select());
    }
    delete folders;
}

RecBody MHviewItem::fetchBody( const RecMail &mail )
{
    qDebug( "MH fetchBody" );
    return wrapper->fetchBody( mail );
}

QPopupMenu * MHviewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        m->insertItem(QObject::tr("Refresh folder list",contextName),0);
        m->insertItem(QObject::tr("Create new folder",contextName),1);
        m->insertItem(QObject::tr("Delete all mails",contextName),2);
        m->insertItem(QObject::tr("Move/Copie all mails",contextName),3);
    }
    return m;
}

void MHviewItem::createFolder()
{
    Newmdirdlg ndirdlg(0,0,true);
    if ( QPEApplication::execDialog( &ndirdlg ) )
    {
        QString ndir = ndirdlg.Newdir();
        if (wrapper->createMbox(ndir))
        {
            refresh(true);
        }
    }
}

void MHviewItem::downloadMails()
{
    AccountView*bl = accountView();
    if (!bl) return;
    bl->downloadMails(folder,getWrapper());
}

QStringList MHviewItem::subFolders()
{
    return currentFolders;
}

void MHviewItem::contextMenuSelected(int which)
{
    switch (which)
    {
    case 0:
        refresh(true);
        break;
    case 1:
        createFolder();
        break;
    case 2:
        deleteAllMail(getWrapper(),folder);
        break;
    case 3:
        downloadMails();
        break;
    default:
        break;
    }
}

MHfolderItem::~MHfolderItem()
{}

MHfolderItem::MHfolderItem( const FolderP &folderInit, MHviewItem *parent , QListViewItem*after  )
        : AccountViewItem(folderInit, parent,after )
{
    mbox = parent;
    initName();
}

MHfolderItem::MHfolderItem(const FolderP& folderInit, MHfolderItem *parent, QListViewItem*after, MHviewItem*master)
        : AccountViewItem(folderInit, parent,after )
{
    folder = folderInit;
    mbox = master;
    initName();
}

void MHfolderItem::initName()
{
    QString bName = folder->getDisplayName();
    if (bName.startsWith("/")&&bName.length()>1)
    {
        bName.replace(0,1,"");
    }
    int pos = bName.findRev("/");
    if (pos > 0)
    {
        bName.replace(0,pos+1,"");
    }
    if (bName.lower() == "outgoing")
    {
        setPixmap( 0, PIXMAP_OUTBOXFOLDER );
    }
    else if (bName.lower() == "inbox")
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    } else if (bName.lower() == "drafts") {
        setPixmap(0, Resource::loadPixmap("inline/edit"));
    } else {
        setPixmap( 0, PIXMAP_MBOXFOLDER );
    }
    setText( 0, bName );
}

const FolderP&MHfolderItem::getFolder()const
{
    return folder;
}

void MHfolderItem::refresh(QList<RecMail>&target)
{
    if (folder->may_select())
        mbox->getWrapper()->listMessages( folder->getName(),target );
}

RecBody MHfolderItem::fetchBody(const RecMail&aMail)
{
    return mbox->getWrapper()->fetchBody(aMail);
}

void MHfolderItem::deleteFolder()
{
    int yesno = QMessageBox::warning(0,QObject::tr("Delete folder",contextName),
                                     QObject::tr("<center>Realy delete folder <br><b>%1</b><br>and all if it content?</center>",contextName).arg(folder->getDisplayName()),
                                     QObject::tr("Yes",contextName),
                                     QObject::tr("No",contextName),QString::null,1,1);
    qDebug("Auswahl: %i",yesno);
    if (yesno == 0)
    {
        if (mbox->getWrapper()->deleteMbox(folder))
        {
            QListView*v=listView();
            MHviewItem * box = mbox;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            mbox->refresh(true);
            if (v)
            {
                v->setSelected(box,true);
            }
        }
    }
}

QPopupMenu * MHfolderItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        m->insertItem(QObject::tr("Move/Copie all mails",contextName),2);
        m->insertItem(QObject::tr("Delete all mails",contextName),0);
        m->insertItem(QObject::tr("Create new subfolder",contextName),3);
        m->insertItem(QObject::tr("Delete folder",contextName),1);
    }
    return m;
}

void MHfolderItem::downloadMails()
{
    AccountView*bl = mbox->accountView();
    if (!bl) return;
    bl->downloadMails(folder,mbox->getWrapper());
}

void MHfolderItem::createFolder()
{
    Newmdirdlg ndirdlg(0,0,true);
    if ( QPEApplication::execDialog( &ndirdlg ) )
    {
        QString ndir = ndirdlg.Newdir();
        if (mbox->getWrapper()->createMbox(ndir,folder))
        {
            QListView*v=listView();
            MHviewItem * box = mbox;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            mbox->refresh(true);
            if (v)
            {
                v->setSelected(box,true);
            }
        }
    }
}

void MHfolderItem::contextMenuSelected(int which)
{
    switch(which)
    {
    case 0:
        deleteAllMail(mbox->getWrapper(),folder);
        break;
    case 1:
        deleteFolder();
        break;
    case 2:
        downloadMails();
        break;
    case 3:
        createFolder();
        break;
    default:
        break;
    }
}

bool MHfolderItem::isDraftfolder()
{
    if (folder && folder->getName()==AbstractMail::defaultLocalfolder()+"/"+AbstractMail::draftFolder()) return true;
    return false;
}

/**
 * Generic stuff
 */

const QString AccountViewItem::contextName="AccountViewItem";

AccountViewItem::AccountViewItem( AccountView *parent )
        : QListViewItem( parent )
{
    init();
    m_Backlink = parent;
}

AccountViewItem::AccountViewItem( QListViewItem *parent)
        : QListViewItem( parent),folder(0)
{
    init();
}

AccountViewItem::AccountViewItem( QListViewItem *parent , QListViewItem*after  )
        :QListViewItem( parent,after ),folder(0)
{
    init();
}

AccountViewItem::AccountViewItem( const Opie::osmart_pointer<Folder>&folderInit,QListViewItem *parent , QListViewItem*after  )
       :QListViewItem( parent,after ),folder(folderInit)
{
    init();
}

void AccountViewItem::init()
{
    m_Backlink = 0;
}

AccountViewItem::~AccountViewItem()
{
    folder = 0;
}

AccountView*AccountViewItem::accountView()
{
    return m_Backlink;
}

void AccountViewItem::deleteAllMail(AbstractMail*wrapper,const FolderP&folder)
{
    if (!wrapper) return;
    QString fname="";
    if (folder) fname = folder->getDisplayName();
    int yesno = QMessageBox::warning(0,QObject::tr("Delete all mails",contextName),
                                     QObject::tr("<center>Realy delete all mails in box <br>%1</center>",contextName).
                                     arg(fname),
                                     QObject::tr("Yes",contextName),
                                     QObject::tr("No",contextName),QString::null,1,1);
    qDebug("Auswahl: %i",yesno);
    if (yesno == 0)
    {
        if (wrapper->deleteAllMail(folder))
        {
            AccountView * view = (AccountView*)listView();
            if (view) view->refreshCurrent();
        }
    }
}

void AccountViewItem::removeChilds()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
}

bool AccountViewItem::matchName(const QString&name)const
{
    if (!folder) return false;
    return folder->getDisplayName()==name;
}


AccountViewItem*AccountViewItem::findSubItem(const QString&path,AccountViewItem*start)
{
    AccountViewItem*pitem,*sitem;
    if (!start) pitem = (AccountViewItem*)firstChild();
    else pitem = (AccountViewItem*)start->firstChild();
    while (pitem)
    {
        if (pitem->matchName(path))
        {
            break;
        }
        if (pitem->childCount()>0)
        {
            sitem = findSubItem(path,pitem);
            if (sitem)
            {
                pitem = sitem;
                break;
            }
        }
        pitem=(AccountViewItem*)pitem->nextSibling();
    }
    return pitem;
}

bool AccountViewItem::isDraftfolder()
{
    return false;
}
