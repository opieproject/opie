// -*- Mode: C++; -*-
/*
                            This file is part of the Opie Project
                            Copyright (C) 2004 Rajko Albrecht <alwin@handhelds.org>
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

#ifndef _osmart_pointer_h
#define _osmart_pointer_h

/*!
 * \file osmart_pointer.h
 * \brief smart pointer and reference counter
 * \author Rajko Albrecht
 *
 */

namespace Opie {

//! simple reference counter class
class oref_count {
protected:
    //! reference count member
    long m_RefCount;
public:
    //! first reference must be added after "new" via Pointer()
    oref_count() : m_RefCount(0)
    {}
    virtual ~oref_count() {}
    //! add a reference
    void Incr() {
        ++m_RefCount;
    }
    //! delete a reference
    void Decr() {
        --m_RefCount;
    }
    //! is it referenced
    bool Shared() { return (m_RefCount > 0); }
};

//! reference counting wrapper class
template<class T> class osmart_pointer {
    //! pointer to object
    /*!
     * this object must contain Incr(), Decr() and Shared() 
     * methode as public members. The best way is, that it will be a child
     * class of RefCount
     */
    T *ptr;
public:
    //! standart constructor
    osmart_pointer() { ptr = NULL; }
    //! standart destructor
    /*!
     * release the reference, if it were the last reference, destroys
     * ptr
     */
    ~osmart_pointer()
    {
        if (ptr){
            ptr->Decr();
            if (!ptr->Shared())
                delete ptr;
        }
    }
    //! construction
    osmart_pointer(T* t) { if (ptr = t) ptr->Incr(); }
    //! Pointer copy
    osmart_pointer(const osmart_pointer<T>& p) 
    { if (ptr = p.ptr) ptr->Incr(); }
    //! pointer copy by assignment
    osmart_pointer<T>& operator= (const osmart_pointer<T>& p) 
    {
        // already same: nothing to do
        if (ptr == p.ptr) return *this;
        // decouple reference
        if (ptr) { ptr->Decr(); if (!ptr->Shared()) delete ptr; } 
        // establish new reference
        if (ptr = p.ptr) ptr->Incr();
        return *this;
    }
    osmart_pointer<T>& operator= (T*p)
    {
        if (ptr==p)return *this;
        if (ptr) {
            ptr->Decr();
            if (!ptr->Shared()) delete ptr;
        }
        if (ptr=p) ptr->Incr();
        return *this;
    }

    //! cast to conventional pointer
    operator T* () const { return ptr; }
    
    //! deref: fails for NULL pointer
    T& operator* () {return *ptr; }
    //! deref: fails for NULL pointer
    const T& operator* ()const {return *ptr; }

    //! deref with method call
    T* operator-> () {return ptr; }
    //! deref with const method call
    const T* operator-> ()const {return ptr; }
    
    //! supports "if (pointer)"
    operator bool () const { return (ptr != NULL); }
    //! "if (pointer)" as non const
    operator bool () { return ptr != NULL;}
    
    //! support if (!pointer)"
    bool operator! () const { return (ptr == NULL); }
    //! support if (!pointer)" as non const
    bool operator! () { return (ptr == NULL); }
};

}

#endif
