#ifndef OPIE_CONTACTS_FIELDS
#define OPIE_CONTACTS_FIELDS

class QStringList;

class OContactFields{

 public:
    static QStringList trphonefields( bool sorted = true );
    static QStringList untrphonefields( bool sorted = true );
    static QStringList trdetailsfields( bool sorted = true );
    static QStringList untrdetailsfields( bool sorted = true );
    static QStringList trfields( bool sorted = true );
    static QStringList untrfields( bool sorted = true );

};

#endif
