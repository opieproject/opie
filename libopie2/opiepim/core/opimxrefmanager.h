#ifndef OPIM_XREF_MANAGER_H
#define OPIM_XREF_MANAGER_H

#include <qstringlist.h>

#include <opie/opimxref.h>

/**
 * This is a simple manager for
 * OPimXRefs.
 * It allows addition, removing, replacing
 * clearing and 'querying' the XRef...
 */
class OPimXRefManager {
public:
    OPimXRefManager();
    OPimXRefManager( const OPimXRefManager& );
    ~OPimXRefManager();

    OPimXRefManager& operator=( const OPimXRefManager& );
    bool operator==( const OPimXRefManager& );

    void add( const OPimXRef& );
    void remove( const OPimXRef& );
    void replace( const OPimXRef& );

    void clear();

    /**
     * apps participating
     */
    QStringList apps()const;
    OPimXRef::ValueList list()const;
    OPimXRef::ValueList list( const QString& appName )const;
    OPimXRef::ValueList list( int uid )const;

private:
    OPimXRef::ValueList m_list;
};

#endif
