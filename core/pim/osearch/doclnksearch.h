//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DOCLNKSEARCH_H
#define DOCLNKSEARCH_H

#include "searchgroup.h"

class DocLnkSet;

/**
@author Patrick S. Vogt
*/
class DocLnkSearch : public SearchGroup
{
public:
    DocLnkSearch(QListView* parent, QString name);

    ~DocLnkSearch();

    virtual void expand();
private:
    DocLnkSet *_docs;
};

#endif
