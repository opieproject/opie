
#ifndef OSQL_BACKEND_H
#define OSQL_BACKEND_H

#include <qcstring.h>
#include <qstring.h>
#include <qvaluelist.h>

/**
 * OSQLBackEnd represents an available backend
 * to the Opie Database Service
 * It's used to easily extend OSQL services by
 * 3rd party plugins.
 * It's used to show
 */
class OSQLBackEnd  /*: public QShared */ {
public:
    typedef QValueList<OSQLBackEnd> ValueList;
    /**
     * A basic c'tor
     * @param name the user visible name of the service
     * @param vendor the vendor of the service
     * @param license the license of the service
     * @param library what is the name of lib if builtin it's builtin
     */
    OSQLBackEnd( const QString& name = QString::null,
                 const QString& vendor = QString::null,
                 const QString& license = QString::null,
                 const QCString& library = QCString() );
    OSQLBackEnd( const OSQLBackEnd& );
    OSQLBackEnd &operator=( const OSQLBackEnd& );
    bool operator==(const OSQLBackEnd& );

    ~OSQLBackEnd();

    /** @return the name */
    QString name()const;

    /** @return the vendor */
    QString vendor()const;

    /** @return the license */
    QString license()const;

    /** @return the name of the library */
    QCString library() const;

    bool isDefault()const;
    int preference()const;

    /** @param name the name to set */
    void setName( const QString& name );

    /** @param vendor the vendor to set */
    void setVendor( const QString& vendor );

    /** @param license the license applied */
    void setLicense( const QString& license );

    /** @param the lib to set */
    void setLibrary( const QCString& lib );

    void setDefault( bool );
    void setPreference( int );

private:
    QString m_name;
    QString m_vendor;
    QString m_license;
    QCString m_lib;
    bool m_default :1;
    int m_pref;
};

#endif
