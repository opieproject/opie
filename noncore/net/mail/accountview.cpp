#include "accountview.h"
#include "imapwrapper.h"
#include "mailtypes.h"
#include "defines.h"

IMAPviewItem::IMAPviewItem( IMAPaccount *a, QListView *parent )
    : AccountViewItem( parent )
{
    account = a;
    wrapper = new IMAPwrapper( account );
    setPixmap( 0, PIXMAP_IMAPFOLDER );
    setText( 0, account->getAccountName() );
    setOpen( true );
}

IMAPviewItem::~IMAPviewItem()
{
    delete wrapper;
}

IMAPwrapper *IMAPviewItem::getWrapper()
{
    return wrapper;
}

void IMAPviewItem::refresh(QList<RecMail>&)
{
    QList<IMAPFolder> *folders = wrapper->listFolders();
    
    QListViewItem *child = firstChild();
    while ( child ) {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    
    IMAPFolder *it;
    for ( it = folders->first(); it; it = folders->next() ) {
        (void) new IMAPfolderItem( it, this );
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

IMAPfolderItem::IMAPfolderItem( IMAPFolder *folderInit, IMAPviewItem *parent )
    : AccountViewItem( parent )
{
    folder = folderInit;
    imap = parent;
    setPixmap( 0, PIXMAP_IMAPFOLDER );
    setText( 0, folder->getDisplayName() );
}

void IMAPfolderItem::refresh(QList<RecMail>&target) 
{
    imap->getWrapper()->listMessages( folder->getName(),target );
}

RecBody IMAPfolderItem::fetchBody(const RecMail&aMail)
{
    return imap->getWrapper()->fetchBody(aMail);
}

AccountView::AccountView( QWidget *parent, const char *name, WFlags flags )
    : QListView( parent, name, flags )
{
    connect( this, SIGNAL( clicked( QListViewItem * ) ),
             SLOT( refresh( QListViewItem * ) ) );
}

void AccountView::populate( QList<Account> list )
{
    clear();
    
    Account *it;
    for ( it = list.first(); it; it = list.next() ) {
        if ( it->getType().compare( "IMAP" ) == 0 ) {
            IMAPaccount *imap = static_cast<IMAPaccount *>(it);
            qDebug( "added IMAP " + imap->getAccountName() );
            (void) new IMAPviewItem( imap, this );
        }
    }
}

void AccountView::refresh(QListViewItem *item) {
    qDebug("AccountView refresh...");
    if ( item ) { 
        QList<RecMail> headerlist;
        headerlist.setAutoDelete(true);
        AccountViewItem *view = static_cast<AccountViewItem *>(item);
        view->refresh(headerlist);
        emit refreshMailview(&headerlist);
    }
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
