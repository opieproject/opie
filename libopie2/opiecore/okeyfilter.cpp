/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2004 Rajko 'Alwin' Albrecht <alwin@handhelds.org>
           .=l.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
          .>+-=
_;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=         redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :          the terms of the GNU Library General Public
.="- .-=="i,     .._        License as published by the Free Software
- .   .-<_>     .<>         Foundation; either version 2 of the License,
    ._= =}       :          or (at your option) any later version.
   .%`+i>       _;_.
   .i_,=:_.      -<s.       This program is distributed in the hope that
    +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
   : ..    .:,     . . .    without even the implied warranty of
   =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;     Library General Public License for more
++=   -.     .`     .:      details.
:     =  ...= . :.=-
-.   .:....=;==+<;          You should have received a copy of the GNU
 -_. . .   )=.  =           Library General Public License along with
   --        :-=`           this library; see the file COPYING.LIB.
                            If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
*/

#include "okeyfilter.h"
#include "device/odevice.h"
#include "odebug.h"

namespace Opie {
namespace Core {

class OKeyFilterPrivate:public OKeyFilter, QWSServer::KeyboardFilter
{
    static QValueList<QWSServer::KeyboardFilter*> filterList;
    static QValueList<QWSServer::KeyboardFilter*> preFilterList;
protected:
    OKeyFilterPrivate(const OKeyFilterPrivate&):OKeyFilter(),QWSServer::KeyboardFilter(){};
    virtual void addPreHandler(QWSServer::KeyboardFilter*);
    virtual void remPreHandler(QWSServer::KeyboardFilter*);

public:
    OKeyFilterPrivate();
    virtual ~OKeyFilterPrivate();
    virtual bool filter( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );
    virtual void addHandler(QWSServer::KeyboardFilter*);
    virtual void remHandler(QWSServer::KeyboardFilter*);
};

QValueList<QWSServer::KeyboardFilter*> OKeyFilterPrivate::filterList;
QValueList<QWSServer::KeyboardFilter*> OKeyFilterPrivate::preFilterList;

OKeyFilter::OKeyFilter()
{
}

OKeyFilter::~OKeyFilter()
{
}

OKeyFilter* OKeyFilter::inst()
{
    static OKeyFilter*ofilter = 0;
    if (!ofilter) {
        ofilter = new OKeyFilterPrivate;
    }
    return ofilter;
}

bool OKeyFilterPrivate::filter( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
    QValueList<QWSServer::KeyboardFilter*>::Iterator iter;
    for (iter=preFilterList.begin();iter!=preFilterList.end();++iter) {
        if ((*iter)->filter(unicode,keycode,modifiers,isPress,autoRepeat)) {
            return true;
        }
    }
    for (iter=filterList.begin();iter!=filterList.end();++iter) {
        if ((*iter)->filter(unicode,keycode,modifiers,isPress,autoRepeat)) {
            return true;
        }
    }
    return false;
}

void OKeyFilterPrivate::addHandler(QWSServer::KeyboardFilter*aF)
{
    if (filterList.find(aF)!=filterList.end()) {
        return;
    }
    odebug << "adding a keyboard filter handler"<<oendl;
    filterList.append(aF);
}

void OKeyFilterPrivate::remHandler(QWSServer::KeyboardFilter*aF)
{
    QValueList<QWSServer::KeyboardFilter*>::Iterator iter;
    if ( (iter=filterList.find(aF))==filterList.end() ) {
        return;
    }
    odebug << "removing a keyboard filter handler"<<oendl;
    filterList.remove(iter);
}

void OKeyFilterPrivate::addPreHandler(QWSServer::KeyboardFilter*aF)
{
    if (preFilterList.find(aF)!=preFilterList.end()) {
        return;
    }
    odebug << "adding a preferred keyboard filter handler"<<oendl;
    preFilterList.append(aF);
}

void OKeyFilterPrivate::remPreHandler(QWSServer::KeyboardFilter*aF)
{
    QValueList<QWSServer::KeyboardFilter*>::Iterator iter;
    if ( (iter=preFilterList.find(aF))==preFilterList.end() ) {
        return;
    }
    odebug << "removing a preferred keyboard filter handler"<<oendl;
    preFilterList.remove(iter);
}

OKeyFilterPrivate::OKeyFilterPrivate()
    :OKeyFilter(),QWSServer::KeyboardFilter()
{
    filterList.clear();
    preFilterList.clear();
    if ( isQWS( ) ) {
        QWSServer::setKeyboardFilter ( this );
    }
}

OKeyFilterPrivate::~OKeyFilterPrivate()
{
}

/* namespace Core */
}
/* namespace Opie */
}
