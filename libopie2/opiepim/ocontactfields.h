#ifndef OPIE_CONTACTS_FIELDS
#define OPIE_CONTACTS_FIELDS

class QStringList;

#include <qmap.h>
#include <qstring.h>
#include <opie/ocontact.h>

#define CONTACT_FIELD_ORDER_NAME "opie-contactfield-order"
#define DEFAULT_FIELD_ORDER "__________"
 
class OContactFields{

 public:
    OContactFields();
    ~OContactFields();
    /** Set the index for combo boxes.
     *  Sets the <b>index</b> of combo <b>num</b>.
     *  @param num selects the number of the combo
     *  @param index sets the index in the combo
     */
    void setFieldOrder( int num, int index );

    /** Get the index for combo boxes.
     * Returns the index of combo <b>num</b> or defindex
     * if none was defined..
     * @param num Selects the number of the combo
     * @param defIndex will be returned if none was defined (either 
     * globally in the config file, nor by the contact which was used 
     * by loadFromRecord() )
     */
    int getFieldOrder( int num, int defIndex);

    /** Store fieldorder to contact. */
    void saveToRecord( OContact& );
    /** Get Fieldorder from contact. */
    void loadFromRecord( const OContact& );

 private:
    QString fieldOrder;
    QString globalFieldOrder;
    bool changedFieldOrder;

 public:
    static QStringList personalfields( bool sorted = true, bool translated = false );
    static QStringList phonefields( bool sorted = true, bool translated = false );
    static QStringList detailsfields( bool sorted = true, bool translated = false );
    static QStringList fields( bool sorted = true, bool translated = false );
    
    static QStringList trpersonalfields( bool sorted = true );
    static QStringList untrpersonalfields( bool sorted = true );
    static QStringList trphonefields( bool sorted = true );
    static QStringList untrphonefields( bool sorted = true );
    static QStringList trdetailsfields( bool sorted = true );
    static QStringList untrdetailsfields( bool sorted = true );
    static QStringList trfields( bool sorted = true );
    static QStringList untrfields( bool sorted = true );

    static QMap<int, QString> idToTrFields();
    static QMap<QString, int> trFieldsToId();
    static QMap<int, QString> idToUntrFields();
    static QMap<QString, int> untrFieldsToId();

};

#endif
