
#include "profile.h"
#include "qlistview.h"
#include "configdialog.h"

class ConfigListItem : public QListViewItem {
public:
    ConfigListItem( QListView* item, const Profile& );
    ~ConfigListItem();
    Profile profile()const;

private:
    Profile m_prof;
};
ConfigListItem::ConfigListItem( QListView* item, const Profile& prof )
    : QListViewItem( item ), m_prof( prof )
{
    setText(0, prof.name() );
}
ConfigListItem::~ConfigListItem() {

}
Profile ConfigListItem::profile()const {
    return m_prof;
}

/* Dialog */

ConfigDialog::ConfigDialog( const Profile::ValueList& lis, QWidget* parent )
    : ConfigureBase( parent, 0, TRUE )
{
    //init();
    {
        Profile::ValueList::ConstIterator it;
        for (it = lis.begin(); it != lis.end(); ++it ) {
            new ConfigListItem( lstView, (*it) );
        }
    }
}
ConfigDialog::~ConfigDialog() {

}
Profile::ValueList ConfigDialog::list()const {
/* iterate over the list */
    Profile::ValueList lst;
    QListViewItemIterator it(lstView);
    for ( ; it.current(); ++it ) {
        ConfigListItem* item = (ConfigListItem*)it.current();
        lst.append( item->profile() );
    }
    return lst;
}
/* our slots */
void ConfigDialog::slotRemove() {
    ConfigListItem* item = (ConfigListItem*)lstView->currentItem();
    if (!item )
        return;

    lstView->takeItem( item );
    delete item;
}
void ConfigDialog::slotEdit() {

}
void ConfigDialog::slotAdd() {

}

