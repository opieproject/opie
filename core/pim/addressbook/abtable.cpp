/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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

#include <qpe/categoryselect.h>
#include <qpe/config.h>
#include <qpe/stringutil.h>
#include <qpe/qcopenvelope_qws.h>

#include <qasciidict.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>

#include "abtable.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <ctype.h> //toupper() for key hack

static bool contactCompare( const Contact &cnt, const QRegExp &r, int category );

//### qtmail/addresslist.cpp hardcodes this filename as well
static QString journalFileName()
{
    QString str = getenv("HOME");
    str +="/.abjournal";
    return str;
}



/*!
  \class AbTableItem abtable.h

  \brief QTableItem based class for showing a field of an entry
*/

AbTableItem::AbTableItem( QTable *t, EditType et, const QString &s,
			  const QString &secondSortKey)
    : QTableItem( t, et, s )
{
    //    sortKey = s.lower() + QChar( '\0' ) + secondSortKey.lower();
    sortKey = Qtopia::buildSortKey( s, secondSortKey );
}

int AbTableItem::alignment() const
{
    return AlignLeft|AlignVCenter;
}

QString AbTableItem::key() const
{
    return sortKey;
}

// A way to reset the item, without out doing a delete or a new...
void AbTableItem::setItem( const QString &txt, const QString &secondKey )
{
    setText( txt );
    sortKey = Qtopia::buildSortKey( txt, secondKey );

    //    sortKey = txt.lower() + QChar( '\0' ) + secondKey.lower();
}

/*!
  \class AbPickItem abtable.h

  \brief QTableItem based class for showing slection of an entry
*/

AbPickItem::AbPickItem( QTable *t ) :
    QTableItem(t, WhenCurrent, "?")
{
}

QWidget *AbPickItem::createEditor() const
{
    QComboBox* combo = new QComboBox( table()->viewport() );
    ( (AbPickItem*)this )->cb = combo;
    AbTable* t = static_cast<AbTable*>(table());
    QStringList c = t->choiceNames();
    int cur = 0;
    for (QStringList::ConstIterator it = c.begin(); it!=c.end(); ++it) {
	if ( *it == text() )
	    cur = combo->count();
	combo->insertItem(*it);
    }
    combo->setCurrentItem(cur);
    return combo;
}

void AbPickItem::setContentFromEditor( QWidget *w )
{
    if ( w->inherits("QComboBox") )
	setText( ( (QComboBox*)w )->currentText() );
    else
	QTableItem::setContentFromEditor( w );
}

/*!
  \class AbTable abtable.h

  \brief QTable based class for showing a list of entries
*/

AbTable::AbTable( const QValueList<int> *order, QWidget *parent, const char *name )
// #ifdef QT_QTABLE_NOHEADER_CONSTRUCTOR
//     : QTable( 0, 0, parent, name, TRUE ),
// #else
    : QTable( parent, name ),
// #endif
      lastSortCol( -1 ),
      asc( TRUE ),
      intFields( order ),
      currFindRow( -2 ),
      mCat( 0 )
{
    mCat.load( categoryFileName() );
    setSelectionMode( NoSelection );
    init();
    setSorting( TRUE );
    connect( this, SIGNAL(clicked(int,int,int,const QPoint &)),
             this, SLOT(itemClicked(int,int)) );
}

AbTable::~AbTable()
{
}

void AbTable::init()
{
    showChar = '\0';
    setNumRows( 0 );
    setNumCols( 2 );

    horizontalHeader()->setLabel( 0, tr( "Full Name" ));
    horizontalHeader()->setLabel( 1, tr( "Contact" ));
    setLeftMargin( 0 );
    verticalHeader()->hide();
}

void AbTable::columnClicked( int col )
{
    if ( !sorting() )
	return;

    if ( lastSortCol == -1 )
	lastSortCol = col;

    if ( col == lastSortCol ) {
	asc = !asc;
    } else {
	lastSortCol = col;
	asc = TRUE;
    }
    resort();
}

void AbTable::resort()
{
    if ( sorting() ) {
	if ( lastSortCol == -1 )
	    lastSortCol = 0;
	sortColumn( lastSortCol, asc, TRUE );
	updateVisible();
    }
}

Contact AbTable::currentEntry()
{
    Contact cnt;
    AbTableItem *abItem;
    abItem = static_cast<AbTableItem*>(item( currentRow(), 0 ));
    if ( abItem ) {
	cnt = contactList[abItem];
    }
    return cnt;
}

void AbTable::replaceCurrentEntry( const Contact &newContact )
{
    int row = currentRow();
    updateJournal( newContact, Contact::ACTION_REPLACE, row );
    updateVisible();

    journalFreeReplace( newContact, row );
}

void AbTable::deleteCurrentEntry()
{
    int row = currentRow();
    AbTableItem *abItem;
    abItem = static_cast<AbTableItem*>(item( row, 0 ));
    Contact oldContact;
    oldContact = contactList[abItem];
    updateJournal( oldContact, Contact::ACTION_REMOVE, row );

    // a little wasteful, but it ensure's there is only one place
    // where we delete.
    journalFreeRemove( row );
    updateVisible();

    if ( numRows() == 0 )
	emit empty( TRUE );
}

void AbTable::clear()
{
    contactList.clear();
    for ( int r = 0; r < numRows(); ++r ) {
	for ( int c = 0; c < numCols(); ++c ) {
            if ( cellWidget( r, c ) )
                clearCellWidget( r, c );
	    clearCell( r, c );
	}
    }
    setNumRows( 0 );
}

void AbTable::refresh()
{
    int rows = numRows();
    QString value;
    AbTableItem *abi;
    for ( int r = 0; r < rows; ++r ) {
	abi = static_cast<AbTableItem*>( item(r, 0) );
	value = findContactContact( contactList[abi] );
	static_cast<AbTableItem*>( item(r, 1) )->setItem( value, abi->text() );
    }
    resort();
}

void AbTable::keyPressEvent( QKeyEvent *e )
{
    char key = toupper( e->ascii() );

    if ( key >= 'A' && key <= 'Z' )
	moveTo( key );

    switch( e->key() ) {
	case Qt::Key_Space:
	case Qt::Key_Return:
	case Qt::Key_Enter:
	    emit details();
	    break;
	default:
	    QTable::keyPressEvent( e );
    }
}

void AbTable::moveTo( char c )
{

    int rows = numRows();
    QString value;
    AbTableItem *abi;
    int r;
    if ( asc ) {
	r = 0;
	while ( r < rows-1) {
	    abi = static_cast<AbTableItem*>( item(r, 0) );
	    QChar first = abi->key()[0];
	    //### is there a bug in QChar to char comparison???
	    if ( first.row() || first.cell() >= c )
		break;
	    r++;
	}
    } else {
	//### should probably disable reverse sorting instead
	r = rows - 1;
	while ( r > 0 ) {
	    abi = static_cast<AbTableItem*>( item(r, 0) );
	    QChar first = abi->key()[0];
	    //### is there a bug in QChar to char comparison???
	    if ( first.row() || first.cell() >= c )
		break;
	    r--;
	}
    }
    setCurrentCell( r, currentColumn() );
}


QString AbTable::findContactName( const Contact &entry )
{
    // We use the fileAs, then company, defaultEmail
    QString str;
    str = entry.fileAs();
    if ( str.isEmpty() ) {
	str = entry.company();
	if ( str.isEmpty() ) {
	    str = entry.defaultEmail();
	}
    }
    return str;
}

QString AbTable::findContactContact( const Contact &entry )
{
    QString value;
    value = "";
    for ( QValueList<int>::ConstIterator it = intFields->begin();
	  it != intFields->end(); ++it ) {
	switch ( *it ) {
	default:
	    break;
	case Qtopia::Title:
	    value = entry.title();
	    break;
	case Qtopia::Suffix:
	    value = entry.suffix();
	    break;
	case Qtopia::FileAs:
	    value = entry.fileAs();
	    break;
	case Qtopia::DefaultEmail:
	    value = entry.defaultEmail();
	case Qtopia::Emails:
	    value = entry.emails();
	    break;
	case Qtopia::HomeStreet:
	    value = entry.homeStreet();
	    break;
	case Qtopia::HomeCity:
	    value = entry.homeCity();
	    break;
	case Qtopia::HomeState:
	    value = entry.homeState();
	    break;
	case Qtopia::HomeZip:
	    value = entry.homeZip();
	    break;
	case Qtopia::HomeCountry:
	    value = entry.homeCountry();
	    break;
	case Qtopia::HomePhone:
	    value = entry.homePhone();
	    break;
	case Qtopia::HomeFax:
	    value = entry.homeFax();
	    break;
	case Qtopia::HomeMobile:
	    value = entry.homeMobile();
	    break;
	case Qtopia::HomeWebPage:
	    value = entry.homeWebpage();
	    break;
	case Qtopia::Company:
	    value = entry.company();
	    break;
	case Qtopia::BusinessCity:
	    value = entry.businessCity();
	    break;
	case Qtopia::BusinessStreet:
	    value = entry.businessStreet();
	    break;
	case Qtopia::BusinessZip:
	    value = entry.businessZip();
	    break;
	case Qtopia::BusinessCountry:
	    value = entry.businessCountry();
	    break;
	case Qtopia::BusinessWebPage:
	    value = entry.businessWebpage();
	    break;
	case Qtopia::JobTitle:
	    value = entry.jobTitle();
	    break;
	case Qtopia::Department:
	    value = entry.department();
	    break;
	case Qtopia::Office:
	    value = entry.office();
	    break;
	case Qtopia::BusinessPhone:
	    value = entry.businessPhone();
	    break;
	case Qtopia::BusinessFax:
	    value = entry.businessFax();
	    break;
	case Qtopia::BusinessMobile:
	    value = entry.businessMobile();
	    break;
	case Qtopia::BusinessPager:
	    value = entry.businessPager();
	    break;
	case Qtopia::Profession:
	    value = entry.profession();
	    break;
	case Qtopia::Assistant:
	    value = entry.assistant();
	    break;
	case Qtopia::Manager:
	    value = entry.manager();
	    break;
	case Qtopia::Spouse:
	    value = entry.spouse();
	    break;
	case Qtopia::Gender:
	    value = entry.gender();
	    break;
	case Qtopia::Birthday:
	    value = entry.birthday();
	    break;
	case Qtopia::Anniversary:
	    value = entry.anniversary();
	    break;
	case Qtopia::Nickname:
	    value = entry.nickname();
	    break;
	case Qtopia::Children:
	    value = entry.children();
	    break;
	case Qtopia::Notes:
	    value = entry.notes();
	    break;
	}
	if ( !value.isEmpty() )
	    break;
    }
    return value;
}

void AbTable::addEntry( const Contact &newCnt )
{
    int row = numRows();
    setNumRows( row + 1 );
    updateJournal( newCnt, Contact::ACTION_ADD );
    insertIntoTable( newCnt, row );
    setCurrentCell( row, 0 );
    updateVisible();
}

void AbTable::resizeRows( int size ) {
/*
	if (numRows()) {
		for (int i = 0; i < numRows(); i++) {
			setRowHeight( i, size );
		}
	}*/
	updateVisible();
}

void AbTable::updateJournal( const Contact &cnt,
			     Contact::journal_action action, int row )
{
    QFile f( journalFileName() );
    if ( !f.open(IO_WriteOnly|IO_Append) )
	 return;
    QString buf;
    QCString str;
    buf = "<Contact ";
    cnt.save( buf );
    buf += " action=\"" + QString::number( (int)action ) + "\" ";
    if ( action == Contact::ACTION_REMOVE || action == Contact::ACTION_REPLACE)
	buf += " actionrow=\"" + QString::number(row) + "\" ";
    buf += "/>\n";
    QCString cstr = buf.utf8();
    f.writeBlock( cstr.data(), cstr.length() );
    QCopEnvelope( "QPE/PIM", "addressbookUpdated()" );
}

bool AbTable::save( const QString &fn )
{
//     QTime t;
//     t.start();

    QString strNewFile = fn + ".new";
    QFile f( strNewFile );
    if ( !f.open( IO_WriteOnly|IO_Raw ) )
	return false;

    int total_written;
    QString out;
    out = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE Addressbook ><AddressBook>\n"
	  " <Groups>\n"
	  " </Groups>\n"
	  " <Contacts>\n";
    QMapIterator<AbTableItem*, Contact> it;
    for ( it = contactList.begin(); it != contactList.end(); ++it ) {
	out += "<Contact ";
	it.data().save( out );
	out += "/>\n";
	QCString cstr = out.utf8();
	total_written = f.writeBlock( cstr.data(), cstr.length() );
	if ( total_written != int(cstr.length()) ) {
	    f.close();
	    QFile::remove( strNewFile );
	    return false;
	}
	out = "";
    }
    out += " </Contacts>\n</AddressBook>\n";

    QCString cstr = out.utf8();
    total_written = f.writeBlock( cstr.data(), cstr.length() );
    if ( total_written != int(cstr.length()) ) {
	f.close();
 	QFile::remove( strNewFile );
 	return false;
    }
    f.close();

//     qDebug("saving: %d", t.elapsed() );

    // move the file over, I'm just going to use the system call
    // because, I don't feel like using QDir.
    if ( ::rename( strNewFile.latin1(), fn.latin1() ) < 0 ) {
	qWarning( "problem renaming file %s to %s, errno: %d",
		  strNewFile.latin1(), fn.latin1(), errno );
	// remove the tmp file...
	QFile::remove( strNewFile );
    }
    // remove the journal...
    QFile::remove( journalFileName() );
    return true;
}

void AbTable::load( const QString &fn )
{
    setSorting( false );
    loadFile( fn, false );
    // merge in the journal
    if ( QFile::exists( journalFileName() ) ) {
	loadFile( journalFileName(), true );
	save( fn );
    }
    setSorting( true );
    resort();
}

void AbTable::loadFile( const QString &strFile, bool journalFile )
{
//     QTime t;
//     t.start();
    QFile f( strFile );
    if ( !f.open(IO_ReadOnly) )
	return;
    QList<Contact> list;
    list.setAutoDelete( TRUE );
    QByteArray ba = f.readAll();
    f.close();
   if (ba.isEmpty() )
       return;
    char *uc = ba.data();//(QChar *)data.unicode();
    int len = ba.size();//data.length();
    bool foundAction = false;
    Contact::journal_action action;
    bool foundKey = false;
    int journalKey = 0;

    const int JOURNALACTION = Qtopia::Notes + 1;
    const int JOURNALROW = JOURNALACTION + 1;

    // **********************************
    // CHANGE THE SIZE OF THE DICT IF YOU ADD ANY MORE FIELDS!!!!
    // **********************************
    QAsciiDict<int> dict( 47 );
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
    dict.insert( "action", new int(JOURNALACTION) );
    dict.insert( "actionrow", new int(JOURNALROW) );

    int i = 0;
    int num = 0;
    char *point;
    while ( (point = strstr( uc+i, "<Contact " ) ) != NULL ) {
	i = point - uc;
	// if we are reading the standard file, we just need to
	// insert info, so just say we'll do an insert...
	action = Contact::ACTION_ADD;
	// new Contact
	Contact *cnt = new Contact;
	i += 9;
	while ( 1 ) {
	    while ( i < len && (uc[i] == ' ' || uc[i] == '\n' || uc[i] == '\r') )
		i++;
	    if ( i >= len-2 || (uc[i] == '/' && uc[i+1] == '>') )
		break;
	    // we have another attribute read it.
	    int j = i;
	    while ( j < len && uc[j] != '=' )
		j++;
	    char *attr = uc+i;
	    uc[j] = '\0';
	    //qDebug("attr=%s", attr.latin1() );
	    i = ++j; // skip =
	    while ( i < len && uc[i] != '"' )
		i++;
	    j = ++i;
	    bool haveEnt = FALSE;
	    bool haveUtf = FALSE;
	    while ( j < len && uc[j] != '"' ) {
		if ( uc[j] == '&' )
		    haveEnt = TRUE;
		if ( ((unsigned char)uc[j]) > 0x7f )
		    haveUtf = TRUE;
		j++;
	    }

	    if ( j == i ) {
		// empty value
		i = j + 1;
		continue;
	    }

	    QString value = haveUtf ? QString::fromUtf8( uc+i, j-i )
			    : QString::fromLatin1( uc+i, j-i );
	    if ( haveEnt )
		value = Qtopia::plainString( value );
	    i = j + 1;

	    int *find = dict[ attr ];
	    if ( !find ) {
		cnt->setCustomField(attr, value);
		continue;
	    }
#if 1
	    switch( *find ) {
	    case Qtopia::AddressUid:
		cnt->setUid( value.toInt() );
		break;
	    case Qtopia::AddressCategory:
		cnt->setCategories( Qtopia::Record::idsFromString( value ));
		break;
	    case JOURNALACTION:
		action = Contact::journal_action(value.toInt());
		break;
	    case JOURNALROW:
		journalKey = value.toInt();
		break;

	    default:
		cnt->insert( *find, value );
		break;
	    }
#endif
	}

	// sadly we can't delay adding of items from the journal to get
	// the proper effect, but then, the journal should _never_ be
	// that huge, and recovering from a crash is not necessarily
	// a *fast* thing.
	switch ( action ) {
	case Contact::ACTION_ADD:
	    if ( journalFile ) {
		int myrows = numRows();
		setNumRows( myrows + 1 );
		insertIntoTable( *cnt, myrows );
		delete cnt;
	    }
	    else
		list.append( cnt );
	    break;
	case Contact::ACTION_REMOVE:
	    // yup, we don't use the entry to remove the object...
	    journalFreeRemove( journalKey  );
	    delete cnt;
	    break;
	case Contact::ACTION_REPLACE:
	    journalFreeReplace( *cnt, journalKey );
	    delete cnt;
	    break;
	default:
	    break;
	}
	num++;
	foundAction = false;
	foundKey = false;
// 	if ( num % 100 == 0 ) {
// 	    qDebug("loading file, num=%d, t=%d", num, t.elapsed() );
// 	}
    }
    if ( list.count() > 0 ) {
	internalAddEntries( list );
    }
//    qDebug("done loading %d, t=%d", num, t.elapsed() );

}

void AbTable::realignTable( int row )
{
    QTableItem *ti1,
	*ti2;
    int totalRows = numRows();
    for ( int curr = row; curr < totalRows - 1; curr++ ) {
	// the same info from the todo list still applies, but I
	// don't think it is _too_ bad.
	ti1 = item( curr + 1, 0 );
	ti2 = item( curr + 1, 1 );
	takeItem( ti1 );
	takeItem( ti2 );
	setItem( curr, 0, ti1 );
	setItem( curr, 1, ti2 );
    }
    setNumRows( totalRows - 1 );
    resort();
}

void AbTable::insertIntoTable( const Contact &cnt, int row )
{
    QString strName,
	    strContact;

    strName = findContactName( cnt );
    strContact = findContactContact( cnt );

    AbTableItem *ati;
    ati = new AbTableItem( this, QTableItem::Never, strName, strContact);
    contactList.insert( ati, cnt );
    setItem( row, 0, ati );
    ati = new AbTableItem( this, QTableItem::Never, strContact, strName);
    setItem( row, 1, ati );

    //### cannot do this; table only has two columns at this point
    //    setItem( row, 2, new AbPickItem( this ) );

    // resort at some point?
}

void AbTable::internalAddEntries( QList<Contact> &list )
{
    setUpdatesEnabled( FALSE );
    setNumRows( list.count() );
    int row = 0;
    Contact *it;
    for ( it = list.first(); it; it = list.next() )
	insertIntoTable( *it, row++ );
    resort();
    setUpdatesEnabled( TRUE );
}


void AbTable::journalFreeReplace( const Contact &cnt, int row )
{
    QString strName,
	strContact;
    AbTableItem *ati = 0l;

    strName = findContactName( cnt );
    strContact = findContactContact( cnt );
    ati = static_cast<AbTableItem*>(item(row, 0));
    if ( ati != 0 ) {
        contactList.remove( ati );
        ati->setItem( strName, strContact );
        contactList.insert( ati, cnt );

        ati = static_cast<AbTableItem*>(item(row, 1));
        ati->setItem( strContact, strName );
    }else{
        int myrows = numRows();
        setNumRows( myrows + 1 );
        insertIntoTable( cnt, myrows );
        // gets deleted when returning
    }
}

void AbTable::journalFreeRemove( int row )
{
    AbTableItem *ati;
    ati = static_cast<AbTableItem*>(item(row, 0));
    if ( !ati )
	return;
    contactList.remove( ati );
    realignTable( row );
}

#if QT_VERSION <= 230
#ifndef SINGLE_APP
void QTable::paintEmptyArea( QPainter *p, int cx, int cy, int cw, int ch )
{
    // Region of the rect we should draw
    QRegion reg( QRect( cx, cy, cw, ch ) );
    // Subtract the table from it
    reg = reg.subtract( QRect( QPoint( 0, 0 ), tableSize() ) );
    // And draw the rectangles (transformed as needed)
    QArray<QRect> r = reg.rects();
    for (unsigned int i=0; i<r.count(); i++)
        p->fillRect( r[i], colorGroup().brush( QColorGroup::Base ) );
}
#endif
#endif


// int AbTable::rowHeight( int ) const
// {
//     return 18;
// }

// int AbTable::rowPos( int row ) const
// {
//     return 18*row;
// }

// int AbTable::rowAt( int pos ) const
// {
//     return QMIN( pos/18, numRows()-1 );
// }

void AbTable::slotDoFind( const QString &findString, bool caseSensitive,
                          bool backwards, int category )
{
    if ( currFindRow < -1 )
	currFindRow = currentRow() - 1;
    clearSelection( TRUE );
    int rows,
        row;
    AbTableItem *ati;
    QRegExp r( findString );
    r.setCaseSensitive( caseSensitive );
    rows = numRows();
    static bool wrapAround = true;

    if ( !backwards ) {
	for ( row = currFindRow + 1; row < rows; row++ ) {
	    ati = static_cast<AbTableItem*>( item(row, 0) );
	    if ( contactCompare( contactList[ati], r, category ) )
		break;

	}
    } else {
	for ( row = currFindRow - 1; row > -1; row-- ) {
	    ati = static_cast<AbTableItem*>( item(row, 0) );
	    if ( contactCompare( contactList[ati], r, category ) )
		break;
	}
    }
    if ( row >= rows || row < 0 ) {
	if ( row < 0 )
	    currFindRow = rows;
	else
	    currFindRow = -1;

	if ( wrapAround )
	    emit signalWrapAround();
	else
	    emit signalNotFound();

	wrapAround = !wrapAround;
    } else {
	currFindRow = row;
	QTableSelection foundSelection;
	foundSelection.init( currFindRow, 0 );
	foundSelection.expandTo( currFindRow, numCols() - 1 );
	addSelection( foundSelection );
	setCurrentCell( currFindRow, numCols() - 1 );
	wrapAround = true;
    }
}

static bool contactCompare( const Contact &cnt, const QRegExp &r, int category )
{
    bool returnMe;
    QArray<int> cats;
    cats = cnt.categories();

    returnMe = false;
    if ( (category == -1 && cats.count() == 0) || category == -2 )
	returnMe = cnt.match( r );
    else {
	int i;
	for ( i = 0; i < int(cats.count()); i++ ) {
	    if ( cats[i] == category ) {
		returnMe = cnt.match( r );
		break;
	    }
	}
    }
    return returnMe;
}

void AbTable::fitColumns()
{
    int contentsWidth = visibleWidth();
    int n = numCols();
    int pw = n == 3 ? columnWidth(2) : 0;
    setColumnWidth( 0, contentsWidth - contentsWidth / 2 );
    setColumnWidth( 1, contentsWidth / 2 - pw );
}

void AbTable::show()
{
    fitColumns();
    QTable::show();
}

void AbTable::setChoiceNames( const QStringList& list)
{
    choicenames = list;
    if ( choicenames.isEmpty() ) {
	// hide pick column
	setNumCols( 2 );
    } else {
	// show pick column
	setNumCols( 3 );
	setColumnWidth( 2, fontMetrics().width(tr( "Pick" ))+8 );
	horizontalHeader()->setLabel( 2, tr( "Pick" ));
    }
    fitColumns();
}

void AbTable::itemClicked(int,int col)
{
    if ( col == 2 ) {
	return;
    } else {
	emit details();
    }
}

QStringList AbTable::choiceNames() const
{
    return choicenames;
}

void AbTable::setChoiceSelection(int /*index*/, const QStringList& /*list*/)
{
    /* ######

	QString selname = choicenames.at(index);
	for (each row) {
	    Contact *c = contactForRow(row);
	    if ( list.contains(c->email) ) {
		list.remove(c->email);
		setText(row, 2, selname);
	    }
	}
	for (remaining list items) {
	    Contact *c = new contact(item);
	    setText(newrow, 2, selname);
	}

    */
}

QStringList AbTable::choiceSelection(int /*index*/) const
{
    QStringList r;
    /* ######

	QString selname = choicenames.at(index);
	for (each row) {
	    Contact *c = contactForRow(row);
	    if ( text(row,2) == selname ) {
		r.append(c->email);
	    }
	}

    */
    return r;
}

void AbTable::setShowCategory( const QString &c )
{
    showCat = c;
    updateVisible();
}

void AbTable::setShowByLetter( char c )
{
    showChar = tolower(c);
    updateVisible();
}

QString AbTable::showCategory() const
{
    return showCat;
}


QStringList AbTable::categories()
{
    mCat.load( categoryFileName() );
    QStringList categoryList = mCat.labels( "Contacts" );
    return categoryList;
}

void AbTable::updateVisible()
{
    int visible,
	totalRows,
	id,
	totalCats,
	it,
	row;
    bool hide;
    AbTableItem *ati;
    Contact *cnt;
    QString fileAsName;
    QString tmpStr;
    visible = 0;

    setPaintingEnabled( FALSE );

    totalRows = numRows();
    id = mCat.id( "Contacts", showCat );
    QArray<int> cats;
    for ( row = 0; row < totalRows; row++ ) {
	ati = static_cast<AbTableItem*>( item(row, 0) );
	cnt = &contactList[ati];
	cats = cnt->categories();
	fileAsName = cnt->fileAs();
	hide = false;
	if ( !showCat.isEmpty() ) {
	    if ( showCat == tr( "Unfiled" ) ) {
		if ( cats.count() > 0 )
		    hide = true;
	    } else {
		// do some comparing
		if ( !hide ) {
		    hide = true;
		    totalCats = int(cats.count());
		    for ( it = 0; it < totalCats; it++ ) {
			if ( cats[it] == id ) {
			    hide = false;
			    break;
			}
		    }
		}
	    }
	}
	if ( showChar != '\0' ) {
	    tmpStr = fileAsName.left(1);
	    tmpStr = tmpStr.lower();
	    if ( tmpStr != QString(QChar(showChar)) && showChar != '#' ) {
		    hide = true;
	    }
	    if ( showChar == '#' ) {
		    if (tmpStr == "a")
			    hide = true;

		    if (tmpStr == "b")
			    hide = true;

		    if (tmpStr == "c")
			    hide = true;

		    if (tmpStr == "d")
			    hide = true;

		    if (tmpStr == "e")
			    hide = true;

		    if (tmpStr == "f")
			    hide = true;

		    if (tmpStr == "g")
			    hide = true;

		    if (tmpStr == "h")
			    hide = true;

		    if (tmpStr == "i")
			    hide = true;

		    if (tmpStr == "j")
			    hide = true;

		    if (tmpStr == "k")
			    hide = true;

		    if (tmpStr == "l")
			    hide = true;

		    if (tmpStr == "m")
			    hide = true;

		    if (tmpStr == "n")
			    hide = true;

		    if (tmpStr == "o")
			    hide = true;

		    if (tmpStr == "p")
			    hide = true;

		    if (tmpStr == "q")
			    hide = true;

		    if (tmpStr == "r")
			    hide = true;

		    if (tmpStr == "s")
			    hide = true;

		    if (tmpStr == "t")
			    hide = true;

		    if (tmpStr == "u")
			    hide = true;

		    if (tmpStr == "v")
			    hide = true;

		    if (tmpStr == "w")
			    hide = true;

		    if (tmpStr == "x")
			    hide = true;

		    if (tmpStr == "y")
			    hide = true;

		    if (tmpStr == "z")
			    hide = true;
	    }

	}
	if ( hide ) {
	    if ( currentRow() == row )
		setCurrentCell( -1, 0 );
	    if ( rowHeight(row) > 0 )
		hideRow( row );
	} else {
	    if ( rowHeight(row) == 0 ) {
		showRow( row );
		adjustRow( row );
	    }
	    visible++;
	}
    }
    if ( !visible )
	setCurrentCell( -1, 0 );

    setPaintingEnabled( TRUE );
}


void AbTable::setPaintingEnabled( bool e )
{
    if ( e != enablePainting ) {
	if ( !enablePainting ) {
	    enablePainting = true;
	    rowHeightChanged( 0 );
	    viewport()->update();
	} else {
	    enablePainting = false;
	}
    }
}

void AbTable::rowHeightChanged( int row )
{
    if ( enablePainting )
	QTable::rowHeightChanged( row );
}
