#ifndef _HLIST_H
#define _HLIST_H

#include <qstringlist.h>
#include <qdatetime.h>

class HList
{
public:
    HList(){};
    virtual ~HList(){};

    const QStringList entries(const QDate&)const;
};

#endif
