#ifndef OPIE_PIM_ACCESS_TEMPLATE_H
#define OPIE_PIM_ACCESS_TEMPLATE_H

#include <qarray.h>

#include <opie/opimrecord.h>
#include <opie/opimaccessbackend.h>
#include <opie/orecordlist.h>

template <class T = OPimRecord >
class OPimAccessTemplate {
public:
    typedef ORecordList<T> List;
    typedef OPimAccessBackend<T> BackEnd;
    OPimAccessTemplate( BackEnd* end)
        : m_backEnd( end ) {
    }
    ~OPimAccessTemplate() {
        delete m_backEnd;
    }
    virtual void load() {
        m_backEnd->load();
    }
    virtual void reload() {
        m_backEnd->reload();
    }
    virtual void save() {
        m_backEnd->save();
    }

    /*
     *do array to Records conversion
     * QArray<int> ids
     */
    virtual List allRecords()const {
        QArray<int> ints = m_backEnd->allRecords();

        List lis( ints,  this );
        return lis;
    }
    virtual List queryByExample( const T& t, int sortOrder ) {
        QArray<int> ints = m_backEnd->query( t, sortOrder );
        List lis( ints, this );

        return lis;
    }
    /* implement QCache here */
    virtual T find( int uid ) {
        T t = m_backEnd->find( uid );
        return t;
    }

    /* invalidate cache here */
    virtual void clear() {
        invalidateCache();
        m_backEnd->clear();
    }
    virtual bool add( const T& t ) {
        return m_backEnd->add( t );
    }

    /* only the uid matters */
    virtual bool remove( const T& t ) {
        /* remove from cache */
        return m_backEnd->remove( t.uid() );
    }
    virtual bool remove( int uid ) {
        /* remove from cache */
        return m_backEnd->remove(uid);
    }
    virtual bool replace( const T& t) {
        return m_backEnd->replace( t );
    }
protected:
    void invalidateCache() {

    }
    BackEnd* m_backEnd;

};

#endif
