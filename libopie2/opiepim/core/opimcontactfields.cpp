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

#include "opimcontactfields.h"

/* OPIE */
#include <opie2/opimcontact.h>
#include <qpe/recordfields.h> // We should use our own enum in the future ..
#include <qpe/config.h>

/* QT */
#include <qobject.h>
#include <qstringlist.h>


namespace Opie
{
/*!
  \internal
  Returns a list of personal field names for a contact.
*/
QStringList OPimContactFields::personalfields( bool sorted, bool translated )
{
    QStringList list;
    QMap<int, QString> mapIdToStr;
    if ( translated )
        mapIdToStr = idToTrFields();
    else
        mapIdToStr = idToUntrFields();

    list.append( mapIdToStr[ Qtopia::AddressUid ] );
    list.append( mapIdToStr[ Qtopia::AddressCategory ] );

    list.append( mapIdToStr[ Qtopia::Title ] );
    list.append( mapIdToStr[ Qtopia::FirstName ] );
    list.append( mapIdToStr[ Qtopia::MiddleName ] );
    list.append( mapIdToStr[ Qtopia::LastName ] );
    list.append( mapIdToStr[ Qtopia::Suffix ] );
    list.append( mapIdToStr[ Qtopia::FileAs ] );

    list.append( mapIdToStr[ Qtopia::JobTitle ] );
    list.append( mapIdToStr[ Qtopia::Department ] );
    list.append( mapIdToStr[ Qtopia::Company ] );

    list.append( mapIdToStr[ Qtopia::Notes ] );
    list.append( mapIdToStr[ Qtopia::Groups ] );

    if ( sorted ) list.sort();
    return list;
}

/*!
  \internal
  Returns a list of details field names for a contact.
*/
QStringList OPimContactFields::detailsfields( bool sorted, bool translated )
{
    QStringList list;
    QMap<int, QString> mapIdToStr;
    if ( translated )
        mapIdToStr = idToTrFields();
    else
        mapIdToStr = idToUntrFields();

    list.append( mapIdToStr[ Qtopia::Office ] );
    list.append( mapIdToStr[ Qtopia::Profession ] );
    list.append( mapIdToStr[ Qtopia::Assistant ] );
    list.append( mapIdToStr[ Qtopia::Manager ] );

    list.append( mapIdToStr[ Qtopia::Spouse ] );
    list.append( mapIdToStr[ Qtopia::Gender ] );
    list.append( mapIdToStr[ Qtopia::Birthday ] );
    list.append( mapIdToStr[ Qtopia::Anniversary ] );
    list.append( mapIdToStr[ Qtopia::Nickname ] );
    list.append( mapIdToStr[ Qtopia::Children ] );

    if ( sorted ) list.sort();
    return list;
}

/*!
  \internal
  Returns a list of phone field names for a contact.
*/
QStringList OPimContactFields::phonefields( bool sorted, bool translated )
{
    QStringList list;
    QMap<int, QString> mapIdToStr;
    if ( translated )
        mapIdToStr = idToTrFields();
    else
        mapIdToStr = idToUntrFields();

    list.append( mapIdToStr[ Qtopia::BusinessPhone ] );
    list.append( mapIdToStr[ Qtopia::BusinessFax ] );
    list.append( mapIdToStr[ Qtopia::BusinessMobile ] );
    list.append( mapIdToStr[ Qtopia::BusinessPager ] );
    list.append( mapIdToStr[ Qtopia::BusinessWebPage ] );

    list.append( mapIdToStr[ Qtopia::DefaultEmail ] );
    list.append( mapIdToStr[ Qtopia::Emails ] );

    list.append( mapIdToStr[ Qtopia::HomePhone ] );
    list.append( mapIdToStr[ Qtopia::HomeFax ] );
    list.append( mapIdToStr[ Qtopia::HomeMobile ] );
    // list.append( mapIdToStr[Qtopia::HomePager] );
    list.append( mapIdToStr[ Qtopia::HomeWebPage ] );

    if ( sorted ) list.sort();

    return list;
}

/*!
  \internal
  Returns a list of field names for a contact.
*/
QStringList OPimContactFields::fields( bool sorted, bool translated )
{
    QStringList list;
    QMap<int, QString> mapIdToStr;
    if ( translated )
        mapIdToStr = idToTrFields();
    else
        mapIdToStr = idToUntrFields();

    list += personalfields( sorted, translated );

    list += phonefields( sorted, translated );

    list.append( mapIdToStr[ Qtopia::BusinessStreet ] );
    list.append( mapIdToStr[ Qtopia::BusinessCity ] );
    list.append( mapIdToStr[ Qtopia::BusinessState ] );
    list.append( mapIdToStr[ Qtopia::BusinessZip ] );
    list.append( mapIdToStr[ Qtopia::BusinessCountry ] );

    list.append( mapIdToStr[ Qtopia::HomeStreet ] );
    list.append( mapIdToStr[ Qtopia::HomeCity ] );
    list.append( mapIdToStr[ Qtopia::HomeState ] );
    list.append( mapIdToStr[ Qtopia::HomeZip ] );
    list.append( mapIdToStr[ Qtopia::HomeCountry ] );

    list += detailsfields( sorted, translated );

    if ( sorted ) list.sort();

    return list;
}


/*!
  \internal
  Returns an untranslated list of personal field names for a contact.
*/
QStringList OPimContactFields::untrpersonalfields( bool sorted )
{
    return personalfields( sorted, false );
}


/*!
  \internal
  Returns a translated list of personal field names for a contact.
*/
QStringList OPimContactFields::trpersonalfields( bool sorted )
{
    return personalfields( sorted, true );
}


/*!
  \internal
  Returns an untranslated list of details field names for a contact.
*/
QStringList OPimContactFields::untrdetailsfields( bool sorted )
{
    return detailsfields( sorted, false );
}


/*!
  \internal
  Returns a translated list of details field names for a contact.
*/
QStringList OPimContactFields::trdetailsfields( bool sorted )
{
    return detailsfields( sorted, true );
}


/*!
  \internal
  Returns a translated list of phone field names for a contact.
*/
QStringList OPimContactFields::trphonefields( bool sorted )
{
    return phonefields( sorted, true );
}

/*!
  \internal
  Returns an untranslated list of phone field names for a contact.
*/
QStringList OPimContactFields::untrphonefields( bool sorted )
{
    return phonefields( sorted, false );
}


/*!
  \internal
  Returns a translated list of field names for a contact.
*/
QStringList OPimContactFields::trfields( bool sorted )
{
    return fields( sorted, true );
}

/*!
  \internal
  Returns an untranslated list of field names for a contact.
*/
QStringList OPimContactFields::untrfields( bool sorted )
{
    return fields( sorted, false );
}

QMap<int, QString> OPimContactFields::idToTrFields()
{
    QMap<int, QString> ret_map;

    ret_map.insert( Qtopia::AddressUid, QObject::tr( "User Id" ) );
    ret_map.insert( Qtopia::AddressCategory, QObject::tr( "Categories" ) );

    ret_map.insert( Qtopia::Title, QObject::tr( "Name Title" ) );
    ret_map.insert( Qtopia::FirstName, QObject::tr( "First Name" ) );
    ret_map.insert( Qtopia::MiddleName, QObject::tr( "Middle Name" ) );
    ret_map.insert( Qtopia::LastName, QObject::tr( "Last Name" ) );
    ret_map.insert( Qtopia::Suffix, QObject::tr( "Suffix" ) );
    ret_map.insert( Qtopia::FileAs, QObject::tr( "File As" ) );

    ret_map.insert( Qtopia::JobTitle, QObject::tr( "Job Title" ) );
    ret_map.insert( Qtopia::Department, QObject::tr( "Department" ) );
    ret_map.insert( Qtopia::Company, QObject::tr( "Company" ) );
    ret_map.insert( Qtopia::BusinessPhone, QObject::tr( "Business Phone" ) );
    ret_map.insert( Qtopia::BusinessFax, QObject::tr( "Business Fax" ) );
    ret_map.insert( Qtopia::BusinessMobile, QObject::tr( "Business Mobile" ) );

    // email
    ret_map.insert( Qtopia::DefaultEmail, QObject::tr( "Default Email" ) );
    ret_map.insert( Qtopia::Emails, QObject::tr( "Emails" ) );

    ret_map.insert( Qtopia::HomePhone, QObject::tr( "Home Phone" ) );
    ret_map.insert( Qtopia::HomeFax, QObject::tr( "Home Fax" ) );
    ret_map.insert( Qtopia::HomeMobile, QObject::tr( "Home Mobile" ) );

    // business
    ret_map.insert( Qtopia::BusinessStreet, QObject::tr( "Business Street" ) );
    ret_map.insert( Qtopia::BusinessCity, QObject::tr( "Business City" ) );
    ret_map.insert( Qtopia::BusinessState, QObject::tr( "Business State" ) );
    ret_map.insert( Qtopia::BusinessZip, QObject::tr( "Business Zip" ) );
    ret_map.insert( Qtopia::BusinessCountry, QObject::tr( "Business Country" ) );
    ret_map.insert( Qtopia::BusinessPager, QObject::tr( "Business Pager" ) );
    ret_map.insert( Qtopia::BusinessWebPage, QObject::tr( "Business WebPage" ) );

    ret_map.insert( Qtopia::Office, QObject::tr( "Office" ) );
    ret_map.insert( Qtopia::Profession, QObject::tr( "Profession" ) );
    ret_map.insert( Qtopia::Assistant, QObject::tr( "Assistant" ) );
    ret_map.insert( Qtopia::Manager, QObject::tr( "Manager" ) );

    // home
    ret_map.insert( Qtopia::HomeStreet, QObject::tr( "Home Street" ) );
    ret_map.insert( Qtopia::HomeCity, QObject::tr( "Home City" ) );
    ret_map.insert( Qtopia::HomeState, QObject::tr( "Home State" ) );
    ret_map.insert( Qtopia::HomeZip, QObject::tr( "Home Zip" ) );
    ret_map.insert( Qtopia::HomeCountry, QObject::tr( "Home Country" ) );
    ret_map.insert( Qtopia::HomeWebPage, QObject::tr( "Home Web Page" ) );

    //personal
    ret_map.insert( Qtopia::Spouse, QObject::tr( "Spouse" ) );
    ret_map.insert( Qtopia::Gender, QObject::tr( "Gender" ) );
    ret_map.insert( Qtopia::Birthday, QObject::tr( "Birthday" ) );
    ret_map.insert( Qtopia::Anniversary, QObject::tr( "Anniversary" ) );
    ret_map.insert( Qtopia::Nickname, QObject::tr( "Nickname" ) );
    ret_map.insert( Qtopia::Children, QObject::tr( "Children" ) );

    // other
    ret_map.insert( Qtopia::Notes, QObject::tr( "Notes" ) );


    return ret_map;
}

QMap<int, QString> OPimContactFields::idToUntrFields()
{
    QMap<int, QString> ret_map;

    ret_map.insert( Qtopia::AddressUid, "User Id" );
    ret_map.insert( Qtopia::AddressCategory, "Categories" );

    ret_map.insert( Qtopia::Title, "Name Title" );
    ret_map.insert( Qtopia::FirstName, "First Name" );
    ret_map.insert( Qtopia::MiddleName, "Middle Name" );
    ret_map.insert( Qtopia::LastName, "Last Name" );
    ret_map.insert( Qtopia::Suffix, "Suffix" );
    ret_map.insert( Qtopia::FileAs, "File As" );

    ret_map.insert( Qtopia::JobTitle, "Job Title" );
    ret_map.insert( Qtopia::Department, "Department" );
    ret_map.insert( Qtopia::Company, "Company" );
    ret_map.insert( Qtopia::BusinessPhone, "Business Phone" );
    ret_map.insert( Qtopia::BusinessFax, "Business Fax" );
    ret_map.insert( Qtopia::BusinessMobile, "Business Mobile" );

    // email
    ret_map.insert( Qtopia::DefaultEmail, "Default Email" );
    ret_map.insert( Qtopia::Emails, "Emails" );

    ret_map.insert( Qtopia::HomePhone, "Home Phone" );
    ret_map.insert( Qtopia::HomeFax, "Home Fax" );
    ret_map.insert( Qtopia::HomeMobile, "Home Mobile" );

    // business
    ret_map.insert( Qtopia::BusinessStreet, "Business Street" );
    ret_map.insert( Qtopia::BusinessCity, "Business City" );
    ret_map.insert( Qtopia::BusinessState, "Business State" );
    ret_map.insert( Qtopia::BusinessZip, "Business Zip" );
    ret_map.insert( Qtopia::BusinessCountry, "Business Country" );
    ret_map.insert( Qtopia::BusinessPager, "Business Pager" );
    ret_map.insert( Qtopia::BusinessWebPage, "Business WebPage" );

    ret_map.insert( Qtopia::Office, "Office" );
    ret_map.insert( Qtopia::Profession, "Profession" );
    ret_map.insert( Qtopia::Assistant, "Assistant" );
    ret_map.insert( Qtopia::Manager, "Manager" );

    // home
    ret_map.insert( Qtopia::HomeStreet, "Home Street" );
    ret_map.insert( Qtopia::HomeCity, "Home City" );
    ret_map.insert( Qtopia::HomeState, "Home State" );
    ret_map.insert( Qtopia::HomeZip, "Home Zip" );
    ret_map.insert( Qtopia::HomeCountry, "Home Country" );
    ret_map.insert( Qtopia::HomeWebPage, "Home Web Page" );

    //personal
    ret_map.insert( Qtopia::Spouse, "Spouse" );
    ret_map.insert( Qtopia::Gender, "Gender" );
    ret_map.insert( Qtopia::Birthday, "Birthday" );
    ret_map.insert( Qtopia::Anniversary, "Anniversary" );
    ret_map.insert( Qtopia::Nickname, "Nickname" );
    ret_map.insert( Qtopia::Children, "Children" );

    // other
    ret_map.insert( Qtopia::Notes, "Notes" );
    ret_map.insert( Qtopia::Groups, "Groups" );


    return ret_map;
}

QMap<QString, int> OPimContactFields::trFieldsToId()
{
    QMap<int, QString> idtostr = idToTrFields();
    QMap<QString, int> ret_map;


    QMap<int, QString>::Iterator it;
    for ( it = idtostr.begin(); it != idtostr.end(); ++it )
        ret_map.insert( *it, it.key() );


    return ret_map;
}

/* ======================================================================= */

QMap<QString, int> OPimContactFields::untrFieldsToId()
{
    QMap<int, QString> idtostr = idToUntrFields();
    QMap<QString, int> ret_map;


    QMap<int, QString>::Iterator it;
    for ( it = idtostr.begin(); it != idtostr.end(); ++it )
        ret_map.insert( *it, it.key() );


    return ret_map;
}


OPimContactFields::OPimContactFields() :
        fieldOrder( DEFAULT_FIELD_ORDER ),
        changedFieldOrder( false )
{
    // Get the global field order from the config file and
    // use it as a start pattern
    Config cfg ( "AddressBook" );
    cfg.setGroup( "ContactFieldOrder" );
    globalFieldOrder = cfg.readEntry( "General", DEFAULT_FIELD_ORDER );
}

OPimContactFields::~OPimContactFields()
{

    // We will store the fieldorder into the config file
    // to reuse it for the future..
    if ( changedFieldOrder )
    {
        Config cfg ( "AddressBook" );
        cfg.setGroup( "ContactFieldOrder" );
        cfg.writeEntry( "General", globalFieldOrder );
    }
}



void OPimContactFields::saveToRecord( OPimContact &cnt )
{

    qDebug( "ocontactfields saveToRecord: >%s<", fieldOrder.latin1() );

    // Store fieldorder into this contact.
    cnt.setCustomField( CONTACT_FIELD_ORDER_NAME, fieldOrder );

    globalFieldOrder = fieldOrder;
    changedFieldOrder = true;

}

void OPimContactFields::loadFromRecord( const OPimContact &cnt )
{
    qDebug( "ocontactfields loadFromRecord" );
    qDebug( "loading >%s<", cnt.fullName().latin1() );

    // Get fieldorder for this contact. If none is defined,
    // we will use the global one from the config file..

    fieldOrder = cnt.customField( CONTACT_FIELD_ORDER_NAME );

    qDebug( "fieldOrder from contact>%s<", fieldOrder.latin1() );

    if ( fieldOrder.isEmpty() )
    {
        fieldOrder = globalFieldOrder;
    }


    qDebug( "effective fieldOrder in loadFromRecord >%s<", fieldOrder.latin1() );
}

void OPimContactFields::setFieldOrder( int num, int index )
{
    qDebug( "qcontactfields setfieldorder pos %i -> %i", num, index );

    fieldOrder[ num ] = QString::number( index, 16 ) [ 0 ];

    // We will store this new fieldorder globally to
    // remember it for contacts which have none
    globalFieldOrder = fieldOrder;
    changedFieldOrder = true;

    qDebug( "fieldOrder >%s<", fieldOrder.latin1() );
}

int OPimContactFields::getFieldOrder( int num, int defIndex )
{
    qDebug( "ocontactfields getFieldOrder" );
    qDebug( "fieldOrder >%s<", fieldOrder.latin1() );

    // Get index of combo as char..
    QChar poschar = fieldOrder[ num ];

    bool ok;
    int ret = 0;
    // Convert char to number..
    if ( !( poschar == QChar::null ) )
        ret = QString( poschar ).toInt( &ok, 16 );
    else
        ok = false;

    // Return default value if index for
    // num was not set or if anything else happened..
    if ( !ok ) ret = defIndex;

    qDebug( "returning >%i<", ret );

    return ret;

}

}
