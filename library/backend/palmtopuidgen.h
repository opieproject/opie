#ifndef QTPALMTOP_UIDGEN_H
#define QTPALMTOP_UIDGEN_H
/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** Licensees holding valid Qtopia Developer license may use this
** file in accordance with the Qtopia Developer License Agreement
** provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE.
**
** email sales@trolltech.com for information about Qtopia License
** Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <time.h>
#include <qmap.h>
#include "qpcglobal.h"

#if defined(QPC_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QPC_EXPORT QMap< int, bool >;
// MOC_SKIP_END
#endif

namespace Qtopia {


class QPC_EXPORT UidGen
{
public:
    enum Type { Qtopia, PalmtopCenter };

    UidGen() : type( Qtopia ), sign( -1 ), ids()
{
#ifdef PALMTOPCENTER
    type = PalmtopCenter;
    sign = 1;
#endif
}
    UidGen( Type t ) : type(t), sign(1), ids()
{
    if ( t == Qtopia )
	sign = -1;
}

    virtual ~UidGen() { }

    int generate() const
{
    int id = sign * (int) ::time(NULL);
    while ( ids.contains( id ) ) {
	id += sign;
	
	// check for overflow cases; if so, wrap back to beginning of
	// set ( -1 or 1 )
	if ( sign == -1 && id > 0 || sign == 1 && id < 0 )
	    id = sign;
    }
    return id;
}

    void store(int id) { ids.insert(id, TRUE); }
    bool isUnique(int id) const { return (!ids.contains(id)); }

private:
    Type type;
    int sign;
    QMap<int, bool> ids;

};

}

#endif
