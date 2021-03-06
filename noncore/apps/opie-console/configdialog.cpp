
#include <qpe/qpeapplication.h>

#include <qlistview.h>

#include "configdialog.h"
#include "profileeditordialog.h"

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

ConfigDialog::ConfigDialog( const Profile::ValueList& lis, MetaFactory* fa,
                            QWidget* parent )
    : ConfigureBase( parent, 0, TRUE ), m_fact( fa )
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
    Profile p;

    if(!lstView->currentItem()) return;

    // Load profile
    p = ((ConfigListItem*)lstView->currentItem())->profile();

    ProfileEditorDialog dlg(m_fact, p);

    dlg.setCaption(tr("Edit Connection Profile"));
    int ret = QPEApplication::execDialog( &dlg );

    if(ret == QDialog::Accepted)
    {
        if(lstView->currentItem()) delete lstView->currentItem();

        // use dlg.terminal()!
        Profile p = dlg.profile();

        new ConfigListItem(lstView, p);
    }
}

void ConfigDialog::slotAdd() {
    ProfileEditorDialog dlg(m_fact);

    dlg.setCaption(tr("New Connection"));
    int ret = QPEApplication::execDialog( &dlg );

    if(ret == QDialog::Accepted)
    {
        // TODO: Move into general profile save part
        // assignments
        //QString type = dlg.term_type();
        //if(type == "VT102") profile = Profile::VT102;

        // get profile from editor
        Profile p = dlg.profile();

        new ConfigListItem(lstView, p);
    }
}

