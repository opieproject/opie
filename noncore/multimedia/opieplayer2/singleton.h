/*
   Copyright (C) 2002 Simon Hausmann <simon@lst.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef SINGLETON_H
#define SINGLETON_H

#include "threadutil.h"

template <class Product>
struct DefaultSingletonCreator
{
    static Product *create() { return new Product; }
};

template <class Product>
struct NullSingletonCreator
{
    static Product *create() { return 0; }
};

template 
<
    class T,
    template <class> class Creator = DefaultSingletonCreator
>
class Singleton
{
public:
    static T &self()
    {
        if ( !s_self ) {
            ThreadUtil::AutoLock lock( s_guard );
            if ( !s_self )
                s_self = Creator<T>::create();
        }
        return *s_self;
    }

protected:
    Singleton()
    { s_self = static_cast<T *>( this ); }
    ~Singleton()
    { s_self = 0; }

private:
    Singleton( const Singleton<T, Creator> &rhs );
    Singleton<T, Creator> &operator=( const Singleton<T, Creator> &rhs );

    static T *s_self;
    static ThreadUtil::Mutex s_guard;
};

template <class T, template <class> class Creator>
T *Singleton<T, Creator>::s_self = 0;

template <class T, template <class> class Creator>
ThreadUtil::Mutex Singleton<T, Creator>::s_guard;

#endif // SINGLETON_H
/* vim: et sw=4 ts=4
 */
