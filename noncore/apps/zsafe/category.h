/*
** $Id: category.h,v 1.1 2003-07-22 19:23:28 zcarsten Exp $
*/

#ifndef _CATEGORY_H_
#define _CATEGORY_H_

#include <qstring.h>
#include <qlistview.h>
#include <qpixmap.h>

// class to hold one category item

class Category
{
public:
	Category();
	~Category();

        QString getIconName();
        QString getCategoryName();
        QListViewItem * getListItem();
        QPixmap getIcon();
        void setIconName(QString name);
        void setCategoryName(QString name);
        void setListItem(QListViewItem *item);
        void setIcon(QPixmap item);
        void initListItem();

protected:
 	QString iconName; // full path to the icon
	QString categoryName; // name of the category
        QPixmap       icon; // pixmap icon
	QListViewItem *listItem; // list view item

private:

};
#endif // _CATEGORY_H_



