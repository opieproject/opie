/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#ifndef OPIE_TEMPLATE_BASE_H
#define OPIE_TEMPLATE_BASE_H

#include <qarray.h>

#include <opie2/opimrecord.h>


namespace Opie {
/**
 * Templates do not have a base class, This is why
 * we've this class
 * this is here to give us the possibility
 * to have a common base class
 * You may not want to use that interface internaly
 * POOR mans interface
 */
class OPimBasePrivate;
struct OPimBase {
    /**
     * return the rtti
     */
    virtual int rtti()= 0;
    virtual OPimRecord* record()const = 0;
    virtual OPimRecord* record(int uid)const = 0;
    virtual bool add( const OPimRecord& ) = 0;
    virtual bool remove( int uid ) = 0;
    virtual bool remove( const OPimRecord& ) = 0;
    virtual void clear() = 0;
    virtual bool load() = 0;
    virtual bool save() = 0;
    virtual QArray<int> records()const = 0;
    /*
     * ADD editing here?
     * -zecke
     */
private:
    OPimBasePrivate* d;     

};
/**
 * internal template base
 * T needs to implement the copy c'tor!!!
 */
class OTemplateBasePrivate;
template <class T = OPimRecord>
class OTemplateBase : public OPimBase {
public:
    enum CacheDirection { Forward=0, Reverse };
    OTemplateBase() {
    };
    virtual ~OTemplateBase() {
    }
    virtual T find( int uid )const = 0;

    /**
     * read ahead find
     */
    virtual T find( int uid, const QArray<int>& items,
                    uint current, CacheDirection dir = Forward )const = 0;
    virtual void cache( const T& )const = 0;
    virtual void setSaneCacheSize( int ) = 0;

    /* reimplement of OPimBase */
    int rtti();
    OPimRecord* record()const;
    OPimRecord* record(int uid )const;
    static T* rec();
    
private:
    OTemplateBasePrivate *d;    
};

/*
 * implementation
 */
template <class T>
int
OTemplateBase<T>::rtti() {
    return T::rtti();
}
template <class T>
OPimRecord* OTemplateBase<T>::record()const {
    T* t = new T;
    return t;
}
template <class T>
OPimRecord* OTemplateBase<T>::record(int uid )const {
    T  t2 = find(uid );
    T* t1 = new T(t2);

    return t1;
};
template <class T>
T* OTemplateBase<T>::rec() {
    return new T;
}

}

#endif
