/*
** $Id: categorylist.h,v 1.1 2003-07-22 19:23:15 zcarsten Exp $
*/
#ifndef _CATEGORY_LIST_H_
#define _CATEGORY_LIST_H_

#include <qstring.h>
#include <qasciidict.h>
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

        QAsciiDict<Category> categoryList;

private:

};
#endif // _CATEGORY_LIST_H_



