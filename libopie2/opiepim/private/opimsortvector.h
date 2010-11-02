/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 Holger Freyther <freyther@handhelds.org>
              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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

#ifndef OPIE_PIM_SORT_VECTOR_H
#define OPIE_PIM_SORT_VECTOR_H

#include <opie2/opimglobal.h>

#include <qvector.h>

namespace Opie {
namespace Internal {
template<class T>
struct OPimSortVectorContainer {
    T item;
};

template<class T>
class  OPimSortVector : public QVector<OPimSortVectorContainer<T> > {
    typedef OPimSortVectorContainer<T> VectorItem;
public:
    OPimSortVector( uint size, bool asc, int sort );
    int compareItems( QCollection::Item d1, QCollection::Item d2 );
    bool insert( uint, const T& t );
    UID uidAt( uint i )const;

protected:
    int testString( const QString&, const QString& )const;
    int testDate( const QDate&, const QDate& )const;
    int testTime( const QTime&, const QTime& )const;
    int testDateTime( const QDateTime& left,
		      const QDateTime& right )const;
    int testDaysUntilNextDate( const QDate& left,
          const QDate& right )const;
protected:
    bool sortAscending()const;
    int  sortOrder()const;

private:
    bool m_ascending : 1;
    int m_sort;
    virtual int compareItems( const T& item1, const T& item2 ) = 0;
};

template<class T>
OPimSortVector<T>::OPimSortVector( uint size, bool asc, int sort )
    : QVector<VectorItem>( size ), m_ascending( asc ),
      m_sort( sort ) {
    this->setAutoDelete( true );
}

/**
 * Returns:
 *       0 if item1 == item2
 *
 *   non-zero if item1 != item2
 *
 *   This function returns int rather than bool so that reimplementations
 *   can return one of three values and use it to sort by:
 *
 *   0 if item1 == item2
 *
 *   > 0 (positive integer) if item1 > item2
 *
 *   < 0 (negative integer) if item1 < item2
 *
 */
template<class T>
int OPimSortVector<T>::compareItems( QCollection::Item d1, QCollection::Item d2 ) {
    return compareItems( ((VectorItem*)d1)->item,
                         ((VectorItem*)d2)->item );
}

template<class T>
bool OPimSortVector<T>::sortAscending()const {
    return m_ascending;
}

template<class T>
int OPimSortVector<T>::sortOrder()const {
    return m_sort;
}

template<class T>
bool OPimSortVector<T>::insert( uint i, const T& record ) {
    VectorItem *item = new VectorItem;
    item->item = record;
    return QVector<VectorItem>::insert( i, item );
}

template<class T>
UID OPimSortVector<T>::uidAt( uint index )const {
    return this->at( index )->item.uid();
}

template<class T>
inline int OPimSortVector<T>::testString( const QString& left,
                                          const QString& right )const {
    return QString::compare( left, right );
}


template<class T>
inline int OPimSortVector<T>::testDate( const QDate& left,
                                        const QDate& right )const {
    int ret = 0;
    if ( !left .isValid() ) ret++;
    if ( !right.isValid() ) ret--;

    if ( left.isValid() && right.isValid() )
        ret += left < right ? -1 : 1;

    return ret;
}

template<class T>
inline int OPimSortVector<T>::testTime( const QTime& left,
                                        const QTime& right )const {
    int ret = 0;
    if ( !left .isValid() ) ret++;
    if ( !right.isValid() ) ret--;

    if ( left.isValid() && right.isValid() ){
	    ret += left < right ? -1 : 1;
    }

    return ret;
}

template<class T>
inline int OPimSortVector<T>::testDateTime( const QDateTime& left,
                                        const QDateTime& right )const {
    int ret = 0;
    if ( !left .isValid() ) ret++;
    if ( !right.isValid() ) ret--;

    if ( left.isValid() && right.isValid() ){
	    ret += left < right ? -1 : 1;
    }

    return ret;

}

template<class T>
inline int OPimSortVector<T>::testDaysUntilNextDate( const QDate& left,
                                        const QDate& right )const {
    int ret = 0;
    if ( !left .isValid() ) ret++;
    if ( !right.isValid() ) ret--;

    if ( left.isValid() && right.isValid() ){
        int currentYear = QDate::currentDate().year();
        QDate nextLeft( currentYear, left.month(), left.day() );
        if ( QDate::currentDate().daysTo(nextLeft) < 0 )
            nextLeft.setYMD( currentYear+1, left.month(), left.day() );
        QDate nextRight( currentYear, right.month(), right.day() );
        if ( QDate::currentDate().daysTo(nextRight) < 0 )
            nextRight.setYMD( currentYear+1, right.month(), right.day() );

        ret += QDate::currentDate().daysTo(nextLeft) < QDate::currentDate().daysTo(nextRight) ? -1 : 1;
    }

    return ret;
}

}
}

#endif
