/* C implementation of RC2 encryption algorithm, as described in RFC2268 */
/* By Matthew Palmer <mjp16@uow.edu.au> */
/* $Id: categorylist.cpp,v 1.1 2003-07-22 19:23:20 zcarsten Exp $ */

#include "categorylist.h"

CategoryList::CategoryList()
{
}

CategoryList::~CategoryList()
{
}

void CategoryList::insert (QString key, Category *category)
{
   categoryList.insert ((const char *) key, category);
}

Category *CategoryList::find (QString key)
{
   return categoryList.find ((const char *) key);
}

void CategoryList::remove (QString key)
{
   categoryList.remove ((const char *) key);
}

void CategoryList::clear()
{
   categoryList.clear();
}


