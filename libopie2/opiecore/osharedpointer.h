// -*- Mode: C++; -*-
/*
                            This file is part of the Opie Project
                            Copyright (C) 2004 Rajko Albrecht <alwin@handhelds.org>
			    Copyright (C) 2004 Holger Hans Peter Freyther <freyther@handhelds.org>
                            Copyright (C) The Opie Team <opie-devel@handhelds.org>
             =.
           .=l.
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


#ifndef O_SHARED_POINTER_H
#define O_SHARED_POINTER_H

#include <opie2/osmartpointer.h>

/*!
 * \file  osharedpointer.h
 * \brief Adapter with Refcounting around any data type
 * \author Rajko Albrecht and Holger Hans Peter Freyther
 */

namespace Opie {
namespace Core {
template<class T>
class OSharedPointerData : public ORefCount {
public:
    OSharedPointerData( T* dt ) {
        data = dt;
    }

    ~OSharedPointerData() {
        delete data;
    }

    T *data;
};


/**
 * \brief A small Adapter around any Pointer
 *
 *
 *
 */
template<class T>
class OSharedPointer {
    typedef OSharedPointerData<T> Data;
    Data* data;

    void unref() {
        if (data) { data->Decr(); if (!data->Shared()) delete data; }
        data = 0;
    }

public:
    OSharedPointer () { data = 0;}

    OSharedPointer( T* t ) {
        data = new Data( t );
        data->Incr();
    }

    OSharedPointer( const OSharedPointer<T>& p ) {
        if ( data = p.data ) data->Incr();
    }

    ~OSharedPointer() {
        unref();
    }

    OSharedPointer<T> &operator=( const OSharedPointer<T>& p ) {
        // already same: nothing to do
        if (data == p.data) return *this;
        // decouple reference
        unref();
        // establish new reference
        if (data = p.data) data->Incr();
        return *this;
    }

    OSharedPointer<T> &operator=( T *p ) {
        unref();
        data = new Data( p );
        data->Incr();

        return *this;
    }

    operator T* () const { return data->data;  }
    T& operator*()       { return *data->data; }
    const T& operator*()const { return data->data; }

    //! deref with method call
    T* operator-> () {return data->data; }
    //! deref with const method call
    const T* operator-> ()const {return data->data; }


    //! supports "if (pointer)"
    operator bool () const { return (data != 0 && data->data != 0); }
    //! "if (pointer)" as non const
    operator bool () { return ( data != 0 && data->data != NULL );}

    //! support if (!pointer)"
    bool operator! () const { return (data == 0 || data->data == 0); }
    //! support if (!pointer)" as non const
    bool operator! () { return (data == 0 || data->data == 0); }
};

}
}

#endif
