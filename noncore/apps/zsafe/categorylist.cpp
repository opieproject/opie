/* C implementation of RC2 encryption algorithm, as described in RFC2268 */
/* By Matthew Palmer <mjp16@uow.edu.au> */
/* $Id: categorylist.cpp,v 1.2 2003-08-13 16:54:10 zcarsten Exp $ */

#include "categorylist.h"

CategoryList::CategoryList()
{
}

CategoryList::~CategoryList()
{
}

void CategoryList::insert (QString key, Category *category)
{
   categoryList.insert (key, category);
}

Category *CategoryList::find (QString key)
{
   return categoryList.find (key);
}

void CategoryList::remove (QString key)
{
   categoryList.remove (key);
}

void CategoryList::clear()
{
   categoryList.clear();
}


