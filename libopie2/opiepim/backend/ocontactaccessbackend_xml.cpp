/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
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
/*
 * XML Backend for the OPIE-Contact Database.
 */


/* OPIE */
#include <opie2/ocontactaccessbackend_xml.h>
#include <opie2/xmltree.h>
#include <opie2/ocontactaccessbackend.h>
#include <opie2/ocontactaccess.h>
#include <opie2/opimrecord.h>
#include <opie2/odebug.h>

#include <qpe/global.h>
#include <qpe/stringutil.h>
#include <qpe/timestring.h>

/* QT */
#include <qasciidict.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qarray.h>
#include <qmap.h>
#include <qtextstream.h>

/* STD */
#include <stdlib.h>
#include <errno.h>

using namespace Opie::Core;


namespace Opie {
OPimContactAccessBackend_XML::OPimContactAccessBackend_XML ( const QString& appname, const QString& filename )
    : m_changed( false )
    , m_journalEnabled( false )
    , m_journalName()
    , m_fileName( filename )
    , m_appName( appname )
{
    // Just m_contactlist should call delete if an entry
    // is removed.
    m_contactList.setAutoDelete( true );
    m_uidToContact.setAutoDelete( false );

    if( !m_appName.isEmpty() ) {
        // Set journal file name
        m_journalName = getenv("HOME");
        m_journalName +="/.abjournal" + appname;
    }

    // Expecting to access the default filename if nothing else is set
    if ( m_fileName.isEmpty() )
        m_fileName = Global::applicationFileName( "addressbook","addressbook.xml" );
}

bool OPimContactAccessBackend_XML::save()
{
    if ( m_fileName.isEmpty() )
        return false;

    if ( !m_changed )
        return true;

    QString strNewFile = m_fileName + ".new";
    QFile f( strNewFile );
    if ( !f.open( IO_WriteOnly|IO_Raw ) )
        return false;

    OFileWriter fw( &f );
    if( !write( fw ) ) {
        f.close();
        QFile::remove( strNewFile );
        return false;
    }

    f.close();

    // move the file over, I'm just going to use the system call
    // because, I don't feel like using QDir.
    if ( ::rename( strNewFile.latin1(), m_fileName.latin1() ) < 0 ) {
        // remove the tmp file...
        QFile::remove( strNewFile );
    }

    /* The journalfile should be removed now... */
    removeJournal();

    m_changed = false;
    return true;
}

/*!
  \internal
  Appends the contact information to \a buf.
*/
void OPimContactAccessBackend_XML::saveEntry( const OPimContact *contact, const QIntDict<QString> &revdict, QString &buf )
{
    QMap<int, QString> map = contact->toMap();
    // I'm expecting "<Contact " in front of this...
    for ( QMap<int, QString>::ConstIterator it = map.begin();
            it != map.end(); ++it )
    {
        const QString &value = it.data();
        int key = it.key();
        if ( !value.isEmpty() )
        {
            if ( key == Qtopia::AddressCategory || key == Qtopia::AddressUid || key == FIELDID_ACTION )
                continue;

            QString *name = revdict[ key ];
            if( name )
                buf += *name + "=\"" + Qtopia::escapeString( value ) + "\" ";
        }
    }
    buf += contact->customToXml();
    if ( contact->categories().count() > 0 )
        buf += "Categories=\"" + contact->idsToString( contact->categories() ) + "\" ";
    buf += "Uid=\"" + QString::number( contact->uid() ) + "\" ";

    OPimRecord::ChangeAction action = contact->action();
    if( action != OPimRecord::ACTION_ADD )
        buf += "change_action=\"" + OPimRecord::actionToStr( action ) + "\" ";

    // You need to close this yourself
}

bool OPimContactAccessBackend_XML::write( OAbstractWriter &wr )
{
    QAsciiDict<int> dict(OPimEvent::FRecChildren+1);
    initDict( dict );
    QIntDict<QString> revdict( dict.size() );
    revdict.setAutoDelete( true );
    // Now we need to reverse the dictionary (!)
    for( QAsciiDictIterator<int> it( dict ); it.current(); ++it ) {
        revdict.insert( (*it), new QString( it.currentKey() ) );
    }

    // Write Header
    QString out = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE Addressbook ><AddressBook>\n"
        " <Groups>\n"
        " </Groups>\n"
        " <Contacts>\n";
    QCString cstr = out.utf8();
    if ( !wr.writeString( cstr ) )
        return false;

    // Write all contacts
    QListIterator<OPimContact> it( m_contactList );
    for ( ; it.current(); ++it ) {
        out = "<Contact ";
        saveEntry( (*it), revdict, out );
        out += "/>\n";
        cstr = out.utf8();
        if ( !wr.writeString( cstr ) )
            return false;
    }

    // Write Footer
    out = " </Contacts>\n</AddressBook>\n";
    cstr = out.utf8();
    if ( !wr.writeString( cstr ) )
        return false;

    return true;
}

bool OPimContactAccessBackend_XML::load ()
{
    if ( m_fileName.isEmpty() )
        return false;

    m_contactList.clear();
    m_uidToContact.clear();

    /* Load XML-File if it exists */
    XMLElement *root = XMLElement::load( m_fileName );
    if(root) {
        bool res = loadXml( root, false );
        delete root;
        if ( !res )
            return false;
    }
    else
        return false;

    /* We use the time of the last read to check if the file was
     * changed externally.
     */
    QFileInfo fi( m_fileName );
    m_readtime = fi.lastModified ();

    /* The returncode of the journalfile is ignored due to the
     * fact that it does not exist when this class is instantiated !
     * But there may such a file exist, if the application crashed.
     * Therefore we try to load it to get the changes before the #
     * crash happened...
     */
    root = XMLElement::load( m_journalName );
    if(root) {
        loadXml( root, true );
        delete root;
    }

    return true;
}

void OPimContactAccessBackend_XML::clear ()
{
    m_contactList.clear();
    m_uidToContact.clear();

    m_changed = false;
}

bool OPimContactAccessBackend_XML::wasChangedExternally()
{
    if ( m_fileName.isEmpty() )
        return false;

    QFileInfo fi( m_fileName );
    QDateTime lastmod = fi.lastModified ();
    return (lastmod != m_readtime);
}

UIDArray OPimContactAccessBackend_XML::allRecords() const
{
    UIDArray uid_list( m_contactList.count() );

    uint counter = 0;
    QListIterator<OPimContact> it( m_contactList );
    for( ; it.current(); ++it ) {
        uid_list[counter++] = (*it)->uid();
    }

    return ( uid_list );
}

OPimContact OPimContactAccessBackend_XML::find ( int uid ) const
{
    OPimContact foundContact; //Create empty contact

    OPimContact* found = m_uidToContact.find( QString().setNum( uid ) );

    if ( found ) {
        foundContact = *found;
    }

    return ( foundContact );
}


UIDArray OPimContactAccessBackend_XML::matchRegexp(  const QRegExp &r ) const
{
    UIDArray m_currentQuery( m_contactList.count() );
    QListIterator<OPimContact> it( m_contactList );
    uint arraycounter = 0;

    for( ; it.current(); ++it ) {
        if ( (*it)->match( r ) ) {
            m_currentQuery[arraycounter++] = (*it)->uid();
        }

    }
    // Shrink to fit..
    m_currentQuery.resize(arraycounter);

    return m_currentQuery;
}



#if 0
// Currently only asc implemented..
UIDArray OPimContactAccessBackend_XML::sorted( bool asc,  int , int ,  int )
{
    QMap<QString, int> nameToUid;
    QStringList names;
    UIDArray m_currentQuery( m_contactList.count() );

    // First fill map and StringList with all Names
    // Afterwards sort namelist and use map to fill array to return..
    QListIterator<OPimContact> it( m_contactList );
    for( ; it.current(); ++it ) {
        names.append( (*it)->fileAs() + QString::number( (*it)->uid() ) );
        nameToUid.insert( (*it)->fileAs() + QString::number( (*it)->uid() ), (*it)->uid() );
    }
    names.sort();

    int i = 0;
    if ( asc ) {
        for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it )
            m_currentQuery[i++] = nameToUid[ (*it) ];
    }else{
        for ( QStringList::Iterator it = names.end(); it != names.begin(); --it )
            m_currentQuery[i++] = nameToUid[ (*it) ];
    }

    return m_currentQuery;

}
#endif


bool OPimContactAccessBackend_XML::add ( const OPimContact &newcontact )
{
    updateJournal (newcontact, OPimRecord::ACTION_ADD);
    addContact_p( newcontact );

    m_changed = true;

    return true;
}

bool OPimContactAccessBackend_XML::replace ( const OPimContact &contact )
{
    m_changed = true;

    OPimContact* found = m_uidToContact.find ( QString().setNum( contact.uid() ) );

    if ( found ) {
        OPimContact* newCont = new OPimContact( contact );

        updateJournal ( *newCont, OPimRecord::ACTION_REPLACE);
        m_contactList.removeRef ( found );
        m_contactList.append ( newCont );
        m_uidToContact.remove( QString().setNum( contact.uid() ) );
        m_uidToContact.insert( QString().setNum( newCont->uid() ), newCont );

        return true;
    }
    else
        return false;
}

bool OPimContactAccessBackend_XML::remove ( int uid )
{
    m_changed = true;

    OPimContact* found = m_uidToContact.find ( QString().setNum( uid ) );

    if ( found ) {
        updateJournal ( *found, OPimRecord::ACTION_REMOVE);
        m_contactList.removeRef ( found );
        m_uidToContact.remove( QString().setNum( uid ) );

        return true;
    }
    else
        return false;
}

bool OPimContactAccessBackend_XML::reload() {
    /* Reload is the same as load in this implementation */
    return ( load() );
}

void OPimContactAccessBackend_XML::addContact_p( const OPimContact &newcontact )
{
    OPimContact* contRef = new OPimContact( newcontact );

    m_contactList.append ( contRef );
    m_uidToContact.insert( QString().setNum( newcontact.uid() ), contRef );
}

bool OPimContactAccessBackend_XML::readInto( OPimXmlReader &rd, OPimContactAccess *target )
{
    QAsciiDict<int> dict(OPimEvent::FRecChildren+1);
    initDict( dict );

    OPimWriteBackXmlHandler<OPimContact> handler( "Contact", dict, target );
    OPimXmlStreamParser parser( handler );
    rd.read( parser );
    return true;
}

void OPimContactAccessBackend_XML::initDict( QAsciiDict<int> &dict ) const
{
    dict.setAutoDelete( TRUE );
    dict.insert( "Uid", new int(Qtopia::AddressUid) );
    dict.insert( "Title", new int(Qtopia::Title) );
    dict.insert( "FirstName", new int(Qtopia::FirstName) );
    dict.insert( "MiddleName", new int(Qtopia::MiddleName) );
    dict.insert( "LastName", new int(Qtopia::LastName) );
    dict.insert( "Suffix", new int(Qtopia::Suffix) );
    dict.insert( "FileAs", new int(Qtopia::FileAs) );
    dict.insert( "Categories", new int(Qtopia::AddressCategory) );
    dict.insert( "DefaultEmail", new int(Qtopia::DefaultEmail) );
    dict.insert( "Emails", new int(Qtopia::Emails) );
    dict.insert( "HomeStreet", new int(Qtopia::HomeStreet) );
    dict.insert( "HomeCity", new int(Qtopia::HomeCity) );
    dict.insert( "HomeState", new int(Qtopia::HomeState) );
    dict.insert( "HomeZip", new int(Qtopia::HomeZip) );
    dict.insert( "HomeCountry", new int(Qtopia::HomeCountry) );
    dict.insert( "HomePhone", new int(Qtopia::HomePhone) );
    dict.insert( "HomeFax", new int(Qtopia::HomeFax) );
    dict.insert( "HomeMobile", new int(Qtopia::HomeMobile) );
    dict.insert( "HomeWebPage", new int(Qtopia::HomeWebPage) );
    dict.insert( "Company", new int(Qtopia::Company) );
    dict.insert( "BusinessStreet", new int(Qtopia::BusinessStreet) );
    dict.insert( "BusinessCity", new int(Qtopia::BusinessCity) );
    dict.insert( "BusinessState", new int(Qtopia::BusinessState) );
    dict.insert( "BusinessZip", new int(Qtopia::BusinessZip) );
    dict.insert( "BusinessCountry", new int(Qtopia::BusinessCountry) );
    dict.insert( "BusinessWebPage", new int(Qtopia::BusinessWebPage) );
    dict.insert( "JobTitle", new int(Qtopia::JobTitle) );
    dict.insert( "Department", new int(Qtopia::Department) );
    dict.insert( "Office", new int(Qtopia::Office) );
    dict.insert( "BusinessPhone", new int(Qtopia::BusinessPhone) );
    dict.insert( "BusinessFax", new int(Qtopia::BusinessFax) );
    dict.insert( "BusinessMobile", new int(Qtopia::BusinessMobile) );
    dict.insert( "BusinessPager", new int(Qtopia::BusinessPager) );
    dict.insert( "Profession", new int(Qtopia::Profession) );
    dict.insert( "Assistant", new int(Qtopia::Assistant) );
    dict.insert( "Manager", new int(Qtopia::Manager) );
    dict.insert( "Spouse", new int(Qtopia::Spouse) );
    dict.insert( "Children", new int(Qtopia::Children) );
    dict.insert( "Gender", new int(Qtopia::Gender) );
    dict.insert( "Birthday", new int(Qtopia::Birthday) );
    dict.insert( "Anniversary", new int(Qtopia::Anniversary) );
    dict.insert( "Nickname", new int(Qtopia::Nickname) );
    dict.insert( "Notes", new int(Qtopia::Notes) );
    dict.insert( "Groups", new int(Qtopia::Groups) );
    // change_action is for the journal & synchronisation only
    dict.insert( "change_action", new int(FIELDID_ACTION) );
}

/* This function loads the xml-database and the journalfile */
bool OPimContactAccessBackend_XML::loadXml( XMLElement *root, bool isJournal )
{
    OPimRecord::ChangeAction action = OPimRecord::ACTION_ADD;
    QMap<int, QString> contactMap;
    QMap<QString, QString> customMap;
    QMap<QString, QString>::Iterator customIt;
    QAsciiDict<int> dict( 47 );
    initDict( dict );

    if(root != 0l ) { // start parsing
        /* Parse all XML-Elements and put the data into the
         * Contact-Class
         */
        XMLElement *element = root->firstChild();
        element = element ? element->firstChild() : 0;

        m_journalEnabled = false;
        
        /* Search Tag "Contacts" which is the parent of all Contacts */
        while( element && !isJournal ) {
            if( element->tagName() != QString::fromLatin1("Contacts") ) {
                element = element->nextChild();
            }
            else {
                element = element->firstChild();
                break;
            }
        }
        /* Parse all Contacts and ignore unknown tags */
        while( element ) {
            if( element->tagName() != QString::fromLatin1("Contact") ) {
                element = element->nextChild();
                continue;
            }
            /* Found alement with tagname "contact", now parse and store all
             * attributes contained
             */
            QString dummy;
            action = OPimRecord::ACTION_ADD;

            XMLElement::AttributeMap aMap = element->attributes();
            XMLElement::AttributeMap::Iterator it;
            contactMap.clear();
            customMap.clear();
            for( it = aMap.begin(); it != aMap.end(); ++it ) {
                int *find = dict[ it.key() ];
                /* Unknown attributes will be stored as "Custom" elements */
                if ( !find ) {
                    //contact.setCustomField(it.key(), it.data());
                    customMap.insert( it.key(),  it.data() );
                    continue;
                }

                /* Check if special conversion is needed and add attribute
                 * into Contact class
                 */
                switch( *find ) {
                    /*
                      case Qtopia::AddressUid:
                      contact.setUid( it.data().toInt() );
                      break;
                      case Qtopia::AddressCategory:
                      contact.setCategories( Qtopia::Record::idsFromString( it.data( )));
                      break;
                    */
                case FIELDID_ACTION:
                    action = OPimRecord::ChangeAction(it.data().toInt());
                    owarn << "ODefBack(journal)::ACTION found: " << action << oendl;
                    break;
                default: // no conversion needed add them to the map
                    contactMap.insert( *find, it.data() );
                    break;
                }
            }
            /* now generate the Contact contact */
            OPimContact contact( contactMap );

            for (customIt = customMap.begin(); customIt != customMap.end(); ++customIt ) {
                contact.setCustomField( customIt.key(),  customIt.data() );
            }

            applyAction( action, contact );

            /* Move to next element */
            element = element->nextChild();
        }

        m_journalEnabled = true;
        
        return true;
    }
    else {
        owarn << "XML document null!" << oendl;
        return false;
    }
}


void OPimContactAccessBackend_XML::updateJournal( const OPimContact& cnt,
                           OPimRecord::ChangeAction action )
{
    if( !m_journalEnabled || m_journalName.isEmpty() )
        return;

    QFile f( m_journalName );
    bool created = !f.exists();
    if ( !f.open(IO_WriteOnly|IO_Append) )
        return;

    QString buf;
    QCString str;

    QAsciiDict<int> dict(OPimEvent::FRecChildren+1);
    initDict( dict );
    QIntDict<QString> revdict( dict.size() );
    revdict.setAutoDelete( true );
    // Now we need to reverse the dictionary (!)
    for( QAsciiDictIterator<int> it( dict ); it.current(); ++it ) {
        revdict.insert( (*it), new QString( it.currentKey() ) );
    }
    
    // if the file was created, we have to set the Tag "<CONTACTS>" to
    // get a XML-File which is readable by our parser.
    // This is just a cheat, but better than rewrite the parser.
    if ( created ) {
        buf = "<Contacts>";
        QCString cstr = buf.utf8();
        f.writeBlock( cstr.data(), cstr.length() );
    }

    buf = "<Contact ";
    saveEntry( &cnt, revdict, buf );
    buf += " action=\"" + QString::number( (int)action ) + "\" ";
    buf += "/>\n";
    QCString cstr = buf.utf8();
    f.writeBlock( cstr.data(), cstr.length() );
}

void OPimContactAccessBackend_XML::removeJournal()
{
    QFile f ( m_journalName );
    if ( f.exists() )
        f.remove();
}


}
