
#include "accountview.h"
#include "accountitem.h"
#include "selectstore.h"

#include <libmailwrapper/settings.h>
#include <libmailwrapper/mailwrapper.h>
#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/abstractmail.h>

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qmessagebox.h>
#include <qpopupmenu.h>

using namespace Opie::Core;
AccountView::AccountView( QWidget *parent, const char *name, WFlags flags )
        : QListView( parent, name, flags )
{
    connect( this, SIGNAL( selectionChanged(QListViewItem*) ),
             SLOT( refresh(QListViewItem*) ) );
    connect( this, SIGNAL( mouseButtonPressed(int,QListViewItem*,const QPoint&,int) ),this,
             SLOT( slotHold(int,QListViewItem*,const QPoint&,int) ) );
    setSorting(0);
}

AccountView::~AccountView()
{
    imapAccounts.clear();
    mhAccounts.clear();
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

    imapAccounts.clear();
    mhAccounts.clear();

    mhAccounts.append(new MHviewItem(AbstractMail::defaultLocalfolder(),this));

    Account *it;
    for ( it = list.first(); it; it = list.next() )
    {
        if ( it->getType() == MAILLIB::A_IMAP )
        {
            IMAPaccount *imap = static_cast<IMAPaccount *>(it);
            odebug << "added IMAP " + imap->getAccountName() << oendl; 
            imapAccounts.append(new IMAPviewItem( imap, this ));
        }
        else if ( it->getType() == MAILLIB::A_POP3 )
        {
            POP3account *pop3 = static_cast<POP3account *>(it);
            odebug << "added POP3 " + pop3->getAccountName() << oendl; 
            /* must not be hold 'cause it isn't required */
            (void) new POP3viewItem( pop3, this );
        }
         else if ( it->getType() == MAILLIB::A_NNTP )
        {
            NNTPaccount *nntp = static_cast<NNTPaccount *>(it);
            odebug << "added NNTP " + nntp->getAccountName() << oendl; 
            /* must not be hold 'cause it isn't required */
            (void) new NNTPviewItem( nntp, this );
       }
     }
}

void AccountView::refresh(QListViewItem *item)
{

    odebug << "AccountView refresh..." << oendl; 
    if ( item )
    {
        m_currentItem = item;
        QValueList<RecMailP> headerlist;
        AccountViewItem *view = static_cast<AccountViewItem *>(item);
        view->refresh(headerlist);
        emit refreshMailview(headerlist);
    }
}

void AccountView::refreshCurrent()
{
    m_currentItem = currentItem();
    if ( !m_currentItem ) return;
    QValueList<RecMailP> headerlist;
    AccountViewItem *view = static_cast<AccountViewItem *>(m_currentItem);
    view->refresh(headerlist);
    emit refreshMailview(headerlist);
}

void AccountView::refreshAll()
{
}

RecBodyP AccountView::fetchBody(const RecMailP&aMail)
{
    QListViewItem*item = selectedItem ();
    if (!item) return new RecBody();
    AccountViewItem *view = static_cast<AccountViewItem *>(item);
    return view->fetchBody(aMail);
}

void AccountView::setupFolderselect(Selectstore*sels)
{
    QPEApplication::showDialog( sels );
    QStringList sFolders;
    unsigned int i = 0;
    for (i=0; i < mhAccounts.count();++i)
    {
        mhAccounts[i]->refresh(false);
        sFolders = mhAccounts[i]->subFolders();
        sels->addAccounts(mhAccounts[i]->getWrapper(),sFolders);
    }
    for (i=0; i < imapAccounts.count();++i)
    {
        if (imapAccounts[i]->offline())
            continue;
        imapAccounts[i]->refreshFolders(false);
        sels->addAccounts(imapAccounts[i]->getWrapper(),imapAccounts[i]->subFolders());
    }
}

void AccountView::downloadMails(const FolderP&fromFolder,AbstractMail*fromWrapper)
{
    AbstractMail*targetMail = 0;
    QString targetFolder = "";
    Selectstore sels;
    setupFolderselect(&sels);
    if (!sels.exec()) return;
    targetMail = sels.currentMail();
    targetFolder = sels.currentFolder();
    if ( (fromWrapper==targetMail && fromFolder->getName()==targetFolder) ||
            targetFolder.isEmpty())
    {
        return;
    }
    if (sels.newFolder() && !targetMail->createMbox(targetFolder))
    {
        QMessageBox::critical(0,tr("Error creating new Folder"),
                              tr("<center>Error while creating<br>new folder - breaking.</center>"));
        return;
    }
    odebug << "Targetfolder: " << targetFolder.latin1() << "" << oendl; 
    odebug << "Fromfolder: " << fromFolder->getName().latin1() << "" << oendl; 
    fromWrapper->mvcpAllMails(fromFolder,targetFolder,targetMail,sels.moveMails());
    refreshCurrent();
}

bool AccountView::currentisDraft()
{
    AccountViewItem *view = static_cast<AccountViewItem *>(currentItem());
    if (!view) return false;
    return view->isDraftfolder();
}
