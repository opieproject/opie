
#include "profile.h"
#include "qlistview.h"
#include "configdialog.h"
#include "profileeditordialog.h"
#include "metafactory.h"
#include "qdialog.h"

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
	ProfileEditorDialog dlg(new MetaFactory());

	dlg.setCaption("Edit Connection Profile");
	dlg.showMaximized();
	int ret = dlg.exec();

	if(ret == QDialog::Accepted)
	{
		// if(listView->current()) delete lstView->current(); ...
		// use dlg.terminal()!
		new ConfigListItem(lstView, Profile(dlg.prof_name(), dlg.prof_type(), Profile::White, Profile::Black, Profile::VT102));
	}
}


void ConfigDialog::slotAdd() {
	qWarning("ConfigDialog::slotAdd");

	ProfileEditorDialog dlg(new MetaFactory());

	dlg.setCaption("New Connection");
	dlg.showMaximized();
	int ret = dlg.exec();

	if(ret == QDialog::Accepted)
	{
		// defaults
		int profile = Profile::VT102;

		// assignments
		QString type = dlg.term_type();
		if(type == "VT102") profile = Profile::VT102;

		// new profile
		Profile p(dlg.prof_name(), dlg.prof_type(), Profile::White, Profile::Black, profile);

		// special settings
		p.writeEntry("Device", dlg.conn_device());
		p.writeEntry("Baud", dlg.conn_baud());
		p.writeEntry("Parity", dlg.conn_parity());
		p.writeEntry("DataBits", dlg.conn_databits());
		p.writeEntry("StopBits", dlg.conn_stopbits());
		p.writeEntry("Flow", dlg.conn_flow());

		new ConfigListItem(lstView, p);
	}
}

