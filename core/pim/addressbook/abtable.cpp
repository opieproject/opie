/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
** Copyright (c) 2002 Stefan Eilers (eilers.stefan@epost.de)
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


#include <qpe/categoryselect.h>
#include <qpe/config.h>
#include <qpe/stringutil.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/timestring.h>

#include <opie/orecordlist.h>

#include <qasciidict.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>
#include <qmessagebox.h>

#include "abtable.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <ctype.h> //toupper() for key hack

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

AbTable::AbTable( const QValueList<int> order, QWidget *parent, const char *name )
	: QTable( parent, name ),
	  lastSortCol( -1 ),
	  asc( TRUE ),
	  intFields( order ),
	  enablePainting( true ),
	  columnVisible( true )
{
	qWarning("C'tor start");

	setSelectionMode( NoSelection );
	init();
	setSorting( TRUE );
	connect( this, SIGNAL(clicked(int,int,int,const QPoint &)),
		 this, SLOT(itemClicked(int,int)) );

	contactList.clear();
	qWarning("C'tor end");
}

AbTable::~AbTable()
{
}

void AbTable::init()
{
	// :SX showChar = '\0';
	setNumRows( 0 );
	setNumCols( 2 );
	
	horizontalHeader()->setLabel( 0, tr( "Full Name" ));
	horizontalHeader()->setLabel( 1, tr( "Contact" ));
	setLeftMargin( 0 );
	verticalHeader()->hide();
	columnVisible = true;
}

void AbTable::setContacts( const OContactAccess::List& viewList )
{
	qWarning("AbTable::setContacts()");

	clear();
	m_viewList = viewList;

	setSorting( false );
	setUpdatesEnabled( FALSE );
	
	OContactAccess::List::Iterator it;
	setNumRows( m_viewList.count() );
	int row = 0;
	for ( it = m_viewList.begin(); it != m_viewList.end(); ++it )
		insertIntoTable( *it, row++ );
	
	setUpdatesEnabled( TRUE );

	setSorting( true );

	resort();

	updateVisible();

}

bool AbTable::selectContact( int UID )
{
	qWarning( "AbTable::selectContact( %d )", UID );
	int rows = numRows();
	AbTableItem *abi;
	OContact* foundContact = 0l;
	bool found = false;

	for ( int r = 0; r < rows; ++r ) {
		abi = static_cast<AbTableItem*>( item(r, 0) );
		foundContact = &contactList[abi];
		if ( foundContact -> uid() == UID ){
			ensureCellVisible( r, 0 );
			setCurrentCell( r, 0 );
			found = true;
			break;
		}
	}

	if ( !found ){
		ensureCellVisible( 0,0 );
		setCurrentCell( 0, 0 );
	}

	return true;
}

void AbTable::insertIntoTable( const OContact& cnt, int row )
{
	// qWarning( "void AbTable::insertIntoTable( const OContact& cnt, %d )", row );
	QString strName,
		strContact;
	
	strName = findContactName( cnt );
	strContact = findContactContact( cnt, row );
	
	AbTableItem *ati;
	ati = new AbTableItem( this, QTableItem::Never, strName, strContact);
	contactList.insert( ati, cnt );
	setItem( row, 0, ati );
	ati = new AbTableItem( this, QTableItem::Never, strContact, strName);
	setItem( row, 1, ati );
	
	//### cannot do this; table only has two columns at this point
	//    setItem( row, 2, new AbPickItem( this ) );
	
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
	//QMessageBox::information( this, "resort", "columnClicked" );
	resort();
}

void AbTable::resort()
{
	qWarning( "void AbTable::resort()" );
	if ( sorting() ) {
		if ( lastSortCol == -1 )
			lastSortCol = 0;
		sortColumn( lastSortCol, asc, TRUE );
		//QMessageBox::information( this, "resort", "resort" );
		updateVisible();
	}
}

OContact AbTable::currentEntry()
{
	qWarning( "OContact AbTable::currentEntry()" );
	OContact cnt;
	AbTableItem *abItem;
	abItem = static_cast<AbTableItem*>(item( currentRow(), 0 ));
	if ( abItem ) {
		cnt = contactList[abItem];
		//cnt = contactList[currentRow()];
	}
	return cnt;
}

int AbTable::currentEntry_UID()
{
	return ( currentEntry().uid() );
}

void AbTable::clear()
{
	qWarning( "void AbTable::clear()" );
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

// Refresh updates column 2 if the contactsettings changed
void AbTable::refresh()
{
	qWarning( "void AbTable::refresh()" );
	int rows = numRows();
	QString value;
	AbTableItem *abi;

	for ( int r = 0; r < rows; ++r ) {
		abi = static_cast<AbTableItem*>( item(r, 0) );
		value = findContactContact( contactList[abi], r );
		static_cast<AbTableItem*>( item(r, 1) )->setItem( value, abi->text() );
	}
	resort();
}

void AbTable::keyPressEvent( QKeyEvent *e )
{
	char key = toupper( e->ascii() );
	
	if ( key >= 'A' && key <= 'Z' )
		moveTo( key );
	
	qWarning("Received key ..");
	switch( e->key() ) {
	case Qt::Key_Space:
	case Qt::Key_Return:
	case Qt::Key_Enter:
		emit signalSwitch();
		break;
// 	case Qt::Key_Up:
// 		qWarning("a");
// 		emit signalKeyUp();
// 		break;
// 	case Qt::Key_Down:
// 		qWarning("b");
// 		emit signalKeyDown();
// 		break;
	default:
		QTable::keyPressEvent( e );
	}
		
}

void AbTable::moveTo( char c )
{
	qWarning( "void AbTable::moveTo( char c )" );
	
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


QString AbTable::findContactName( const OContact &entry )
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



void AbTable::resizeRows() {
	/*
	  if (numRows()) {
	  for (int i = 0; i < numRows(); i++) {
	  setRowHeight( i, size );
	  }
	  }
	  updateVisible();
	*/
}


void AbTable::realignTable()
{
	qWarning( "void AbTable::realignTable()" );

	setPaintingEnabled( FALSE );

	resizeRows();
	fitColumns();

	setPaintingEnabled( TRUE );

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



void AbTable::fitColumns()
{
	qWarning( "void AbTable::fitColumns()" );
	int contentsWidth = visibleWidth() / 2; // :SX Why too low
	// Fix to better value
	// contentsWidth = 130; 
	
	if ( columnVisible == false ){
		showColumn(0);
		columnVisible = true;
	}
	
	qWarning("Width: %d", contentsWidth);

	setColumnWidth( 0, contentsWidth );
	adjustColumn(1);
	if ( columnWidth(1) < contentsWidth )
		setColumnWidth( 1, contentsWidth );
}

void AbTable::show()
{
	qWarning( "void AbTable::show()" );
	realignTable();
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
	qWarning( "AbTable::itemClicked(int, col:%d)", col);
	if ( col == 2 ) {
		return;
	} else {
		qWarning ("Emitting signalSwitch()");
		emit signalSwitch();
	}
}

QStringList AbTable::choiceNames() const
{
	return choicenames;
}

void AbTable::setChoiceSelection( const QValueList<int>& list )
{
	intFields = list;
}

QStringList AbTable::choiceSelection(int /*index*/) const
{
	QStringList r;
	/* ######
	   
	QString selname = choicenames.at(index);
	for (each row) {
	OContact *c = contactForRow(row);
	if ( text(row,2) == selname ) {
	r.append(c->email);
	}
	}
	
	*/
	return r;
}




void AbTable::updateVisible()
{
	int visible,
		totalRows,
		row,
		selectedRow = 0;
		
	visible = 0;
	
	realignTable();

	setPaintingEnabled( FALSE );

	totalRows = numRows();
	for ( row = 0; row < totalRows; row++ ) {
		if ( rowHeight(row) == 0 ) {
			showRow( row );
			adjustRow( row );
			if ( isSelected( row,0 ) || isSelected( row,1 ) )
				selectedRow = row;
		}
		visible++;
	}

	if ( selectedRow )
		setCurrentCell( selectedRow, 0 );

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
QString AbTable::findContactContact( const OContact &entry, int /* row */ )
{
	QString value;
	value = "";
	for ( QValueList<int>::ConstIterator it = intFields.begin();
	      it != intFields.end(); ++it ) {
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
			if ( ! entry.birthday().isNull() )
				value = TimeString::numberDateString( entry.birthday() );
			break;
		case Qtopia::Anniversary:
			if ( ! entry.anniversary().isNull() )
				value = TimeString::numberDateString( entry.anniversary() );
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
