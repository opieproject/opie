/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
** Copyright (C) 2002 by Stefan Eilers (eilers.stefan@epost.de)
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#define QTOPIA_INTERNAL_CONTACT_MRE

#include "ocontact.h"

#include <qpe/stringutil.h>
#include <qpe/timeconversion.h>
#include <qpe/timestring.h>

#include <qobject.h>
#include <qregexp.h>
#include <qstylesheet.h>
#include <qfileinfo.h>
#include <qmap.h>

#include <stdio.h>

/*!
  \class Contact contact.h
  \brief The Contact class holds the data of an address book entry.

  This data includes information the name of the person, contact
  information, and business information such as deparment and job title.

  \ingroup qtopiaemb
  \ingroup qtopiadesktop
*/


/*!
  Creates a new, empty contact.
*/
OContact::OContact()
    : OPimRecord(), mMap(), d( 0 )
{
}

/*!
  \internal
  Creates a new contact.  The properties of the contact are
  set from \a fromMap.
*/
OContact::OContact( const QMap<int, QString> &fromMap ) :
    OPimRecord(), mMap( fromMap ), d( 0 )
{
    QString cats = mMap[ Qtopia::AddressCategory ];
    if ( !cats.isEmpty() )
	setCategories( idsFromString( cats ) );

    QString uidStr = find( Qtopia::AddressUid );

    if ( uidStr.isEmpty() || (uidStr.toInt() == 0) ){
	    qWarning( "Invalid UID found. Generate new one.." );
	setUid( uidGen().generate() );
    }else
	setUid( uidStr.toInt() );

//     if ( !uidStr.isEmpty() )
// 	setUid( uidStr.toInt() );
}

/*!
  Destroys a contact.
*/
OContact::~OContact()
{
}

/*! \fn void OContact::setTitle( const QString &str )
  Sets the title of the contact to \a str.
*/

/*! \fn void OContact::setFirstName( const QString &str )
  Sets the first name of the contact to \a str.
*/

/*! \fn void OContact::setMiddleName( const QString &str )
  Sets the middle name of the contact to \a str.
*/

/*! \fn void OContact::setLastName( const QString &str )
  Sets the last name of the contact to \a str.
*/

/*! \fn void OContact::setSuffix( const QString &str )
  Sets the suffix of the contact to \a str.
*/

/*! \fn void OContact::setFileAs( const QString &str )
  Sets the contact to filed as \a str.
*/

/*! \fn void OContact::setDefaultEmail( const QString &str )
  Sets the default email of the contact to \a str.
*/

/*! \fn void OContact::setHomeStreet( const QString &str )
  Sets the home street address of the contact to \a str.
*/

/*! \fn void OContact::setHomeCity( const QString &str )
  Sets the home city of the contact to \a str.
*/

/*! \fn void OContact::setHomeState( const QString &str )
  Sets the home state of the contact to \a str.
*/

/*! \fn void OContact::setHomeZip( const QString &str )
  Sets the home zip code of the contact to \a str.
*/

/*! \fn void OContact::setHomeCountry( const QString &str )
  Sets the home country of the contact to \a str.
*/

/*! \fn void OContact::setHomePhone( const QString &str )
  Sets the home phone number of the contact to \a str.
*/

/*! \fn void OContact::setHomeFax( const QString &str )
  Sets the home fax number of the contact to \a str.
*/

/*! \fn void OContact::setHomeMobile( const QString &str )
  Sets the home mobile phone number of the contact to \a str.
*/

/*! \fn void OContact::setHomeWebpage( const QString &str )
  Sets the home webpage of the contact to \a str.
*/

/*! \fn void OContact::setCompany( const QString &str )
  Sets the company for contact to \a str.
*/

/*! \fn void OContact::setJobTitle( const QString &str )
  Sets the job title of the contact to \a str.
*/

/*! \fn void OContact::setDepartment( const QString &str )
  Sets the department for contact to \a str.
*/

/*! \fn void OContact::setOffice( const QString &str )
  Sets the office for contact to \a str.
*/

/*! \fn void OContact::setBusinessStreet( const QString &str )
  Sets the business street address of the contact to \a str.
*/

/*! \fn void OContact::setBusinessCity( const QString &str )
  Sets the business city of the contact to \a str.
*/

/*! \fn void OContact::setBusinessState( const QString &str )
  Sets the business state of the contact to \a str.
*/

/*! \fn void OContact::setBusinessZip( const QString &str )
  Sets the business zip code of the contact to \a str.
*/

/*! \fn void OContact::setBusinessCountry( const QString &str )
  Sets the business country of the contact to \a str.
*/

/*! \fn void OContact::setBusinessPhone( const QString &str )
  Sets the business phone number of the contact to \a str.
*/

/*! \fn void OContact::setBusinessFax( const QString &str )
  Sets the business fax number of the contact to \a str.
*/

/*! \fn void OContact::setBusinessMobile( const QString &str )
  Sets the business mobile phone number of the contact to \a str.
*/

/*! \fn void OContact::setBusinessPager( const QString &str )
  Sets the business pager number of the contact to \a str.
*/

/*! \fn void OContact::setBusinessWebpage( const QString &str )
  Sets the business webpage of the contact to \a str.
*/

/*! \fn void OContact::setProfession( const QString &str )
  Sets the profession of the contact to \a str.
*/

/*! \fn void OContact::setAssistant( const QString &str )
  Sets the assistant of the contact to \a str.
*/

/*! \fn void OContact::setManager( const QString &str )
  Sets the manager of the contact to \a str.
*/

/*! \fn void OContact::setSpouse( const QString &str )
  Sets the spouse of the contact to \a str.
*/

/*! \fn void OContact::setGender( const QString &str )
  Sets the gender of the contact to \a str.
*/

/*! \fn void OContact::setNickname( const QString &str )
  Sets the nickname of the contact to \a str.
*/

/*! \fn void OContact::setNotes( const QString &str )
  Sets the notes about the contact to \a str.
*/

/*! \fn QString OContact::title() const
  Returns the title of the contact.
*/

/*! \fn QString OContact::firstName() const
  Returns the first name of the contact.
*/

/*! \fn QString OContact::middleName() const
  Returns the middle name of the contact.
*/

/*! \fn QString OContact::lastName() const
  Returns the last name of the contact.
*/

/*! \fn QString OContact::suffix() const
  Returns the suffix of the contact.
*/

/*! \fn QString OContact::fileAs() const
  Returns the string the contact is filed as.
*/

/*! \fn QString OContact::defaultEmail() const
  Returns the default email address of the contact.
*/

/*! \fn QString OContact::emails() const
  Returns the list of email address for a contact separated by ';'s in a single
  string.
*/

/*! \fn QString OContact::homeStreet() const
  Returns the home street address of the contact.
*/

/*! \fn QString OContact::homeCity() const
  Returns the home city of the contact.
*/

/*! \fn QString OContact::homeState() const
  Returns the home state of the contact.
*/

/*! \fn QString OContact::homeZip() const
  Returns the home zip of the contact.
*/

/*! \fn QString OContact::homeCountry() const
  Returns the home country of the contact.
*/

/*! \fn QString OContact::homePhone() const
  Returns the home phone number of the contact.
*/

/*! \fn QString OContact::homeFax() const
  Returns the home fax number of the contact.
*/

/*! \fn QString OContact::homeMobile() const
  Returns the home mobile number of the contact.
*/

/*! \fn QString OContact::homeWebpage() const
  Returns the home webpage of the contact.
*/

/*! \fn QString OContact::company() const
  Returns the company for the contact.
*/

/*! \fn QString OContact::department() const
  Returns the department for the contact.
*/

/*! \fn QString OContact::office() const
  Returns the office for the contact.
*/

/*! \fn QString OContact::jobTitle() const
  Returns the job title of the contact.
*/

/*! \fn QString OContact::profession() const
  Returns the profession of the contact.
*/

/*! \fn QString OContact::assistant() const
  Returns the assistant of the contact.
*/

/*! \fn QString OContact::manager() const
  Returns the manager of the contact.
*/

/*! \fn QString OContact::businessStreet() const
  Returns the business street address of the contact.
*/

/*! \fn QString OContact::businessCity() const
  Returns the business city of the contact.
*/

/*! \fn QString OContact::businessState() const
  Returns the business state of the contact.
*/

/*! \fn QString OContact::businessZip() const
  Returns the business zip of the contact.
*/

/*! \fn QString OContact::businessCountry() const
  Returns the business country of the contact.
*/

/*! \fn QString OContact::businessPhone() const
  Returns the business phone number of the contact.
*/

/*! \fn QString OContact::businessFax() const
  Returns the business fax number of the contact.
*/

/*! \fn QString OContact::businessMobile() const
  Returns the business mobile number of the contact.
*/

/*! \fn QString OContact::businessPager() const
  Returns the business pager number of the contact.
*/

/*! \fn QString OContact::businessWebpage() const
  Returns the business webpage of the contact.
*/

/*! \fn QString OContact::spouse() const
  Returns the spouse of the contact.
*/

/*! \fn QString OContact::gender() const
  Returns the gender of the contact.
*/

/*! \fn QString OContact::nickname() const
  Returns the nickname of the contact.
*/

/*! \fn QString OContact::children() const
  Returns the children of the contact.
*/

/*! \fn QString OContact::notes() const
  Returns the notes relating to the the contact.
*/

/*! \fn QString OContact::groups() const
  \internal
  Returns the groups for the contact.
*/

/*! \fn QStringList OContact::groupList() const
  \internal
*/

/*! \fn QString OContact::field(int) const
  \internal
*/

/*! \fn void OContact::saveJournal( journal_action, const QString & = QString::null )
  \internal
*/

/*! \fn void OContact::setUid( int id )
  \internal
  Sets the uid for this record to \a id.
*/

/*! \enum OContact::journal_action
  \internal
*/

/*!
  \internal
*/
QMap<int, QString> OContact::toMap() const
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
QString OContact::toRichText() const
{
    QString text;
    QString value, comp, state;

    // name, jobtitle and company
    if ( !(value = fullName()).isEmpty() )
	text += "<b>" + Qtopia::escapeString(value) + "</b><br>";
    if ( !(value = jobTitle()).isEmpty() )
	text += Qtopia::escapeString(value) + "<br>";

    comp = company();
    if ( !(value = department()).isEmpty() ) {
	text += Qtopia::escapeString(value);
	if ( comp )
	    text += ", ";
	else
	    text += "<br>";
    }
    if ( !comp.isEmpty() )
	text += Qtopia::escapeString(comp) + "<br>";

    // business address
    if ( !businessStreet().isEmpty() || !businessCity().isEmpty() ||
	 !businessZip().isEmpty() || !businessCountry().isEmpty() ) {
	text += "<br>";
	text += QObject::tr( "<b>Work Address:</b>" );
	text +=  "<br>";
    }

    if ( !(value = businessStreet()).isEmpty() )
	text += Qtopia::escapeString(value) + "<br>";
    state =  businessState();
    if ( !(value = businessCity()).isEmpty() ) {
	text += Qtopia::escapeString(value);
	if ( state )
	    text += ", " + Qtopia::escapeString(state);
	text += "<br>";
    } else if ( !state.isEmpty() )
	text += Qtopia::escapeString(state) + "<br>";
    if ( !(value = businessZip()).isEmpty() )
	text += Qtopia::escapeString(value) + "<br>";
    if ( !(value = businessCountry()).isEmpty() )
	text += Qtopia::escapeString(value) + "<br>";

    // home address
    if ( !homeStreet().isEmpty() || !homeCity().isEmpty() ||
	 !homeZip().isEmpty() || !homeCountry().isEmpty() ) {
	text += "<br>";
	text += QObject::tr( "<b>Home Address:</b>" );
	text +=  "<br>";
    }

    if ( !(value = homeStreet()).isEmpty() )
	text += Qtopia::escapeString(value) + "<br>";
    state =  homeState();
    if ( !(value = homeCity()).isEmpty() ) {
	text += Qtopia::escapeString(value);
	if ( !state.isEmpty() )
	    text += ", " + Qtopia::escapeString(state);
	text += "<br>";
    } else if (!state.isEmpty())
	text += Qtopia::escapeString(state) + "<br>";
    if ( !(value = homeZip()).isEmpty() )
	text += Qtopia::escapeString(value) + "<br>";
    if ( !(value = homeCountry()).isEmpty() )
	text += Qtopia::escapeString(value) + "<br>";

    // the others...
    QString str;
    str = emails();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Email Addresses: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = homePhone();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Home Phone: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = homeFax();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Home Fax: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = homeMobile();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Home Mobile: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = homeWebpage();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Home Web Page: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = businessWebpage();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Business Web Page: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = office();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Office: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = businessPhone();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Business Phone: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = businessFax();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Business Fax: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = businessMobile();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Business Mobile: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = businessPager();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Business Pager: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = profession();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Profession: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = assistant();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Assistant: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = manager();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Manager: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = gender();
    if ( !str.isEmpty() && str.toInt() != 0 ) {
	if ( str.toInt() == 1 )
	    str = QObject::tr( "Male" );
	else if ( str.toInt() == 2 )
	    str = QObject::tr( "Female" );
	text += "<b>" + QObject::tr("Gender: ") + "</b>" + str + "<br>";
    }
    str = spouse();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Spouse: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    if ( birthday().isValid() ){
	    str = TimeString::numberDateString( birthday() );
	    text += "<b>" + QObject::tr("Birthday: ") + "</b>"
		    + Qtopia::escapeString(str) + "<br>";
    }
    if ( anniversary().isValid() ){
	    str = TimeString::numberDateString( anniversary() );
	    text += "<b>" + QObject::tr("Anniversary: ") + "</b>"
		    + Qtopia::escapeString(str) + "<br>";
    }
    str = nickname();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Nickname: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";

    if ( categoryNames().count() ){
	    text += "<b>" + QObject::tr( "Category:") + "</b> ";
	    text += categoryNames().join(", ");
	    text += "<br>";
    }

    // notes last
    if ( (value = notes()) ) {
	QRegExp reg("\n");

	//QString tmp = Qtopia::escapeString(value);
	QString tmp = QStyleSheet::convertFromPlainText(value);
	//tmp.replace( reg, "<br>" );
	text += "<br>" + tmp + "<br>";
    }
    return text;
}

/*!
  \internal
*/
void OContact::insert( int key, const QString &v )
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
void OContact::replace( int key, const QString & v )
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
QString OContact::find( int key ) const
{
    return mMap[key];
}

/*!
  \internal
*/
QString OContact::displayAddress( const QString &street,
				 const QString &city,
				 const QString &state,
				 const QString &zip,
				 const QString &country ) const
{
    QString s = street;
    if ( !street.isEmpty() )
	s+= "\n";
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
QString OContact::displayBusinessAddress() const
{
    return displayAddress( businessStreet(), businessCity(),
			   businessState(), businessZip(),
			   businessCountry() );
}

/*!
  \internal
*/
QString OContact::displayHomeAddress() const
{
    return displayAddress( homeStreet(), homeCity(),
			   homeState(), homeZip(),
			   homeCountry() );
}

/*!
  Returns the full name of the contact
*/
QString OContact::fullName() const
{
    QString title = find( Qtopia::Title );
    QString firstName = find( Qtopia::FirstName );
    QString middleName = find( Qtopia::MiddleName );
    QString lastName = find( Qtopia::LastName );
    QString suffix = find( Qtopia::Suffix );

    QString name = title;
    if ( !firstName.isEmpty() ) {
	if ( !name.isEmpty() )
	    name += " ";
	name += firstName;
    }
    if ( !middleName.isEmpty() ) {
	if ( !name.isEmpty() )
	    name += " ";
	name += middleName;
    }
    if ( !lastName.isEmpty() ) {
	if ( !name.isEmpty() )
	    name += " ";
	name += lastName;
    }
    if ( !suffix.isEmpty() ) {
	if ( !name.isEmpty() )
	    name += " ";
	name += suffix;
    }
    return name.simplifyWhiteSpace();
}

/*!
  Returns a list of the names of the children of the contact.
*/
QStringList OContact::childrenList() const
{
    return QStringList::split( " ", find( Qtopia::Children ) );
}

/*! \fn void OContact::insertEmail( const QString &email )

  Insert \a email into the email list. Ensures \a email can only be added
  once. If there is no default email address set, it sets it to the \a email.
*/

/*! \fn void OContact::removeEmail( const QString &email )

  Removes the \a email from the email list. If the default email was \a email,
  then the default email address is assigned to the first email in the
  email list
*/

/*! \fn void OContact::clearEmails()

  Clears the email list.
 */

/*! \fn void OContact::insertEmails( const QStringList &emailList )

  Appends the \a emailList to the exiting email list
 */

/*!
  Returns a list of email addresses belonging to the contact, including
  the default email address.
*/
QStringList OContact::emailList() const
{
    QString emailStr = emails();

    QStringList r;
    if ( !emailStr.isEmpty() ) {
	qDebug(" emailstr ");
	QStringList l = QStringList::split( emailSeparator(), emailStr );
	for ( QStringList::ConstIterator it = l.begin();it != l.end();++it )
	    r += (*it).simplifyWhiteSpace();
    }

    return r;
}

/*!
  \overload

  Generates the string for the contact to be filed as from the first,
  middle and last name of the contact.
*/
void OContact::setFileAs()
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
void OContact::save( QString &buf ) const
{
    static const QStringList SLFIELDS = fields();
    // I'm expecting "<Contact " in front of this...
    for ( QMap<int, QString>::ConstIterator it = mMap.begin();
	  it != mMap.end(); ++it ) {
	const QString &value = it.data();
	int key = it.key();
	if ( !value.isEmpty() ) {
	    if ( key == Qtopia::AddressCategory || key == Qtopia::AddressUid)
		continue;

	    key -= Qtopia::AddressCategory+1;
	    buf += SLFIELDS[key];
	    buf += "=\"" + Qtopia::escapeString(value) + "\" ";
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
QStringList OContact::fields()
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
void OContact::setEmails( const QString &str )
{
    replace( Qtopia::Emails, str );
    if ( str.isEmpty() )
        setDefaultEmail( QString::null );
}

/*!
  Sets the list of children for the contact to those contained in \a str.
*/
void OContact::setChildren( const QString &str )
{
    replace( Qtopia::Children, str );
}

/*!
  Returns TRUE if the contact matches the regular expression \a regexp.
  Otherwise returns FALSE.
*/
bool OContact::match( const QString &regexp ) const
{
    return match(QRegExp(regexp));
}

/*!
  \overload
  Returns TRUE if the contact matches the regular expression \a regexp.
  Otherwise returns FALSE.
*/
bool OContact::match( const QRegExp &r ) const
{
    bool match;
    match = false;
    QMap<int, QString>::ConstIterator it;
    for ( it = mMap.begin(); it != mMap.end(); ++it ) {
	if ( (*it).find( r ) > -1 ) {
	    match = true;
	    break;
	}
    }
    return match;
}


QString OContact::toShortText() const
{
	return ( fullName() );
}
QString OContact::type() const
{
	return QString::fromLatin1( "OContact" );
}

// Definition is missing ! (se)
QMap<QString,QString> OContact::toExtraMap() const
{
	qWarning ("Function not implemented: OContact::toExtraMap()");
	QMap <QString,QString> useless;
	return useless;
}

class QString OContact::recordField( int pos ) const
{
	QStringList SLFIELDS = fields(); // ?? why this ? (se)
	return SLFIELDS[pos];
}

// In future releases, we should store birthday and anniversary
// internally as QDate instead of QString !
// QString is always too complicate to interprete (DD.MM.YY, DD/MM/YY, MM/DD/YY, etc..)(se)

/*! \fn void OContact::setBirthday( const QDate& date )
  Sets the birthday for the contact to \a date. If date is null
  the current stored date will be removed.
*/
void OContact::setBirthday( const QDate &v )
{
	if ( v.isNull() ){
		qWarning( "Remove Birthday");
		replace( Qtopia::Birthday, QString::null );
		return;
	}

	if ( v.isValid() )
		replace( Qtopia::Birthday, TimeConversion::toString( v ) );

}


/*! \fn void OContact::setAnniversary( const QDate &date )
  Sets the anniversary of the contact to \a date. If date is
  null, the current stored date will be removed.
*/
void OContact::setAnniversary( const QDate &v )
{
	if ( v.isNull() ){
		qWarning( "Remove Anniversary");
		replace( Qtopia::Anniversary, QString::null );
		return;
	}

	if ( v.isValid() )
		replace( Qtopia::Anniversary, TimeConversion::toString( v ) );
}

/*! \fn QDate OContact::birthday() const
  Returns the birthday of the contact.
*/
QDate OContact::birthday() const
{
	QString str = find( Qtopia::Birthday );
	qWarning ("Birthday %s", str.latin1() );
	if ( !str.isEmpty() )
		return  TimeConversion::fromString ( str );
	else
		return QDate();
}


/*! \fn QDate OContact::anniversary() const
  Returns the anniversary of the contact.
*/
QDate OContact::anniversary() const
{
	QDate empty;
	QString str = find( Qtopia::Anniversary );
	qWarning ("Anniversary %s", str.latin1() );
	if ( !str.isEmpty() )
		return TimeConversion::fromString ( str );
	else
		return empty;
}


void OContact::insertEmail( const QString &v )
{
    //qDebug("insertEmail %s", v.latin1());
    QString e = v.simplifyWhiteSpace();
    QString def = defaultEmail();

    // if no default, set it as the default email and don't insert
    if ( def.isEmpty() ) {
	setDefaultEmail( e ); // will insert into the list for us
	return;
    }

    // otherwise, insert assuming doesn't already exist
    QString emailsStr = find( Qtopia::Emails );
    if ( emailsStr.contains( e ))
	return;
    if ( !emailsStr.isEmpty() )
	emailsStr += emailSeparator();
    emailsStr += e;
    replace( Qtopia::Emails, emailsStr );
}

void OContact::removeEmail( const QString &v )
{
    QString e = v.simplifyWhiteSpace();
    QString def = defaultEmail();
    QString emailsStr = find( Qtopia::Emails );
    QStringList emails = emailList();

    // otherwise, must first contain it
    if ( !emailsStr.contains( e ) )
	return;

    // remove it
    //qDebug(" removing email from list %s", e.latin1());
    emails.remove( e );
    // reset the string
    emailsStr = emails.join(emailSeparator()); // Sharp's brain dead separator
    replace( Qtopia::Emails, emailsStr );

    // if default, then replace the default email with the first one
    if ( def == e ) {
	//qDebug("removeEmail is default; setting new default");
	if ( !emails.count() )
	    clearEmails();
	else // setDefaultEmail will remove e from the list
	    setDefaultEmail( emails.first() );
    }
}
void OContact::clearEmails()
{
    mMap.remove( Qtopia::DefaultEmail );
    mMap.remove( Qtopia::Emails );
}
void OContact::setDefaultEmail( const QString &v )
{
    QString e = v.simplifyWhiteSpace();

    //qDebug("OContact::setDefaultEmail %s", e.latin1());
    replace( Qtopia::DefaultEmail, e );

    if ( !e.isEmpty() )
	insertEmail( e );

}

void OContact::insertEmails( const QStringList &v )
{
    for ( QStringList::ConstIterator it = v.begin(); it != v.end(); ++it )
	insertEmail( *it );
}

void OContact::setUid( int i )
{ 
	OPimRecord::setUid(i); 
	replace( Qtopia::AddressUid , QString::number(i)); 
}
