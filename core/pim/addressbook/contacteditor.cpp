/*
 * Copyright (c) 2002 Michael R. Crawford <mike@tuxnami.org>
 *
 * This file is an add-on for the OPIE Palmtop Environment
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the pacakaging
 * of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * This is a rewrite of the abeditor.h file, modified to provide a more
 * intuitive interface to TrollTech's original Address Book editor.  This
 * is made to operate exactly in interface with the exception of name.
 *
 */

#include "contacteditor.h"
#include "addresspicker.h"

#include <qpe/categoryselect.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpedialog.h>
#include <qpe/timeconversion.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qscrollview.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qmainwindow.h>
#include <qvaluelist.h>
#include <qpopupmenu.h> 

static inline bool containsAlphaNum( const QString &str );
static inline bool constainsWhiteSpace( const QString &str );

// helper functions, convert our comma delimited list to proper
// file format...
void parseEmailFrom( const QString &txt, QString &strDefaultEmail,
		     QString &strAll );

// helper convert from file format to comma delimited...
void parseEmailTo( const QString &strDefaultEmail,
		   const QString &strOtherEmail, QString &strBack );

ContactEditor::ContactEditor(	const OContact &entry,
				const QValueList<int> *newOrderedValues,
				QStringList *slNewOrdered,
				QWidget *parent,
				const char *name,
				WFlags fl )
	: QDialog( parent, name, TRUE, fl ),
	  orderedValues( newOrderedValues ),
	  slOrdered( *slNewOrdered ),
	  m_personalView ( false )
{

	init();
	initMap();
	setEntry( entry );
	qDebug("finish");
}

ContactEditor::~ContactEditor() {
}

void ContactEditor::init() {

	useFullName = TRUE;

	int i = 0;
/** SHut up and stop leaking
	slHomeAddress = new QStringList;
	slBusinessAddress = new QStringList;
	slChooserNames = new QStringList;
	slChooserValues = new QStringList;

	slDynamicEntries = new QStringList;
*/
	//*slDynamicEntries = *slOrdered;

	QStringList trlChooserNames;

	for (i = 0; i <= 6; i++) {
		slHomeAddress.append( "" );
		slBusinessAddress.append( "" );
	}

	{
		hasGender = FALSE;
		hasTitle = FALSE;
		hasCompany = FALSE;
		hasNotes = FALSE;
		hasStreet = FALSE;
		hasStreet2 = FALSE;
		hasPOBox = FALSE;
		hasCity = FALSE;
		hasState = FALSE;
		hasZip = FALSE;
		hasCountry = FALSE;

		QStringList::ConstIterator it = slOrdered.begin();

		for ( i = 0; it != slOrdered.end(); i++, ++it ) {

			if ( (*it) == "Business Fax" ) {
				trlChooserNames.append( tr( "Business Fax" ) );
				slChooserNames.append( *it );
				slChooserValues.append("" );
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home Fax" ) {
				trlChooserNames.append( tr( "Home Fax" ) );
				slChooserNames.append( *it );
				slChooserValues.append("" );
				//slDynamicEntries->remove( it );
				continue;
			}


			if ( (*it) == "Business Phone" ) {
				trlChooserNames.append( tr( "Business Phone" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home Phone" ) {
				trlChooserNames.append( tr( "Home Phone" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}

/*
			if ( (*it).right( 2 ) == tr( "IM" ) ) {
				slChooserNames.append( *it );
				slChooserValues.append( ""  );
				//slDynamicEntries->remove( it );
				continue;
			} */

			if ( (*it) == "Business Mobile" ) {
				trlChooserNames.append( tr( "Business Mobile" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home Mobile" ) {
				trlChooserNames.append( tr( "Home Mobile" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}


			if ( (*it) == "Business WebPage" ) {
				trlChooserNames.append( tr( "Business WebPage" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home Web Page" ) {
				trlChooserNames.append( tr( "Home Web Page" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Business Pager" ) {
				trlChooserNames.append( tr( "Business Pager" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Default Email" ) {
				trlChooserNames.append( tr( "Default Email" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Emails" ) {
				trlChooserNames.append( tr( "Emails" ) );
				slChooserNames.append( *it );
				slChooserValues.append( "" );
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Name Title" || 
			     *it == "First Name" || 
			     *it == "Middle Name" || 
			     *it == "Last Name" || 
			     *it == "File As" || 
			     *it == "Default Email" || 
			     *it == "Emails" || 
			     *it == "Groups"  || 
			     *it == "Anniversary" || 
			     *it == "Birthday" )
				continue;

			if ( *it == "Name Title" ) {
				//slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "First Name" ) {
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Middle Name" ) {
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Last Name" ) {
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Suffix" ) {
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "File As" ) {
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Gender" ) {
				hasGender = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Job Title" ) {
				hasTitle = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( ( *it == "Company") || (*it == "Organization" ) ) {
				hasCompany = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Notes" ) {
				hasNotes = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( *it == "Groups" ) {
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Business Street" ) {
				hasStreet = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home Street" ) {
				hasStreet = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}
/*
			if ( (*it).right( 8 ) == tr( "Street 2" ) ) {
				hasStreet2 = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it).right( 8 ) == tr( "P.O. Box" ) ) {
				hasPOBox = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			} */

			if ( (*it) == "Business City" ) {
				hasCity = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Business State" ) {
				hasState = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Business Zip" ) {
				hasZip = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Business Country" ) {
				hasCountry = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home City" ) {
				hasCity = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home State" ) {
				hasState = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home Zip" ) {
				hasZip = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			if ( (*it) == "Home Country" ) {
				hasCountry = TRUE;
			//	slDynamicEntries->remove( it );
				continue;
			}

			slDynamicEntries.append( *it );
		}
	}

	QVBoxLayout *vb = new QVBoxLayout( this );

	tabMain = new QTabWidget( this );
	vb->addWidget( tabMain );

	QWidget *tabViewport = new QWidget ( tabMain );

	vb = new QVBoxLayout( tabViewport );

	svGeneral = new QScrollView( tabViewport );
	vb->addWidget( svGeneral, 0, 0 );
	svGeneral->setResizePolicy( QScrollView::AutoOneFit );
	svGeneral->setFrameStyle( QFrame::NoFrame );

	QWidget *container = new QWidget( svGeneral->viewport() );
	svGeneral->addChild( container );

	QGridLayout *gl = new QGridLayout( container, 1, 1, 2, 4 );
	gl->setResizeMode( QLayout::FreeResize );

	btnFullName = new QPushButton( tr( "Full Name..." ), container );
	gl->addWidget( btnFullName, 0, 0 );
	txtFullName = new QLineEdit( container );
	gl->addWidget( txtFullName, 0, 1 );

	QLabel *l = new QLabel( tr( "Job Title" ), container );
	gl->addWidget( l, 1, 0 );
	txtJobTitle = new QLineEdit( container );
	gl->addWidget( txtJobTitle, 1, 1 );

	l = new QLabel( tr( "Organization" ), container );
	gl->addWidget( l, 2, 0 );
	txtOrganization = new QLineEdit( container );
	gl->addWidget( txtOrganization, 2, 1 );

	cmbChooserField1 = new QComboBox( FALSE, container );
	cmbChooserField1->setMaximumWidth( 90 );
	gl->addWidget( cmbChooserField1, 3, 0 );
	txtChooserField1 = new QLineEdit( container );
	gl->addWidget( txtChooserField1, 3, 1 );

	cmbChooserField2 = new QComboBox( FALSE, container );
	cmbChooserField2->setMaximumWidth( 90 );
	gl->addWidget( cmbChooserField2, 4, 0 );
	txtChooserField2 = new QLineEdit( container );
	gl->addWidget( txtChooserField2, 4, 1 );

	cmbChooserField3 = new QComboBox( FALSE, container );
	cmbChooserField3->setMaximumWidth( 90 );
	gl->addWidget( cmbChooserField3, 5, 0 );
	txtChooserField3 = new QLineEdit( container );
	gl->addWidget( txtChooserField3, 5, 1 );

	l = new QLabel( tr( "File As" ), container );
	gl->addWidget( l, 6, 0 );
	cmbFileAs = new QComboBox( TRUE, container );
	gl->addWidget( cmbFileAs, 6, 1 );

	labCat = new QLabel( tr( "Category" ), container );
	gl->addWidget( labCat, 7, 0 );
	cmbCat = new CategorySelect( container );
	gl->addWidget( cmbCat, 7, 1 );

	// We don't need categories for the personal view
	if ( m_personalView ){
		qWarning("Disable Category..");
		labCat->hide();
		cmbCat->hide();
	} else {
		labCat->show();
		cmbCat->show();
	}

	btnNote = new QPushButton( tr( "Notes..." ), container );
	gl->addWidget( btnNote, 8, 1 );

	tabMain->insertTab( tabViewport, tr( "General" ) );

	tabViewport = new QWidget ( tabMain );

	vb = new QVBoxLayout( tabViewport );

	svAddress = new QScrollView( tabViewport );
	vb->addWidget( svAddress, 0, 0 );
	svAddress->setResizePolicy( QScrollView::AutoOneFit );
	svAddress->setFrameStyle( QFrame::NoFrame );

	container = new QWidget( svAddress->viewport() );
	svAddress->addChild( container );

	gl = new QGridLayout( container, 8, 3, 2, 4 ); // row 7 QSpacerItem

	cmbAddress = new QComboBox( FALSE, container );
	cmbAddress->insertItem( tr( "Business" ) );
	cmbAddress->insertItem( tr( "Home" ) );
	gl->addMultiCellWidget( cmbAddress, 0, 0, 0, 1 );

	l = new QLabel( tr( "Address" ), container );
	gl->addWidget( l, 1, 0 );
	txtAddress = new QLineEdit( container );
	gl->addMultiCellWidget( txtAddress, 1, 1, 1, 2 );
/*
	l = new QLabel( tr( "Address 2" ), container );
	gl->addWidget( l, 2, 0 );
	txtAddress2 = new QLineEdit( container );
	gl->addMultiCellWidget( txtAddress2, 2, 2, 1, 2 );

	l = new QLabel( tr( "P.O. Box" ), container );
	gl->addWidget( l, 3, 0 );
	txtPOBox = new QLineEdit( container );
	gl->addMultiCellWidget( txtPOBox, 3, 3, 1, 2 );
*/
	l = new QLabel( tr( "City" ), container );
	gl->addWidget( l, 2, 0 );
	txtCity = new QLineEdit( container );
	gl->addMultiCellWidget( txtCity, 2, 2, 1, 2 );

	l = new QLabel( tr( "State" ), container );
	gl->addWidget( l, 3, 0 );
	txtState = new QLineEdit( container );
	gl->addMultiCellWidget( txtState, 3, 3, 1, 2 );

	l = new QLabel( tr( "Zip Code" ), container );
	gl->addWidget( l, 4, 0 );
	txtZip = new QLineEdit( container );
	gl->addMultiCellWidget( txtZip, 4, 4, 1, 2 );

	l = new QLabel( tr( "Country" ), container );
	gl->addWidget( l, 5, 0 );
	cmbCountry = new QComboBox( TRUE, container );
	cmbCountry->insertItem( tr( "" ) );
	cmbCountry->insertItem( tr ( "United States" ) );
	cmbCountry->insertItem( tr ( "United Kingdom" ) );
	cmbCountry->insertItem( tr ( "Afganistan" ) );
	cmbCountry->insertItem( tr ( "Albania" ) );
	cmbCountry->insertItem( tr ( "Algeria" ) );
	cmbCountry->insertItem( tr ( "American Samoa" ) );
	cmbCountry->insertItem( tr ( "Andorra" ) );
	cmbCountry->insertItem( tr ( "Angola" ) );
	cmbCountry->insertItem( tr ( "Anguilla" ) );
	cmbCountry->insertItem( tr ( "Antartica" ) );
	cmbCountry->insertItem( tr ( "Argentina" ) );
	cmbCountry->insertItem( tr ( "Armania" ) );
	cmbCountry->insertItem( tr ( "Aruba" ) );
	cmbCountry->insertItem( tr ( "Australia" ) );
	cmbCountry->insertItem( tr ( "Austria" ) );
	cmbCountry->insertItem( tr ( "Azerbaijan" ) );
	cmbCountry->insertItem( tr ( "Bahamas" ) );
	cmbCountry->insertItem( tr ( "Bahrain" ) );
	cmbCountry->insertItem( tr ( "Bangladesh" ) );
	cmbCountry->insertItem( tr ( "Barbados" ) );
	cmbCountry->insertItem( tr ( "Belarus" ) );
	cmbCountry->insertItem( tr ( "Belgium" ) );
	cmbCountry->insertItem( tr ( "Belize" ) );
	cmbCountry->insertItem( tr ( "Benin" ) );
	cmbCountry->insertItem( tr ( "Bermuda" ) );
	cmbCountry->insertItem( tr ( "Bhutan" ) );
	cmbCountry->insertItem( tr ( "Boliva" ) );
	cmbCountry->insertItem( tr ( "Botswana" ) );
	cmbCountry->insertItem( tr ( "Bouvet Island" ) );
	cmbCountry->insertItem( tr ( "Brazil" ) );
	cmbCountry->insertItem( tr ( "Brunei Darussalam" ) );
	cmbCountry->insertItem( tr ( "Bulgaria" ) );
	cmbCountry->insertItem( tr ( "Burkina Faso" ) );
	cmbCountry->insertItem( tr ( "Burundi" ) );
	cmbCountry->insertItem( tr ( "Cambodia" ) );
	cmbCountry->insertItem( tr ( "Camaroon" ) );
	cmbCountry->insertItem( tr ( "Canada" ) );
	cmbCountry->insertItem( tr ( "Cape Verde" ) );
	cmbCountry->insertItem( tr ( "Cayman Islands" ) );
	cmbCountry->insertItem( tr ( "Chad" ) );
	cmbCountry->insertItem( tr ( "Chile" ) );
	cmbCountry->insertItem( tr ( "China" ) );
	cmbCountry->insertItem( tr ( "Christmas Island" ) );
	cmbCountry->insertItem( tr ( "Colombia" ) );
	cmbCountry->insertItem( tr ( "Comoros" ) );
	cmbCountry->insertItem( tr ( "Congo" ) );
	cmbCountry->insertItem( tr ( "Cook Island" ) );
	cmbCountry->insertItem( tr ( "Costa Rica" ) );
	cmbCountry->insertItem( tr ( "Cote d'Ivoire" ) );
	cmbCountry->insertItem( tr ( "Croatia" ) );
	cmbCountry->insertItem( tr ( "Cuba" ) );
	cmbCountry->insertItem( tr ( "Cyprus" ) );
	cmbCountry->insertItem( tr ( "Czech Republic" ) );
	cmbCountry->insertItem( tr ( "Denmark" ) );
	cmbCountry->insertItem( tr ( "Djibouti" ) );
	cmbCountry->insertItem( tr ( "Dominica" ) );
	cmbCountry->insertItem( tr ( "Dominican Republic" ) );
	cmbCountry->insertItem( tr ( "East Timor" ) );
	cmbCountry->insertItem( tr ( "Ecuador" ) );
	cmbCountry->insertItem( tr ( "Egypt" ) );
	cmbCountry->insertItem( tr ( "El Salvador" ) );
	cmbCountry->insertItem( tr ( "Equatorial Guinea" ) );
	cmbCountry->insertItem( tr ( "Eritrea" ) );
	cmbCountry->insertItem( tr ( "Estonia" ) );
	cmbCountry->insertItem( tr ( "Ethiopia" ) );
	cmbCountry->insertItem( tr ( "Falkland Islands" ) );
	cmbCountry->insertItem( tr ( "Faroe Islands" ) );
	cmbCountry->insertItem( tr ( "Fiji" ) );
	cmbCountry->insertItem( tr ( "Finland" ) );
	cmbCountry->insertItem( tr ( "France" ) );
	cmbCountry->insertItem( tr ( "French Guiana" ) );
	cmbCountry->insertItem( tr ( "French Polynesia" ) );
	cmbCountry->insertItem( tr ( "Gabon" ) );
	cmbCountry->insertItem( tr ( "Gambia" ) );
	cmbCountry->insertItem( tr ( "Georgia" ) );
	cmbCountry->insertItem( tr ( "Germany" ) );
	cmbCountry->insertItem( tr ( "Gahna" ) );
	cmbCountry->insertItem( tr ( "Gibraltar" ) );
	cmbCountry->insertItem( tr ( "Greece" ) );
	cmbCountry->insertItem( tr ( "Greenland" ) );
	cmbCountry->insertItem( tr ( "Grenada" ) );
	cmbCountry->insertItem( tr ( "Guadelupe" ) );
	cmbCountry->insertItem( tr ( "Guam" ) );
	cmbCountry->insertItem( tr ( "Guatemala" ) );
	cmbCountry->insertItem( tr ( "Guinea" ) );
	cmbCountry->insertItem( tr ( "Guinea-bissau" ) );
	cmbCountry->insertItem( tr ( "Guyana" ) );
	cmbCountry->insertItem( tr ( "Haiti" ) );
	cmbCountry->insertItem( tr ( "Holy See" ) );
	cmbCountry->insertItem( tr ( "Honduras" ) );
	cmbCountry->insertItem( tr ( "Hong Kong" ) );
	cmbCountry->insertItem( tr ( "Hungary" ) );
	cmbCountry->insertItem( tr ( "Iceland" ) );
	cmbCountry->insertItem( tr ( "India" ) );
	cmbCountry->insertItem( tr ( "Indonesia" ) );
	cmbCountry->insertItem( tr ( "Ireland" ) );
	cmbCountry->insertItem( tr ( "Israel" ) );
	cmbCountry->insertItem( tr ( "Italy" ) );
	cmbCountry->insertItem( tr ( "Jamacia" ) );
	cmbCountry->insertItem( tr ( "Japan" ) );
	cmbCountry->insertItem( tr ( "Jordan" ) );
	cmbCountry->insertItem( tr ( "Kazakhstan" ) );
	cmbCountry->insertItem( tr ( "Kenya" ) );
	cmbCountry->insertItem( tr ( "Kribati" ) );
	cmbCountry->insertItem( tr ( "Korea" ) );
	cmbCountry->insertItem( tr ( "Kuwait" ) );
	cmbCountry->insertItem( tr ( "Kyrgystan" ) );
	cmbCountry->insertItem( tr ( "Laos" ) );
	cmbCountry->insertItem( tr ( "Latvia" ) );
	cmbCountry->insertItem( tr ( "Lebanon" ) );
	cmbCountry->insertItem( tr ( "Lesotho" ) );
	cmbCountry->insertItem( tr ( "Liberia" ) );
	cmbCountry->insertItem( tr ( "Liechtenstein" ) );
	cmbCountry->insertItem( tr ( "Lithuania" ) );
	cmbCountry->insertItem( tr ( "Luxembourg" ) );
	cmbCountry->insertItem( tr ( "Macau" ) );
	cmbCountry->insertItem( tr ( "Macedonia" ) );
	cmbCountry->insertItem( tr ( "Madagascar" ) );
	cmbCountry->insertItem( tr ( "Malawi" ) );
	cmbCountry->insertItem( tr ( "Malaysia" ) );
	cmbCountry->insertItem( tr ( "Maldives" ) );
	cmbCountry->insertItem( tr ( "Mali" ) );
	cmbCountry->insertItem( tr ( "Malta" ) );
	cmbCountry->insertItem( tr ( "Martinique" ) );
	cmbCountry->insertItem( tr ( "Mauritania" ) );
	cmbCountry->insertItem( tr ( "Mauritius" ) );
	cmbCountry->insertItem( tr ( "Mayotte" ) );
	cmbCountry->insertItem( tr ( "Mexico" ) );
	cmbCountry->insertItem( tr ( "Micronesia" ) );
	cmbCountry->insertItem( tr ( "Moldova" ) );
	cmbCountry->insertItem( tr ( "Monaco" ) );
	cmbCountry->insertItem( tr ( "Mongolia" ) );
	cmbCountry->insertItem( tr ( "Montserrat" ) );
	cmbCountry->insertItem( tr ( "Morocco" ) );
	cmbCountry->insertItem( tr ( "Mozambique" ) );
	cmbCountry->insertItem( tr ( "Myanmar" ) );
	cmbCountry->insertItem( tr ( "Namibia" ) );
	cmbCountry->insertItem( tr ( "Nauru" ) );
	cmbCountry->insertItem( tr ( "Nepal" ) );
	cmbCountry->insertItem( tr ( "Netherlands" ) );
	cmbCountry->insertItem( tr ( "New Caledonia" ) );
	cmbCountry->insertItem( tr ( "New Zealand" ) );
	cmbCountry->insertItem( tr ( "Nicaragua" ) );
	cmbCountry->insertItem( tr ( "Niger" ) );
	cmbCountry->insertItem( tr ( "Nigeria" ) );
	cmbCountry->insertItem( tr ( "Niue" ) );
	cmbCountry->insertItem( tr ( "Norway" ) );
	cmbCountry->insertItem( tr ( "Oman" ) );
	cmbCountry->insertItem( tr ( "Pakistan" ) );
	cmbCountry->insertItem( tr ( "Palau" ) );
	cmbCountry->insertItem( tr ( "Palestinian Territory" ) );
	cmbCountry->insertItem( tr ( "Panama" ) );
	cmbCountry->insertItem( tr ( "Papua New Guinea" ) );
	cmbCountry->insertItem( tr ( "Paraguay" ) );
	cmbCountry->insertItem( tr ( "Peru" ) );
	cmbCountry->insertItem( tr ( "Philippines" ) );
	cmbCountry->insertItem( tr ( "Pitcairn" ) );
	cmbCountry->insertItem( tr ( "Poland" ) );
	cmbCountry->insertItem( tr ( "Portugal" ) );
	cmbCountry->insertItem( tr ( "Puerto Rico" ) );
	cmbCountry->insertItem( tr ( "Qatar" ) );
	cmbCountry->insertItem( tr ( "Reunion" ) );
	cmbCountry->insertItem( tr ( "Romania" ) );
	cmbCountry->insertItem( tr ( "Russia" ) );
	cmbCountry->insertItem( tr ( "Rwanda" ) );
	cmbCountry->insertItem( tr ( "Saint Lucia" ) );
	cmbCountry->insertItem( tr ( "Samoa" ) );
	cmbCountry->insertItem( tr ( "San Marino" ) );
	cmbCountry->insertItem( tr ( "Saudi Arabia" ) );
	cmbCountry->insertItem( tr ( "Senegal" ) );
	cmbCountry->insertItem( tr ( "Seychelles" ) );
	cmbCountry->insertItem( tr ( "Sierra Leone" ) );
	cmbCountry->insertItem( tr ( "Singapore" ) );
	cmbCountry->insertItem( tr ( "Slovakia" ) );
	cmbCountry->insertItem( tr ( "Slovenia" ) );
	cmbCountry->insertItem( tr ( "Solomon Islands" ) );
	cmbCountry->insertItem( tr ( "Somalia" ) );
	cmbCountry->insertItem( tr ( "South Africa" ) );
	cmbCountry->insertItem( tr ( "Spain" ) );
	cmbCountry->insertItem( tr ( "Sri Lanka" ) );
	cmbCountry->insertItem( tr ( "St. Helena" ) );
	cmbCountry->insertItem( tr ( "Sudan" ) );
	cmbCountry->insertItem( tr ( "Suriname" ) );
	cmbCountry->insertItem( tr ( "Swaziland" ) );
	cmbCountry->insertItem( tr ( "Sweden" ) );
	cmbCountry->insertItem( tr ( "Switzerland" ) );
	cmbCountry->insertItem( tr ( "Taiwan" ) );
	cmbCountry->insertItem( tr ( "Tajikistan" ) );
	cmbCountry->insertItem( tr ( "Tanzania" ) );
	cmbCountry->insertItem( tr ( "Thailand" ) );
	cmbCountry->insertItem( tr ( "Togo" ) );
	cmbCountry->insertItem( tr ( "Tokelau" ) );
	cmbCountry->insertItem( tr ( "Tonga" ) );
	cmbCountry->insertItem( tr ( "Tunisia" ) );
	cmbCountry->insertItem( tr ( "Turkey" ) );
	cmbCountry->insertItem( tr ( "Turkmenistan" ) );
	cmbCountry->insertItem( tr ( "Tuvalu" ) );
	cmbCountry->insertItem( tr ( "Uganda" ) );
	cmbCountry->insertItem( tr ( "Ukraine" ) );
	cmbCountry->insertItem( tr ( "Uruguay" ) );
	cmbCountry->insertItem( tr ( "Uzbekistan" ) );
	cmbCountry->insertItem( tr ( "Vanuatu" ) );
	cmbCountry->insertItem( tr ( "Venezuela" ) );
	cmbCountry->insertItem( tr ( "Viet Nam" ) );
	cmbCountry->insertItem( tr ( "Virgin Islands" ) );
	cmbCountry->insertItem( tr ( "Western Sahara" ) );
	cmbCountry->insertItem( tr ( "Yemen" ) );
	cmbCountry->insertItem( tr ( "Yugoslavia" ) );
	cmbCountry->insertItem( tr ( "Zambia" ) );
	cmbCountry->insertItem( tr ( "Zimbabwe" ) );

	cmbCountry->setMaximumWidth( 135 );

	gl->addMultiCellWidget( cmbCountry, 5, 5, 1, 2 );

	cmbChooserField4 = new QComboBox( FALSE, container );
	cmbChooserField4->setMaximumWidth( 90 );
	gl->addWidget( cmbChooserField4, 6, 0 );
	txtChooserField4 = new QLineEdit( container );
	gl->addMultiCellWidget( txtChooserField4, 6, 6, 1, 2 );

	QSpacerItem *space = new QSpacerItem(1,1,
                                             QSizePolicy::Maximum,
                                             QSizePolicy::MinimumExpanding );
	gl->addItem( space, 7, 0 );

	tabMain->insertTab( tabViewport, tr( "Address" ) );

	tabViewport = new QWidget ( tabMain );

	vb = new QVBoxLayout( tabViewport );

	svDetails = new QScrollView( tabViewport );
	vb->addWidget( svDetails, 0, 0 );
	svDetails->setResizePolicy( QScrollView::AutoOneFit );
	svDetails->setFrameStyle( QFrame::NoFrame );

	container = new QWidget( svDetails->viewport() );
	svDetails->addChild( container );

	gl = new QGridLayout( container, 1, 2, 2, 4 );

	int counter = 0;

	// Birthday
	l = new QLabel( tr("Birthday"), container );
	gl->addWidget( l, counter, 0 );

 	QPopupMenu* m1 = new QPopupMenu( container );
 	birthdayPicker = new DateBookMonth( m1, 0, TRUE );
 	m1->insertItem( birthdayPicker );

	birthdayButton= new QToolButton( container, "buttonStart" );
 	birthdayButton->setPopup( m1 );
	birthdayButton->setPopupDelay(0);
	gl->addWidget( birthdayButton, counter , 1  );
	connect( birthdayPicker, SIGNAL( dateClicked( int, int, int ) ),
		 this, SLOT( slotBirthdayDateChanged( int, int, int ) ) );

	++counter;

	// Anniversary
	l = new QLabel( tr("Anniversary"), container );
	gl->addWidget( l, counter, 0 );

 	m1 = new QPopupMenu( container );
 	anniversaryPicker = new DateBookMonth( m1, 0, TRUE );
 	m1->insertItem( anniversaryPicker );

	anniversaryButton= new QToolButton( container, "buttonStart" );
 	anniversaryButton->setPopup( m1 );
	anniversaryButton->setPopupDelay(0);
	gl->addWidget( anniversaryButton, counter , 1  );
	connect( anniversaryPicker, SIGNAL( dateClicked( int, int, int ) ),
		 this, SLOT( slotAnniversaryDateChanged( int, int, int ) ) );

	++counter;

	// Gender
	l = new QLabel( tr("Gender"), container );
	gl->addWidget( l, counter, 0 );
	cmbGender = new QComboBox( container );
	cmbGender->insertItem( "", 0 );
	cmbGender->insertItem( tr("Male"), 1);
	cmbGender->insertItem( tr("Female"), 2);
	gl->addWidget( cmbGender, counter, 1 );

	++counter;

	// Create Labels and lineedit fields for every dynamic entry
	QStringList::ConstIterator it = slDynamicEntries.begin();
	for (i = counter; it != slDynamicEntries.end(); i++, ++it) {
		l = new QLabel( QString::null , container );
		listName.append( l );
		gl->addWidget( l, i, 0 );
		QLineEdit *e = new QLineEdit( container );
		listValue.append( e );
		gl->addWidget( e, i, 1);
	}
	// Fill labels with names..
	loadFields();


	tabMain->insertTab( tabViewport, tr( "Details" ) );

	dlgNote = new QDialog( this, "Note Dialog", TRUE );
	dlgNote->setCaption( tr("Enter Note") );
	QVBoxLayout *vbNote = new QVBoxLayout( dlgNote );
	txtNote = new QMultiLineEdit( dlgNote );
	vbNote->addWidget( txtNote );
	connect( btnNote, SIGNAL(clicked()), this, SLOT(slotNote()) );

	dlgName = new QDialog( this, "Name Dialog", TRUE );
	dlgName->setCaption( tr("Edit Name") );
	gl = new QGridLayout( dlgName, 5, 2, 2, 3 );

	l = new QLabel( tr("First Name"), dlgName );
	gl->addWidget( l, 0, 0 );
	txtFirstName = new QLineEdit( dlgName );
	gl->addWidget( txtFirstName, 0, 1 );

	l = new QLabel( tr("Middle Name"), dlgName );
	gl->addWidget( l, 1, 0 );
	txtMiddleName = new QLineEdit( dlgName );
	gl->addWidget( txtMiddleName, 1, 1 );

	l = new QLabel( tr("Last Name"), dlgName );
	gl->addWidget( l, 2, 0 );
	txtLastName = new QLineEdit( dlgName );
	gl->addWidget( txtLastName, 2, 1 );

	l = new QLabel( tr("Suffix"), dlgName );
	gl->addWidget( l, 3, 0 );
	txtSuffix = new QLineEdit( dlgName );
	gl->addWidget( txtSuffix, 3, 1 );
        space = new QSpacerItem(1,1,
                                QSizePolicy::Maximum,
                                QSizePolicy::MinimumExpanding );
        gl->addItem( space, 4,  0 );

	cmbChooserField1->insertStringList( trlChooserNames );
	cmbChooserField2->insertStringList( trlChooserNames );
	cmbChooserField3->insertStringList( trlChooserNames );
	cmbChooserField4->insertStringList( trlChooserNames );

	cmbChooserField1->setCurrentItem( 0 );
	cmbChooserField2->setCurrentItem( 1 );
	cmbChooserField3->setCurrentItem( 2 );

	connect( btnFullName, SIGNAL(clicked()), this, SLOT(slotName()) );

	connect( txtFullName, SIGNAL(textChanged(const QString &)), this, SLOT(slotFullNameChange(const QString &)) );

	connect( txtChooserField1, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotChooser1Change(const QString &)) );
	connect( txtChooserField2, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotChooser2Change(const QString &)) );
	connect( txtChooserField3, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotChooser3Change(const QString &)) );
	connect( txtChooserField4, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotChooser4Change(const QString &)) );
	connect( txtAddress, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotAddressChange(const QString &)) );
	//connect( txtAddress2, SIGNAL(textChanged(const QString &)), this, SLOT(slotAddress2Change(const QString &)) );
	//connect( txtPOBox, SIGNAL(textChanged(const QString &)), this, SLOT(slotPOBoxChange(const QString &)) );
	connect( txtCity, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotCityChange(const QString &)) );
	connect( txtState, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotStateChange(const QString &)) );
	connect( txtZip, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotZipChange(const QString &)) );
	connect( cmbCountry, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotCountryChange(const QString &)) );
	connect( cmbCountry, SIGNAL(activated(const QString &)),
                 this, SLOT(slotCountryChange(const QString &)) );
	connect( cmbChooserField1, SIGNAL(activated(int)),
                 this, SLOT(slotCmbChooser1Change(int)) );
	connect( cmbChooserField2, SIGNAL(activated(int)),
                 this, SLOT(slotCmbChooser2Change(int)) );
	connect( cmbChooserField3, SIGNAL(activated(int)),
                 this, SLOT(slotCmbChooser3Change(int)) );
	connect( cmbChooserField4, SIGNAL(activated(int)),
                 this, SLOT(slotCmbChooser4Change(int)) );
	connect( cmbAddress, SIGNAL(activated(int)),
                 this, SLOT(slotAddressTypeChange(int)) );

	new QPEDialogListener(this);
}

void ContactEditor::initMap()
{
    /*
    // since the fields and the XML fields exist, create a map
    // between them...
    Config cfg1( "AddressBook" );
    Config cfg2( "AddressBook" );
    QString strCfg1,
	    strCfg2;
    int i;

    // This stuff better exist...
    cfg1.setGroup( "AddressFields" );
o    cfg2.setGroup( "XMLFields" );
    i = 0;
    strCfg1 = cfg1.readEntry( "Field" + QString::number(i), QString::null );
    strCfg2 = cfg2.readEntry( "XMLField" + QString::number(i++),
			      QString::null );
    while ( !strCfg1.isNull() && !strCfg2.isNull() ) {
	mapField.insert( strCfg1, strCfg2 );
	strCfg1 = cfg1.readEntry( "Field" + QString::number(i),
				  QString::null );
	strCfg2 = cfg2.readEntry( "XMLField" + QString::number(i++),
				  QString::null );
    }
    */
}

void ContactEditor::slotChooser1Change( const QString &textChanged ) {

	int index = cmbChooserField1->currentItem();

	slChooserValues[index] = textChanged;

}

void ContactEditor::slotChooser2Change( const QString &textChanged ) {

	int index = cmbChooserField2->currentItem();

	slChooserValues[index] = textChanged;

}

void ContactEditor::slotChooser3Change( const QString &textChanged ) {

	int index = cmbChooserField3->currentItem();

	slChooserValues[index] = textChanged;

}

void ContactEditor::slotChooser4Change( const QString &textChanged ) {

	int index = cmbChooserField4->currentItem();

	slChooserValues[index] = textChanged;

}

void ContactEditor::slotAddressChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[0] = textChanged;
	} else {
		slHomeAddress[0] = textChanged;
	}
}

void ContactEditor::slotAddress2Change( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[1] = textChanged;
	} else {
		slHomeAddress[1] = textChanged;
	}
}

void ContactEditor::slotPOBoxChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[2] = textChanged;
	} else {
		slHomeAddress[2] = textChanged;
	}
}

void ContactEditor::slotCityChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[3] = textChanged;
	} else {
		slHomeAddress[3] = textChanged;
	}
}

void ContactEditor::slotStateChange( const QString &textChanged ) {


	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[4] = textChanged;
	} else {
		slHomeAddress[4] = textChanged;
	}
}

void ContactEditor::slotZipChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[5] = textChanged;
	} else {
		slHomeAddress[5] = textChanged;
	}
}

void ContactEditor::slotCountryChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[6] = textChanged;
	} else {
		slHomeAddress[6] = textChanged;
	}
}

void ContactEditor::slotCmbChooser1Change( int index ) {

	txtChooserField1->setText( slChooserValues[index] );
        txtChooserField1->setFocus();
}

void ContactEditor::slotCmbChooser2Change( int index ) {

	txtChooserField2->setText( slChooserValues[index] );
        txtChooserField2->setFocus();
}

void ContactEditor::slotCmbChooser3Change( int index ) {

	txtChooserField3->setText( slChooserValues[index] );
        txtChooserField3->setFocus();

}

void ContactEditor::slotCmbChooser4Change( int index ) {

	txtChooserField4->setText( slChooserValues[index] );
        txtChooserField4->setFocus();

}

void ContactEditor::slotAddressTypeChange( int index ) {

	if ( index == 0 ) {

		txtAddress->setText( slBusinessAddress[0] );
		//txtAddress2->setText( (*slBusinessAddress)[1] );
		//txtPOBox->setText( (*slBusinessAddress)[2] );
		txtCity->setText( slBusinessAddress[3] );
		txtState->setText( slBusinessAddress[4] );
		txtZip->setText( slBusinessAddress[5] );
		QLineEdit *txtTmp = cmbCountry->lineEdit();
		txtTmp->setText( slBusinessAddress[6] );

	} else {

		txtAddress->setText( slHomeAddress[0] );
		//txtAddress2->setText( (*slHomeAddress)[1] );
		//txtPOBox->setText( (*slHomeAddress)[2] );
		txtCity->setText( slHomeAddress[3] );
		txtState->setText( slHomeAddress[4] );
		txtZip->setText( slHomeAddress[5] );
		QLineEdit *txtTmp = cmbCountry->lineEdit();
		txtTmp->setText( slHomeAddress[6] );

	}

}

void ContactEditor::slotFullNameChange( const QString &textChanged ) {

	int index = cmbFileAs->currentItem();

	cmbFileAs->clear();

	cmbFileAs->insertItem( parseName( textChanged, 0 ) );
	cmbFileAs->insertItem( parseName( textChanged, 1 ) );
	cmbFileAs->insertItem( parseName( textChanged, 2 ) );
	cmbFileAs->insertItem( parseName( textChanged, 3 ) );

	cmbFileAs->setCurrentItem( index );

	useFullName = TRUE;

}

// Loads the detail fields
void ContactEditor::loadFields() {

	QStringList::ConstIterator it;
	QListIterator<QLabel> lit( listName );
	for ( it = slDynamicEntries.begin(); *lit; ++lit, ++it) {

		if ( *it ==  "Department"  )
			(*lit)->setText( tr( "Department" ) );

		if ( *it ==  "Company" )
			(*lit)->setText( tr( "Company" ) );

		if ( *it ==  "Office" )
			(*lit)->setText( tr( "Office" ) );

		if ( *it == "Profession" )
			(*lit)->setText( tr( "Profession" ) );

		if ( *it ==  "Assistant" )
			(*lit)->setText( tr( "Assistant" ) );

		if ( *it == "Manager" )
			(*lit)->setText( tr( "Manager" ) );

		if ( *it == "Spouse" )
			(*lit)->setText( tr( "Spouse" ) );

		if ( *it == "Nickname" )
			(*lit)->setText( tr( "Nickname" ) );

		if ( *it == "Children" )
			(*lit)->setText( tr( "Children" ) );
	}
	// Set DatePicker
	qWarning ("**Info: %s", ent.birthday().latin1() );
	if ( !ent.birthday().isEmpty() ){
		birthdayButton->setText( ent.birthday() );
		birthdayPicker->setDate( TimeConversion::fromString ( ent.birthday() ) );
	} else
		birthdayButton->setText( tr ("Unknown") );
		
	qWarning ("**Info: %s", ent.anniversary().latin1() );
	if ( !ent.anniversary().isEmpty() ){
		anniversaryButton->setText( ent.anniversary() );
		anniversaryPicker->setDate( TimeConversion::fromString ( ent.birthday() ) );
	} else
		anniversaryButton->setText( tr ("Unknown") );

}

void ContactEditor::accept() {

	if ( isEmpty() ) {
		cleanupFields();
		reject();
	} else {
		saveEntry();
		cleanupFields();
		QDialog::accept();
	}

}

void ContactEditor::slotNote() {

	dlgNote->showMaximized();
	if ( !dlgNote->exec() ) {
		txtNote->setText( ent.notes() );
	}
}

void ContactEditor::slotName() {

	QString tmpName;
	if (useFullName == TRUE) {
		txtFirstName->setText( parseName(txtFullName->text(), NAME_F) );
		txtMiddleName->setText( parseName(txtFullName->text(), NAME_M) );
		txtLastName->setText( parseName(txtFullName->text(), NAME_L) );
		txtSuffix->setText( parseName(txtFullName->text(), NAME_S) );
	}
	dlgName->showMaximized();
	if ( dlgName->exec() ) {

		tmpName = txtFirstName->text() + " " + txtMiddleName->text() + " " + txtLastName->text() + " " + txtSuffix->text();
		txtFullName->setText( tmpName.simplifyWhiteSpace() );
		slotFullNameChange( txtFullName->text() );
		useFullName = FALSE;
	}

}

void ContactEditor::setNameFocus() {

	txtFullName->setFocus();

}

bool ContactEditor::isEmpty() {
	// Test and see if the record should be saved.
	// More strict than the original qtopia, needs name or fileas to save

	QString t = txtFullName->text();
	if ( !t.isEmpty() && containsAlphaNum( t ) )
		return false;

	t = cmbFileAs->currentText();
	if ( !t.isEmpty() && containsAlphaNum( t ) )
		return false;

	return true;

}

QString ContactEditor::parseName( const QString fullName, int type ) {

	QString simplifiedName( fullName.simplifyWhiteSpace() );
	QString strFirstName;
	QString strMiddleName;
	QString strLastName;
	QString strSuffix;
	QString strTitle;
	int commapos;
	int spCount;
	int spPos;
	int spPos2;


	commapos = simplifiedName.find( ',', 0, TRUE);
	spCount = simplifiedName.contains( ' ', TRUE );

	if ( commapos == -1 ) {

		switch (spCount) {
			case 0:
				//return simplifiedName;
				if (txtLastName->text() != "") {
					strLastName = simplifiedName;
					break;
				}
				if (txtMiddleName->text() != "") {
					strMiddleName = simplifiedName;
					break;
				}
				if (txtSuffix->text() != "") {
					strSuffix = simplifiedName;
					break;
				}
				strFirstName = simplifiedName;
				break;

			case 1:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strFirstName = simplifiedName.left( spPos );
				strLastName = simplifiedName.mid( spPos + 1 );
				break;

			case 2:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strFirstName = simplifiedName.left( spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strMiddleName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				strLastName = simplifiedName.mid( spPos2 + 1 );
				break;

			case 3:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strFirstName = simplifiedName.left( spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strMiddleName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				spPos = simplifiedName.find( ' ', spPos2 + 1, TRUE );
				strLastName = simplifiedName.mid( spPos2 + 1, (spPos - 1) - spPos2 );
				strSuffix = simplifiedName.mid( spPos + 1 );
				break;

			case 4:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strTitle = simplifiedName.left( spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strFirstName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				spPos = simplifiedName.find( ' ', spPos2 + 1, TRUE );
				strMiddleName = simplifiedName.mid( spPos2 + 1, (spPos - 1) - spPos2 );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strLastName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				strSuffix = simplifiedName.mid( spPos2 + 1 );
				break;

			default:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strTitle = simplifiedName.left( spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strFirstName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				spPos = simplifiedName.find( ' ', spPos2 + 1, TRUE );
				strMiddleName = simplifiedName.mid( spPos2 + 1, (spPos - 1) - spPos2 );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strLastName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				strSuffix = simplifiedName.mid( spPos2 + 1 );
				break;
		}
	} else {
		simplifiedName.replace( commapos, 1, " " );
		simplifiedName = simplifiedName.simplifyWhiteSpace();

		switch (spCount) {
			case 0:
				//return simplifiedName;
				if (txtLastName->text() != "") {
					strLastName = simplifiedName;
					break;
				}
				if (txtMiddleName->text() != "") {
					strMiddleName = simplifiedName;
					break;
				}
				if (txtSuffix->text() != "") {
					strSuffix = simplifiedName;
					break;
				}
				strFirstName = simplifiedName;
				break;

			case 1:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strLastName = simplifiedName.left( spPos );
				strFirstName = simplifiedName.mid( spPos + 1 );
				break;

			case 2:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strLastName = simplifiedName.left( spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strFirstName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				strMiddleName = simplifiedName.mid( spPos2 + 1 );
				break;

			case 3:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strLastName = simplifiedName.left( spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strFirstName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				spPos = simplifiedName.find( ' ', spPos2 + 1, TRUE );
				strMiddleName = simplifiedName.mid( spPos2 + 1, (spPos - 1) - spPos2 );
				strSuffix = simplifiedName.mid( spPos + 1 );
				break;

			case 4:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strLastName = simplifiedName.left( spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strTitle = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				spPos = simplifiedName.find( ' ', spPos2 + 1, TRUE );
				strFirstName = simplifiedName.mid( spPos2 + 1, (spPos - 1) - spPos2 );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strMiddleName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				strSuffix = simplifiedName.mid( spPos2 + 1 );
				break;

			default:
				spPos = simplifiedName.find( ' ', 0, TRUE );
				strLastName = simplifiedName.left( spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strTitle = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				spPos = simplifiedName.find( ' ', spPos2 + 1, TRUE );
				strFirstName = simplifiedName.mid( spPos2 + 1, (spPos - 1) - spPos );
				spPos2 = simplifiedName.find( ' ', spPos + 1, TRUE );
				strMiddleName = simplifiedName.mid( spPos + 1, (spPos2 - 1) - spPos );
				strSuffix = simplifiedName.mid( spPos2 + 1 );
				break;
		}
	}
	switch (type) {
		case NAME_FL:
			return strFirstName + " " + strLastName;

		case NAME_LF:
			return strLastName + ", " + strFirstName;

		case NAME_LFM:
			return strLastName + ", " + strFirstName + " " + strMiddleName;

		case NAME_FMLS:
			return strFirstName + " " + strMiddleName + " " + strLastName + " " + strSuffix;

		case NAME_F:
			return strFirstName;

		case NAME_M:
			return strMiddleName;

		case NAME_L:
			return strLastName;

		case NAME_S:
			return strSuffix;

	}
	return QString::null;
}

void ContactEditor::cleanupFields() {

	QStringList::Iterator it = slChooserValues.begin();
	for ( int i = 0; it != slChooserValues.end(); i++, ++it ) {
		(*it) = "";
	}

	for ( int i = 0; i < 7; i++ ) {
		slHomeAddress[i] = "";
		slBusinessAddress[i] = "";
	}

	QStringList::ConstIterator cit;
	QListIterator<QLineEdit> itLE( listValue );
	for ( cit = slDynamicEntries.begin(); cit != slDynamicEntries.end(); ++cit, ++itLE) {
		(*itLE)->setText( "" );
	}

	txtFirstName->setText("");
	txtMiddleName->setText("");
	txtLastName->setText("");
	txtSuffix->setText("");
	txtNote->setText("");
	txtFullName->setText("");
	txtJobTitle->setText("");
	txtOrganization->setText("");
	txtChooserField1->setText("");
	txtChooserField2->setText("");
	txtChooserField3->setText("");
	txtAddress->setText("");
	//txtAddress2->setText("");
	txtCity->setText("");
	//txtPOBox->setText("");
	txtState->setText("");
	txtZip->setText("");
	QLineEdit *txtTmp = cmbCountry->lineEdit();
	txtTmp->setText("");
	txtTmp = cmbFileAs->lineEdit();
	txtTmp->setText("");

}

void ContactEditor::setEntry( const OContact &entry ) {

	cleanupFields();


	ent = entry;

	useFullName = FALSE;
	txtFirstName->setText( ent.firstName() );
	txtMiddleName->setText( ent.middleName() );
	txtLastName->setText( ent.lastName() );
	txtSuffix->setText( ent.suffix() );

	QString *tmpString = new QString;
	*tmpString = ent.firstName() + " " + ent.middleName() +
		+ " " + ent.lastName() + " " + ent.suffix();

	txtFullName->setText( tmpString->simplifyWhiteSpace() );

	cmbFileAs->setEditText( ent.fileAs() );

	if (hasTitle)
		txtJobTitle->setText( ent.jobTitle() );

	if (hasCompany)
		txtOrganization->setText( ent.company() );

	if (hasNotes)
		txtNote->setText( ent.notes() );

	if (hasStreet) {
		slHomeAddress[0] = ent.homeStreet();
		slBusinessAddress[0] = ent.businessStreet();
	}
/*
	if (hasStreet2) {
		(*slHomeAddress)[1] = ent.homeStreet2();
		(*slBusinessAddress)[1] = ent.businessStreet2();
	}

	if (hasPOBox) {
		(*slHomeAddress)[2] = ent.homePOBox();
		(*slBusinessAddress)[2] = ent.businessPOBox();
	}
*/
	if (hasCity) {
		slHomeAddress[3] = ent.homeCity();
		slBusinessAddress[3] = ent.businessCity();
	}

	if (hasState) {
		slHomeAddress[4] = ent.homeState();
		slBusinessAddress[4] = ent.businessState();
	}

	if (hasZip) {
		slHomeAddress[5] = ent.homeZip();
		slBusinessAddress[5] = ent.businessZip();
	}

	if (hasCountry) {
		slHomeAddress[6] = ent.homeCountry();
		slBusinessAddress[6] = ent.businessCountry();
	}

	QStringList::ConstIterator it;
	QListIterator<QLineEdit> itLE( listValue );
	for ( it = slDynamicEntries.begin(); it != slDynamicEntries.end(); ++it, ++itLE) {
		if ( *it ==  "Department"  )
			(*itLE)->setText( ent.department() );

		if ( *it == "Company" )
			(*itLE)->setText( ent.company() );

		if ( *it ==  "Office" )
			(*itLE)->setText( ent.office() );

		if ( *it ==  "Profession" )
			(*itLE)->setText( ent.profession() );

		if ( *it == "Assistant" )
			(*itLE)->setText( ent.assistant() );

		if ( *it == "Manager" )
			(*itLE)->setText( ent.manager() );

		if ( *it == "Spouse" )
			(*itLE)->setText( ent.spouse() );

		if ( *it == "Nickname" )
			(*itLE)->setText( ent.nickname() );

		if ( *it == "Children" )
			(*itLE)->setText( ent.children() );

	}

	QStringList::Iterator itV;
	for ( it = slChooserNames.begin(), itV = slChooserValues.begin(); it != slChooserNames.end(); ++it, ++itV ) {

		if ( ( *it == "Business Phone") || ( *it == "Work Phone" ) )
			*itV = ent.businessPhone();
/*
		if ( *it == "Business 2 Phone" )
			*itV = ent.business2Phone();
*/
		if ( ( *it == "Business Fax") || ( *it == "Work Fax" ) )
			*itV = ent.businessFax();

		if ( ( *it == "Business Mobile" ) || ( *it == "work Mobile" ) )
			*itV = ent.businessMobile();
/*
		if ( *it == "Company Phone" )
			*itV = ent.companyPhone();
*/
		if ( *it == "Default Email" )
			*itV = ent.defaultEmail();

		if ( *it == "Emails" )
			*itV = ent.emailList().join(", ");  // :SX

		if ( *it == "Home Phone" )
			*itV = ent.homePhone();
/*
		if ( *it == "Home 2 Phone" )
			*itV = ent.home2Phone();
*/
		if ( *it == "Home Fax" )
			*itV = ent.homeFax();

		if ( *it == "Home Mobile" )
			*itV = ent.homeMobile();
/*
		if ( *it == "Car Phone" )
			*itV = ent.carPhone();

		if ( *it == "ISDN Phone" )
			*itV = ent.ISDNPhone();

		if ( *it == "Other Phone" )
			*itV = ent.otherPhone();
*/
		if ( ( *it == "Business Pager" ) || ( *it == "Work Pager" ) )
			*itV = ent.businessPager();
/*
		if ( *it == "Home Pager")
			*itV = ent.homePager();

		if ( *it == "AIM IM" )
			*itV = ent.AIMIM();

		if ( *it == "ICQ IM" )
			*itV = ent.ICQIM();

		if ( *it == "Jabber IM" )
			*itV = ent.jabberIM();

		if ( *it == "MSN IM" )
			*itV = ent.MSNIM();

		if ( *it == "Yahoo IM" )
			*itV = ent.yahooIM();
*/
		if ( *it == "Home Web Page" )
			*itV = ent.homeWebpage();

		if ( ( *it == "Business WebPage" ) || ( *it == "Work Web Page" ) )
			*itV = ent.businessWebpage();


	}

	
	cmbCat->setCategories( ent.categories(), "Contacts", tr("Contacts") );

	QString gender = ent.gender();
	cmbGender->setCurrentItem( gender.toInt() );

	txtNote->setText( ent.notes() );

	slotCmbChooser1Change( cmbChooserField1->currentItem() );
	slotCmbChooser2Change( cmbChooserField2->currentItem() );
	slotCmbChooser3Change( cmbChooserField3->currentItem() );

	slotAddressTypeChange( cmbAddress->currentItem() );

	loadFields();

}

void ContactEditor::saveEntry() {

	if ( useFullName == TRUE ) {
		txtFirstName->setText( parseName( txtFullName->text(), NAME_F ) );
		txtMiddleName->setText( parseName( txtFullName->text(), NAME_M ) );
		txtLastName->setText( parseName( txtFullName->text(), NAME_L ) );
		txtSuffix->setText( parseName( txtFullName->text(), NAME_S ) );

		useFullName = FALSE;
}

	/*if ( ent.firstName() != txtFirstName->text() ||
			ent.lastName != txtLastName->text() ||
			ent.middleName != txtMiddleName->text() ) {
	*/
		ent.setFirstName( txtFirstName->text() );
		ent.setLastName( txtLastName->text() );
		ent.setMiddleName( txtMiddleName->text() );
		ent.setSuffix( txtSuffix->text() );

	//}

	ent.setFileAs( cmbFileAs->currentText() );

	ent.setCategories( cmbCat->currentCategories() );

	if (hasTitle)
		ent.setJobTitle( txtJobTitle->text() );

	if (hasCompany)
		ent.setCompany( txtOrganization->text() );

	if (hasNotes)
		ent.setNotes( txtNote->text() );

	if (hasStreet) {
		ent.setHomeStreet( slHomeAddress[0] );
		ent.setBusinessStreet( slBusinessAddress[0] );
	}
/*
	if (hasStreet2) {
		ent.setHomeStreet2( (*slHomeAddress)[1] );
		ent.setBusinessStreet2( (*slBusinessAddress)[1] );
	}

	if (hasPOBox) {
		ent.setHomePOBox( (*slHomeAddress)[2] );
		ent.setBusinessPOBox( (*slBusinessAddress)[2] );
	}
*/
	if (hasCity) {
		ent.setHomeCity( slHomeAddress[3] );
		ent.setBusinessCity( slBusinessAddress[3] );
	}

	if (hasState) {
		ent.setHomeState( slHomeAddress[4] );
		ent.setBusinessState( slBusinessAddress[4] );
	}

	if (hasZip) {
		ent.setHomeZip( slHomeAddress[5] );
		ent.setBusinessZip( slBusinessAddress[5] );
	}

	if (hasCountry) {
		ent.setHomeCountry( slHomeAddress[6] );
		ent.setBusinessCountry( slBusinessAddress[6] );
	}

	QStringList::ConstIterator it;
	QListIterator<QLineEdit> itLE( listValue );
	for ( it = slDynamicEntries.begin(); it != slDynamicEntries.end(); ++it, ++itLE) {
		if ( *it == "Department" )
			ent.setDepartment( (*itLE)->text() );

		if ( *it == "Company" )
			ent.setCompany( (*itLE)->text() );

		if ( *it == "Office" )
			ent.setOffice( (*itLE)->text() );

		if ( *it == "Profession" )
			ent.setProfession( (*itLE)->text() );

		if ( *it == "Assistant" )
			ent.setAssistant( (*itLE)->text() );

		if ( *it == "Manager" )
			ent.setManager( (*itLE)->text() );

		if ( *it == "Spouse" )
			ent.setSpouse( (*itLE)->text() );

		if ( *it == "Nickname" )
			ent.setNickname( (*itLE)->text() );

		if ( *it == "Children" )
			ent.setChildren( (*itLE)->text() );

	}

	QStringList::ConstIterator itV;
	for ( it = slChooserNames.begin(), itV = slChooserValues.begin(); it != slChooserNames.end(); ++it, ++itV ) {

		if ( ( *it == "Business Phone" ) || ( *it == "Work Phone"  ) )
			ent.setBusinessPhone( *itV );
/*
		if ( *it == tr("Business 2 Phone" )
			ent.setBusiness2Phone( *itV );
*/
		if ( ( *it == "Business Fax" ) || ( *it == "Work Fax" ) )
			ent.setBusinessFax( *itV );

		if ( ( *it == "Business Mobile" ) || ( *it == "Work Mobile" ) )
			ent.setBusinessMobile( *itV );
/*
		if ( *it == "Company Phone" )
			ent.setCompanyPhone( *itV );
*/
		//if ( *it == "Default Email" )
			//ent.setDefaultEmail( *itV );

		if ( *it == "Emails" ){
			QString allemail;
			QString defaultmail;
			parseEmailFrom( *itV, defaultmail, allemail );
			// ent.clearEmails();
			ent.setDefaultEmail(  defaultmail );
			ent.setEmails( allemail );
		}

		if ( *it == "Home Phone" )
			ent.setHomePhone( *itV );
/*
		if ( *it == "Home 2 Phone" )
			ent.setHome2Phone( *itV );
*/
		if ( *it == "Home Fax" )
			ent.setHomeFax( *itV );

		if ( *it == "Home Mobile" )
			ent.setHomeMobile( *itV );
/*
		if ( *it == "Car Phone" )
			ent.setCarPhone( *itV );

		if ( *it == "ISDN Phone" )
			ent.setISDNPhone( *itV );

		if ( *it == "Other Phone" )
			ent.setOtherPhone( *itV );
*/
		if ( ( *it == "Business Pager" ) || ( *it == "Work Pager" ) )
			ent.setBusinessPager( *itV );
/*
		if ( *it == "Home Pager" )
			ent.setHomePager( *itV );

		if ( *it == "AIM IM" )
			ent.setAIMIM( *itV );

		if ( *it == "ICQ IM" )
			ent.setICQIM( *itV );

		if ( *it == "Jabber IM" )
			ent.setJabberIM( *itV );

		if ( *it == "MSN IM" )
			ent.setMSNIM( *itV );

		if ( *it == "Yahoo IM" )
			ent.setYahooIM( *itV );
*/
		if ( *it == "Home Web Page" )
			ent.setHomeWebpage( *itV );

		if ( ( *it == "Business WebPage" ) || ( *it == "Work Web Page" ) )
			ent.setBusinessWebpage( *itV );


	}

	int gender = cmbGender->currentItem();
	ent.setGender( QString::number( gender ) );

	QString str = txtNote->text();
	if ( !str.isNull() )
		ent.setNotes( str );

}

void parseEmailFrom( const QString &txt, QString &strDefaultEmail,
		     QString &strAll )
{
    int where,
	start;
    if ( txt.isEmpty() )
	return;
    // find the first
    where = txt.find( ',' );
    if ( where < 0 ) {
	strDefaultEmail = txt;
	strAll = txt;
    } else {
	strDefaultEmail = txt.left( where ).stripWhiteSpace();
	strAll = strDefaultEmail;
	while ( where > -1 ) {
	    strAll.append(" ");
	    start = where;
	    where = txt.find( ',', where + 1 );
	    if ( where > - 1 )
		strAll.append( txt.mid(start + 1, where - start - 1).stripWhiteSpace() );
	    else // grab until the end...
		strAll.append( txt.right(txt.length() - start - 1).stripWhiteSpace() );
	}
    }
}

void parseEmailTo( const QString &strDefaultEmail,
		   const QString &strOtherEmail, QString &strBack )
{
    // create a comma dilimeted set of emails...
    // use the power of short circuiting...
    bool foundDefault = false;
    QString strTmp;
    int start = 0;
    int where;
    // start at the beginng.
    strBack = strDefaultEmail;
    where = 0;
    while ( where > -1 ) {
	start = where;
	where = strOtherEmail.find( ' ', where + 1 );
	if ( where > 0 ) {
	    strTmp = strOtherEmail.mid( start, where - start ).stripWhiteSpace();
	} else
	    strTmp = strOtherEmail.right( strOtherEmail.length() - start ).stripWhiteSpace();
	if ( foundDefault || strTmp != strDefaultEmail ) {
	    strBack.append( ", " );
	    strBack.append( strTmp );
	} else
	    foundDefault = true;
    }
}


static inline bool containsAlphaNum( const QString &str )
{
    int i,
	count = str.length();
    for ( i = 0; i < count; i++ )
	if ( !str[i].isSpace() )
	    return TRUE;
    return FALSE;
}

static inline bool constainsWhiteSpace( const QString &str )
{
    int i,
	count = str.length();
    for (i = 0; i < count; i++ )
	if ( str[i].isSpace() )
	    return TRUE;
    return FALSE;
}

void ContactEditor::setPersonalView( bool personal )
{
	m_personalView = personal;
	if ( personal ){
		cmbCat->hide();
		labCat->hide();
	} else{
		cmbCat->show();
		labCat->show();
		
	}
}

void ContactEditor::slotAnniversaryDateChanged( int year, int month, int day)
{
    QDate date;
    date.setYMD( year, month, day );
    QString dateString = TimeString::numberDateString( date );
    anniversaryButton->setText( dateString );
    ent.setAnniversary ( dateString );
}

void ContactEditor::slotBirthdayDateChanged( int year, int month, int day)
{
    QDate date;
    date.setYMD( year, month, day );
    QString dateString = TimeString::numberDateString( date );
    birthdayButton->setText( dateString );
    ent.setBirthday ( dateString );
}
