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
#include "qimpencombining.h"
#include "qimpenmatch.h"
#include "qimpenhelp.h"

#include <qpe/qpeapplication.h>
#include <qpe/global.h>
#include <qpe/config.h>
#include <qpe/stringutil.h>

#include <qtextview.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qtextstream.h>

/* XPM */
static const char * const left_xpm[] = {
"16 16 2 1",
"       c None",
".      c #000000",
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
"       c None",
".      c #000000",
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

HandwritingHelp::HandwritingHelp( QIMPenProfile *p, QWidget *parent, const char *name, WFlags f )
    : QTabWidget( parent, name, f )
{
    setCaption( tr("Handwriting Help") );
    QTextView *help = new QTextView( this );
    help->setFrameStyle( QFrame::NoFrame );
    help->setText(
	tr( "<ul><li>When you start to use the handwriting recogniser "
	    "write slowly, accurately and firmly."
	    "<li>Use the guide lines when drawing your characters."
	    "<li>When drawing a character with multiple strokes, each "
	    "successive stroke must be drawn before the grayed strokes are erased."
	    "<li>Practice your handwriting using the handwriting trainer."
	    "<li>When adding your own character templates make sure they "
	    "are sufficiently different from other characters' templates."
	    "</ul>") );

    addTab( help, tr("Tips") );

    HandwritingTrainer *trainer = new HandwritingTrainer( p, this );
    addTab( trainer, tr("Trainer") );
}

void HandwritingHelp::showEvent( QShowEvent * )
{
    Global::hideInputMethod();
}

void HandwritingHelp::hideEvent( QHideEvent * )
{
    Global::showInputMethod();
}

//---------------------------------------------------------------------------

HandwritingTrainer::HandwritingTrainer( QIMPenProfile *p, QWidget *parent, const char *name )
    : QWidget( parent, name ), profile(p)
{
    QGridLayout *gl = new QGridLayout( this, 5, 2, 0, 4 );
    gl->setRowStretch( 1, 1 );
    gl->setRowStretch( 2, 1 );
    gl->setColStretch( 1, 1 );

    charSetCombo = new QComboBox( this );
    gl->addMultiCellWidget( charSetCombo, 0, 0, 0, 1 );
    connect( charSetCombo, SIGNAL(activated(int)), SLOT(selectCharSet(int)));
    QIMPenCharSetIterator it( profile->charSets() );
    for ( ; it.current(); ++it ) {
	charSetCombo->insertItem( it.current()->description() );
    }

    charList = new QListBox( this );
    charList->setHScrollBarMode( QListBox::AlwaysOff );
    charList->setFixedWidth( 80 );
    connect( charList, SIGNAL(highlighted(int)), this, SLOT(selectChar(int)) );
    gl->addMultiCellWidget( charList, 1, 2, 0, 0 );

    QLabel *help = new QLabel( this );
    help->setAlignment( AlignLeft | AlignVCenter | WordBreak );
    gl->addWidget( help, 1, 1 );
    help->setText(
	tr( "Select a character from the list.  The writing area on the left "
	    "shows the reference character.  Practice writing in the area on "
	    "the right.") );

    result = new QLabel( this );
    gl->addMultiCellWidget( result, 2, 3, 1, 1 );

    matcher = new QIMPenMatch( this );
    matcher->setCharSet( currentSet );
    connect( matcher, SIGNAL(noMatch()), this, SLOT(noMatch()) );
    connect( matcher, SIGNAL(matchedCharacters(const QIMPenCharMatchList &)),
	     this, SLOT(matched(const QIMPenCharMatchList &)) );

    QHBoxLayout *hb = new QHBoxLayout();
    gl->addLayout( hb, 3, 0 );
    prevBtn = new QPushButton( this );
    prevBtn->setPixmap( QPixmap( (const char **)left_xpm ) );
    connect( prevBtn, SIGNAL(clicked()), SLOT(prevChar()));
    hb->addWidget( prevBtn );

    nextBtn = new QPushButton( this );
    nextBtn->setPixmap( QPixmap( (const char **)right_xpm ) );
    connect( nextBtn, SIGNAL(clicked()), SLOT(nextChar()));
    hb->addWidget( nextBtn );

    refPw = new QIMPenWidget( this );
    refPw->setReadOnly( TRUE );
    gl->addWidget( refPw, 4, 0 );

    pracPw = new QIMPenWidget( this );
    connect( matcher, SIGNAL(removeStroke()), pracPw, SLOT(removeStroke()) );
    connect( pracPw, SIGNAL(beginStroke()),
	     this, SLOT(beginStroke()) );
    connect( pracPw, SIGNAL(stroke( QIMPenStroke * )),
	     this, SLOT(strokeEntered( QIMPenStroke * )) );
    connect( pracPw, SIGNAL(beginStroke()),
	     matcher, SLOT(beginStroke()) );
    connect( pracPw, SIGNAL(stroke( QIMPenStroke * )),
	     matcher, SLOT(strokeEntered( QIMPenStroke * )) );
    gl->addWidget( pracPw, 4, 1 );

    redrawTimer = new QTimer( this );
    connect( redrawTimer, SIGNAL(timeout()), this, SLOT(redrawChar()) );
    redrawTimer->start( 5000 );

    currentSet = 0;
    charSetCombo->setCurrentItem( 1 );
    selectCharSet( 1 );
}

HandwritingTrainer::~HandwritingTrainer()
{
}

void HandwritingTrainer::showEvent( QShowEvent * )
{
    redrawChar();
    redrawTimer->start( 5000 );
}

void HandwritingTrainer::setCurrentChar( QIMPenChar *c )
{
    currentChar = c;
    refPw->showCharacter( currentChar );
    pracPw->clear();
    if ( currentChar ) {
	prevBtn->setEnabled( findPrev() != 0 );
	nextBtn->setEnabled( findNext() != 0 );
    }
    result->setText( "" );
    redrawTimer->start( 5000 );
}

void HandwritingTrainer::selectChar( int i )
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
}

void HandwritingTrainer::selectCharSet( int i )
{
    if ( currentSet ) {
	refPw->removeCharSet( 0 );
	pracPw->removeCharSet( 0 );
    }
    currentSet = profile->charSets().at( i );
    fillCharList();
    refPw->insertCharSet( currentSet );
    pracPw->insertCharSet( currentSet );
    matcher->setCharSet( currentSet );
    if ( charList->count() ) {
	charList->setSelected( 0, TRUE );
	selectChar(0);
    }
}

void HandwritingTrainer::noMatch()
{
    result->setText( "No match" );
}

void HandwritingTrainer::matched( const QIMPenCharMatchList &ml )
{
    int maxErr = 20000 + (*ml.begin()).penChar->strokeLength(0) * 1000;
    int baseErr = (*ml.begin()).penChar->strokeLength(0) * 250;
    unsigned int numStrokes = (*ml.begin()).penChar->strokeCount();
    QIMPenCharMatchList::ConstIterator it;
    /*
    for ( it = ml.begin(); it != ml.end(); ++it ) {
	if ( (*it).penChar->strokeCount() == numStrokes ) {
	    if ( (*it).error > maxErr )
		maxErr = (*it).error;
	}
    }
    */
    int i;
    QString res;
    QTextStream ts(&res, IO_WriteOnly);
    ts << "<qt>" << tr("Matched: ");
    for ( i = 0, it = ml.begin(); it != ml.end() && i < 4; ++it, i++ ) {
	if ( (*it).penChar->strokeCount() == numStrokes ) {
	    int rate = 100 - ( ((*it).error - baseErr) * 100 ) / maxErr;
	    if ( it != ml.begin() ) {
		if ( rate < -10 )
		    continue;
		ts << "<br>";
		ts << tr("Similar to: ");
	    }
	    ts << "<big>";
	    if ( (*it).penChar->character() == currentChar->character() )
		ts << "<b>";
	    ts << Qtopia::escapeString((*it).penChar->name());
	    ts << " (" << rateString(rate) << ")";
	    if ( (*it).penChar->character() == currentChar->character() )
		ts << "</b>";
	    ts << "</big>";
	}
    }
    ts << "</qt>";
    result->setText( res );
}

QString HandwritingTrainer::rateString( int rate ) const
{
    if ( rate < 1 )
	rate = 1;
    if ( rate > 100 )
	rate = 100;
    return tr("%1%").arg(rate);
}

void HandwritingTrainer::prevChar()
{
    QIMPenChar *pc = findPrev();
    if ( pc )
	setCurrentChar( pc );
}

void HandwritingTrainer::nextChar()
{
    QIMPenChar *pc = findNext();
    if ( pc )
	setCurrentChar( pc );
}

void HandwritingTrainer::redrawChar()
{
    if ( currentChar )
	refPw->showCharacter( currentChar );
}

void HandwritingTrainer::beginStroke()
{
    redrawTimer->start( 5000 );
}

void HandwritingTrainer::strokeEntered( QIMPenStroke * )
{
    pracPw->greyStroke();
}

QIMPenChar *HandwritingTrainer::findPrev()
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

QIMPenChar *HandwritingTrainer::findNext()
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

void HandwritingTrainer::fillCharList()
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

