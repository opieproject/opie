#include <stdlib.h>
#include "accountview.h"
#include "mailtypes.h"
#include "defines.h"
#include "newmaildir.h"
#include <qmessagebox.h>
#include <qpopupmenu.h>

/**
 * POP3 Account stuff
 */
POP3viewItem::POP3viewItem( POP3account *a, QListView *parent )
    : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    setPixmap( 0, PIXMAP_POP3FOLDER );
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
    QList<Folder> *folders = wrapper->listFolders();
    QListViewItem *child = firstChild();
    while ( child ) {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    Folder *it;
    QListViewItem*item = 0;
    for ( it = folders->first(); it; it = folders->next() ) {
        item = new POP3folderItem( it, this , item );
        item->setSelectable(it->may_select());
    }
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    folders->setAutoDelete(false);
    delete folders;
}

RecBody POP3viewItem::fetchBody( const RecMail &mail )
{
    qDebug( "POP3 fetchBody" );
    return wrapper->fetchBody( mail );
}

POP3folderItem::~POP3folderItem()
{
    delete folder;
}

POP3folderItem::POP3folderItem( Folder *folderInit, POP3viewItem *parent , QListViewItem*after  )
    : AccountViewItem( parent,after )
{
    folder = folderInit;
    pop3 = parent;
    if (folder->getDisplayName().lower()!="inbox") {
        setPixmap( 0, PIXMAP_POP3FOLDER );
    } else {
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

/**
 * IMAP Account stuff
 */
IMAPviewItem::IMAPviewItem( IMAPaccount *a, QListView *parent )
    : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    setPixmap( 0, PIXMAP_IMAPFOLDER );
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

IMAPfolderItem*IMAPviewItem::findSubItem(const QString&path,IMAPfolderItem*start)
{
    IMAPfolderItem*pitem,*sitem;
    if (!start) pitem = (IMAPfolderItem*)firstChild();
    else pitem = (IMAPfolderItem*)start->firstChild();
    while (pitem) {
        if (pitem->matchName(path)) {
            break;
        }
        if (pitem->childCount()>0) {
            sitem = findSubItem(path,pitem);
            if (sitem) {
                pitem = sitem;
                break;
            }
        }
        pitem=(IMAPfolderItem*)pitem->nextSibling();
    }
    return pitem;
}

void IMAPviewItem::refresh(QList<RecMail>&)
{
    refreshFolders(false);
}

void IMAPviewItem::refreshFolders(bool force)
{
    if (childCount()>0 && force==false) return;
    QList<Folder> *folders = wrapper->listFolders();

    QListViewItem *child = firstChild();    
    while ( child ) {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }

    Folder *it;
    QListViewItem*item = 0;
    QListViewItem*titem = 0;
    QString fname,del,search;
    int pos;
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    folders->setAutoDelete(false);
    
    for ( it = folders->first(); it; it = folders->next() ) {
        if (it->getDisplayName().lower()=="inbox") {
            item = new IMAPfolderItem( it, this , item );
            folders->remove(it);
            qDebug("inbox found");
            break;
        }
    }
    for ( it = folders->first(); it; it = folders->next() ) {
        fname = it->getDisplayName();
        pos = fname.findRev(it->Separator());
        if (pos != -1) {
            fname = fname.left(pos);
        }
        IMAPfolderItem*pitem = findSubItem(fname);
        if (pitem) {
            titem = item;
            item = new IMAPfolderItem(it,pitem,pitem->firstChild(),this);
            /* setup the short name */
            item->setText(0,it->getDisplayName().right(it->getDisplayName().length()-pos-1));
            item = titem;
        } else {
            item = new IMAPfolderItem( it, this , item );
        }
    }
    delete folders;
}

QPopupMenu * IMAPviewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m) {
        m->insertItem(QObject::tr("Refresh folder list","IMAPviewItem"),0);
        m->insertItem(QObject::tr("Create new folder","IMAPviewItem"),1);
        m->insertSeparator();
        m->insertItem(QObject::tr("Disconnect","IMAPviewItem"),2);
    }
    return m;
}

void IMAPviewItem::createNewFolder()
{
    Newmdirdlg ndirdlg;
    ndirdlg.showMaximized();
    if (ndirdlg.exec()) {
        QString ndir = ndirdlg.Newdir();
        bool makesubs = ndirdlg.subpossible();
        QString delemiter = "/";
        IMAPfolderItem*item = (IMAPfolderItem*)firstChild();
        if (item) {
            delemiter = item->Delemiter();
        }
        if (wrapper->createMbox(ndir,0,delemiter,makesubs)) {
            refreshFolders(true);
        }
    }
}

void IMAPviewItem::contextMenuSelected(int id)
{
    qDebug("Id selected: %i",id);
    switch (id) {
    case 0:
        refreshFolders(true);
        break;
    case 1:
        createNewFolder();
        break;
    default:
        break;
    }
}

RecBody IMAPviewItem::fetchBody(const RecMail&)
{
    return RecBody();
}

IMAPfolderItem::~IMAPfolderItem()
{
    delete folder;
}

IMAPfolderItem::IMAPfolderItem( Folder *folderInit, IMAPviewItem *parent , QListViewItem*after )
    : AccountViewItem( parent , after )
{
    folder = folderInit;
    imap = parent;
    if (folder->getDisplayName().lower()!="inbox") {
        setPixmap( 0, PIXMAP_IMAPFOLDER );
    } else {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

IMAPfolderItem::IMAPfolderItem( Folder *folderInit, IMAPfolderItem *parent , QListViewItem*after, IMAPviewItem *master  )
    : AccountViewItem( parent,after )
{
    folder = folderInit;
    imap = master;
    if (folder->getDisplayName().lower()!="inbox") {
        setPixmap( 0, PIXMAP_IMAPFOLDER );
    } else {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

const QString& IMAPfolderItem::Delemiter()const
{
    return folder->Separator();
}

bool IMAPfolderItem::matchName(const QString&name)const
{
    return folder->getDisplayName()==name;
}

void IMAPfolderItem::refresh(QList<RecMail>&target)
{
    if (folder->may_select()) {
        imap->getWrapper()->listMessages( folder->getName(),target );
    } else {
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
    if (m) {
        if (folder->may_select()) {
            m->insertItem(QObject::tr("Refresh header list","IMAPfolderItem"),0);
            m->insertItem(QObject::tr("Delete all mails","IMAPfolderItem"),1);
        }
        if (folder->no_inferior()==false) {
            m->insertItem(QObject::tr("Create new subfolder","IMAPfolderItem"),2);
        }
        if (folder->getDisplayName().lower()!="inbox") {
            m->insertItem(QObject::tr("Delete folder","IMAPfolderItem"),3);
        }
    }
    return m;
}

void IMAPfolderItem::deleteAllMails()
{
    int yesno = QMessageBox::warning(0,QObject::tr("Delete all mails","IMAPfolderItem"),
        QObject::tr("<center>Realy delete all mails in box <br>%1</center>","IMAPfolderItem").arg(folder->getDisplayName()),
        QObject::tr("Yes","IMAPfolderItem"),
        QObject::tr("No","IMAPfolderItem"),QString::null,1,1);
    qDebug("Auswahl: %i",yesno);
    if (yesno == 0) {
        if (imap->getWrapper()->deleteAllMail(folder)) {
            AccountView * view = (AccountView*)listView();
            if (view) view->refreshCurrent();
        }
    }
}

void IMAPfolderItem::createNewFolder()
{
    Newmdirdlg ndirdlg;
    ndirdlg.showMaximized();
    if (ndirdlg.exec()) {
        QString ndir = ndirdlg.Newdir();
        bool makesubs = ndirdlg.subpossible();
        QString delemiter = Delemiter();
        if (imap->wrapper->createMbox(ndir,folder,delemiter,makesubs)) {
            imap->refreshFolders(true);
        }
    }
}

void IMAPfolderItem::deleteFolder()
{
    int yesno = QMessageBox::warning(0,QObject::tr("Delete folder","IMAPfolderItem"),
        QObject::tr("<center>Realy delete folder <br><b>%1</b><br>and all if it content?</center>","IMAPfolderItem").arg(folder->getDisplayName()),
        QObject::tr("Yes","IMAPfolderItem"),
        QObject::tr("No","IMAPfolderItem"),QString::null,1,1);
    qDebug("Auswahl: %i",yesno);
    if (yesno == 0) {
        if (imap->getWrapper()->deleteMbox(folder)) {
            /* be carefull - after that this object is destroyd so don't call 
             * any member of it after that call!!*/
            imap->refreshFolders(true);
        }
    }
}

void IMAPfolderItem::contextMenuSelected(int id)
{
    qDebug("Selected id: %i",id);
    AccountView * view = (AccountView*)listView();
    switch(id) {
    case 0:
        view->refreshCurrent();
        break;
    case 1:
        deleteAllMails();
        break;
    case 2:
        createNewFolder();
        break;
    case 3:
        deleteFolder();
        break;
    default:
        break;
    }
}

/**
 * Generic stuff
 */

AccountView::AccountView( QWidget *parent, const char *name, WFlags flags )
    : QListView( parent, name, flags )
{
    connect( this, SIGNAL( selectionChanged( QListViewItem * ) ),
            SLOT( refresh( QListViewItem * ) ) );
    connect( this, SIGNAL( mouseButtonPressed(int, QListViewItem *,const QPoint&,int  ) ),this,
            SLOT( slotHold( int, QListViewItem *,const QPoint&,int  ) ) );         

    setSorting(0);
}

void AccountView::slotContextMenu(int id)
{
    AccountViewItem *view = static_cast<AccountViewItem *>(currentItem());
    if (!view) return;
    view->contextMenuSelected(id);
}

void AccountView::slotHold(int button, QListViewItem * item,const QPoint&,int)
{
    if (button==1) {return;}
    if (!item) return;
    AccountViewItem *view = static_cast<AccountViewItem *>(item);
    QPopupMenu*m = view->getContextMenu();
    if (!m) return;
    connect(m,SIGNAL(activated(int)),this,SLOT(slotContextMenu(int)));
    m->setFocus();
    m->exec( QPoint( QCursor::pos().x(), QCursor::pos().y()) );
    delete m;
}

void AccountView::populate( QList<Account> list )
{
    clear();

    QString localfolders = (QString) getenv( "HOME" ) + QString("/Applications/opiemail/localmail/");
    (void) new MBOXviewItem(localfolders,this);

    Account *it;
    for ( it = list.first(); it; it = list.next() ) {
        if ( it->getType().compare( "IMAP" ) == 0 ) {
            IMAPaccount *imap = static_cast<IMAPaccount *>(it);
            qDebug( "added IMAP " + imap->getAccountName() );
            (void) new IMAPviewItem( imap, this );
        } else if ( it->getType().compare( "POP3" ) == 0 ) {
            POP3account *pop3 = static_cast<POP3account *>(it);
            qDebug( "added POP3 " + pop3->getAccountName() );
            (void) new POP3viewItem( pop3, this );
        }
    }
}

void AccountView::refresh(QListViewItem *item) {

    qDebug("AccountView refresh...");
    if ( item ) {
        m_currentItem = item;
        QList<RecMail> headerlist;
        headerlist.setAutoDelete(true);
        AccountViewItem *view = static_cast<AccountViewItem *>(item);
        view->refresh(headerlist);
        emit refreshMailview(&headerlist);
    }
}

void AccountView::refreshCurrent()
{
    m_currentItem = currentItem();
   if ( !m_currentItem ) return;
   QList<RecMail> headerlist;
   headerlist.setAutoDelete(true);
   AccountViewItem *view = static_cast<AccountViewItem *>(m_currentItem);
   view->refresh(headerlist);
   emit refreshMailview(&headerlist);
}

void AccountView::refreshAll()
{

}

RecBody AccountView::fetchBody(const RecMail&aMail)
{
    QListViewItem*item = selectedItem ();
    if (!item) return RecBody();
    AccountViewItem *view = static_cast<AccountViewItem *>(item);
    return view->fetchBody(aMail);
}

/**
 * MBOX Account stuff
 */

MBOXviewItem::MBOXviewItem( const QString&aPath, QListView *parent )
    : AccountViewItem( parent )
{
    m_Path = aPath;
    wrapper = AbstractMail::getWrapper( m_Path );
    setPixmap( 0, PIXMAP_LOCALFOLDER );
    setText( 0, " Local Folders" );
    setOpen( true );
}

MBOXviewItem::~MBOXviewItem()
{
    delete wrapper;
}

AbstractMail *MBOXviewItem::getWrapper()
{
    return wrapper;
}

void MBOXviewItem::refresh( QList<RecMail> & )
{
    QList<Folder> *folders = wrapper->listFolders();
    QListViewItem *child = firstChild();
    while ( child ) {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    Folder *it;
    QListViewItem*item = 0;
    for ( it = folders->first(); it; it = folders->next() ) {
        item = new MBOXfolderItem( it, this , item );
        item->setSelectable(it->may_select());
    }
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    folders->setAutoDelete(false);
    delete folders;
}

RecBody MBOXviewItem::fetchBody( const RecMail &mail )
{
    qDebug( "POP3 fetchBody" );
    return wrapper->fetchBody( mail );
}

MBOXfolderItem::~MBOXfolderItem()
{
    delete folder;
}

MBOXfolderItem::MBOXfolderItem( Folder *folderInit, MBOXviewItem *parent , QListViewItem*after  )
    : AccountViewItem( parent,after )
{
    folder = folderInit;
    mbox = parent;
    if (folder->getDisplayName().lower() == "outgoing") {
        setPixmap( 0, PIXMAP_OUTBOXFOLDER );
    } else if (folder->getDisplayName().lower() == "inbox") {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    } else {
        setPixmap( 0, PIXMAP_MBOXFOLDER );	
	}
    setText( 0, folder->getDisplayName() );
}

void MBOXfolderItem::refresh(QList<RecMail>&target)
{
    if (folder->may_select())
        mbox->getWrapper()->listMessages( folder->getName(),target );
}

RecBody MBOXfolderItem::fetchBody(const RecMail&aMail)
{
    return mbox->getWrapper()->fetchBody(aMail);
}
