#include "accountview.h"
#include "mailtypes.h"
#include "defines.h"


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
}

POP3viewItem::~POP3viewItem()
{
    delete wrapper;
}

void POP3viewItem::refresh( QList<RecMail> &target )
{
    qDebug( "POP3: refresh" );
    wrapper->listMessages("INBOX", target );
}


RecBody POP3viewItem::fetchBody( const RecMail &mail )
{
    qDebug( "POP3 fetchBody" );
    return wrapper->fetchBody( mail );
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

void IMAPviewItem::refresh(QList<RecMail>&)
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
        item = new IMAPfolderItem( it, this , item );
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
    : AccountViewItem( parent,after )
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

/**
 * Generic stuff
 */

AccountView::AccountView( QWidget *parent, const char *name, WFlags flags )
    : QListView( parent, name, flags )
{
    connect( this, SIGNAL( clicked( QListViewItem * ) ),
             SLOT( refresh( QListViewItem * ) ) );
    setSorting(-1);
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
