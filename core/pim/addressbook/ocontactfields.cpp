
#include "ocontactfields.h"
#include <qstringlist.h>
#include <qobject.h>

/*!
  \internal
  Returns a  list of details field names for a contact.
*/
QStringList OContactFields::untrdetailsfields( bool sorted )
{
    QStringList list;

    list.append( "Office" );
    list.append( "Profession" );
    list.append( "Assistant" );
    list.append( "Manager" );
    list.append( "Spouse" );
    list.append( "Gender" );
    list.append( "Birthday" );
    list.append( "Anniversary" );
    list.append( "Nickname" );
    list.append( "Children" );

    if (sorted) list.sort();

    return list;
}

/*!
  \internal
  Returns a translated list of phone field names for a contact.
*/
QStringList OContactFields::trphonefields( bool sorted )
{
    QStringList list;
    list.append( QObject::tr( "Business Phone" ) );
    list.append( QObject::tr( "Business Fax" ) );
    list.append( QObject::tr( "Business Mobile" ) );

    list.append( QObject::tr( "Default Email" ) );
    list.append( QObject::tr( "Emails" ) );

    list.append( QObject::tr( "Home Phone" ) );
    list.append( QObject::tr( "Home Fax" ) );
    list.append( QObject::tr( "Home Mobile" ) );

    if (sorted) list.sort();

    return list;
}


/*!
  \internal
  Returns a translated list of details field names for a contact.
*/
QStringList OContactFields::trdetailsfields( bool sorted )
{
    QStringList list;

    list.append( QObject::tr( "Office" ) );
    list.append( QObject::tr( "Profession" ) );
    list.append( QObject::tr( "Assistant" ) );
    list.append( QObject::tr( "Manager" ) );

    list.append( QObject::tr( "Spouse" ) );
    list.append( QObject::tr( "Gender" ) );
    list.append( QObject::tr( "Birthday" ) );
    list.append( QObject::tr( "Anniversary" ) );
    list.append( QObject::tr( "Nickname" ) );
    list.append( QObject::tr( "Children" ) );

    if (sorted) list.sort();
    return list;
}


/*!
  \internal
  Returns a translated list of field names for a contact.
*/
QStringList OContactFields::trfields( bool sorted )
{
    QStringList list;

    list.append( QObject::tr( "Name Title") );
    list.append( QObject::tr( "First Name" ) );
    list.append( QObject::tr( "Middle Name" ) );
    list.append( QObject::tr( "Last Name" ) );
    list.append( QObject::tr( "Suffix" ) );
    list.append( QObject::tr( "File As" ) );

    list.append( QObject::tr( "Job Title" ) );
    list.append( QObject::tr( "Department" ) );
    list.append( QObject::tr( "Company" ) );

    list += trphonefields();

    list.append( QObject::tr( "Business Street" ) );
    list.append( QObject::tr( "Business City" ) );
    list.append( QObject::tr( "Business State" ) );
    list.append( QObject::tr( "Business Zip" ) );
    list.append( QObject::tr( "Business Country" ) );
    list.append( QObject::tr( "Business Pager" ) );
    list.append( QObject::tr( "Business WebPage" ) );

    list.append( QObject::tr( "Home Street" ) );
    list.append( QObject::tr( "Home City" ) );
    list.append( QObject::tr( "Home State" ) );
    list.append( QObject::tr( "Home Zip" ) );
    list.append( QObject::tr( "Home Country" ) );
    list.append( QObject::tr( "Home Web Page" ) );
    list += trdetailsfields();
    list.append( QObject::tr( "Notes" ) );
    list.append( QObject::tr( "Groups" ) );

    if (sorted) list.sort();

    return list;
}

/*!
  \internal
  Returns a list of phone field names for a contact.
*/
QStringList OContactFields::untrphonefields( bool sorted )
{
    QStringList list;

    list.append( "Business Phone" );
    list.append( "Business Fax" );
    list.append( "Business Mobile" );

    list.append( "Default Email" );
    list.append( "Emails" );

    list.append( "Home Phone" );
    list.append( "Home Fax" );
    list.append( "Home Mobile" );

    if (sorted) list.sort();

    return list;
}

/*!
  \internal
  Returns an untranslated list of field names for a contact.
*/
QStringList OContactFields::untrfields( bool sorted )
{
    QStringList list;

    list.append( "Name Title" );
    list.append( "First Name" );
    list.append( "Middle Name" );
    list.append( "Last Name" );
    list.append( "Suffix" );
    list.append( "File As" );

    list.append( "Job Title" );
    list.append( "Department" );
    list.append( "Company" );
    list += untrphonefields();
    list.append( "Business Street" );
    list.append( "Business City" );
    list.append( "Business State" );
    list.append( "Business Zip" );
    list.append( "Business Country" );
    list.append( "Business Pager" );
    list.append( "Business WebPage" );

    list.append( "Office" );
    list.append( "Profession" );
    list.append( "Assistant" );
    list.append( "Manager" );

    list.append( "Home Street" );
    list.append( "Home City" );
    list.append( "Home State" );
    list.append( "Home Zip" );
    list.append( "Home Country" );
    list.append( "Home Web Page" );

    list.append( "Spouse" );
    list.append( "Gender" );
    list.append( "Birthday" );
    list.append( "Anniversary" );
    list.append( "Nickname" );
    list.append( "Children" );

    list.append( "Notes" );
    list.append( "Groups" );

    if (sorted) list.sort();

    return list;
}
