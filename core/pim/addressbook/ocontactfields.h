#ifndef OPIE_CONTACTS_FIELDS
#define OPIE_CONTACTS_FIELDS

class QStringList;

#include <qmap.h>
#include <qstring.h>
#include <opie/ocontact.h>

#define CONTACT_FIELD_ORDER_NAME "opie-contactfield-order"
#define DEFAULT_FIELD_ORDER ""
 
class OContactFields{

 public:
    OContactFields();
    ~OContactFields();
    void setFieldOrder( int, int );
    int getFieldOrder( int );
    void saveToRecord( OContact& );
    void loadFromRecord( OContact& );

 private:
    QString fieldOrder;

 public:
    static QStringList trphonefields( bool sorted = true );
    static QStringList untrphonefields( bool sorted = true );
    static QStringList trdetailsfields( bool sorted = true );
    static QStringList untrdetailsfields( bool sorted = true );
    static QStringList trfields( bool sorted = true );
    static QStringList untrfields( bool sorted = true );

    static QMap<int, QString> idToTrFields();
    static QMap<QString, int> trFieldsToId();

};

#endif
