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
#ifndef OPIE_PIM_ACCESS_BACKEND
#define OPIE_PIM_ACCESS_BACKEND

#include <qarray.h>
#include <qdatetime.h>

#include <opie2/opimtemplatebase.h>
#include <opie2/opimrecord.h>
#include <opie2/opimbackendoccurrence.h>

namespace Opie {
class OPimAccessBackendPrivate;

/**
 * OPimAccessBackend is the Backend Interface to be used
 * by OTemplateBase based Frontends.
 * For efficency reasons and to support delayed loading
 * most of the Frontend functions can be implemented
 * by this backend.
 * This allows to utilise the best method on each backend.
 * For example we can use SQL queries instead of self made
 * query which is first more efficent and also uses less memory.
 */
template <class T = OPimRecord>
class OPimAccessBackend {
public:
    typedef OTemplateBase<T> Frontend;

    //@{
    OPimAccessBackend(int access = 0);
    virtual ~OPimAccessBackend();
    //@}

    //@{
    virtual bool load() = 0;
    virtual bool reload() = 0;
    virtual bool save() = 0;
    virtual void clear() = 0;
    //@}


    //@{
    virtual UIDArray allRecords()const = 0;
    virtual UIDArray matchRegexp(const QRegExp &r) const;
    virtual UIDArray queryByExample( const T& t, int settings, const QDateTime& d = QDateTime() )const = 0;
    virtual UIDArray queryByExample( const OPimRecord* rec, int, const QDateTime& d = QDateTime() )const;
    virtual UIDArray sorted( const UIDArray&, bool asc, int sortOrder, int sortFilter,  const QArray<int>& cats )const;
    virtual UIDArray sorted( bool asc, int sortOrder, int sortFilter, const QArray<int>& cats )const;
    virtual OPimBackendOccurrence::List occurrences( const QDate& start, const QDate& end)const;
    virtual OPimBackendOccurrence::List occurrences( const QDateTime& dt )const;
    //@}


    //@{
    virtual T find(UID uid )const  = 0;
    virtual T find(UID uid, const QArray<UID>& items,
                   uint current, typename Frontend::CacheDirection )const ;
    //@}


    //@{
    virtual bool add( const T& t ) = 0;
    virtual bool remove( UID uid ) = 0;
    virtual bool replace( const T& t ) = 0;
    //@}



    void setFrontend( Frontend* front );

    /**
     * set the read ahead count
     */
    void setReadAhead( uint count );
protected:
    //@{
    int access()const;
    void cache( const T& t )const;
    void setSaneCacheSize( int );
    uint readAhead()const;
    //@}

private:
    OPimAccessBackendPrivate *d;
    Frontend* m_front;
    uint m_read;
    int m_acc;

};

template <class T>
OPimAccessBackend<T>::OPimAccessBackend(int acc)
    : m_acc( acc )
{
    m_front = 0l;
}
template <class T>
OPimAccessBackend<T>::~OPimAccessBackend() {

}

/*
 * Slow but default matchRegexp Implementation
 * Create a Big Enough QArray and then iterate
 * over all Records and matchRegexp them.
 * At the end we will resize the array to the actual
 * number of items
 */
template <class T>
UIDArray OPimAccessBackend<T>::matchRegexp( const QRegExp& reg )const {
    UIDArray all_rec = allRecords();
    UIDArray result( all_rec.count() );
    uint used_records = 0, all_rec_count = all_rec.count();

    for ( uint i = 0; i < all_rec_count; ++i )
        if (find( all_rec[i], all_rec, i, Frontend::Forward ).match( reg ) )
            result[used_records++] = all_rec[i];

    /* shrink to fit */
    result.resize( used_records );
    return result;
}

template <class T>
UIDArray OPimAccessBackend<T>::queryByExample( const OPimRecord* rec, int settings,
                                               const QDateTime& datetime )const {
    T* tmp_rec = T::safeCast( rec );
    UIDArray ar;
    if ( tmp_rec )
        ar = queryByExample( *tmp_rec, settings, datetime );

    return ar;
}

template <class T>
UIDArray OPimAccessBackend<T>::sorted( const UIDArray& ids, bool,
                                       int, int, const QArray<int>& ) const {
    return ids;
}

template <class T>
UIDArray OPimAccessBackend<T>::sorted( bool asc, int order, int filter,
                                       const QArray<int>& cats )const {
    return sorted( allRecords(), asc, order, filter, cats );
}

template<class T>
OPimBackendOccurrence::List OPimAccessBackend<T>::occurrences( const QDate&,
                                                               const QDate& )const {
    return OPimBackendOccurrence::List();
}

template<class T>
OPimBackendOccurrence::List OPimAccessBackend<T>::occurrences( const QDateTime& dt )const {
    QDate date = dt.date();
    return occurrences( date, date );
}

template <class T>
void OPimAccessBackend<T>::setFrontend( Frontend* fr ) {
    m_front = fr;
}
template <class T>
void OPimAccessBackend<T>::cache( const T& t )const {
    if ( m_front )
        m_front->cache( t );
}

template <class T>
void OPimAccessBackend<T>::setSaneCacheSize( int size) {
    if ( m_front )
        m_front->setSaneCacheSize( size );
}
template <class T>
T OPimAccessBackend<T>::find( int uid, const QArray<int>&,
                              uint, typename Frontend::CacheDirection ) const{
    qDebug( "*** Lookahead feature not supported. Fallback to default find!!" );
    return find( uid );
}
template <class T>
void OPimAccessBackend<T>::setReadAhead( uint count ) {
    m_read = count;
}
template <class T>
uint OPimAccessBackend<T>::readAhead()const {
    return m_read;
}
template <class T>
int OPimAccessBackend<T>::access()const {
    return m_acc;
}

}

/**
 * \fn template <class T> OPimAccessBackend<T>::OPimAccessBackend(int hint )
 * @param hint The access hint from the frontend
 */

/**
 * \fn template <class T> bool OPimAccessBackend<T>::load()
 *  Opens the DataBase and does necessary
 *  initialisation of internal structures.
 *
 *  @return true If the DataBase could be opened and
 *          Information was successfully loaded
 */

/**
 * \fn template <class T> bool OPimAccessBackend<T>::reload()
 * Reinitialise the DataBase and merges the external changes
 * with your local changes.
 *
 * @return True if the DataBase was reloaded.
 *
 */

/**
 * \fn template <class T> bool OPimAccessBackend<T>::save()
 *
 * Save the changes to storage. In case of memory or
 * disk shortage, return false.
 *
 *
 * @return True if the DataBase could be saved to storage.
 */

/**
 * \fn template <class T> bool OPimAccessBackend<T>::clear()
 * Until a \sa save() changes shouldn't be comitted
 *
 *
 * @return True if the DataBase could be cleared
 * @todo Introduce a 'Commit'
 */

/**
 * \fn template <class T> QArray<UID> OPimAccessBackend<T>::allRecords()const
 *  Return an array of  all available uids in the loaded
 *  DataBase.
 *  @see load
 */

/**
 * \fn template <class T> QArray<UID> OPimAccessBackend<T>::matchRegexp(const QRegExp& r)const
 * Return a List of records  that match the regex \par r.
 *
 * @param r The QRegExp to match.
 */

/**
 * \fn template <class T> QArray<UID> OPimAccessBackend<T>::queryByExample(const T& t, int settings, const QDateTime& d = QDateTime() )
 *
 * Implement QueryByExample. An Example record is filled and with the
 * settings and QDateTime it is determined how the query should be executed.
 * Return a list of UIDs that match the Example
 *
 * @param t The Example record
 * @param settings Gives
 *
 */

/**
 * \fn template<class T> QArray<UID> OPimAccessBackend<T>::sorted(const QArray<UID>& ids, bool asc, int sortOrder, int sortFilter, int cat)
 * \brief Sort the List of records according to the preference
 *
 * Implement sorting in your backend. The default implementation is
 * to return the list as it was passed.
 * The default Backend Implementation should do unaccelerated filtering
 *
 *
 * @param ids        The Records to sort
 * @param asc        Sort ascending or descending
 * @param sortOrder
 * @param sortFilter Sort filter
 * @param cat        The Category to include
 */

/**
 * \fn template <class T> T OPimAccessBackend<T>::find(UID uid)const
 * \brief Find the Record with the UID
 *
 * Find the UID in the database and return the record.
 * @param uid The uid to be searched for
 * @return The record or an empty record (T.isEmpty())
 *
 */

/**
 * \fn template <class T> T OPimAccessBackend<T>::find( UID uid, const QArray<UID>& items, uint current, typename Frontend::CacheDirection d)const
 * \brief find a Record and do a read ahead or read behind
 *
 * @param uid     The UID to search for
 * @param items   The list of items from where your search
 * @param current The index of \param uid
 * @param d       The direction to search for
 *
 * @see find
 */


/**
 * \fn template<class T> bool OPimAccessBackend<T>::add(const T& t)
 *
 * \brief Add the record to the internal database
 *
 * If an record with the same t.uid() is already present internally
 * the behaviour is undefined but the state of the database
 * needs to be stable.
 * For modifying a record use \sa replace.
 *
 *
 * @return true if the record could be added or false if not
 * @todo Eilers your opinion on readd/replace
 */

/**
 * \fn template<class T> bool OPimAccessBackend<T>::remove(UID uid)
 * \brief Remove a record by its UID
 *
 * Remove the records with UID from the internal Database.
 *
 * @return True if the record could be removed.
 *
 */

/**
 * \fn template<class T> bool OPimAccessBackend<T>::replace(const T& t)
 * \brief Take this Record and replace the old version.
 *
 * Take \param t as the new record for t.uid(). It is not described
 * what happens if the record is not present in the database.
 * Normally the record is determined by the UID.
 *
 * @param t The record to use internally.
 */

/**
 * \fn template<class T> void OPimAccessBackend<T>::setFrontend( Frontend* fron)
 * \@aram fron The Frontend that uses this backend
 *
 * This function is called by the frontend and is used
 */

/**
 * \fn template<class T> void OPimAccessBackend<T>::setReadAhead(uint count)
 * \brief Set the number of items to Read-Ahead/Read-Behind
 *
 * @param count The number of records to read ahead
 */

/**
 * \fn template<class T> void OPimAccessBackend<T>::cache( const T& t)const
 * \brief Add the Record to the PIM Cache
 *
 * This will add the Record to the PIM cache, which is owned
 * by the FrontEnd. If no FrontEnd is available the item will
 * not be cached.
 *
 *
 * @param t The Item to be added to the Cache
 */

/**
 * \fn template<class T> void OPimAccessBackend<T>::setSaneCacheSize(int items)
 * \brief Give a hint on the number of too cached items
 *
 * Give the Frontend a hint on the number of items to be cached. Use
 * a prime number for best performance.
 *
 * @param items The number of items to be cached
 */

/**
 * \fn template<class T> uint OPimAccessBackend<T>::readAhead()const
 * \brief Return the number of Items to be ReadAhead
 *
 * @return The number of Items to read ahead/read behind
 */

/**
 * \fn template<class T> QArray<OPimBackendOccurence> OPimAccessBackend<T>::occurrences(const QDateTime& start,const QDateTime& end)
 * \brief Get a List of Occurrences for a period of time
 *
 * Return an Array of OPimBackendOccurence for a period of time. If start == end date
 * return only occurrences for the start date. If end is smaller the start date
 * the result is not defined. You could switch dates or return an empty list.
 *
 * @return Return an array of OPimBackendOccurence for the period specified by the parameters
 * @param start The start of the period.
 * @param  end  The end   of the period.
 *
 */

#endif
