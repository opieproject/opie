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

#include "qimpenwidget.h"
#include "qimpenprefbase.h"
#include "qimpensetup.h"

#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include <qcombobox.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qbuttongroup.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qdir.h>
#include <qmessagebox.h>


/* XPM */
static const char * const left_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #000000",
"                ",
"                ",
"                ",
"        .       ",
"       ..       ",
"      ...       ",
"     ....       ",
"    .....       ",
"   ......       ",
"    .....       ",
"     ....       ",
"      ...       ",
"       ..       ",
"        .       ",
"                ",
"                "};


/* XPM */
static const char * const right_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #000000",
"                ",
"                ",
"                ",
"     .          ",
"     ..         ",
"     ...        ",
"     ....       ",
"     .....      ",
"     ......     ",
"     .....      ",
"     ....       ",
"     ...        ",
"     ..         ",
"     .          ",
"                ",
"                "};



QIMPenSetup::QIMPenSetup( QIMPenProfile *p, QWidget *parent,
                const char *name, bool modal, int WFlags )
    : QDialog( parent, name, modal, WFlags ), profileCombo(0), profile(p)
{
    setCaption( tr("Setup Handwriting Input") );

    QVBoxLayout *vb = new QVBoxLayout( this );

#if 1
    profileList.setAutoDelete( true );
    QHBoxLayout *hb = new QHBoxLayout( vb );
    hb->setMargin( 6 );
    QLabel *l = new QLabel( tr("Character Profile:"), this );
    hb->addWidget( l );
    profileCombo = new QComboBox( this );
    connect( profileCombo, SIGNAL(activated(const QString &)),
	     this, SLOT(selectProfile(const QString &)) );
    hb->addWidget( profileCombo );
    loadProfiles();
#else
    profileList.append( profile );
#endif
    
    QTabWidget *tw = new QTabWidget( this );
    vb->addWidget( tw );

    pref = new QIMPenPrefBase( this );
    tw->addTab( pref, tr("Preferences") );

    pref->inputStyle->setExclusive( TRUE );

    style = profile->style() == QIMPenProfile::ToggleCases ? 1 : 0;
    pref->inputStyle->setButton( style );
    connect( pref->inputStyle, SIGNAL(clicked(int)),
	     this, SLOT(styleClicked(int)) );
    pref->inputStyle->setEnabled( profile->canSelectStyle() );

    multiTimeout = profile->multiStrokeTimeout();
    pref->multiStrokeSlider->setValue( multiTimeout );
    multiTimeoutChanged( multiTimeout );
    connect( pref->multiStrokeSlider, SIGNAL(valueChanged(int)),
	     this, SLOT(multiTimeoutChanged(int)) );

    edit = new QIMPenEdit( p, tw );
    tw->addTab( edit, tr("Customize") );
}

void QIMPenSetup::loadProfiles()
{
    QString path = QPEApplication::qpeDir() + "etc/qimpen";
    QDir dir( path, "*.conf" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
	QIMPenProfile *p = new QIMPenProfile( path + "/" + *it );
	profileList.append( p );
	profileCombo->insertItem( p->name() );
	if ( p->name() == profile->name() ) {
	    profileCombo->setCurrentItem( profileCombo->count()-1 );
	    profile = p;
	}
    }
}

void QIMPenSetup::styleClicked( int id )
{
    style = id;
}

void QIMPenSetup::multiTimeoutChanged( int v )
{
    multiTimeout = v;
    pref->multiStrokeLabel->setText( tr("%1 ms").arg(v) );
}

void QIMPenSetup::selectProfile( const QString &p )
{
    if ( p == profile->name() )
	return;

    profile->setStyle( style ? QIMPenProfile::ToggleCases : QIMPenProfile::BothCases );
    profile->setMultiStrokeTimeout( multiTimeout );

    for ( int i = 0; i < (int)profileList.count(); i++ ) {
	if ( profileList.at(i)->name() == p ) {
	    profile = profileList.at(i);
	    style = profile->style() == QIMPenProfile::ToggleCases ? 1 : 0;
	    pref->inputStyle->setButton( style );
	    pref->inputStyle->setEnabled( profile->canSelectStyle() );
	    multiTimeout = profile->multiStrokeTimeout();
	    pref->multiStrokeSlider->setValue( multiTimeout );
	    multiTimeoutChanged( multiTimeout );
	    edit->setProfile( profile );
	    break;
	}
    }
}

void QIMPenSetup::accept()
{
    profile->setStyle( style ? QIMPenProfile::ToggleCases : QIMPenProfile::BothCases );
    profile->setMultiStrokeTimeout( multiTimeout );
    // Save current profile
    if ( profileCombo ) {
	Config config( "handwriting" );
	config.setGroup( "Settings" );
	config.writeEntry( "Profile", profileCombo->currentText() );
    }
    // Save charsets
    bool ok = TRUE;
    for ( int i = 0; i < (int)profileList.count(); i++ ) {
	QIMPenProfile *prof = profileList.at(i);
	QIMPenCharSetIterator it(prof->charSets());
	for ( ; it.current(); ++it ) {
	    if ( !(it.current()->save( QIMPenCharSet::User )) ) {
		ok = FALSE;
		break;
	    }
	}
    }
    if ( !ok ) {
	if ( QMessageBox::critical( 0, tr( "Out of space" ),
		    tr("Unable to save information.\n"
			"Free up some space\n"
			"and try again.\n"
			"\nQuit anyway?"),
		    QMessageBox::Yes|QMessageBox::Escape,
		    QMessageBox::No|QMessageBox::Default )
		!= QMessageBox::No ) {
	    QDialog::accept();
	}
    } else {
	QDialog::accept();
    }
}

//---------------------------------------------------------------------------

QIMPenInputCharDlg::QIMPenInputCharDlg( QWidget *parent, const char *name,
	bool modal, int WFlags)
    : QDialog( parent, name, modal, WFlags )
{
    setCaption( tr("Enter new character") );
    uni = 0;

    QVBoxLayout *vb = new QVBoxLayout( this, 10 );

    QHBoxLayout *hb = new QHBoxLayout();
    vb->addLayout( hb );

    QLabel *label = new QLabel( tr("Character:"), this );
    hb->addWidget( label );

    QComboBox *cb = new QComboBox( TRUE, this );
    connect( cb, SIGNAL(activated(int)), SLOT(setSpecial(int)) );
    connect( cb, SIGNAL(textChanged(const QString &)),
	    SLOT(setCharacter(const QString &)) );
    addSpecial( cb );
    cb->setEditText( "" );
    hb->addWidget( cb );

    hb = new QHBoxLayout();
    vb->addLayout( hb );

    QPushButton *pb = new QPushButton( "OK", this );
    connect( pb, SIGNAL(clicked()), SLOT(accept()));
    hb->addWidget( pb );
    pb = new QPushButton( "Cancel", this );
    connect( pb, SIGNAL(clicked()), SLOT(reject()));
    hb->addWidget( pb );

    cb->setFocus();
}

void QIMPenInputCharDlg::addSpecial( QComboBox *cb )
{
    int i = 0;
    while ( qimpen_specialKeys[i].code != Key_unknown ) {
	cb->insertItem( qimpen_specialKeys[i].name );
	i++;
    }
}

void QIMPenInputCharDlg::setSpecial( int sp )
{
    uni = qimpen_specialKeys[sp].code << 16;
}

void QIMPenInputCharDlg::setCharacter( const QString &string )
{
    uni = string[0].unicode();
}

//---------------------------------------------------------------------------

class CharListItem : public QListBoxText
{
public:
    CharListItem( const QString &text, uint c )
	: QListBoxText( text )
    {
	_code = c;
    }

    uint code() const { return _code; }

protected:
    uint _code;
};

/*!
  \class QIMPenEdit qimpensetup.h

  Class to allow users to input totally useless character definitions
  which could match any number of the default set.
*/

QIMPenEdit::QIMPenEdit( QIMPenProfile *p, QWidget *parent,
                const char *name )
    : QWidget( parent, name ), profile(p)
{
    currentChar = 0;
    currentCode = 0;
    inputChar = new QIMPenChar();

    QVBoxLayout *tvb = new QVBoxLayout( this, 5 );

    QGridLayout *gl = new QGridLayout( tvb, 4, 2 );
    gl->setRowStretch( 1, 1 );
    gl->addRowSpacing( 2, 35 );
    gl->addRowSpacing( 3, 35 );

    charSetCombo = new QComboBox( this );
    gl->addMultiCellWidget( charSetCombo, 0, 0, 0, 1 );
    connect( charSetCombo, SIGNAL(activated(int)), SLOT(selectCharSet(int)));
    QIMPenCharSetIterator it( profile->charSets() );
    for ( ; it.current(); ++it ) {
        charSetCombo->insertItem( it.current()->description() );
    }

    charList = new QListBox( this );
    charList->setMinimumHeight( charList->sizeHint().height() );
    connect( charList, SIGNAL(highlighted(int)), SLOT(selectChar(int)) );
    gl->addWidget( charList, 1, 0 );

    pw = new QIMPenWidget( this );
    pw->setFixedHeight( 75 );
    gl->addMultiCellWidget( pw, 2, 3, 0, 0 );
    connect( pw, SIGNAL(stroke(QIMPenStroke *)),
                 SLOT(newStroke(QIMPenStroke *)) );

    QVBoxLayout *vb = new QVBoxLayout();
    gl->addLayout( vb, 1, 1 );
    newBtn = new QPushButton( tr("New..."), this );
    connect( newBtn, SIGNAL(clicked()), SLOT(addNewChar()) );
    vb->addWidget( newBtn );

    addBtn = new QPushButton( tr("Add"), this );
    connect( addBtn, SIGNAL(clicked()), SLOT(addChar()) );
    vb->addWidget( addBtn );

    removeBtn = new QPushButton( tr("Remove"), this );
    connect( removeBtn, SIGNAL(clicked()), SLOT(removeChar()) );
    vb->addWidget( removeBtn );

    QPushButton *pb = new QPushButton( tr("Default"), this );
    connect( pb, SIGNAL(clicked()), SLOT(defaultChars()) );
    vb->addWidget( pb );

    QHBoxLayout *hb = new QHBoxLayout();
    gl->addLayout( hb, 2, 1 );
    prevBtn = new QPushButton( this );
    prevBtn->setPixmap( QPixmap( (const char **)left_xpm ) );
    connect( prevBtn, SIGNAL(clicked()), SLOT(prevChar()));
    hb->addWidget( prevBtn );

    nextBtn = new QPushButton( this );
    nextBtn->setPixmap( QPixmap( (const char **)right_xpm ) );
    connect( nextBtn, SIGNAL(clicked()), SLOT(nextChar()));
    hb->addWidget( nextBtn );

    pb = new QPushButton( tr("Clear"), this );
    connect( pb, SIGNAL(clicked()), SLOT(clearChar()) );
    gl->addWidget( pb, 3, 1 );

    //--
#if !defined(Q_WS_QWS)
    hb = new QHBoxLayout( tvb );
    pb = new QPushButton( tr("OK"), this );
    connect( pb, SIGNAL(clicked()), SLOT(accept()) );
    hb->addWidget( pb );

    pb = new QPushButton( tr("Cancel"), this );
    connect( pb, SIGNAL(clicked()), SLOT(reject()) );
    hb->addWidget( pb );
#endif
    selectCharSet( 0 );
    charList->setFocus();

    resize( minimumSize() );
    enableButtons();
}

void QIMPenEdit::setProfile( QIMPenProfile *p )
{
    profile = p;
    charSetCombo->clear();
    QIMPenCharSetIterator it( profile->charSets() );
    for ( ; it.current(); ++it ) {
        charSetCombo->insertItem( it.current()->description() );
    }
    selectCharSet( 0 );
    charList->setFocus();
    enableButtons();
}

void QIMPenEdit::selectCharSet( QIMPenCharSet *c )
{
    int i = 0;
    QIMPenCharSetIterator it( profile->charSets() );
    for ( ; it.current(); ++it, i++ ) {
        if ( it.current() == c ) {
	    charSetCombo->setCurrentItem( i );
	    selectCharSet( i );
	}
    }
}


/*!
  Fill the character list box with the characters.  Duplicates are not
  inserted.
*/
void QIMPenEdit::fillCharList()
{
    charList->clear();
    QIMPenCharIterator it( currentSet->characters() );
    CharListItem *li = 0;
    for ( ; it.current(); ++it ) {
	uint ch = it.current()->character();
	QString n = it.current()->name();
	if ( !n.isEmpty() )
	    li = new CharListItem( n, ch );
	if ( li ) {
	    CharListItem *i = (CharListItem *)charList->findItem( li->text() );
	    if ( !i || i->code() != ch ) {
		charList->insertItem( li );
	    } else {
		delete li;
		li = 0;
	    }
	}
    }
    currentChar = 0;
}

void QIMPenEdit::enableButtons()
{
    bool add =  !inputChar->isEmpty();
    newBtn->setEnabled( add );
    addBtn->setEnabled( add );
    removeBtn->setEnabled( currentChar );
}

/*!
  Find the previous character with the same code as the current one.
  returns 0 if there is no previous character.
*/
QIMPenChar *QIMPenEdit::findPrev()
{
    if ( !currentChar )
	return 0;
    QIMPenCharIterator it( currentSet->characters() );
    bool found = FALSE;
    for ( it.toLast(); it.current(); --it ) {
        if ( !found && it.current() == currentChar )
            found = TRUE;
        else if ( found && it.current()->character() == currentCode &&
                !it.current()->testFlag( QIMPenChar::Deleted ) ) {
            return it.current();
        }
    }

    return 0;
}

/*!
  Find the next character with the same code as the current one.
  returns 0 if there is no next character.
*/
QIMPenChar *QIMPenEdit::findNext()
{
    if ( !currentChar )
	return 0;
    QIMPenCharIterator it( currentSet->characters() );
    bool found = FALSE;
    for ( ; it.current(); ++it ) {
	if ( !found && it.current() == currentChar )
	    found = TRUE;
	else if ( found && it.current()->character() == currentCode &&
		    !it.current()->testFlag( QIMPenChar::Deleted ) ) {
	    return it.current();
	}
    }

    return 0;
}

void QIMPenEdit::setCurrentChar( QIMPenChar *pc )
{
    currentChar = pc;
    pw->showCharacter( currentChar );
    if ( currentChar ) {
	prevBtn->setEnabled( findPrev() != 0 );
	nextBtn->setEnabled( findNext() != 0 );
    }
}

void QIMPenEdit::prevChar()
{
    QIMPenChar *pc = findPrev();
    if ( pc )
	setCurrentChar( pc );
}

void QIMPenEdit::nextChar()
{
    QIMPenChar *pc = findNext();
    if ( pc )
	setCurrentChar( pc );
}

void QIMPenEdit::clearChar()
{
    inputChar->clear();
    pw->clear();
    enableButtons();
}

void QIMPenEdit::selectChar( int i )
{
    currentChar = 0;
    currentCode = ((CharListItem *)charList->item(i))->code();
    QIMPenCharIterator it(currentSet->characters() );
    for ( ; it.current(); ++it ) {
	if ( it.current()->character() == currentCode &&
	     !it.current()->testFlag( QIMPenChar::Deleted ) ) {
	    setCurrentChar( it.current() );
	    break;
	}
    }
    if ( !it.current() )
	setCurrentChar( 0 );
    inputChar->clear();
}

void QIMPenEdit::selectCharSet( int i )
{
    if ( currentSet )
        pw->removeCharSet( 0 );
    currentSet = profile->charSets().at( i );
    fillCharList();
    pw->insertCharSet( currentSet );
    inputChar->clear();
    if ( charList->count() ) {
        charList->setSelected( 0, TRUE );
        selectChar(0);
    }
}

void QIMPenEdit::addChar()
{
    if ( !inputChar->isEmpty() ) {
        QIMPenChar *pc = new QIMPenChar( *inputChar );
        pc->setCharacter( currentCode );

	// User characters override all matching system characters.
	// Copy and mark deleted identical system characters.
	QIMPenCharIterator it(currentSet->characters() );
	QIMPenChar *sc = 0;
	while ( (sc = it.current()) != 0 ) {
	    ++it;
	    if ( sc->character() == currentCode &&
		 sc->testFlag( QIMPenChar::System ) &&
		 !sc->testFlag( QIMPenChar::Deleted ) ) {
		QIMPenChar *cc = new QIMPenChar( *sc );
		cc->clearFlag( QIMPenChar::System );
		currentSet->addChar( cc );
		sc->setFlag( QIMPenChar::Deleted );
	    }
	}

        currentSet->addChar( pc );
        setCurrentChar( pc );
        inputChar->clear();
    }
}

void QIMPenEdit::addNewChar()
{
    if ( !inputChar->isEmpty() ) {
	QIMPenInputCharDlg dlg( 0, 0, TRUE );
	if ( dlg.exec() ) {
	    currentCode = dlg.unicode();
	    addChar();
	    fillCharList();
	    for ( unsigned int i = 0; i < charList->count(); i++ ) {
		CharListItem *li = (CharListItem *)charList->item(i);
		if ( li->code() == dlg.unicode() ) {
		    charList->setSelected( i, TRUE );
		    break;
		}
	    }
	}
    }
}

void QIMPenEdit::removeChar()
{
    if ( currentChar ) {
	QIMPenChar *pc = findPrev();
	if ( !pc ) pc = findNext();
	if ( currentChar->testFlag( QIMPenChar::System ) )
	    currentChar->setFlag( QIMPenChar::Deleted );
	else
	    currentSet->removeChar( currentChar );
	setCurrentChar( pc );
    }
}

void QIMPenEdit::defaultChars()
{
    if ( currentCode ) {
	currentChar = 0;
	bool haveSystem = FALSE;
	QIMPenCharIterator it(currentSet->characters() );
	for ( ; it.current(); ++it ) {
	    if ( it.current()->character() == currentCode &&
		 it.current()->testFlag( QIMPenChar::System ) ) {
		haveSystem = TRUE;
		break;
	    }
	}
	if ( haveSystem ) {
	    it.toFirst();
	    while ( it.current() ) {
		QIMPenChar *pc = it.current();
		++it;
		if ( pc->character() == currentCode ) {
		    if ( pc->testFlag( QIMPenChar::System ) ) {
			pc->clearFlag( QIMPenChar::Deleted );
			if ( !currentChar )
			    currentChar = pc;
		    } else {
			currentSet->removeChar( pc );
		    }
		}
	    }
	    setCurrentChar( currentChar );
	}
    }
}

void QIMPenEdit::newStroke( QIMPenStroke *st )
{
    inputChar->addStroke( st );
    enableButtons();
}

