#ifndef OPIM_XREF_H
#define OPIM_XREF_H

#include <qarray.h>
#include <qvaluelist.h>

#include <opie/opimxrefpartner.h>

/**
 * this is a Cross Referecne between
 * two Cross Reference Partners
 */
class OPimXRef {
public:
    typedef QValueList<OPimXRef> ValueList;
    enum Partners { One, Two };
    OPimXRef( const OPimXRefPartner& ONE, const OPimXRefPartner& );
    OPimXRef();
    OPimXRef( const OPimXRef& );
    ~OPimXRef();

    OPimXRef &operator=( const OPimXRef& );
    bool operator==( const OPimXRef& );

    OPimXRefPartner partner( enum Partners )const;

    void setPartner( enum Partners,  const OPimXRefPartner& );

    bool containsString( const QString& service)const;
    bool containsUid( int uid )const;

private:
    QArray<OPimXRefPartner> m_partners;

    class Private;
    Private *d;
};

#endif
