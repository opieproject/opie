/*
                             This file is part of the Opie Project
                             Originally a part of the KDE Project
                             (C) 2001 Carsten Pfeiffer <pfeiffer@kde.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef OSORTABLEVALUELIST_H
#define OSORTABLEVALUELIST_H

#if QT_VERSION > 290
#include <qtl.h>
#include <qpair.h>
#else
#include <opie2/otl.h>
#include <opie2/opair.h>
#endif
#include <qvaluelist.h>

template<class T, class Key = int> class OSortableItem : public QPair<Key,T>
{
public:
    OSortableItem( Key i, const T& t ) : QPair<Key, T>( i, t ) {}
    OSortableItem( const OSortableItem<T, Key> &rhs )
        : QPair<Key,T>( rhs.first, rhs.second ) {}

    OSortableItem() {}

    OSortableItem<T, Key> &operator=( const OSortableItem<T, Key>& i ) {
        first  = i.first;
        second = i.second;
        return *this;
    }

    // operators for sorting
    bool operator> ( const OSortableItem<T, Key>& i2 ) const {
        return (i2.first < first);
    }
    bool operator< ( const OSortableItem<T, Key>& i2 ) const {
        return (first < i2.first);
    }
    bool operator>= ( const OSortableItem<T, Key>& i2 ) const {
        return (first >= i2.first);
    }
    bool operator<= ( const OSortableItem<T, Key>& i2 ) const {
        return !(i2.first < first);
    }
    bool operator== ( const OSortableItem<T, Key>& i2 ) const {
        return (first == i2.first);
    }
    bool operator!= ( const OSortableItem<T, Key>& i2 ) const {
        return (first != i2.first);
    }

    T& value() {
        return second;
    }
    const T& value() const {
        return second;
    }

    Key index() const {
        return first;
    }
};


// convenience
template <class T, class Key = int>
class OSortableValueList : public QValueList<OSortableItem<T, Key> >
{
public:
    void insert( Key i, const T& t ) {
        QValueList<OSortableItem<T, Key> >::append( OSortableItem<T, Key>( i, t ) );
    }
    // add more as you please...

    T& operator[]( Key i ) {
        return QValueList<OSortableItem<T, Key> >::operator[]( i ).value();
    }
    const T& operator[]( Key i ) const {
        return QValueList<OSortableItem<T, Key> >::operator[]( i ).value();
    }

    void sort() {
        qHeapSort( *this );
    }
};

// template <class T> class OSortableValueListIterator : public QValueListIterator<OSortableItem<T>  >
// {
// };

#endif // OSORTABLEVALUELIST_H
