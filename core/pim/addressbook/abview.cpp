#include "abview.h"

#include <qlayout.h>

#include <qpe/global.h>

#include <opie/ocontactaccessbackend_vcard.h>


// Is defined in LibQPE
extern QString categoryFileName();

QString addressbookPersonalVCardName()
{
	QString filename = Global::applicationFileName("addressbook",
						       "businesscard.vcf");
	return filename;
}


AbView::AbView ( QWidget* parent, const QValueList<int>& ordered ):
	QWidget(parent),
	mCat(0),
	m_inSearch( false ),
	m_inPersonal( false ),
	m_curr_category( 0 ),
	m_curr_View( TableView ),
	m_prev_View( TableView ),
	m_curr_Contact ( 0 ),
	m_contactdb ( 0l ),
	m_storedDB ( 0l ),
	m_viewStack( 0l ),
	m_abTable( 0l ),
	m_orderedFields( ordered )
{
	// Load default database and handle syncing myself.. !
	m_contactdb = new OContactAccess ( "addressbook", 0l, 0l, false ),
	mCat.load( categoryFileName() );

	// Create Layout and put WidgetStack into it.
  	QVBoxLayout *vb = new QVBoxLayout( this );
 	m_viewStack = new QWidgetStack( this );
	vb->addWidget( m_viewStack );
	
	// Creat TableView
 	QVBox* tableBox = new QVBox( m_viewStack );
 	m_abTable = new AbTable( m_orderedFields, tableBox, "table" );
	m_abTable->setCurrentCell( 0, 0 );
	m_abTable->setFocus();

	// Add TableView to WidgetStack and raise it
	m_viewStack -> addWidget( tableBox , TableView );

	// Create CardView and add it to WidgetStack
 	QVBox* cardBox = new QVBox( m_viewStack );
	m_ablabel = new AbLabel( cardBox, "CardView");
	m_viewStack -> addWidget( cardBox , CardView );

	// Connect views to me
	connect ( m_abTable, SIGNAL( signalSwitch( void ) ), 
		  this, SLOT( slotSwitch( void ) ) ); 
	connect ( m_ablabel, SIGNAL( signalOkPressed( void ) ),
		  this, SLOT( slotSwitch( void ) ) );

	load();
}

AbView::~AbView()
{
	m_contactdb -> save();
	delete m_contactdb;

	if ( m_storedDB ){
		m_storedDB -> save();
		delete m_storedDB;
	}
}


void AbView::setView( Views view )
{
	qWarning("AbView::setView( Views view )");
 	m_curr_View = view;
 	load();
}

void AbView::addEntry( const OContact &newContact )
{
	qWarning("abview:AddContact");
	m_contactdb->add ( newContact );
	load();
	
}
void AbView::removeEntry( const int UID )
{
	qWarning("abview:RemoveContact");
	m_contactdb->remove( UID );
	load();
}

void AbView::replaceEntry( const OContact &contact )
{
	qWarning("abview:ReplaceContact");
	m_contactdb->replace( contact );
	load();

}

OContact AbView::currentEntry()
{
	switch ( (int) m_curr_View ) {
	case TableView:
		return ( m_abTable -> currentEntry() );
		break;
	case CardView:
		return ( m_ablabel -> currentEntry() );
		break;
	}
	return OContact();
}

bool AbView::save()
{
	qWarning("abView:Save data");

	return m_contactdb->save();
}

void AbView::load()
{
	qWarning("abView:Load data");
	
	m_list = m_contactdb->allRecords();
	clearForCategory();

	// Feed all views with new lists
	updateListinViews();
	
	qWarning ("Number of contacts: %d", m_list.count());

	updateView(); 
	
}

void AbView::reload()
{
	qWarning( "void AbView::reload()" );

	m_contactdb->reload();
	load();
}

void AbView::clear()
{
	// :SX
}

void AbView::setShowByCategory( Views view, const QString& cat )
{
	qWarning("AbView::setShowCategory( Views view, const QString& cat )");

// 	if ( view == PersonalView ){
// 		if ( ! m_inPersonal )
// 			showPersonal( true );
		
// 	}else{
// 		if  ( m_inPersonal )
// 			showPersonal( false );

// 			m_curr_View = view;
// 	}

	m_curr_View = view;

	emit signalClearLetterPicker();

	if ( !cat.isNull() ) 
		m_curr_category = mCat.id("Contacts", cat );
	else
		m_curr_category = -1; // Set to all

	qWarning ("Categories: Selected %s.. Number: %d", cat.latin1(), m_curr_category);

	load();

}
void AbView::setShowByLetter( char c )
{
	qWarning("void AbView::setShowByLetter( %c )", c );
	OContact query;
	if ( c == 0 ){
		load();
		return;
	}else{
		query.setLastName( QString("%1*").arg(c) );
		m_list = m_contactdb->queryByExample( query, OContactAccess::WildCards );
		clearForCategory();
		m_curr_Contact = 0;
	}
	updateView();
}

void AbView::setListOrder( const QValueList<int>& ordered )
{
	m_orderedFields = ordered;
	updateView();
}


QString AbView::showCategory() const 
{
	return mCat.label( "Contacts", m_curr_category );
}

void AbView::showPersonal( bool personal )
{
	qWarning ("void AbView::showPersonal( %d )", personal);

	if ( personal ){

		if ( m_inPersonal )
			return;

		// Now switch to vCard Backend and load data.
		// The current default backend will be stored
		// to avoid unneeded load/stores.
		m_storedDB = m_contactdb;

		OContactAccessBackend* vcard_backend = new OContactAccessBackend_VCard( QString::null, 
									  addressbookPersonalVCardName() );
		m_contactdb = new OContactAccess ( "addressbook", QString::null , vcard_backend, true );

		m_inPersonal = true;
		m_curr_View = CardView;
		
	}else{

		if ( !m_inPersonal )
			return;

		// Remove vCard Backend and restore default
		m_contactdb->save();
		delete m_contactdb;

		m_contactdb = m_storedDB;
		m_storedDB = 0l;

		m_curr_View = TableView;
		m_inPersonal = false;
		
	}
	load();
}

QStringList AbView::categories() 
{
	mCat.load( categoryFileName() );
	QStringList categoryList = mCat.labels( "Contacts" );
	return categoryList;
}
 
// BEGIN: Slots 
void AbView::slotDoFind( const QString &str, bool caseSensitive, bool useRegExp, 
			 bool , QString cat = QString::null )
{
	qWarning( "void AbView::slotDoFind" );
	// Use the current Category if nothing else selected

	int category = 0;

	if ( cat.isEmpty() )
		category = m_curr_category;
	else{
		category = mCat.id("Contacts", cat );
	}

	qWarning ("Find in Category %d", category);

	QRegExp r( str );
	r.setCaseSensitive( caseSensitive );
	r.setWildcard( !useRegExp );
	
	// Get all matching entries out of the database
	m_list = m_contactdb->matchRegexp( r );

	qWarning( "found: %d", m_list.count() );
	if ( m_list.count() == 0 ){
		emit signalNotFound();
		return;
	}

	// Now remove all contacts with wrong category (if any selected)
	// This algorithm is a litte bit ineffective, but
	// we will not have a lot of matching entries..
	clearForCategory();
	
	// Now show all found entries
	updateView();
}

void AbView::offSearch()
{
	m_inSearch = false;

	load();
}

void AbView::slotSwitch(){
	qWarning("AbView::slotSwitch()");

	m_prev_View = m_curr_View;
	switch ( (int) m_curr_View ){
	case TableView:
		qWarning("Switching to CardView");
		m_curr_View = CardView;
		break;
	case CardView:
		qWarning("Switching to TableView");
		m_curr_View = TableView;
		break;
	}
	updateView();
	
}

// END: Slots

void AbView::clearForCategory()
{
	OContactAccess::List::Iterator it;
	// Now remove all contacts with wrong category if any category selected

	OContactAccess::List allList = m_list;
	if ( m_curr_category != -1 ){
		for ( it = allList.begin(); it != allList.end(); ++it ){
			if ( !contactCompare( *it, m_curr_category ) ){ 
				qWarning("Removing %d", (*it).uid());
				m_list.remove( (*it).uid() );
			}
		}
	}

}

bool AbView::contactCompare( const OContact &cnt, int category )
{
	qWarning ("bool AbView::contactCompare( const OContact &cnt, %d )", category);

	bool returnMe;
	QArray<int> cats;
	cats = cnt.categories();

	qWarning ("Number of categories: %d", cats.count() );

	returnMe = false;
	if ( cats.count() == 0  )
		returnMe = true;
	else {
		int i;
		for ( i = 0; i < int(cats.count()); i++ ) {
			qWarning("Comparing %d with %d",cats[i],category );
			if ( cats[i] == category ) {
				returnMe = true;
				break;
			}
		}
	}
	qWarning ("Return: %d", returnMe);
	return returnMe;
}

// In Some rare cases we have to update all lists..
void AbView::updateListinViews()
{
		m_abTable -> setContacts( m_list );
		m_ablabel -> setContacts( m_list );
}

void  AbView::updateView()
{
	qWarning("AbView::updateView()");

	if ( m_viewStack -> visibleWidget() ){
		m_viewStack -> visibleWidget() -> clearFocus();
	}

	// If we switching the view, we have to store some information
	if ( m_list.count() ){
		switch ( (int) m_prev_View ) {
		case TableView:
			m_curr_Contact = m_abTable -> currentEntry_UID();
			break;
		case CardView:
			m_curr_Contact = m_ablabel -> currentEntry_UID();
			break;
		}
	}else
		m_curr_Contact = 0;
	
	m_prev_View = m_curr_View;

	// Switch to new View
	switch ( (int) m_curr_View ) {
	case TableView:
		m_abTable -> setChoiceSelection( m_orderedFields );
		m_abTable -> setContacts( m_list );
		if ( m_curr_Contact != 0 )
			m_abTable -> selectContact ( m_curr_Contact );
		m_abTable -> setFocus();
		emit signalViewSwitched ( (int) m_curr_View );
		break;
	case CardView:
		m_ablabel -> setContacts( m_list );
		if ( m_curr_Contact != 0 )
			m_ablabel -> selectContact( m_curr_Contact );
		m_ablabel -> setFocus();
		emit signalViewSwitched ( (int) m_curr_View );
		break;
	}
	
	// Raise the current View
	m_viewStack -> raiseWidget( m_curr_View );
}


