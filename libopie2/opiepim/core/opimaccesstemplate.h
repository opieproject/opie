/*
                             This file is part of the Opie Project
                             Copyright (C) Holger Freyther <zecke@handhelds.org>
                             Copyright (C) Stefan Eilers <eilers.stefan@epost.de>
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
#ifndef OPIE_PIM_ACCESS_TEMPLATE_H
#define OPIE_PIM_ACCESS_TEMPLATE_H

/* OPIE */
#include <opie2/opimrecord.h>
#include <opie2/opimaccessbackend.h>
#include <opie2/opimrecordlist.h>

#include <opie2/opimtemplatebase.h>
#include <opie2/odebug.h>

/* QT */
#include <qarray.h>
#include <qdatetime.h>

namespace Opie {

class OPimAccessTemplatePrivate;
/**
 * Thats the frontend to our OPIE PIM
 * Library. Either you want to use it's
 * interface or you want to implement
 * your own Access lib
 * Just create a OPimRecord and inherit from
 * the templates
 */

template <class T = OPimRecord >
class OPimAccessTemplate : public OTemplateBase<T> {
public:
    /**
     *
     */
    enum Access {
        Random = 0,
        SortedAccess
    };
    typedef OPimRecordList<T> List;
    typedef OPimAccessBackend<T> BackEnd;
    typedef OPimCache<T> Cache;

    //@{
    OPimAccessTemplate( BackEnd* end);
    virtual ~OPimAccessTemplate();
    //@}

    //@{
    bool load();
    virtual bool reload();
    bool save();
    void clear() ;
    //@}


    bool wasChangedExternally()const;

    //@{
    virtual List allRecords()const;
    virtual List matchRegexp(  const QRegExp &r ) const;

    /**
     * Return all possible settings for queryByExample().
     *  @return All settings provided by the current backend
     * (i.e.: WildCards & IgnoreCase)
     * @see QuerySettings in OPimBase for details of the parameter, queryByExample()
     */
    uint querySettings();

    /**
     * Check whether settings are correct for queryByExample().
     * @return <i>true</i> if the given settings are correct and possible.
     * @see QuerySettings in OPimBase for details of the parameter
     */
     bool hasQuerySettings ( int querySettings ) const;

    /**
     * Query by example search interface.
     * "Query by Example" provides a very powerful search engine. Use the query object
     * (this may be a contact, a todo or databook event)
     * as a search mask which is converted into a query regarding the querySettings. If a time period is needed
     * (as for OpimBase::DateDiff), you have to use the date/time in the query object and the startperiod (the last parameter).
     * @see QuerySettings in class OPimBase
     * @param query The object which contains the query set
     * @param querySettings This parameter defines what should be searched and how the query should be interpreted
     * @param startperiod Defines the start of a period for some special queries.
     */
    virtual List queryByExample( const T& query, int querySettings, const QDateTime& startperiod = QDateTime() );

    /**
     * Generic query by example search interface. This is a special version which handles generic OPimRecord types. They are converted
     * automatically into the right datatype.
     * "Query by Example" provides a very powerful search engine. Use the query object (this may be a contact, a todo or databook event)
     * as a search mask which is converted into a query regarding the querySettings. If a time period is needed
     * (as for OpimBase::DateDiff), you have to use the date/time in the query object and the startperiod (the last parameter).
     * @see QuerySettings in class OPimBase
     * @param query The object which contains the query set
     * @param querySettings This parameter defines what should be searched and how the query should be interpreted
     * @param startperiod Defines the start of a period for some special queries.
     */
    virtual List queryByExample( const OPimRecord* query, int querySettings, const QDateTime& startperiod = QDateTime() );
    /**
     * Incremental query by example search interface. Providing incremental search, this one provides the feature
     * to search in a list of records which may be returned by an other search.
     * "Query by Example" provides a very powerful search engine. Use the query object (this may be a contact, a todo or databook event)
     * as a search mask which is converted into a query regarding the querySettings. If a time period is needed
     * (as for OpimBase::DateDiff), you have to use the date/time in the query object and the startperiod (the last parameter).
     * @see QuerySettings in class OPimBase
     * @param uidlist List of uid's which should be incorporated into the next search
     * @param query The object which contains the query set
     * @param querySettings This parameter defines what should be searched and how the query should be interpreted
     * @param startperiod Defines the start of a period for some special queries.
     */
    virtual List queryByExample( const OPimAccessTemplate::List& uidlist, const T& query, int querySettings,
				 const QDateTime& startperiod = QDateTime() );

    virtual T find( UID uid )const;
    virtual T find( UID uid, const QArray<int>&,
		    uint current, typename OTemplateBase<T>::CacheDirection dir = OTemplateBase<T>::Forward )const;
    //@}

    /**
     * Get sorted lists..
     * @see OPimContactAccess, OPimTodoAccess and ODateBookAccess regarding more info for the following params:
     * @param list of UID's received by allRecords() or others...
     * @param sortOrder Setting the sort order defined by enum SortOrder
     * @param ascending Sort in ascending order if true, otherwise descending
     * @param sortFilter Setting the sort filter defined by enum SortFilter
     * @param cat number of category.
     */
    virtual List sorted( const List& list, bool ascending, int sortOrder,
			 int sortFilter, int cat )const;

    /**
     * Get sorted lists..
     * @see OPimContactAccess, OPimTodoAccess and ODateBookAccess regarding more info for the following params:
     * @param list of UID's received by allRecords() or others...
     * @param sortOrder Setting the sort order defined by enum SortOrder
     * @param ascending Sort in ascending order if true, otherwise descending
     * @param sortFilter Setting the sort filter defined by enum SortFilter
     * @param cats List of categories.
     */
    virtual List sorted( const List& list, bool ascending, int sortOrder,
			 int sortFilter, const QArray<UID>& cats )const;

    /**
     * Get sorted lists..
     * @see OPimContactAccess, OPimTodoAccess and ODateBookAccess regarding more info for the following params:
     * @param ascending Sort in ascending order if true, otherwise descending
     * @param sortOrder Setting the sort order defined by enum SortOrder
     * @param sortFilter Setting the sort filter defined by enum SortFilter
     * @param cat number of category.
     */
    virtual List sorted( bool ascending, int sortOrder, int sortFilter, int cat )const;

    /**
     * Get sorted lists..
     * @see OPimContactAccess, OPimTodoAccess and ODateBookAccess regarding more info for the following params:
     * @param ascending Sort in ascending order if true, otherwise descending
     * @param sortOrder Setting the sort order defined by enum SortOrder
     * @param sortFilter Setting the sort filter defined by enum SortFilter
     * @param cats List of categories.
     */
    virtual List sorted( bool ascending, int sortOrder, int sortFilter,
			 const QArray<UID>& cats )const;
    /**
     * (Re)Implementation
     */
    //@{
    UIDArray matchRegexpSimple( const QRegExp& r )const;
    UIDArray queryByExampleSimple( const OPimRecord*, int, const QDateTime& )const;
    UIDArray sortedSimple( const UIDArray&, bool asc, int sortOrder,
                           int sortFilter, int cat )const;
    UIDArray sortedSimple( const UIDArray&, bool asc, int sortOrder,
                           int sortFilter, const QArray<int>& )const;
    UIDArray sortedSimple( bool ascending, int sortOrder, int sortFilter,
                           int cat )const;
    UIDArray sortedSimple( bool ascending, int sortOrder, int sortFilter,
                           const QArray<int>& )const;
    OPimOccurrence::List occurrences( const QDate& start,  const QDate& end )const;
    OPimOccurrence::List occurrences( const QDateTime& dt )const;
    //@}

    //@{
    virtual bool add( const T& t ) ;
    bool add( const OPimRecord& );
    bool add( const OPimRecord* );
    virtual bool remove( const T& t );
    bool remove( UID uid );
    bool remove( const OPimRecord& );
    virtual bool replace( const T& t) ;

    //@}

    void setReadAhead( uint count );
    void cache( const T& )const;
    void setSaneCacheSize( int );

    QArray<UID> records()const;
protected:
    /**
     * invalidate the cache
     */
    void invalidateCache();

    void setBackEnd( BackEnd* end );
    /**
     * returns the backend
     */
    BackEnd* backEnd();
    BackEnd* m_backEnd;

    Cache m_cache;

private:
    OPimAccessTemplatePrivate *d;

};

/**
 * c'tor BackEnd
 * enum Access a small hint on how to handle the backend
 */
template <class T>
OPimAccessTemplate<T>::OPimAccessTemplate( BackEnd* end )
    : OTemplateBase<T>(), m_backEnd( end ), d( 0 )
{
    if ( end )
        end->setFrontend( this );
}
template <class T>
OPimAccessTemplate<T>::~OPimAccessTemplate() {
    delete m_backEnd;
}

/**
 * load from the backend
 */
template <class T>
bool OPimAccessTemplate<T>::load() {
    invalidateCache();
    return m_backEnd->load();
}

/** Reload database.
 * You should execute this function if the external database
 * was changed.
 * This function will load the external database and afterwards
 * rejoin the local changes. Therefore the local database will be set consistent.
 */
template <class T>
bool OPimAccessTemplate<T>::reload() {
    invalidateCache();
    return m_backEnd->reload();
}

/**
 * Save contacts database.
 * Save is more a "commit". After calling this function, all changes are public available.
 * @return true if successful
 */
template <class T>
bool OPimAccessTemplate<T>::save() {
    return m_backEnd->save();
}


/**
 * return a List of records
 * you can iterate over them
 */
template <class T>
typename OPimAccessTemplate<T>::List OPimAccessTemplate<T>::allRecords()const {
    QArray<int> ints = m_backEnd->allRecords();
    List lis(ints, this );
    return lis;
}

/**
 * return a List of records
 * that match the regex
 */
template <class T>
typename OPimAccessTemplate<T>::List OPimAccessTemplate<T>::matchRegexp( const QRegExp &r )const {
    QArray<int> ints = m_backEnd->matchRegexp( r );
    List lis(ints, this );
    return lis;
}

/**
 * find the OPimRecord uid
 */
template <class T>
QArray<int> OPimAccessTemplate<T>::records()const {
    return m_backEnd->allRecords();
}

template <class T>
uint OPimAccessTemplate<T>::querySettings(){
	return m_backEnd->querySettings();
}

template <class T>
bool OPimAccessTemplate<T>::hasQuerySettings ( int querySettings ) const {
	return m_backEnd->hasQuerySettings( querySettings );
}


template <class T>
typename OPimAccessTemplate<T>::List
OPimAccessTemplate<T>::queryByExample( const T& t, int settings, const QDateTime& d ) {
    QArray<int> ints = m_backEnd->queryByExample( t, settings, d );

    List list(ints, this );
    return list;
}

template <class T>
typename OPimAccessTemplate<T>::List
OPimAccessTemplate<T>::queryByExample( const OPimRecord* t, int settings, const QDateTime& d ) {
    T tempInstance;

    if ( t->rtti() == tempInstance.rtti() ) {
	    QArray<int> ints = m_backEnd->queryByExample( t, settings, d );
	    List list( ints, this );
	    return list;
    } else {
	owarn << "Query not possible: Objecttype mismatch" << oendl;
    }

    return List();
}

template <class T>
typename OPimAccessTemplate<T>::List
OPimAccessTemplate<T>::queryByExample( const OPimAccessTemplate::List& uidlist, const T& t, int settings, const QDateTime& d ) {
    QArray<int> ints = m_backEnd->queryByExample( uidlist.uids(), t, settings, d );

    List list( ints, this );
    return list;
}


template <class T>
T OPimAccessTemplate<T>::find( UID uid ) const{
    // First search in cache..
    if ( m_cache.contains( uid ) )
	    return m_cache.find( uid );

    T t = m_backEnd->find( uid );
    cache( t );

    return t;
}


/**
 * read ahead cache find method ;)
 */
template <class T>
T OPimAccessTemplate<T>::find( UID uid, const QArray<int>& ar,
                               uint current, typename OTemplateBase<T>::CacheDirection dir )const {
    /*
     * better do T.isEmpty()
     * after a find this way we would
     * avoid two finds in QCache...
     */
    if (m_cache.contains( uid ) )
        return m_cache.find( uid );


    T t = m_backEnd->find( uid, ar, current, dir );
    cache( t );
    return t;
}

/**
 * clears the backend and invalidates the backend
 */
template <class T>
void OPimAccessTemplate<T>::clear() {
    invalidateCache();
    m_backEnd->clear();
}


/**
 * add T to the backend
 * @param t The item to add.
 * @return <i>true</i> if added successfully.
 */
template <class T>
bool OPimAccessTemplate<T>::add( const T& t ) {
    cache( t );
    return m_backEnd->add( t );
}

template <class T>
bool OPimAccessTemplate<T>::add( const OPimRecord& rec ) {
    /* same type */
    T tempInstance;
    if ( rec.rtti() == tempInstance.rtti() ) {
        const T& t = static_cast<const T&>(rec);
        return add(t);
    } else {
	owarn << "Adding not possible: Objecttype mismatch" << oendl;
    }
    return false;
}

/**
 * Add an Opie PimRecord.
 * Info: Take this if  you are working with OPimRecords and you need to add it into any database.
 *       But take care that the accessing database is compatible to the real type of OPimRecord !!
 *       Otherwise this access will be rejected !
 */
template <class T>
bool OPimAccessTemplate<T>::add( const OPimRecord* rec) {
    /* same type, but pointer  */
    T tempInstance;
    if ( rec -> rtti() == tempInstance.rtti() ) {
        const T* t = static_cast<const T*>(rec);
        return add( *t );
    } else {
	owarn << "Adding not possible: Objecttype mismatch" << oendl;
    }
    return false;
}

/**
 * remove T from the backend
 * @param t The item to remove
 * @return <i>true</i> if successful.
 */
template <class T>
bool OPimAccessTemplate<T>::remove( const T& t ) {
    return remove( t.uid() );
}

/**
 * remove the OPimRecord with uid
 * @param uid The ID of the item to remove
 * @return <i>true</i> if successful.
 */
template <class T>
bool OPimAccessTemplate<T>::remove( UID uid ) {
    m_cache.remove( uid );
    return m_backEnd->remove( uid );
}
template <class T>
bool OPimAccessTemplate<T>::remove( const OPimRecord& rec) {
    return remove( rec.uid() );
}

/**
 * replace T from backend
 * @param t The item to replace
 * @return <i>true</i> if successful.
 */
template <class T>
bool OPimAccessTemplate<T>::replace( const T& t ) {
    m_cache.replace( t );
    return m_backEnd->replace( t );
}

/**
 * @internal
 */
template <class T>
void OPimAccessTemplate<T>::invalidateCache() {
    m_cache.invalidate();
}
template <class T>
typename OPimAccessTemplate<T>::BackEnd* OPimAccessTemplate<T>::backEnd() {
    return m_backEnd;
}

/**
 * if the resource was changed externally
 * You should use the signal handling instead of polling possible changes !
 * zecke: Do you implement a signal for otodoaccess ?
 */
template <class T>
bool OPimAccessTemplate<T>::wasChangedExternally()const {
    return false;
}
template <class T>
void OPimAccessTemplate<T>::setBackEnd( BackEnd* end ) {
    m_backEnd = end;
    if (m_backEnd )
        m_backEnd->setFrontend( this );
}
template <class T>
void OPimAccessTemplate<T>::cache( const T& t ) const{
    /* hacky we need to work around the const*/
    ((OPimAccessTemplate<T>*)this)->m_cache.add( t );
}
template <class T>
void OPimAccessTemplate<T>::setSaneCacheSize( int size ) {
    m_cache.setSize( size );
}
template <class T>
void OPimAccessTemplate<T>::setReadAhead( uint count ) {
    m_backEnd->setReadAhead( count );
}


template <class T>
typename OPimAccessTemplate<T>::List OPimAccessTemplate<T>::sorted( const OPimAccessTemplate::List& lst,
                                                                    bool ascending, int sortOrder,
                                                                    int sortFilter, int cat )const {
    QArray<int> cats( 1 );
    cats[0] = cat;
    UIDArray ints = m_backEnd->sorted( lst.uids(), ascending, sortOrder,
                                       sortFilter, cats );
    return List(ints, this);
}

template<class T>
typename OPimAccessTemplate<T>::List OPimAccessTemplate<T>::sorted( const OPimAccessTemplate::List& lst,
                                                                    bool ascending, int sortOrder,
                                                                    int sortFilter, const QArray<UID>& cats )const {
    UIDArray ints = m_backEnd->sorted( lst.uids(), ascending, sortOrder,
                                       sortFilter, cats );
    return List(ints, this);
}

template<class T>
typename OPimAccessTemplate<T>::List OPimAccessTemplate<T>::sorted( bool ascending, int sortOrder,
                                                                    int sortFilter, int cat )const {
    QArray<int> cats( 1 );
    cats[0] = cat;
    UIDArray ints = m_backEnd->sorted( ascending, sortOrder,
                                       sortFilter, cats );
    return List(ints, this);
}

template<class T>
typename OPimAccessTemplate<T>::List OPimAccessTemplate<T>::sorted( bool ascending, int sortOrder,
                                                                    int sortFilter, const QArray<UID>& cats )const {
    UIDArray ints = m_backEnd->sorted( ascending, sortOrder,
                                       sortFilter, cats );
    return List(ints, this);
}

template <class T>
OPimOccurrence::List OPimAccessTemplate<T>::occurrences( const QDate& start,
                                                         const QDate& end ) const {
    /*
     * Some magic involved to go from single OPimBackendOccurrence
     * to multiple OPimOccurrence's
     */
    return OPimBase::convertOccurrenceFromBackend( m_backEnd->occurrences( start, end ) );
}

template<class T>
OPimOccurrence::List OPimAccessTemplate<T>::occurrences( const QDateTime& dt )const {
    return OPimBase::convertOccurrenceFromBackend(  m_backEnd->occurrences( dt ) );
}

/*
 *Implementations!!
 */
template <class T>
UIDArray OPimAccessTemplate<T>::matchRegexpSimple( const QRegExp &r )const {
    return m_backEnd->matchRegexp( r );
}

template <class T>
UIDArray OPimAccessTemplate<T>::queryByExampleSimple( const OPimRecord* rec,
                                                      int settings,
                                                      const QDateTime& d )const {
    return m_backEnd->queryByExample( rec, settings, d );
}

template <class T>
UIDArray OPimAccessTemplate<T>::sortedSimple( const UIDArray& lst,
                                              bool ascending,
                                              int sortOrder, int sortFilter,
                                              int cat ) const{
    QArray<int> cats( 1 );
    cats[0] = cat;
    return m_backEnd->sorted( lst, ascending, sortOrder, sortFilter, cats );
}

template <class T>
UIDArray OPimAccessTemplate<T>::sortedSimple( const UIDArray& lst,
                                              bool ascending,
                                              int sortOrder, int sortFilter,
                                              const QArray<int>& cats ) const{
    return m_backEnd->sorted( lst, ascending, sortOrder, sortFilter, cats );
}

template <class T>
UIDArray OPimAccessTemplate<T>::sortedSimple( bool ascending,
                                              int sortOrder, int sortFilter,
                                              int cat ) const{
    QArray<int> cats( 1 );
    cats[0] = cat;

    return m_backEnd->sorted( ascending, sortOrder, sortFilter, cats );
}

template <class T>
UIDArray OPimAccessTemplate<T>::sortedSimple( bool ascending,
                                              int sortOrder, int sortFilter,
                                              const QArray<int>& cats ) const{
    return m_backEnd->sorted( ascending, sortOrder, sortFilter, cats );
}
}

#endif
