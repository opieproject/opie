#include "listviewitemoipkg.h"
#include "packagelistview.h"

#include <qlistview.h>
	
ListViewItemOipkg::ListViewItemOipkg(QListView *parent, QString name, QCheckListItem::Type qct, Type type)
	: QCheckListItem(parent,name,qct)
{
  _type = type;
}
ListViewItemOipkg::ListViewItemOipkg(QListView *parent, QString name, Type type)
	: QCheckListItem(parent,name,CheckBox)
{
  _type = type;
}

ListViewItemOipkg::ListViewItemOipkg(ListViewItemOipkg *parent, QString name, Type type)
	: QCheckListItem(parent,name,CheckBox)
{
  _type = type;
}
         
ListViewItemOipkg::ListViewItemOipkg(PackageListView *parent, QString name, Type type)
	: QCheckListItem(parent,name,Controller)
{
  _type = type;
}

ListViewItemOipkg::ListViewItemOipkg(ListViewItemOipkg *parent, Type type, QString name)
	: QCheckListItem(parent,name,Controller)
{
  _type = type;
}

ListViewItemOipkg::~ListViewItemOipkg()
{
//  delete _popupMenu;
}
