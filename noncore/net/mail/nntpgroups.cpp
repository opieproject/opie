#include "nntpgroups.h"

#include <libmailwrapper/settings.h>

#include <qlistview.h>
#include <qlineedit.h>

NNTPGroups::NNTPGroups(NNTPaccount *account, QWidget* parent, const char* name, WFlags fl)
    : NNTPGroupsUI(parent,name,fl),subscribedGroups()
{
    m_Account = account;
    fillGroups();
}

NNTPGroups::~NNTPGroups()
{
    
}

void NNTPGroups::slotGetNG()
{
    if (!m_Account) return;
    GroupListView->clear();
    NNTPwrapper tmp( m_Account );
    QString filter = Groupfilteredit->text();
    QStringList list =  tmp.listAllNewsgroups(filter);
    subscribedGroupsNotListed = subscribedGroups;
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
         QCheckListItem *item;
         item = new QCheckListItem( GroupListView, (*it), QCheckListItem::CheckBox );
         if ( subscribedGroups.contains( (*it) ) >= 1 ) {
             item->setOn( true );
             subscribedGroupsNotListed.remove((*it));
         }
    }
}

void NNTPGroups::fillGroups()
{
    if (!m_Account) return;
    subscribedGroups = m_Account->getGroups();
    for ( QStringList::Iterator it = subscribedGroups.begin(); it != subscribedGroups.end(); ++it ) {
         QCheckListItem *item;
         item = new QCheckListItem( GroupListView, (*it), QCheckListItem::CheckBox );
         item->setOn( true );
    }
}

void NNTPGroups::storeValues()
{
    if (!m_Account) return;
    QListViewItemIterator list_it( GroupListView );
    subscribedGroups.clear();
    for ( ; list_it.current(); ++list_it ) {
        if ( ( (QCheckListItem*)list_it.current() )->isOn() ) {
            qDebug(list_it.current()->text(0) );
            subscribedGroups.append(  list_it.current()->text(0) );
        }
    }
    subscribedGroups+=subscribedGroupsNotListed;
    m_Account->setGroups( subscribedGroups );
}
