/* C implementation of RC2 encryption algorithm, as described in RFC2268 */
/* By Matthew Palmer <mjp16@uow.edu.au> */
/* $Id: category.cpp,v 1.2 2004-03-01 20:04:34 chicken Exp $ */

#include "category.h"

Category::Category()
{
}

Category::~Category()
{
}

QString Category::getIconName()
{
   return iconName;
}

QString Category::getCategoryName()
{
   return categoryName;
}

QListViewItem * Category::getListItem()
{
   return listItem;
}

QPixmap Category::getIcon()
{
   return icon;
}

void Category::setIconName(QString name)
{
   iconName = name;
}

void Category::setCategoryName(QString name)
{
   categoryName = name;
}

void Category::setListItem(QListViewItem *item)
{
   listItem = item;
}

void Category::setIcon(QPixmap item)
{
   icon = item;
}

void Category::initListItem()
{
   if (listItem)
   {
      listItem->setText (0, categoryName);
      listItem->setPixmap (0, icon);
   }
}
