/*
** $Id: categorylist.h,v 1.2 2003-08-13 16:54:10 zcarsten Exp $
*/
#ifndef _CATEGORY_LIST_H_
#define _CATEGORY_LIST_H_

#include <qstring.h>

#include <qdict.h>

#include "category.h"

// list of Category

class CategoryList
{
public:
	CategoryList();
	~CategoryList();

        void insert (QString key, Category *category);
        Category *find (QString key);
        void remove (QString key);
        void clear();

        QDict<Category> categoryList;

private:

};
#endif // _CATEGORY_LIST_H_



