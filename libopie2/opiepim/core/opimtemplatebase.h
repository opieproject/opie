/*
                             This file is part of the Opie Project
                             Copyright (C) Holger Freyther <zecke@handhelds.org>
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
#ifndef OTEMPLATEBASE_H
#define OTEMPLATEBASE_H

/* OPIE */
#include <opie2/opimrecord.h>
#include <opie2/opimcache.h>
#include <opie2/opimoccurrence.h>
#include <opie2/opimbackendoccurrence.h>

/* QT */
#include <qarray.h>
#include <qdatetime.h>

namespace Opie {

class OPimBasePrivate;

/**
 * This is the base class for all our Interfaces to the
 * PIM Records. It is pointer based and can be used
 * generically for all types of Records.
 *
 */
struct OPimBase {
    //@{
    OPimBase();
    virtual ~OPimBase();
    //@}

    //@{
    /**
     * return the rtti
     */
    virtual int rtti() const = 0;
    virtual OPimRecord* record()const = 0;
    virtual OPimRecord* record(int uid)const = 0;
    //@}

    //@{
    virtual bool add( const OPimRecord& ) = 0;
    virtual bool add( const OPimRecord* ) = 0;

    virtual bool remove( int uid ) = 0;
    virtual bool remove( const OPimRecord& ) = 0;
    //@}

    //@{
    virtual void clear() = 0;
    virtual bool load() = 0;
    virtual bool save() = 0;
    //@}

    //@{
    virtual QArray<UID> records()const = 0;

    /** Constants for query.
     * Use this constants to set the query parameters.
     * Note: <i>query_IgnoreCase</i> just make sense with one of the other attributes !
     * @see queryByExample()
     */
    enum QuerySettings {
        WildCards  = 0x0001, /** Use Wildcards */
        IgnoreCase = 0x0002, /** Ignore the Case  */
        RegExp     = 0x0004, /** Do a Regular Expression match */
        ExactMatch = 0x0008, /** It needs to exactly match     */
        MatchOne   = 0x0010, /** Only one Entry must match */
        DateDiff   = 0x0020, /** Find all entries from today until given date */
        DateYear   = 0x0040, /** The year matches */
        DateMonth  = 0x0080, /** The month matches */
        DateDay    = 0x0100, /** The day matches */
        LastItem   = 0xffff  /** the last possible name */
    };

    /**
     * Common Attributes for the Sort Order
     */
    enum SortOrderBase {
        SortSummary = 0,       /** Sort by a Summary of the records */
        SortByCategory = 1,    /** Sort by Category */
        SortByDate = 2,        /** Sort by Date */
        SortCustom  = 10,      /** The First available sort number for the OPimAccessTemplates */
        LastSortOrderBase = 0xffff /** make this enum 16bit large */
    };

    /**
     * Sort with the help of the \sa sorted function
     * a list of Items.
     * The Item you provide in SortOrder will be used
     * for sorting.
     *
     * @see sorted
     */
    enum SortFilterBase {
        FilterCategory = 1,
        FilterCustom = 1024,
        LastSortFilterBase = 0xffffffff
    };

    virtual UIDArray matchRegexpSimple( const QRegExp& r )const = 0;
    virtual UIDArray queryByExampleSimple( const OPimRecord*, int settings,
                                           const QDateTime& d = QDateTime() )const  = 0;
    virtual UIDArray sortedSimple( const UIDArray& uid, bool ascending,
                                   int sortOrder, int sortFilter, int cat)const = 0;
    virtual UIDArray sortedSimple( const UIDArray& uid, bool ascending,
                                   int sortOrder, int sortFilter, const QArray<UID>& cats )const = 0;
    virtual UIDArray sortedSimple( bool ascending, int sortOrder, int sortFilter, int cat)const = 0;
    virtual UIDArray sortedSimple( bool ascending, int sortOrder, int sortFilter, const QArray<UID>& cats )const = 0;
    virtual OPimOccurrence::List occurrences( const QDate& start,  const QDate& end )const = 0;
    virtual OPimOccurrence::List occurrences( const QDateTime& dt )const = 0;
    //@}


protected:
    OPimOccurrence::List convertOccurrenceFromBackend( const OPimBackendOccurrence::List& )const;

private:
    OPimBasePrivate* d;
};


/**
 * internal template base
 * Attention: T needs to implement the copy c'tor!!!
 */
class OTemplateBasePrivate;
template <class T = OPimRecord>
class OTemplateBase : public OPimBase {
public:
    /**
     * The Direction for ReadAhead/ReadBehind which will
     * be used by the backends to Cache Items in
     * advance.
     * For example if you know that you will access the
     * next ten items you can give the backend a hint
     * to read ahead or read before.
     */
    enum CacheDirection {
        Forward=0, /** Go forward   when caching (++ to the index) */
        Reverse    /** Go backward  when caching (-- to the index) */
    };


    //@{
    OTemplateBase() {};
    virtual ~OTemplateBase() {}
    //@}


    //@{
    virtual T find( int uid )const = 0;

    /**
     * read ahead find
     */
    virtual T find( int uid, const QArray<int>& items,
                    uint current, CacheDirection dir = Forward )const = 0;
    //@}

    //@{
   /**
    * Put element into Cache
    */
    virtual void cache( const T& )const = 0;
    virtual void setSaneCacheSize( int ) = 0;

    OPimRecord* record()const;
    OPimRecord* record(int uid )const;
    static T* rec();
    //@}

private:
    OTemplateBasePrivate *d;
};


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
}

template <class T>
T* OTemplateBase<T>::rec() {
    return new T;
}
}

#endif
