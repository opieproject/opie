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

#include "abeditor.h"
#include "addresspicker.h"

#include <qpe/categoryselect.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpedialog.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qscrollview.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qmainwindow.h>


static inline bool containsAlphaNum( const QString &str );
static inline bool constainsWhiteSpace( const QString &str );


// helper functions, convert our comma delimited list to proper
// file format...
void parseEmailFrom( const QString &txt, QString &strDefaultEmail,
		     QString &strAll );

// helper convert from file format to comma delimited...
void parseEmailTo( const QString &strDefaultEmail,
		   const QString &strOtherEmail, QString &strBack );



AbEditor::AbEditor( const Contact &entry, const QValueList<int> *newOrdered,
		    QStringList *slNewOrdered,
		    QWidget *parent, const char *name, WFlags fl )
    : QDialog( parent, name, TRUE, fl ),
      orderedValues( newOrdered ),
      slOrdered( slNewOrdered )
{
    init();
    initMap();
    setEntry( entry );
}

AbEditor::~AbEditor()
{
}

void AbEditor::init()
{
    middleEdit = 0;
    QVBoxLayout *vb = new QVBoxLayout( this );
    svPage = new QScrollView( this );
    svPage->setHScrollBarMode( QScrollView::AlwaysOff );
    vb->addWidget( svPage );
    svPage->setResizePolicy( QScrollView::AutoOneFit );
    svPage->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( svPage->viewport() );
    svPage->addChild( container );

    QGridLayout *gl = new QGridLayout( container, 20, 2, 4, 2 );

    QLabel *l = new QLabel( tr("First Name"), container );
    gl->addWidget( l, 0, 0 );
    firstEdit = new QLineEdit( container );
    gl->addWidget( firstEdit, 0, 1 );

    l = new QLabel( tr("Last Name"), container );
    gl->addWidget( l, 1, 0 );
    lastEdit = new QLineEdit( container );
    gl->addWidget( lastEdit, 1, 1 );

    l = new QLabel( tr("Categories"), container );
    gl->addWidget( l, 2, 0 );

    cmbCat = new CategorySelect( container );
    gl->addWidget( cmbCat, 2, 1 );

    int i;
    bool foundGender,
	foundNotes;
    foundGender = foundNotes = false;
    QStringList::ConstIterator it = slOrdered->begin();
    for ( i = 0; it != slOrdered->end(); i++, ++it ) {
	if ( !foundGender && *it == tr("Gender") ) {
	    foundGender = true;
	} else if ( !foundNotes && *it == tr("Notes") ) {
	    foundNotes = true;
	} else {
	    l = new QLabel( *it, container );
	    listName.append( l );
	    gl->addWidget( l, i + 3, 0 );
	    QLineEdit *e = new QLineEdit( container );
	    listValue.append( e );
	    gl->addWidget( e, i + 3, 1 );
	    if ( *it == tr( "Middle Name" ) )
		middleEdit = e;
	}
    }
    l = new QLabel( tr("Gender"), container );
    gl->addWidget( l, slOrdered->count() + 3, 0 );
    genderCombo = new QComboBox( container );
    genderCombo->insertItem( "", 0 );
    genderCombo->insertItem( tr( "Male" ), 1 );
    genderCombo->insertItem( tr( "Female" ), 2 );
    gl->addWidget( genderCombo, slOrdered->count() + 3, 1 );

    dlgNote = new QDialog( this, "Note Dialog", TRUE );
    dlgNote->setCaption( tr("Enter Note") );
    QVBoxLayout *vbNote = new QVBoxLayout( dlgNote );
    //    lblNote = new QLabel( dlgNote );
    //    lblNote->setMinimumSize( lblNote->sizeHint() + QSize( 0, 4 ) );
    //    vbNote->addWidget( lblNote );
    txtNote = new QMultiLineEdit( dlgNote );
    vbNote->addWidget( txtNote );

    QHBoxLayout *hb = new QHBoxLayout( vb );
    hb->addStretch( 2 );
    QPushButton *pb = new QPushButton( tr("Notes..."), this );
    hb->addWidget( pb );
    connect( pb, SIGNAL(clicked()), this, SLOT(slotNote()) );

    new QPEDialogListener(this);
}

void AbEditor::initMap()
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
    cfg2.setGroup( "XMLFields" );
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

void AbEditor::loadFields()
{
    QStringList::ConstIterator it;
    QListIterator<QLabel> lit( listName );
    for ( it = slOrdered->begin(); *lit; ++lit, ++it ) {
	(*lit)->setText( *it );
    }
}

void AbEditor::setEntry( const Contact &entry )
{
    ent = entry;
    QListIterator<QLineEdit> it( listValue );
    firstEdit->setText( ent.firstName() );
    lastEdit->setText( ent.lastName() );
    cmbCat->setCategories( ent.categories(), "Contacts", tr("Contacts") );

    // ### Fix...
    QValueList<int>::ConstIterator itVl;
    for ( itVl = orderedValues->begin(); *it && itVl != orderedValues->end();
          ++itVl, ++it ) {
	switch( *itVl ) {
        case Qtopia::Title:
            (*it)->setText(ent.title());
            break;
        case Qtopia::MiddleName:
            (*it)->setText( ent.middleName() );
            break;
        case Qtopia::Suffix:
            (*it)->setText( ent.suffix() );
            break;

            // email
        case Qtopia::DefaultEmail:
	case Qtopia::Emails:
	    {
		QString strDefEmail = ent.defaultEmail();
		QString strAllEmail = ent.emails();
		QString strFinal;
		parseEmailTo( strDefEmail, strAllEmail, strFinal );
		(*it)->setText( strFinal );
		// make sure we see the "default"
		(*it)->home( false );
		break;
	    }

            // home
        case Qtopia::HomeStreet:
            (*it)->setText(ent.homeStreet() );
            break;
        case Qtopia::HomeCity:
            (*it)->setText( ent.homeCity() );
            break;
        case Qtopia::HomeState:
            (*it)->setText( ent.homeState() );
            break;
        case Qtopia::HomeZip:
            (*it)->setText( ent.homeZip() );
            break;
        case Qtopia::HomeCountry:
            (*it)->setText( ent.homeCountry() );
            break;
        case Qtopia::HomePhone:
            (*it)->setText( ent.homePhone() );
            break;
        case Qtopia::HomeFax:
            (*it)->setText( ent.homeFax() );
            break;
        case Qtopia::HomeMobile:
            (*it)->setText( ent.homeMobile() );
            break;
        case Qtopia::HomeWebPage:
            (*it)->setText( ent.homeWebpage() );
            break;

            // business
        case Qtopia::Company:
            (*it)->setText( ent.company() );
            break;
        case Qtopia::BusinessStreet:
           (*it)->setText( ent.businessStreet() );
            break;
        case Qtopia::BusinessCity:
            (*it)->setText( ent.businessCity() );
            break;
        case Qtopia::BusinessState:
            (*it)->setText( ent.businessState() );
            break;
        case Qtopia::BusinessZip:
            (*it)->setText( ent.businessZip() );
            break;
        case Qtopia::BusinessCountry:
            (*it)->setText( ent.businessCountry() );
            break;
        case Qtopia::BusinessWebPage:
            (*it)->setText( ent.businessWebpage() );
            break;
        case Qtopia::JobTitle:
            (*it)->setText( ent.jobTitle() );
            break;
        case Qtopia::Department:
            (*it)->setText( ent.department() );
            break;
        case Qtopia::Office:
            (*it)->setText( ent.office() );
            break;
        case Qtopia::BusinessPhone:
            (*it)->setText( ent.businessPhone() );
            break;
        case Qtopia::BusinessFax:
            (*it)->setText( ent.businessFax() );
            break;
        case Qtopia::BusinessMobile:
            (*it)->setText( ent.businessMobile() );
            break;
        case Qtopia::BusinessPager:
            (*it)->setText( ent.businessPager() );
            break;
        case Qtopia::Profession:
            (*it)->setText( ent.profession() );
            break;
        case Qtopia::Assistant:
            (*it)->setText( ent.assistant() );
            break;
        case Qtopia::Manager:
            (*it)->setText( ent.manager() );
            break;

            // personal
        case Qtopia::Spouse:
            (*it)->setText( ent.spouse() );
            break;
        case Qtopia::Children:
            (*it)->setText( ent.children() );
            break;
        case Qtopia::Birthday:
            (*it)->setText( ent.birthday() );
            break;
        case Qtopia::Anniversary:
            (*it)->setText( ent.anniversary() );
            break;
        case Qtopia::Nickname:
            (*it)->setText( ent.nickname() );
            break;

	}
    }

    QString gender = ent.gender();
    genderCombo->setCurrentItem( gender.toInt() );

    txtNote->setText( ent.notes() );
}

void AbEditor::accept()
{
    if ( isEmpty() )
	reject();
    else {
	saveEntry();
	QDialog::accept();
    }
}

bool AbEditor::isEmpty()
{
    // analyze all the fields and make sure there is _something_ there
    // that warrants saving...
    QString t = firstEdit->text();
    if ( !t.isEmpty() && containsAlphaNum( t ) )
	return false;

    t = lastEdit->text();
    if ( !t.isEmpty() && containsAlphaNum( t ) )
	return false;

    QListIterator<QLineEdit> it( listValue );
    for ( ; it.current(); ++it ) {
	t = it.current()->text();
	if ( !t.isEmpty() && containsAlphaNum( t ) )
	    return false;
    }

    t = txtNote->text();
    if ( !t.isEmpty() && containsAlphaNum( t ) )
	return false;

    return true;
}

void AbEditor::saveEntry()
{
    QString strDefaultEmail, strOtherEmail;

    // determine if there has been a change in names
    if ( ent.firstName() != firstEdit->text() ||
	 ent.lastName() != lastEdit->text()
	 || (middleEdit && ent.middleName() != middleEdit->text()) ) {
	// set the names
	ent.setFirstName( firstEdit->text() );
	ent.setLastName( lastEdit->text() );
	if ( middleEdit )
	    ent.setMiddleName( middleEdit->text() );
	ent.setFileAs();
    }

    ent.setCategories( cmbCat->currentCategories() );

    QListIterator<QLineEdit> it( listValue );
    int i;
    QValueList<int>::ConstIterator<int> vlIt;
    for ( i = 0, vlIt = orderedValues->begin();
	  it.current(); ++it, ++vlIt, i++ ) {
	switch( *vlIt ) {
	case Qtopia::Title:
	    ent.setTitle( it.current()->text() );
	    break;
	case Qtopia::MiddleName:
	    ent.setMiddleName( it.current()->text() );
	    break;
	case Qtopia::Suffix:
	    ent.setSuffix( it.current()->text() );
	    break;
// 	case Qtopia::Category:
// 	    {
// // 		QStringList slCat = QStringList::split( ";", value );
// // 		QValueList<int> cat;
// // 		for ( QStringList::ConstIterator it = slCat.begin();
// // 		      it != slCat.end(); ++it )
// // 		    cat.append( (*it).toInt() );
// // 		ent.setCategories( cat );
// 	    }
// 	    break;

	    // email
	case Qtopia::DefaultEmail:
	case Qtopia::Emails:
 	 	    parseEmailFrom( it.current()->text(), strDefaultEmail,
	 			    strOtherEmail );
	 	    ent.setDefaultEmail( strDefaultEmail );
	 	    ent.setEmails( strOtherEmail  );
	    break;

	    // home
	case Qtopia::HomeStreet:
	    ent.setHomeStreet( it.current()->text() );
	    break;
	case Qtopia::HomeCity:
	    ent.setHomeCity( it.current()->text() );
	    break;
	case Qtopia::HomeState:
	    ent.setHomeState( it.current()->text() );
	    break;
	case Qtopia::HomeZip:
	    ent.setHomeZip( it.current()->text() );
	    break;
	case Qtopia::HomeCountry:
	    ent.setHomeCountry( it.current()->text() );
	    break;
	case Qtopia::HomePhone:
	    ent.setHomePhone( it.current()->text() );
	    break;
	case Qtopia::HomeFax:
	    ent.setHomeFax( it.current()->text() );
	    break;
	case Qtopia::HomeMobile:
	    ent.setHomeMobile( it.current()->text() );
	    break;
	case Qtopia::HomeWebPage:
	    ent.setHomeWebpage( it.current()->text() );
	    break;

	    // business
	case Qtopia::Company:
	    ent.setCompany( it.current()->text() );
	    break;
	case Qtopia::BusinessStreet:
	    ent.setBusinessStreet( it.current()->text() );
	    break;
	case Qtopia::BusinessCity:
	    ent.setBusinessCity( it.current()->text() );
	    break;
	case Qtopia::BusinessState:
	    ent.setBusinessState( it.current()->text() );
	    break;
	case Qtopia::BusinessZip:
	    ent.setBusinessZip( it.current()->text() );
	    break;
	case Qtopia::BusinessCountry:
	    ent.setBusinessCountry( it.current()->text() );
	    break;
	case Qtopia::BusinessWebPage:
	    ent.setBusinessWebpage( it.current()->text() );
	    break;
	case Qtopia::JobTitle:
	    ent.setJobTitle( it.current()->text() );
	    break;
	case Qtopia::Department:
	    ent.setDepartment( it.current()->text() );
	    break;
	case Qtopia::Office:
	    ent.setOffice( it.current()->text() );
	    break;
	case Qtopia::BusinessPhone:
	    ent.setBusinessPhone( it.current()->text() );
	    break;
	case Qtopia::BusinessFax:
	    ent.setBusinessFax( it.current()->text() );
	    break;
	case Qtopia::BusinessMobile:
	    ent.setBusinessMobile( it.current()->text() );
	    break;
	case Qtopia::BusinessPager:
	    ent.setBusinessPager( it.current()->text() );
	    break;
	case Qtopia::Profession:
	    ent.setProfession( it.current()->text() );
	    break;
	case Qtopia::Assistant:
	    ent.setAssistant( it.current()->text() );
	    break;
	case Qtopia::Manager:
	    ent.setManager( it.current()->text() );
	    break;

	    // personal
	case Qtopia::Spouse:
	    ent.setSpouse( it.current()->text() );
	    break;
	case Qtopia::Children:
	    ent.setChildren( it.current()->text() );
	    break;
	case Qtopia::Birthday:
	    ent.setBirthday( it.current()->text() );
	    break;
	case Qtopia::Anniversary:
	    ent.setAnniversary( it.current()->text() );
	    break;
	case Qtopia::Nickname:
	    ent.setNickname( it.current()->text() );
	    break;
	default:
	    break;

	}
    }

    int gender = genderCombo->currentItem();
    ent.setGender( QString::number( gender ) );

    QString str = txtNote->text();
    if ( !str.isNull() )
	ent.setNotes( str );
}

void AbEditor::slotNote()
{
    dlgNote->showMaximized();
    if ( !dlgNote->exec() ) {
	// reset the note...
	txtNote->setText( ent.notes() );
    }
}

void AbEditor::setNameFocus()
{
    firstEdit->setFocus();
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

