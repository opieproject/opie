#ifndef OPIM_XREF_PARTNER_H
#define OPIM_XREF_PARTNER_H

#include <qstring.h>

/**
 * This class represents one partner
 * of a Cross Reference.
 * In Opie one application
 * can link one uid
 * with one tableId( fieldId ) to another.
 */
class OPimXRefPartner {
public:
    OPimXRefPartner( const QString& appName = QString::null,
                     int uid = 0, int field = -1 );
    OPimXRefPartner( const OPimXRefPartner& );
    OPimXRefPartner& operator=( const OPimXRefPartner& );
    ~OPimXRefPartner();

    bool operator==(const OPimXRefPartner& );

    QString appName()const;
    int uid()const;
    int field()const;

    void setAppName( const QString& appName );
    void setUid( int uid );
    void setField( int field );
private:
    QString m_app;
    int m_uid;
    int m_field;

    class Private;
    Private* d;
};


#endif
