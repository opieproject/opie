#ifndef OPIE_PIM_ACCESS_TEMPLATE_H
#define OPIE_PIM_ACCESS_TEMPLATE_H

#include <qarray.h>

#include <opie/opimrecord.h>
#include <opie/opimaccessbackend.h>
#include <opie/orecordlist.h>

#include "otemplatebase.h"

template <class T = OPimRecord >
class OPimAccessTemplate : public OTemplateBase<T> {
public:
    typedef ORecordList<T> List;
    typedef OPimAccessBackend<T> BackEnd;
    OPimAccessTemplate( BackEnd* end);
    virtual ~OPimAccessTemplate();
    virtual void load();
    virtual void reload();
    virtual void save();

    /*
     *do array to Records conversion
     * QArray<int> ids
     */
    virtual List allRecords()const;
    virtual List queryByExample( const T& t, int sortOrder );
    virtual T find( int uid );

    /* invalidate cache here */
    virtual void clear() ;
    virtual bool add( const T& t ) ;

    /* only the uid matters */
    virtual bool remove( const T& t );
    virtual bool remove( int uid );
    virtual bool replace( const T& t) ;
protected:
    void invalidateCache();
    BackEnd* backEnd();
    BackEnd* m_backEnd;

};

template <class T>
OPimAccessTemplate<T>::OPimAccessTemplate( BackEnd* end )
    : OTemplateBase<T>(), m_backEnd( end )
{

}
template <class T>
OPimAccessTemplate<T>::~OPimAccessTemplate() {
    qWarning("~OPimAccessTemplate<T>");
    delete m_backEnd;
}
template <class T>
void OPimAccessTemplate<T>::load() {
    m_backEnd->load();
}
template <class T>
void OPimAccessTemplate<T>::reload() {
    m_backEnd->reload();
}
template <class T>
void OPimAccessTemplate<T>::save() {
    m_backEnd->save();
}
template <class T>
OPimAccessTemplate<T>::List OPimAccessTemplate<T>::allRecords()const {
    QArray<int> ints = m_backEnd->allRecords();
    List lis(ints, this );
    return lis;
}
template <class T>
OPimAccessTemplate<T>::List
OPimAccessTemplate<T>::queryByExample( const T& t, int sortOrder ) {
    QArray<int> ints = m_backEnd->queryByExample( t, sortOrder );

    List lis(ints, this );
    return lis;
}
template <class T>
T OPimAccessTemplate<T>::find( int uid ) {
    T t = m_backEnd->find( uid );
    return t;
}
template <class T>
void OPimAccessTemplate<T>::clear() {
    invalidateCache();
    m_backEnd->clear();
}
template <class T>
bool OPimAccessTemplate<T>::add( const T& t ) {
    return m_backEnd->add( t );
}
template <class T>
bool OPimAccessTemplate<T>::remove( const T& t ) {
    return m_backEnd->remove( t.uid() );
}
template <class T>
bool OPimAccessTemplate<T>::remove( int uid ) {
    return m_backEnd->remove( uid );
}
template <class T>
bool OPimAccessTemplate<T>::replace( const T& t ) {
    return m_backEnd->replace( t );
}
template <class T>
void OPimAccessTemplate<T>::invalidateCache() {

}
template <class T>
OPimAccessTemplate<T>::BackEnd* OPimAccessTemplate<T>::backEnd() {
    return m_backEnd;
}

#endif
