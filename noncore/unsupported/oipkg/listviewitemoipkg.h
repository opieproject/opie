#ifndef LISTVIEWITEMOIPKG_H
#define LISTVIEWITEMOIPKG_H

#include <qpopupmenu.h>
#include <qstring.h>
#include <qlistview.h>
#include <qcheckbox.h>

class PackageListItem;
class PackageListView;

class ListViewItemOipkg :  public QObject, public QCheckListItem
{
Q_OBJECT
public:
	enum Type {Package, Feed, Attribute};

	ListViewItemOipkg(QListView *parent, QString name, QCheckListItem::Type qct, Type type);	ListViewItemOipkg(QListView *parent, QString name,  Type type);
	ListViewItemOipkg(ListViewItemOipkg *parent, QString name, Type type);
	ListViewItemOipkg(PackageListView   *parent, QString name, Type type);
	ListViewItemOipkg(ListViewItemOipkg *parent, Type type, QString name);
	~ListViewItemOipkg();

 	int getType() {return _type;};
	virtual QPopupMenu* getPopupMenu() {return 0;};
protected slots:
  virtual void menuAction(int ) {};
protected:
	int _type;
	static QPopupMenu* _popupMenu; 
};

#endif
