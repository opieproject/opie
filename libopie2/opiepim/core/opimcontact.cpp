/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <eilers.stefan@epost.de>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#define QTOPIA_INTERNAL_CONTACT_MRE

#include "opimcontact.h"

/* OPIE */
#include <opie2/opimresolver.h>
#include <opie2/opimdateconversion.h>
#include <opie2/odebug.h>

#include <qpe/stringutil.h>
#include <qpe/timestring.h>
#include <qpe/config.h>

/* QT */
#include <qstylesheet.h>

/* STD */
#include <stdio.h>

/*!
  \class Contact contact.h
  \brief The Contact class holds the data of an address book entry.

  This data includes information the name of the person, contact
  information, and business information such as deparment and job title.

  \ingroup qtopiaemb
  \ingroup qtopiadesktop
*/


namespace Opie
{
/*!
  Creates a new, empty contact.
*/
OPimContact::OPimContact():OPimRecord(), mMap(), d( 0 )
{}

/*!
  \internal
  Creates a new contact.  The properties of the contact are
  set from \a fromMap.
*/
OPimContact::OPimContact( const QMap<int, QString> &fromMap ):OPimRecord(), mMap( fromMap ), d( 0 )
{
    QString cats = mMap[ Qtopia::AddressCategory ];
    if ( !cats.isEmpty() )
        setCategories( idsFromString( cats ) );

    QString uidStr = find( Qtopia::AddressUid );

    if ( uidStr.isEmpty() || ( uidStr.toInt() == 0 ) )
    {
        owarn << "Invalid UID found. Generate new one.." << oendl;
        setUid( uidGen().generate() );
    }
    else
        setUid( uidStr.toInt() );

    //     if ( !uidStr.isEmpty() )
    //  setUid( uidStr.toInt() );
}

/*!
  Destroys a contact.
*/
OPimContact::~OPimContact()
{}

/*! \fn void OPimContact::setTitle( const QString &str )
  Sets the title of the contact to \a str.
*/

/*! \fn void OPimContact::setFirstName( const QString &str )
  Sets the first name of the contact to \a str.
*/

/*! \fn void OPimContact::setMiddleName( const QString &str )
  Sets the middle name of the contact to \a str.
*/

/*! \fn void OPimContact::setLastName( const QString &str )
  Sets the last name of the contact to \a str.
*/

/*! \fn void OPimContact::setSuffix( const QString &str )
  Sets the suffix of the contact to \a str.
*/

/*! \fn void OPimContact::setFileAs( const QString &str )
  Sets the contact to filed as \a str.
*/

/*! \fn void OPimContact::setDefaultEmail( const QString &str )
  Sets the default email of the contact to \a str.
*/

/*! \fn void OPimContact::setHomeStreet( const QString &str )
  Sets the home street address of the contact to \a str.
*/

/*! \fn void OPimContact::setHomeCity( const QString &str )
  Sets the home city of the contact to \a str.
*/

/*! \fn void OPimContact::setHomeState( const QString &str )
  Sets the home state of the contact to \a str.
*/

/*! \fn void OPimContact::setHomeZip( const QString &str )
  Sets the home zip code of the contact to \a str.
*/

/*! \fn void OPimContact::setHomeCountry( const QString &str )
  Sets the home country of the contact to \a str.
*/

/*! \fn void OPimContact::setHomePhone( const QString &str )
  Sets the home phone number of the contact to \a str.
*/

/*! \fn void OPimContact::setHomeFax( const QString &str )
  Sets the home fax number of the contact to \a str.
*/

/*! \fn void OPimContact::setHomeMobile( const QString &str )
  Sets the home mobile phone number of the contact to \a str.
*/

/*! \fn void OPimContact::setHomeWebpage( const QString &str )
  Sets the home webpage of the contact to \a str.
*/

/*! \fn void OPimContact::setCompany( const QString &str )
  Sets the company for contact to \a str.
*/

/*! \fn void OPimContact::setJobTitle( const QString &str )
  Sets the job title of the contact to \a str.
*/

/*! \fn void OPimContact::setDepartment( const QString &str )
  Sets the department for contact to \a str.
*/

/*! \fn void OPimContact::setOffice( const QString &str )
  Sets the office for contact to \a str.
*/

/*! \fn void OPimContact::setBusinessStreet( const QString &str )
  Sets the business street address of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessCity( const QString &str )
  Sets the business city of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessState( const QString &str )
  Sets the business state of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessZip( const QString &str )
  Sets the business zip code of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessCountry( const QString &str )
  Sets the business country of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessPhone( const QString &str )
  Sets the business phone number of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessFax( const QString &str )
  Sets the business fax number of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessMobile( const QString &str )
  Sets the business mobile phone number of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessPager( const QString &str )
  Sets the business pager number of the contact to \a str.
*/

/*! \fn void OPimContact::setBusinessWebpage( const QString &str )
  Sets the business webpage of the contact to \a str.
*/

/*! \fn void OPimContact::setProfession( const QString &str )
  Sets the profession of the contact to \a str.
*/

/*! \fn void OPimContact::setAssistant( const QString &str )
  Sets the assistant of the contact to \a str.
*/

/*! \fn void OPimContact::setManager( const QString &str )
  Sets the manager of the contact to \a str.
*/

/*! \fn void OPimContact::setSpouse( const QString &str )
  Sets the spouse of the contact to \a str.
*/

/*! \fn void OPimContact::setGender( const QString &str )
  Sets the gender of the contact to \a str.
*/

/*! \fn void OPimContact::setNickname( const QString &str )
  Sets the nickname of the contact to \a str.
*/

/*! \fn void OPimContact::setNotes( const QString &str )
  Sets the notes about the contact to \a str.
*/

/*! \fn QString OPimContact::title() const
  Returns the title of the contact.
*/

/*! \fn QString OPimContact::firstName() const
  Returns the first name of the contact.
*/

/*! \fn QString OPimContact::middleName() const
  Returns the middle name of the contact.
*/

/*! \fn QString OPimContact::lastName() const
  Returns the last name of the contact.
*/

/*! \fn QString OPimContact::suffix() const
  Returns the suffix of the contact.
*/

/*! \fn QString OPimContact::fileAs() const
  Returns the string the contact is filed as.
*/

/*! \fn QString OPimContact::defaultEmail() const
  Returns the default email address of the contact.
*/

/*! \fn QString OPimContact::emails() const
  Returns the list of email address for a contact separated by ';'s in a single
  string.
*/

/*! \fn QString OPimContact::homeStreet() const
  Returns the home street address of the contact.
*/

/*! \fn QString OPimContact::homeCity() const
  Returns the home city of the contact.
*/

/*! \fn QString OPimContact::homeState() const
  Returns the home state of the contact.
*/

/*! \fn QString OPimContact::homeZip() const
  Returns the home zip of the contact.
*/

/*! \fn QString OPimContact::homeCountry() const
  Returns the home country of the contact.
*/

/*! \fn QString OPimContact::homePhone() const
  Returns the home phone number of the contact.
*/

/*! \fn QString OPimContact::homeFax() const
  Returns the home fax number of the contact.
*/

/*! \fn QString OPimContact::homeMobile() const
  Returns the home mobile number of the contact.
*/

/*! \fn QString OPimContact::homeWebpage() const
  Returns the home webpage of the contact.
*/

/*! \fn QString OPimContact::company() const
  Returns the company for the contact.
*/

/*! \fn QString OPimContact::department() const
  Returns the department for the contact.
*/

/*! \fn QString OPimContact::office() const
  Returns the office for the contact.
*/

/*! \fn QString OPimContact::jobTitle() const
  Returns the job title of the contact.
*/

/*! \fn QString OPimContact::profession() const
  Returns the profession of the contact.
*/

/*! \fn QString OPimContact::assistant() const
  Returns the assistant of the contact.
*/

/*! \fn QString OPimContact::manager() const
  Returns the manager of the contact.
*/

/*! \fn QString OPimContact::businessStreet() const
  Returns the business street address of the contact.
*/

/*! \fn QString OPimContact::businessCity() const
  Returns the business city of the contact.
*/

/*! \fn QString OPimContact::businessState() const
  Returns the business state of the contact.
*/

/*! \fn QString OPimContact::businessZip() const
  Returns the business zip of the contact.
*/

/*! \fn QString OPimContact::businessCountry() const
  Returns the business country of the contact.
*/

/*! \fn QString OPimContact::businessPhone() const
  Returns the business phone number of the contact.
*/

/*! \fn QString OPimContact::businessFax() const
  Returns the business fax number of the contact.
*/

/*! \fn QString OPimContact::businessMobile() const
  Returns the business mobile number of the contact.
*/

/*! \fn QString OPimContact::businessPager() const
  Returns the business pager number of the contact.
*/

/*! \fn QString OPimContact::businessWebpage() const
  Returns the business webpage of the contact.
*/

/*! \fn QString OPimContact::spouse() const
  Returns the spouse of the contact.
*/

/*! \fn QString OPimContact::gender() const
  Returns the gender of the contact.
*/

/*! \fn QString OPimContact::nickname() const
  Returns the nickname of the contact.
*/

/*! \fn QString OPimContact::children() const
  Returns the children of the contact.
*/

/*! \fn QString OPimContact::notes() const
  Returns the notes relating to the the contact.
*/

/*! \fn QString OPimContact::groups() const
  \internal
  Returns the groups for the contact.
*/

/*! \fn QStringList OPimContact::groupList() const
  \internal
*/

/*! \fn QString OPimContact::field(int) const
  \internal
*/

/*! \fn void OPimContact::saveJournal( journal_action, const QString & = QString::null )
  \internal
*/

/*! \fn void OPimContact::setUid( int id )
  \internal
  Sets the uid for this record to \a id.
*/

/*! \enum OPimContact::journal_action
  \internal
*/

/*!
  \internal
*/
QMap<int, QString> OPimContact::toMap() const
{
    QMap<int, QString> map = mMap;
    QString cats = idsToString( categories() );
    if ( !cats.isEmpty() )
        map.insert( Qtopia::AddressCategory, cats );
    return map;
}

/*!
  Returns a rich text formatted QString representing the contents the contact.
*/
QString OPimContact::toRichText() const
{
    QString text;
    QString value, comp, state;
    QString str;
    bool marker = false;

    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );
    int addressformat = cfg.readNumEntry( "AddressFormat", Zip_City_State );

    // name, jobtitle and company
    if ( !( value = fullName() ).isEmpty() )
        text += "<b><h3><img src=\"addressbook/AddressBook\"> " + Qtopia::escapeString( value ) + "</h3></b>";

    if ( !( value = jobTitle() ).isEmpty() )
        text += Qtopia::escapeString( value ) + " ";

    comp = company();
    if ( !( value = department() ).isEmpty() )
    {
        text += Qtopia::escapeString( value );
        if ( comp )
            text += ", " + Qtopia::escapeString( comp );
    }
    else if ( comp )
        text += "<br>" + Qtopia::escapeString( comp );
    text += "<br><hr>";

    // defailt email
    QString defEmail = defaultEmail();
    if ( !defEmail.isEmpty() )
    {
        text += "<b><img src=\"addressbook/email\"> " + QObject::tr( "Default Email: " ) + "</b>"
                + Qtopia::escapeString( defEmail );
        marker = true;
    }

    // business address
    if ( !businessStreet().isEmpty() || !businessCity().isEmpty() ||
            !businessZip().isEmpty() || !businessCountry().isEmpty() )
    {
        text += QObject::tr( "<br><b>Work Address:</b>" );
        marker = true;
    }

    if ( !( value = businessStreet() ).isEmpty() )
    {
        text += "<br>" + Qtopia::escapeString( value );
        marker = true;
    }

    switch ( addressformat )
    {
    case Zip_City_State:
        { //  Zip_Code City, State
            state = businessState();
            if ( !( value = businessZip() ).isEmpty() )
            {
                text += "<br>" + Qtopia::escapeString( value ) + " ";
                marker = true;

            }
            if ( !( value = businessCity() ).isEmpty() )
            {
                marker = true;
                if ( businessZip().isEmpty() && !businessStreet().isEmpty() )
                    text += "<br>";
                text += Qtopia::escapeString( value );
                if ( state )
                    text += ", " + Qtopia::escapeString( state );
            }
            else if ( !state.isEmpty() )
            {
                text += "<br>" + Qtopia::escapeString( state );
                marker = true;
            }
            break;
        }
    case City_State_Zip:
        { // City, State Zip_Code
            state = businessState();
            if ( !( value = businessCity() ).isEmpty() )
            {
                marker = true;
                text += "<br>" + Qtopia::escapeString( value );
                if ( state )
                    text += ", " + Qtopia::escapeString( state );
            }
            else if ( !state.isEmpty() )
            {
                text += "<br>" + Qtopia::escapeString( state );
                marker = true;
            }
            if ( !( value = businessZip() ).isEmpty() )
            {
                text += " " + Qtopia::escapeString( value );
                marker = true;
            }
            break;
        }
    }

    if ( !( value = businessCountry() ).isEmpty() )
    {
        text += "<br>" + Qtopia::escapeString( value );
        marker = true;
    }

    // rest of Business data
    str = office();
    if ( !str.isEmpty() )
    {
        text += "<br><b>" + QObject::tr( "Office: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }
    str = businessWebpage();
    if ( !str.isEmpty() )
    {
        text += "<br><b><img src=\"addressbook/webpagework\"> " + QObject::tr( "Business Web Page: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }
    str = businessPhone();
    if ( !str.isEmpty() )
    {
        text += "<br><b><img src=\"addressbook/phonework\"> " + QObject::tr( "Business Phone: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }
    str = businessFax();
    if ( !str.isEmpty() )
    {
        text += "<br><b><img src=\"addressbook/faxwork\"> " + QObject::tr( "Business Fax: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }
    str = businessMobile();
    if ( !str.isEmpty() )
    {
        text += "<br><b><img src=\"addressbook/mobilework\"> " + QObject::tr( "Business Mobile: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }
    str = businessPager();
    if ( !str.isEmpty() )
    {
        text += "<br><b>" + QObject::tr( "Business Pager: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }

    // text += "<br>";

    // home address
    if ( !homeStreet().isEmpty() || !homeCity().isEmpty() ||
            !homeZip().isEmpty() || !homeCountry().isEmpty() )
    {
        text += QObject::tr( "<br><b>Home Address:</b>" );
        marker = true;
    }

    if ( !( value = homeStreet() ).isEmpty() )
    {
        text += "<br>" + Qtopia::escapeString( value );
        marker = true;
    }

    switch ( addressformat )
    {
    case Zip_City_State:
        { //  Zip_Code City, State
            state = homeState();
            if ( !( value = homeZip() ).isEmpty() )
            {
                text += "<br>" + Qtopia::escapeString( value ) + " ";
                marker = true;
            }
            if ( !( value = homeCity() ).isEmpty() )
            {
                marker = true;
                if ( homeZip().isEmpty() && !homeStreet().isEmpty() )
                    text += "<br>";
                text += Qtopia::escapeString( value );
                if ( !state.isEmpty() )
                    text += ", " + Qtopia::escapeString( state );
            }
            else if ( !state.isEmpty() )
            {
                text += "<br>" + Qtopia::escapeString( state );
                marker = true;
            }
            break;
        }
    case City_State_Zip:
        { // City, State Zip_Code
            state = homeState();
            if ( !( value = homeCity() ).isEmpty() )
            {
                marker = true;
                text += "<br>" + Qtopia::escapeString( value );
                if ( state )
                    text += ", " + Qtopia::escapeString( state );
            }
            else if ( !state.isEmpty() )
            {
                text += "<br>" + Qtopia::escapeString( state );
                marker = true;
            }
            if ( !( value = homeZip() ).isEmpty() )
            {
                text += " " + Qtopia::escapeString( value );
                marker = true;
            }
            break;
        }
    }

    if ( !( value = homeCountry() ).isEmpty() )
    {
        text += "<br>" + Qtopia::escapeString( value );
        marker = true;
    }

    // rest of Home data
    str = homeWebpage();
    if ( !str.isEmpty() )
    {
        text += "<br><b><img src=\"addressbook/webpagehome\"> " + QObject::tr( "Home Web Page: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }
    str = homePhone();
    if ( !str.isEmpty() )
    {
        text += "<br><b><img src=\"addressbook/phonehome\"> " + QObject::tr( "Home Phone: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }
    str = homeFax();
    if ( !str.isEmpty() )
    {
        text += "<br><b><img src=\"addressbook/faxhome\"> " + QObject::tr( "Home Fax: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }
    str = homeMobile();
    if ( !str.isEmpty() )
    {
        text += "<br><b><img src=\"addressbook/mobilehome\"> " + QObject::tr( "Home Mobile: " ) + "</b>"
                + Qtopia::escapeString( str );
        marker = true;
    }

    if ( marker )
        text += "<br><hr>";

    // the rest...
    str = emails();
    if ( !str.isEmpty() && ( str != defEmail ) )
        text += "<br><b>" + QObject::tr( "All Emails: " ) + "</b>"
                + Qtopia::escapeString( str );
    str = profession();
    if ( !str.isEmpty() )
        text += "<br><b>" + QObject::tr( "Profession: " ) + "</b>"
                + Qtopia::escapeString( str );
    str = assistant();
    if ( !str.isEmpty() )
        text += "<br><b>" + QObject::tr( "Assistant: " ) + "</b>"
                + Qtopia::escapeString( str );
    str = manager();
    if ( !str.isEmpty() )
        text += "<br><b>" + QObject::tr( "Manager: " ) + "</b>"
                + Qtopia::escapeString( str );
    str = gender();
    if ( !str.isEmpty() && str.toInt() != 0 )
    {
        text += "<br>";
        if ( str.toInt() == 1 )
            str = QObject::tr( "Male" );
        else if ( str.toInt() == 2 )
            str = QObject::tr( "Female" );
        text += "<b>" + QObject::tr( "Gender: " ) + "</b>" + str;
    }
    str = spouse();
    if ( !str.isEmpty() )
        text += "<br><b>" + QObject::tr( "Spouse: " ) + "</b>"
                + Qtopia::escapeString( str );
    if ( birthday().isValid() )
    {
        str = TimeString::numberDateString( birthday() );
        text += "<br><b>" + QObject::tr( "Birthday: " ) + "</b>"
                + Qtopia::escapeString( str );
    }
    if ( anniversary().isValid() )
    {
        str = TimeString::numberDateString( anniversary() );
        text += "<br><b>" + QObject::tr( "Anniversary: " ) + "</b>"
                + Qtopia::escapeString( str );
    }
    str = children();
    if ( !str.isEmpty() )
        text += "<br><b>" + QObject::tr( "Children: " ) + "</b>"
                + Qtopia::escapeString( str );

    str = nickname();
    if ( !str.isEmpty() )
        text += "<br><b>" + QObject::tr( "Nickname: " ) + "</b>"
                + Qtopia::escapeString( str );

    // categories
    if ( categoryNames( "Contacts" ).count() )
    {
        text += "<br><b>" + QObject::tr( "Category:" ) + "</b> ";
        text += categoryNames( "Contacts" ).join( ", " );
    }

    // notes last
    if ( !( value = notes() ).isEmpty() )
    {
        text += "<br><hr><b>" + QObject::tr( "Notes:" ) + "</b> ";
        QRegExp reg( "\n" );

        //QString tmp = Qtopia::escapeString(value);
        QString tmp = QStyleSheet::convertFromPlainText( value );
        //tmp.replace( reg, "<br>" );
        text += "<br>" + tmp + "<br>";
    }
    return text;
}

/*!
  \internal
*/
void OPimContact::insert( int key, const QString &v )
{
    QString value = v.stripWhiteSpace();
    if ( value.isEmpty() )
        mMap.remove( key );
    else
        mMap.insert( key, value );
}

/*!
  \internal
*/
void OPimContact::replace( int key, const QString & v )
{
    QString value = v.stripWhiteSpace();
    if ( value.isEmpty() )
        mMap.remove( key );
    else
        mMap.replace( key, value );
}

/*!
  \internal
*/
QString OPimContact::find( int key ) const
{
    return mMap[ key ];
}

/*!
  \internal
*/
QString OPimContact::displayAddress( const QString &street,
                                  const QString &city,
                                  const QString &state,
                                  const QString &zip,
                                  const QString &country ) const
{
    QString s = street;
    if ( !street.isEmpty() )
        s += "\n";
    s += city;
    if ( !city.isEmpty() && !state.isEmpty() )
        s += ", ";
    s += state;
    if ( !state.isEmpty() && !zip.isEmpty() )
        s += "  ";
    s += zip;
    if ( !country.isEmpty() && !s.isEmpty() )
        s += "\n";
    s += country;
    return s;
}

/*!
  \internal
*/
QString OPimContact::displayBusinessAddress() const
{
    return displayAddress( businessStreet(), businessCity(),
                           businessState(), businessZip(),
                           businessCountry() );
}

/*!
  \internal
*/
QString OPimContact::displayHomeAddress() const
{
    return displayAddress( homeStreet(), homeCity(),
                           homeState(), homeZip(),
                           homeCountry() );
}

/*!
  Returns the full name of the contact
*/
QString OPimContact::fullName() const
{
    QString title = find( Qtopia::Title );
    QString firstName = find( Qtopia::FirstName );
    QString middleName = find( Qtopia::MiddleName );
    QString lastName = find( Qtopia::LastName );
    QString suffix = find( Qtopia::Suffix );

    QString name = title;
    if ( !firstName.isEmpty() )
    {
        if ( !name.isEmpty() )
            name += " ";
        name += firstName;
    }
    if ( !middleName.isEmpty() )
    {
        if ( !name.isEmpty() )
            name += " ";
        name += middleName;
    }
    if ( !lastName.isEmpty() )
    {
        if ( !name.isEmpty() )
            name += " ";
        name += lastName;
    }
    if ( !suffix.isEmpty() )
    {
        if ( !name.isEmpty() )
            name += " ";
        name += suffix;
    }
    return name.simplifyWhiteSpace();
}

/*!
  Returns a list of the names of the children of the contact.
*/
QStringList OPimContact::childrenList() const
{
    return QStringList::split( " ", find( Qtopia::Children ) );
}

/*! \fn void OPimContact::insertEmail( const QString &email )

  Insert \a email into the email list. Ensures \a email can only be added
  once. If there is no default email address set, it sets it to the \a email.
*/

/*! \fn void OPimContact::removeEmail( const QString &email )

  Removes the \a email from the email list. If the default email was \a email,
  then the default email address is assigned to the first email in the
  email list
*/

/*! \fn void OPimContact::clearEmails()

  Clears the email list.
 */

/*! \fn void OPimContact::insertEmails( const QStringList &emailList )

  Appends the \a emailList to the exiting email list
 */

/*!
  Returns a list of email addresses belonging to the contact, including
  the default email address.
*/
QStringList OPimContact::emailList() const
{
    QString emailStr = emails();

    QStringList r;
    if ( !emailStr.isEmpty() )
    {
        odebug << " emailstr " << oendl;
        QStringList l = QStringList::split( emailSeparator(), emailStr );
        for ( QStringList::ConstIterator it = l.begin();it != l.end();++it )
            r += ( *it ).simplifyWhiteSpace();
    }

    return r;
}

/*!
  \overload

  Generates the string for the contact to be filed as from the first,
  middle and last name of the contact.
*/
void OPimContact::setFileAs()
{
    QString lastName, firstName, middleName, fileas;

    lastName = find( Qtopia::LastName );
    firstName = find( Qtopia::FirstName );
    middleName = find( Qtopia::MiddleName );
    if ( !lastName.isEmpty() && !firstName.isEmpty()
            && !middleName.isEmpty() )
        fileas = lastName + ", " + firstName + " " + middleName;
    else if ( !lastName.isEmpty() && !firstName.isEmpty() )
        fileas = lastName + ", " + firstName;
    else if ( !lastName.isEmpty() || !firstName.isEmpty() ||
              !middleName.isEmpty() )
        fileas = firstName + ( firstName.isEmpty() ? "" : " " )
                 + middleName + ( middleName.isEmpty() ? "" : " " )
                 + lastName;

    replace( Qtopia::FileAs, fileas );
}

/*!
  \internal
  Appends the contact information to \a buf.
*/
void OPimContact::save( QString &buf ) const
{
    static const QStringList SLFIELDS = fields();
    // I'm expecting "<Contact " in front of this...
    for ( QMap<int, QString>::ConstIterator it = mMap.begin();
            it != mMap.end(); ++it )
    {
        const QString &value = it.data();
        int key = it.key();
        if ( !value.isEmpty() )
        {
            if ( key == Qtopia::AddressCategory || key == Qtopia::AddressUid )
                continue;

            key -= Qtopia::AddressCategory + 1;
            buf += SLFIELDS[ key ];
            buf += "=\"" + Qtopia::escapeString( value ) + "\" ";
        }
    }
    buf += customToXml();
    if ( categories().count() > 0 )
        buf += "Categories=\"" + idsToString( categories() ) + "\" ";
    buf += "Uid=\"" + QString::number( uid() ) + "\" ";
    // You need to close this yourself
}


/*!
  \internal
  Returns the list of fields belonging to a contact
  Never change order of this list ! It has to be regarding
  enum AddressBookFields !!
*/
QStringList OPimContact::fields()
{
    QStringList list;

    list.append( "Title" );  // Not Used!
    list.append( "FirstName" );
    list.append( "MiddleName" );
    list.append( "LastName" );
    list.append( "Suffix" );
    list.append( "FileAs" );

    list.append( "JobTitle" );
    list.append( "Department" );
    list.append( "Company" );
    list.append( "BusinessPhone" );
    list.append( "BusinessFax" );
    list.append( "BusinessMobile" );

    list.append( "DefaultEmail" );
    list.append( "Emails" );

    list.append( "HomePhone" );
    list.append( "HomeFax" );
    list.append( "HomeMobile" );

    list.append( "BusinessStreet" );
    list.append( "BusinessCity" );
    list.append( "BusinessState" );
    list.append( "BusinessZip" );
    list.append( "BusinessCountry" );
    list.append( "BusinessPager" );
    list.append( "BusinessWebPage" );

    list.append( "Office" );
    list.append( "Profession" );
    list.append( "Assistant" );
    list.append( "Manager" );

    list.append( "HomeStreet" );
    list.append( "HomeCity" );
    list.append( "HomeState" );
    list.append( "HomeZip" );
    list.append( "HomeCountry" );
    list.append( "HomeWebPage" );

    list.append( "Spouse" );
    list.append( "Gender" );
    list.append( "Birthday" );
    list.append( "Anniversary" );
    list.append( "Nickname" );
    list.append( "Children" );

    list.append( "Notes" );
    list.append( "Groups" );

    return list;
}


/*!
  Sets the list of email address for contact to those contained in \a str.
  Email address should be separated by ';'s.
*/
void OPimContact::setEmails( const QString &str )
{
    replace( Qtopia::Emails, str );
    if ( str.isEmpty() )
        setDefaultEmail( QString::null );
}

/*!
  Sets the list of children for the contact to those contained in \a str.
*/
void OPimContact::setChildren( const QString &str )
{
    replace( Qtopia::Children, str );
}

/*!
  \overload
  Returns TRUE if the contact matches the regular expression \a regexp.
  Otherwise returns FALSE.
*/
bool OPimContact::match( const QRegExp &r ) const
{
    setLastHitField( -1 );
    bool match;
    match = false;
    QMap<int, QString>::ConstIterator it;
    for ( it = mMap.begin(); it != mMap.end(); ++it )
    {
        if ( ( *it ).find( r ) > -1 )
        {
            setLastHitField( it.key() );
            match = true;
            break;
        }
    }
    return match;
}


QString OPimContact::toShortText() const
{
    return ( fullName() );
}


QString OPimContact::type() const
{
    return QString::fromLatin1( "OPimContact" );
}


class QString OPimContact::recordField( int pos ) const
{
    QStringList SLFIELDS = fields(); // ?? why this ? (se)
    return SLFIELDS[ pos ];
}

// In future releases, we should store birthday and anniversary
// internally as QDate instead of QString !
// QString is always too complicate to interprete (DD.MM.YY, DD/MM/YY, MM/DD/YY, etc..)(se)

/*! \fn void OPimContact::setBirthday( const QDate& date )
  Sets the birthday for the contact to \a date. If date is null
  the current stored date will be removed.
*/
void OPimContact::setBirthday( const QDate &v )
{
    if ( v.isNull() )
    {
        owarn << "Remove Birthday" << oendl;
        replace( Qtopia::Birthday, QString::null );
        return ;
    }

    if ( v.isValid() )
        replace( Qtopia::Birthday, OPimDateConversion::dateToString( v ) );

}


/*! \fn void OPimContact::setAnniversary( const QDate &date )
  Sets the anniversary of the contact to \a date. If date is
  null, the current stored date will be removed.
*/
void OPimContact::setAnniversary( const QDate &v )
{
    if ( v.isNull() )
    {
        owarn << "Remove Anniversary" << oendl;
        replace( Qtopia::Anniversary, QString::null );
        return ;
    }

    if ( v.isValid() )
        replace( Qtopia::Anniversary, OPimDateConversion::dateToString( v ) );
}


/*! \fn QDate OPimContact::birthday() const
  Returns the birthday of the contact.
*/
QDate OPimContact::birthday() const
{
    QString str = find( Qtopia::Birthday );
    // qWarning ("Birthday %s", str.latin1() );
    if ( !str.isEmpty() )
        return OPimDateConversion::dateFromString ( str );
    else
        return QDate();
}


/*! \fn QDate OPimContact::anniversary() const
  Returns the anniversary of the contact.
*/
QDate OPimContact::anniversary() const
{
    QDate empty;
    QString str = find( Qtopia::Anniversary );
    // qWarning ("Anniversary %s", str.latin1() );
    if ( !str.isEmpty() )
        return OPimDateConversion::dateFromString ( str );
    else
        return empty;
}


void OPimContact::insertEmail( const QString &v )
{
    //odebug << "insertEmail " << v << "" << oendl;
    QString e = v.simplifyWhiteSpace();
    QString def = defaultEmail();

    // if no default, set it as the default email and don't insert
    if ( def.isEmpty() )
    {
        setDefaultEmail( e ); // will insert into the list for us
        return ;
    }

    // otherwise, insert assuming doesn't already exist
    QString emailsStr = find( Qtopia::Emails );
    if ( emailsStr.contains( e ) )
        return ;
    if ( !emailsStr.isEmpty() )
        emailsStr += emailSeparator();
    emailsStr += e;
    replace( Qtopia::Emails, emailsStr );
}


        void OPimContact::removeEmail( const QString &v )
{
    QString e = v.simplifyWhiteSpace();
    QString def = defaultEmail();
    QString emailsStr = find( Qtopia::Emails );
    QStringList emails = emailList();

    // otherwise, must first contain it
    if ( !emailsStr.contains( e ) )
        return ;

    // remove it
    //odebug << " removing email from list " << e << "" << oendl;
    emails.remove( e );
    // reset the string
    emailsStr = emails.join( emailSeparator() ); // Sharp's brain dead separator
    replace( Qtopia::Emails, emailsStr );

    // if default, then replace the default email with the first one
    if ( def == e )
    {
        //odebug << "removeEmail is default; setting new default" << oendl;
        if ( !emails.count() )
            clearEmails();
        else // setDefaultEmail will remove e from the list
            setDefaultEmail( emails.first() );
    }
}


void OPimContact::clearEmails()
{
    mMap.remove( Qtopia::DefaultEmail );
    mMap.remove( Qtopia::Emails );
}


void OPimContact::setDefaultEmail( const QString &v )
{
    QString e = v.simplifyWhiteSpace();

    //odebug << "OPimContact::setDefaultEmail " << e << "" << oendl;
    replace( Qtopia::DefaultEmail, e );

    if ( !e.isEmpty() )
        insertEmail( e );

}


void OPimContact::insertEmails( const QStringList &v )
{
    for ( QStringList::ConstIterator it = v.begin(); it != v.end(); ++it )
        insertEmail( *it );
}


int OPimContact::rtti() const
{
    return OPimResolver::AddressBook;
}


void OPimContact::setUid( int i )
{
    OPimRecord::setUid( i );
    replace( Qtopia::AddressUid , QString::number( i ) );
}
}
