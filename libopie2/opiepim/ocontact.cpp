/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
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
#include "vobject_p.h"
#include "qfiledirect_p.h"

#include <qpe/stringutil.h>
#include <qpe/timeconversion.h>

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

Qtopia::UidGen OContact::sUidGen( Qtopia::UidGen::Qtopia );

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

    if ( uidStr.isEmpty() )
	setUid( uidGen().generate() );
    else
	setUid( uidStr.toInt() );

    if ( !uidStr.isEmpty() )
	setUid( uidStr.toInt() );
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

/*! \fn void OContact::setBirthday( const QString &str )
  Sets the birthday for the contact to \a str.
*/

/*! \fn void OContact::setAnniversary( const QString &str )
  Sets the anniversary of the contact to \a str.
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

/*! \fn QString OContact::birthday() const
  Returns the birthday of the contact.
*/

/*! \fn QString OContact::anniversary() const
  Returns the anniversary of the contact.
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
    str = birthday();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Birthday: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = anniversary();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Anniversary: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";
    str = nickname();
    if ( !str.isEmpty() )
	text += "<b>" + QObject::tr("Nickname: ") + "</b>"
		+ Qtopia::escapeString(str) + "<br>";

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
  \internal
  Returns a translated list of field names for a contact.
*/
QStringList OContact::trfields()
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
    list.append( QObject::tr( "Business Phone" ) );
    list.append( QObject::tr( "Business Fax" ) );
    list.append( QObject::tr( "Business Mobile" ) );

    list.append( QObject::tr( "Default Email" ) );
    list.append( QObject::tr( "Emails" ) );

    list.append( QObject::tr( "Home Phone" ) );
    list.append( QObject::tr( "Home Fax" ) );
    list.append( QObject::tr( "Home Mobile" ) );

    list.append( QObject::tr( "Business Street" ) );
    list.append( QObject::tr( "Business City" ) );
    list.append( QObject::tr( "Business State" ) );
    list.append( QObject::tr( "Business Zip" ) );
    list.append( QObject::tr( "Business Country" ) );
    list.append( QObject::tr( "Business Pager" ) );
    list.append( QObject::tr( "Business WebPage" ) );

    list.append( QObject::tr( "Office" ) );
    list.append( QObject::tr( "Profession" ) );
    list.append( QObject::tr( "Assistant" ) );
    list.append( QObject::tr( "Manager" ) );

    list.append( QObject::tr( "Home Street" ) );
    list.append( QObject::tr( "Home City" ) );
    list.append( QObject::tr( "Home State" ) );
    list.append( QObject::tr( "Home Zip" ) );
    list.append( QObject::tr( "Home Country" ) );
    list.append( QObject::tr( "Home Web Page" ) );

    list.append( QObject::tr( "Spouse" ) );
    list.append( QObject::tr( "Gender" ) );
    list.append( QObject::tr( "Birthday" ) );
    list.append( QObject::tr( "Anniversary" ) );
    list.append( QObject::tr( "Nickname" ) );
    list.append( QObject::tr( "Children" ) );

    list.append( QObject::tr( "Notes" ) );
    list.append( QObject::tr( "Groups" ) );

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

// vcard conversion code
/*!
  \internal
*/
static inline VObject *safeAddPropValue( VObject *o, const char *prop, const QString &value )
{
    VObject *ret = 0;
    if ( o && !value.isEmpty() )
	ret = addPropValue( o, prop, value.latin1() );
    return ret;
}

/*!
  \internal
*/
static inline VObject *safeAddProp( VObject *o, const char *prop)
{
    VObject *ret = 0;
    if ( o )
	ret = addProp( o, prop );
    return ret;
}

/*!
  \internal
*/
static VObject *createVObject( const OContact &c )
{
    VObject *vcard = newVObject( VCCardProp );
    safeAddPropValue( vcard, VCVersionProp, "2.1" );
    safeAddPropValue( vcard, VCLastRevisedProp, TimeConversion::toISO8601( QDateTime::currentDateTime() ) );
    safeAddPropValue( vcard, VCUniqueStringProp, QString::number(c.uid()) );

    // full name
    safeAddPropValue( vcard, VCFullNameProp, c.fullName() );

    // name properties
    VObject *name = safeAddProp( vcard, VCNameProp );
    safeAddPropValue( name, VCFamilyNameProp, c.lastName() );
    safeAddPropValue( name, VCGivenNameProp, c.firstName() );
    safeAddPropValue( name, VCAdditionalNamesProp, c.middleName() );
    safeAddPropValue( name, VCNamePrefixesProp, c.title() );
    safeAddPropValue( name, VCNameSuffixesProp, c.suffix() );

    // home properties
    VObject *home_adr= safeAddProp( vcard, VCAdrProp );
    safeAddProp( home_adr, VCHomeProp );
    safeAddPropValue( home_adr, VCStreetAddressProp, c.homeStreet() );
    safeAddPropValue( home_adr, VCCityProp, c.homeCity() );
    safeAddPropValue( home_adr, VCRegionProp, c.homeState() );
    safeAddPropValue( home_adr, VCPostalCodeProp, c.homeZip() );
    safeAddPropValue( home_adr, VCCountryNameProp, c.homeCountry() );

    VObject *home_phone = safeAddPropValue( vcard, VCTelephoneProp, c.homePhone() );
    safeAddProp( home_phone, VCHomeProp );
    home_phone = safeAddPropValue( vcard, VCTelephoneProp, c.homeMobile() );
    safeAddProp( home_phone, VCHomeProp );
    safeAddProp( home_phone, VCCellularProp );
    home_phone = safeAddPropValue( vcard, VCTelephoneProp, c.homeFax() );
    safeAddProp( home_phone, VCHomeProp );
    safeAddProp( home_phone, VCFaxProp );

    VObject *url = safeAddPropValue( vcard, VCURLProp, c.homeWebpage() );
    safeAddProp( url, VCHomeProp );

    // work properties
    VObject *work_adr= safeAddProp( vcard, VCAdrProp );
    safeAddProp( work_adr, VCWorkProp );
    safeAddPropValue( work_adr, VCStreetAddressProp, c.businessStreet() );
    safeAddPropValue( work_adr, VCCityProp, c.businessCity() );
    safeAddPropValue( work_adr, VCRegionProp, c.businessState() );
    safeAddPropValue( work_adr, VCPostalCodeProp, c.businessZip() );
    safeAddPropValue( work_adr, VCCountryNameProp, c.businessCountry() );

    VObject *work_phone = safeAddPropValue( vcard, VCTelephoneProp, c.businessPhone() );
    safeAddProp( work_phone, VCWorkProp );
    work_phone = safeAddPropValue( vcard, VCTelephoneProp, c.businessMobile() );
    safeAddProp( work_phone, VCWorkProp );
    safeAddProp( work_phone, VCCellularProp );
    work_phone = safeAddPropValue( vcard, VCTelephoneProp, c.businessFax() );
    safeAddProp( work_phone, VCWorkProp );
    safeAddProp( work_phone, VCFaxProp );
    work_phone = safeAddPropValue( vcard, VCTelephoneProp, c.businessPager() );
    safeAddProp( work_phone, VCWorkProp );
    safeAddProp( work_phone, VCPagerProp );

    url = safeAddPropValue( vcard, VCURLProp, c.businessWebpage() );
    safeAddProp( url, VCWorkProp );

    VObject *title = safeAddPropValue( vcard, VCTitleProp, c.jobTitle() );
    safeAddProp( title, VCWorkProp );


    QStringList emails = c.emailList();
    emails.prepend( c.defaultEmail() );
    for( QStringList::Iterator it = emails.begin(); it != emails.end(); ++it ) {
	VObject *email = safeAddPropValue( vcard, VCEmailAddressProp, *it );
	safeAddProp( email, VCInternetProp );
    }

    safeAddPropValue( vcard, VCNoteProp, c.notes() );

    safeAddPropValue( vcard, VCBirthDateProp, c.birthday() );

    if ( !c.company().isEmpty() || !c.department().isEmpty() || !c.office().isEmpty() ) {
	VObject *org = safeAddProp( vcard, VCOrgProp );
	safeAddPropValue( org, VCOrgNameProp, c.company() );
	safeAddPropValue( org, VCOrgUnitProp, c.department() );
	safeAddPropValue( org, VCOrgUnit2Prop, c.office() );
    }

    // some values we have to export as custom fields
    safeAddPropValue( vcard, "X-Qtopia-Profession", c.profession() );
    safeAddPropValue( vcard, "X-Qtopia-Manager", c.manager() );
    safeAddPropValue( vcard, "X-Qtopia-Assistant", c.assistant() );

    safeAddPropValue( vcard, "X-Qtopia-Spouse", c.spouse() );
    safeAddPropValue( vcard, "X-Qtopia-Gender", c.gender() );
    safeAddPropValue( vcard, "X-Qtopia-Anniversary", c.anniversary() );
    safeAddPropValue( vcard, "X-Qtopia-Nickname", c.nickname() );
    safeAddPropValue( vcard, "X-Qtopia-Children", c.children() );

    return vcard;
}


/*!
  \internal
*/
static OContact parseVObject( VObject *obj )
{
    OContact c;

    VObjectIterator it;
    initPropIterator( &it, obj );
    while( moreIteration( &it ) ) {
	VObject *o = nextVObject( &it );
	QCString name = vObjectName( o );
	QCString value = vObjectStringZValue( o );
	if ( name == VCNameProp ) {
	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectTypeInfo( o );
		QString value = vObjectStringZValue( o );
		if ( name == VCNamePrefixesProp )
		    c.setTitle( value );
		else if ( name == VCNameSuffixesProp )
		    c.setSuffix( value );
		else if ( name == VCFamilyNameProp )
		    c.setLastName( value );
		else if ( name == VCGivenNameProp )
		    c.setFirstName( value );
		else if ( name == VCAdditionalNamesProp )
		    c.setMiddleName( value );
	    }
	}
	else if ( name == VCAdrProp ) {
	    bool work = TRUE; // default address is work address
	    QString street;
	    QString city;
	    QString region;
	    QString postal;
	    QString country;

	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectName( o );
		QString value = vObjectStringZValue( o );
		if ( name == VCHomeProp )
		    work = FALSE;
		else if ( name == VCWorkProp )
		    work = TRUE;
		else if ( name == VCStreetAddressProp )
		    street = value;
		else if ( name == VCCityProp )
		    city = value;
		else if ( name == VCRegionProp )
		    region = value;
		else if ( name == VCPostalCodeProp )
		    postal = value;
		else if ( name == VCCountryNameProp )
		    country = value;
	    }
	    if ( work ) {
		c.setBusinessStreet( street );
		c.setBusinessCity( city );
		c.setBusinessCountry( country );
		c.setBusinessZip( postal );
		c.setBusinessState( region );
	    } else {
		c.setHomeStreet( street );
		c.setHomeCity( city );
		c.setHomeCountry( country );
		c.setHomeZip( postal );
		c.setHomeState( region );
	    }
	}
	else if ( name == VCTelephoneProp ) {
	    enum {
		HOME = 0x01,
		WORK = 0x02,
		VOICE = 0x04,
		CELL = 0x08,
		FAX = 0x10,
		PAGER = 0x20,
		UNKNOWN = 0x80
	    };
	    int type = 0;

	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectTypeInfo( o );
		if ( name == VCHomeProp )
		    type |= HOME;
		else if ( name == VCWorkProp )
		    type |= WORK;
		else if ( name == VCVoiceProp )
		    type |= VOICE;
		else if ( name == VCCellularProp )
		    type |= CELL;
		else if ( name == VCFaxProp )
		    type |= FAX;
		else if ( name == VCPagerProp )
		    type |= PAGER;
		else  if ( name == VCPreferredProp )
		    ;
		else
		    type |= UNKNOWN;
	    }
	    if ( (type & UNKNOWN) != UNKNOWN ) {
		if ( ( type & (HOME|WORK) ) == 0 ) // default
		    type |= HOME;
		if ( ( type & (VOICE|CELL|FAX|PAGER) ) == 0 ) // default
		    type |= VOICE;

		if ( (type & (VOICE|HOME) ) == (VOICE|HOME) )
		    c.setHomePhone( value );
		if ( ( type & (FAX|HOME) ) == (FAX|HOME) )
		    c.setHomeFax( value );
		if ( ( type & (CELL|HOME) ) == (CELL|HOME) )
		    c.setHomeMobile( value );
		if ( ( type & (VOICE|WORK) ) == (VOICE|WORK) )
		    c.setBusinessPhone( value );
		if ( ( type & (FAX|WORK) ) == (FAX|WORK) )
		    c.setBusinessFax( value );
		if ( ( type & (CELL|WORK) ) == (CELL|WORK) )
		    c.setBusinessMobile( value );
		if ( ( type & (PAGER|WORK) ) == (PAGER|WORK) )
		    c.setBusinessPager( value );
	    }
	}
	else if ( name == VCEmailAddressProp ) {
	    QString email = vObjectStringZValue( o );
	    bool valid = TRUE;
	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectTypeInfo( o );
		if ( name != VCInternetProp && name != VCHomeProp &&
		     name != VCWorkProp &&
		     name != VCPreferredProp )
		    // ### preffered should map to default email
		    valid = FALSE;
	    }
	    if ( valid ) {
		c.insertEmail( email );
	    }
	}
	else if ( name == VCURLProp ) {
	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectTypeInfo( o );
		if ( name == VCHomeProp )
		    c.setHomeWebpage( value );
		else if ( name == VCWorkProp )
		    c.setBusinessWebpage( value );
	    }
	}
	else if ( name == VCOrgProp ) {
	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectName( o );
		QString value = vObjectStringZValue( o );
		if ( name == VCOrgNameProp )
		    c.setCompany( value );
		else if ( name == VCOrgUnitProp )
		    c.setDepartment( value );
		else if ( name == VCOrgUnit2Prop )
		    c.setOffice( value );
	    }
	}
	else if ( name == VCTitleProp ) {
	    c.setJobTitle( value );
	}
	else if ( name == "X-Qtopia-Profession" ) {
	    c.setProfession( value );
	}
	else if ( name == "X-Qtopia-Manager" ) {
	    c.setManager( value );
	}
	else if ( name == "X-Qtopia-Assistant" ) {
	    c.setAssistant( value );
	}
	else if ( name == "X-Qtopia-Spouse" ) {
	    c.setSpouse( value );
	}
	else if ( name == "X-Qtopia-Gender" ) {
	    c.setGender( value );
	}
	else if ( name == "X-Qtopia-Anniversary" ) {
	    c.setAnniversary( value );
	}
	else if ( name == "X-Qtopia-Nickname" ) {
	    c.setNickname( value );
	}
	else if ( name == "X-Qtopia-Children" ) {
	    c.setChildren( value );
	}


#if 0
	else {
	    printf("Name: %s, value=%s\n", name.data(), vObjectStringZValue( o ) );
	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectName( o );
		QString value = vObjectStringZValue( o );
		printf(" subprop: %s = %s\n", name.data(), value.latin1() );
	    }
	}
#endif
    }
    c.setFileAs();
    return c;
}

/*!
  Writes the list of \a contacts as a set of VCards to the file \a filename.
*/
void OContact::writeVCard( const QString &filename, const QValueList<OContact> &contacts)
{
	QFileDirect f( filename.utf8().data() );
	if ( !f.open( IO_WriteOnly ) ) {
		qWarning("Unable to open vcard write");
		return;
	}

    QValueList<OContact>::ConstIterator it;
    for( it = contacts.begin(); it != contacts.end(); ++it ) {
	VObject *obj = createVObject( *it );
	writeVObject(f.directHandle() , obj );
	cleanVObject( obj );
    }
    cleanStrTbl();
}

/*!
  writes \a contact as a VCard to the file \a filename.
*/
void OContact::writeVCard( const QString &filename, const OContact &contact)
{
	QFileDirect f( filename.utf8().data() );
	if ( !f.open( IO_WriteOnly ) ) {
		qWarning("Unable to open vcard write");
		return;
	}

    VObject *obj = createVObject( contact );
	writeVObject( f.directHandle() , obj );
	cleanVObject( obj );

	cleanStrTbl();
}

/*!
  Returns the set of contacts read as VCards from the file \a filename.
*/
QValueList<OContact> OContact::readVCard( const QString &filename )
{
    qDebug("trying to open %s, exists=%d", filename.utf8().data(), QFileInfo( filename.utf8().data() ).size() );
    VObject *obj = Parse_MIME_FromFileName( (char *)filename.utf8().data() );

    qDebug("vobject = %p", obj );

    QValueList<OContact> contacts;

    while ( obj ) {
	contacts.append( parseVObject( obj ) );

	VObject *t = obj;
	obj = nextVObjectInList(obj);
	cleanVObject( t );
    }

    return contacts;
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
