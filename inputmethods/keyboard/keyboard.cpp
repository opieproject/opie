/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include "keyboard.h"

#include <qpe/global.h>

#include <qwindowsystem_qws.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qtimer.h>
#include <ctype.h>

#include <sys/utsname.h>


#define USE_SMALL_BACKSPACE

Keyboard::Keyboard(QWidget* parent, const char* name, WFlags f) :
    QFrame(parent, name, f),  shift(FALSE), lock(FALSE), ctrl(FALSE),
    alt(FALSE), useLargeKeys(TRUE), useOptiKeys(0), pressedKey(-1),
    unicode(-1), qkeycode(0), modifiers(0)
{
    //  setPalette(QPalette(QColor(240,240,230))); // Beige!
    //  setFont( QFont( "Helvetica", 8 ) );
//    setPalette(QPalette(QColor(200,200,200))); // Gray
    setPalette(QPalette(QColor(220,220,220))); // Gray

    picks = new KeyboardPicks( this );
    picks->setFont( QFont( "smallsmooth", 9 ) );
    setFont( QFont( "smallsmooth", 9 ) );
    picks->initialise();
    QObject::connect( picks, SIGNAL(key(ushort,ushort,ushort,bool,bool) ),
            this, SIGNAL(key(ushort,ushort,ushort,bool,bool)) );

    repeatTimer = new QTimer( this );

    // temporary quick and dirty fix for the "sticky keyboard bug"
    // on ipaq.
    struct utsname name;
    if (uname(&name) != -1)
      {
	QString release=name.release;
	qWarning("System release: %s\n", name.release);
	if(release.find("embedix",0,TRUE) !=-1)
	  {
	    connect( repeatTimer, SIGNAL(timeout()), this, SLOT(repeat()) );
	  }
      }
}

void Keyboard::resizeEvent(QResizeEvent*)
{
    int ph = picks->sizeHint().height();
    picks->setGeometry( 0, 0, width(), ph );
    keyHeight = (height()-ph)/5;
    int nk;
    if ( useOptiKeys ) {
	nk = 15;
    } else if ( useLargeKeys ) {
	nk = 15;
    } else {
	nk = 19;
    }
    defaultKeyWidth = width()/nk;
    xoffs = (width()-defaultKeyWidth*nk)/2;
}

void KeyboardPicks::initialise()
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
    mode = 0;
    dc = new KeyboardConfig(this);
    configs.append(dc);
}

QSize KeyboardPicks::sizeHint() const
{
    return QSize(240,fontMetrics().lineSpacing());
}


void  KeyboardConfig::generateText(const QString &s)
{
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    for (int i=0; i<(int)backspaces; i++) {
	parent->emitKey( 0, Qt::Key_Backspace, 0, true, false );
	parent->emitKey( 0, Qt::Key_Backspace, 0, false, false );
    }
    for (int i=0; i<(int)s.length(); i++) {
	parent->emitKey( s[i].unicode(), 0, 0, true, false );
	parent->emitKey( s[i].unicode(), 0, 0, false, false );
    }
    parent->emitKey( 0, Qt::Key_Space, 0, true, false );
    parent->emitKey( 0, Qt::Key_Space, 0, false, false );
    backspaces = 0;
#endif
}


//PC keyboard layout and scancodes

/*
  Format: length, code, length, code, ..., 0
  
  length is measured in half the width of a standard key.
  If code < 0x80 we have length/2 consecutive standard keys,
  starting with scancode code.
  
  Special keys are hardcoded, one at a time, with length of key
  and code >= 0x80, these are NOT standard PC scancodes, but are looked
  up in specialM[]. (The special keys are not keymappable.)
  
 */

static const uchar * const keyboard_opti[5] = { 
    (const uchar *const) "\001\223\003\240\002\20\002\41\002\26\002\62\002\56\002\45\002\54\003\200\001\223\002\226\002\235\002\234\002\236",
    (const uchar *const) "\001\223\003\201\004\207\002\30\002\24\002\43\004\207\003\203\001\223\006\002\002\065",
    (const uchar *const) "\001\223\003\202\002\60\002\37\002\23\002\22\002\36\002\21\002\55\003\203\001\223\006\005\002\055",
    (const uchar *const) "\001\223\003\205\004\207\002\27\002\61\002\40\004\207\003\204\001\223\006\010\002\014",
    (const uchar *const) "\001\223\003\206\002\44\002\31\002\57\002\42\002\46\002\25\002\207\003\204\001\223\002\013\002\064\002\015\002\230"
};


static const uchar * const keyboard_standard[5] = {

#ifdef USE_SMALL_BACKSPACE
    (const uchar *const)"\002\240\002`\0021\0022\0023\0024\0025\0026\0027\0028\0029\0020\002-\002=\002\200\002\223\002\215\002\216\002\217",
#else
    (const uchar *const)"\002\051\0021\0022\0023\0024\0025\0026\0027\0028\0029\0020\002-\002=\004\200\002\223\002\215\002\216\002\217",
#endif
    //~ + 123...+ BACKSPACE //+ INSERT + HOME + PGUP

    (const uchar *const)"\003\201\002q\002w\002e\002r\002t\002y\002u\002i\002o\002p\002[\002]\002\\\001\224\002\223\002\221\002\220\002\222",
    //TAB + qwerty..  + backslash //+ DEL + END + PGDN

    (const uchar *const)"\004\202\002a\002s\002d\002f\002g\002h\002j\002k\002l\002;\002'\004\203",
    //CAPS + asdf.. + RETURN 

    (const uchar *const)"\005\204\002z\002x\002c\002v\002b\002n\002m\002,\002.\002/\005\204\002\223\002\223\002\211",
    //SHIFT + zxcv... //+ UP

    (const uchar *const)"\003\205\003\206\022\207\003\206\003\205\002\223\002\212\002\213\002\214" 
    //CTRL + ALT + SPACE //+ LEFT + DOWN + RIGHT
    
};


struct ShiftMap {
    char normal;
    char shifted;
};


static const ShiftMap shiftMap[] = {
    { '`', '~' },
    { '1', '!' },
    { '2', '@' },
    { '3', '#' },
    { '4', '$' },
    { '5', '%' },
    { '6', '^' },
    { '7', '&' },
    { '8', '*' },
    { '9', '(' },
    { '0', ')' },
    { '-', '_' },
    { '=', '+' },
    { '\\', '|' },
    { '[', '{' },
    { ']', '}' },
    { ';', ':' },
    { '\'', '"' },
    { ',', '<' },
    { '.', '>' },
    { '/', '?' }
};


/* XPM */
static const char * const uparrow_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
"....a....",
"...aaa...",
"..aaaaa..",
"....a....",
"....a....",
"....a....",
"....a....",
"........."};
/* XPM */
static const char * const leftarrow_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
".........",
"...a.....",
"..aa.....",
".aaaaaaa.",
"..aa.....",
"...a.....",
".........",
"........."};
/* XPM */
static const char * const downarrow_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
"....a....",
"....a....",
"....a....",
"....a....",
"..aaaaa..",
"...aaa...",
"....a....",
"........."};
/* XPM */
static const char * const rightarrow_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
".........",
".....a...",
".....aa..",
".aaaaaaa.",
".....aa..",
".....a...",
".........",
"........."};
/* XPM */
static const char * const insert_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
"a........",
"a.aaa.aaa",
"a.a.a.a..",
"a.a.a..a.",
"a.a.a...a",
"a.a.a.aaa",
".........",
"........."};
/* XPM */
static const char * const delete_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
"aa......a",
"a.a.aaa.a",
"a.a.a.a.a",
"a.a.aaa.a.",
"a.a.a...a",
"aaa.aaa.a",
".........",
"........."};
/* XPM */
static const char * const home_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
"....a....",
"...a.a...",
"..a...a..",
".a.....a.",
"aa.aaa.aa",
".a.a.a.a.",
".a.a.a.a.",
".aaaaaaa.",
"........."};
/* XPM */
static const char * const end_xpm[]={
"10 9 2 1",
"a c #000000",
". c None",
"..........",
"aa.......a",
"a..aaa.aaa",
"aa.a.a.a.a",
"a..a.a.a.a",
"a..a.a.a.a",
"aa.a.a.aaa",
"..........",
".........."};
/* XPM */
static const char * const pageup_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".aaa.aaa.",
".a.a.a.a.",
".aaa..aa.",
".a...aaa.",
".........",
".a.a.aaa.",
".a.a.a.a.",
".aaa.aaa.",
".....a..."};
/* XPM */
static const char * const pagedown_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".aaa.aaa.",
".a.a.a.a.",
".aaa..aa.",
".a...aaa.",
".........",
"...a.....",
".aaa.aaa.",
".a.a.a.a.",
".aaa.a.a."};
/* XPM */
static const char * const expand_xpm[]={
"4 9 2 1",
"a c #408040",
". c None",
"a...",
"aa..",
"aaa.",
"aaaa",
"aaaa",
"aaaa",
"aaa.",
"aa..",
"a..."};
/* XPM */
#ifdef USE_SMALL_BACKSPACE
static const char * const backspace_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
".........",
"...a.....",
"..aa.....",
".aaaaaaaa",
"..aa.....",
"...a.....",
".........",
"........."};
#else
static const char * const backspace_xpm[]={
"21 9 2 1",
"a c #000000",
". c None",
".....................",
".....................",
".....aaa..a..........",
".a...a..a.a.a.aaa.aaa",
"aaaa.aaa..aa..aa..a.a",
".a...a..a.aaa..aa.a.a",
".....aaaa.a.a.aaa.aa.",
"..................a..",
"....................."};
#endif
/* XPM */
static const char * const escape_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
".........",
".aa.aa.aa",
".a..a..a.",
".aa.aa.a.",
".a...a.a.",
".aa.aa.aa",
".........",
"........."};


enum { BSCode = 0x80, TabCode, CapsCode, RetCode, 
       ShiftCode, CtrlCode, AltCode, SpaceCode, BackSlash,
       UpCode, LeftCode, DownCode, RightCode, Blank, Expand,
       Opti, ResetDict,
       Divide, Multiply, Add, Subtract, Decimal, Equal,
       Percent, Sqrt, Inverse, Escape };

typedef struct SpecialMap {
    int qcode;
    ushort unicode;
    const char * label;
    const char * const * xpm;
};


static const SpecialMap specialM[] = {
    {	Qt::Key_Backspace,	8,	"<",     backspace_xpm },
    {	Qt::Key_Tab,		9,	"Tab",   NULL },
    {	Qt::Key_CapsLock,	0,	"Caps",  NULL },
    {	Qt::Key_Return,		13,	"Ret",   NULL },
    {	Qt::Key_Shift,		0,	"Shift", NULL },
    {	Qt::Key_Control,	0,	"Ctrl",  NULL },
    {	Qt::Key_Alt,		0,	"Alt",   NULL },
    {	Qt::Key_Space,		' ',	"",      NULL },
    {	BackSlash,		43,	"\\",    NULL },

    // Need images?
    {	Qt::Key_Up,		0,	"^",     uparrow_xpm },
    {	Qt::Key_Left,		0,	"<",     leftarrow_xpm },
    {	Qt::Key_Down,		0,	"v",     downarrow_xpm },
    {	Qt::Key_Right,		0,	">",     rightarrow_xpm },
    {	Qt::Key_Insert,		0,	"I",     insert_xpm },
    {	Qt::Key_Home,		0,	"H",     home_xpm },
    {	Qt::Key_PageUp,		0,	"U",     pageup_xpm },
    {	Qt::Key_End,		0,	"E",     end_xpm },
    {	Qt::Key_Delete,		0,	"X",     delete_xpm },
    {	Qt::Key_PageDown,	0,	"D",     pagedown_xpm },
    {	Blank,			0,	" ",     NULL },
    {	Expand,			0,	"->",    expand_xpm },
    {	Opti,			0,	"#",     NULL },
    {	ResetDict,		0,	"R",     NULL },
   
    // number pad stuff
    {	Divide,			0,	"/",     NULL },
    {	Multiply,		0,	"*",     NULL },
    {	Add,			0,	"+",     NULL },
    {	Subtract,		0,	"-",     NULL },
    {	Decimal,		0,	".",     NULL },
    {	Equal,			0,	"=",     NULL },
    {	Percent,		0,	"%",     NULL },
    {	Sqrt,			0,	"^1/2",	 NULL },
    {	Inverse,		0,	"1/x",	 NULL },

    {	Escape,			27,	"ESC",	 escape_xpm }
};


static int keycode( int i2, int j, const uchar **keyboard )
{
    if ( j <0 || j >= 5 )
	return 0;
    
    const uchar *row = keyboard[j];

    while ( *row && *row <= i2 ) {
	i2 -= *row;
	row += 2;
    }

    if ( !*row ) return 0;
    
    int k;
    if ( row[1] >= 0x80 ) {
	k = row[1];
    } else {
	k = row[1]+i2/2;
    }
    
    return k;
}


/*
  return scancode and width of first key in row \a j if \a j >= 0,
  or next key on current row if \a j < 0.
  
*/

int Keyboard::getKey( int &w, int j ) {
    static const uchar *row = 0;
    static int key_i = 0;
    static int scancode = 0;
    static int half = 0;
    
    if ( j >= 0 && j < 5 ) {
	if (useOptiKeys)
    	    row = keyboard_opti[j];
	else
    	    row = keyboard_standard[j];
	half=0;
    }

    if ( !row || !*row ) {
	return 0;    
    } else if ( row[1] >= 0x80 ) {
	scancode = row[1];
	w = (row[0] * w + (half++&1)) / 2;
	row += 2;
	return scancode;
    } else if ( key_i <= 0 ) {
	key_i = row[0]/2;
	scancode = row[1];
    }
    key_i--;
    if ( key_i <= 0 )
	row += 2;
    return scancode++;
}
    

void Keyboard::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setClipRect(e->rect());
    drawKeyboard( painter );
    picks->dc->draw( &painter );
}


/*
  Draw the keyboard.

  If key >= 0, only the specified key is drawn.
*/
void Keyboard::drawKeyboard( QPainter &p, int key )
{
    const bool threeD = FALSE;
    const QColorGroup& cg = colorGroup();
    QColor keycolor = // cg.background();
		    	QColor(240,240,230); // Beige!
    QColor keycolor_pressed = cg.mid();
    QColor keycolor_lo = cg.dark();
    QColor keycolor_hi = cg.light();
    QColor textcolor = QColor(0,0,0); // cg.text();

    int margin = threeD ? 1 : 0;
    
//    p.fillRect( 0, , kw-1, keyHeight-2, keycolor_pressed );

    for ( int j = 0; j < 5; j++ ) {
	int y = j * keyHeight + picks->height() + 1;
	int x = xoffs;
	int kw = defaultKeyWidth;
	int k= getKey( kw, j );
	while ( k ) {
	    if ( key < 0 || k == key ) {
		QString s;
		bool pressed = (k == pressedKey);
		bool blank = (k == 0223);
		const char * const * xpm = NULL;
		
		if ( k >= 0x80 ) {
		    s = specialM[k - 0x80].label;

		    xpm = specialM[k - 0x80].xpm;
			
		    if ( k == ShiftCode ) {
			pressed = shift;
		    } else if ( k == CapsCode ) {
			pressed = lock;
		    } else if ( k == CtrlCode ) {
			pressed = ctrl;
		    } else if ( k == AltCode ) {
			pressed = alt;
		    } 
		} else {
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
/*
		    s = QChar( shift^lock ? QWSServer::keyMap()[k].shift_unicode : 
			       QWSServer::keyMap()[k].unicode);
*/
		    // ### Fixme, bad code, needs improving, whole thing needs to
		    // be re-coded to get rid of the way it did things with scancodes etc
		    char shifted = k;
		    if ( !isalpha( k ) ) {
			for ( unsigned i = 0; i < sizeof(shiftMap)/sizeof(ShiftMap); i++ )
			    if ( shiftMap[i].normal == k )
				shifted = shiftMap[i].shifted;
		    } else {
			shifted = toupper( k );
		    }
		    s = QChar( shift^lock ? shifted : k );
#endif
		}

		if (!blank) {
		    if ( pressed )
			p.fillRect( x+margin, y+margin, kw-margin, keyHeight-margin-1, keycolor_pressed );
		    else
			p.fillRect( x+margin, y+margin, kw-margin, keyHeight-margin-1, keycolor );

		    if ( threeD ) {
			p.setPen(pressed ? keycolor_lo : keycolor_hi);
			p.drawLine( x, y+1, x, y+keyHeight-2 );
			p.drawLine( x+1, y+1, x+1, y+keyHeight-3 );
			p.drawLine( x+1, y+1, x+1+kw-2, y+1 );
		    } else if ( j == 0 ) {
			p.setPen(pressed ? keycolor_hi : keycolor_lo);
			p.drawLine( x, y, x+kw, y );
		    }

		    // right
		    p.setPen(pressed ? keycolor_hi : keycolor_lo);
		    p.drawLine( x+kw-1, y, x+kw-1, y+keyHeight-2 );

		    if ( threeD ) {
			p.setPen(keycolor_lo.light());
			p.drawLine( x+kw-2, y+keyHeight-2, x+kw-2, y+1 );
			p.drawLine( x+kw-2, y+keyHeight-2, x+1, y+keyHeight-2 );
		    }

		    if (xpm) {
			p.drawPixmap( x + 1, y + 2, QPixmap((const char**)xpm) );
		    } else {
    			p.setPen(textcolor);
			p.drawText( x - 1, y, kw, keyHeight-2, AlignCenter, s );
		    }
	    
		    if ( threeD ) {
			p.setPen(keycolor_hi);
			p.drawLine( x, y, x+kw-1, y );
		    }

		    // bottom
		    p.setPen(keycolor_lo);
		    p.drawLine( x, y+keyHeight-1, x+kw-1, y+keyHeight-1 );
	    
		} else {
		    p.fillRect( x, y, kw, keyHeight, cg.background() );
		}
	    }

	    x += kw;
	    kw = defaultKeyWidth;
	    k = getKey( kw );
	}
    }
}


void Keyboard::mousePressEvent(QMouseEvent *e)
{
    clearHighlight(); // typing fast?

    int i2 = ((e->x() - xoffs) * 2) / defaultKeyWidth;
    int j = (e->y() - picks->height()) / keyHeight;

    int k = keycode( i2, j, (const uchar **)((useOptiKeys) ? keyboard_opti : keyboard_standard) );
    bool need_repaint = FALSE;
    unicode = -1;
    qkeycode = 0;
    if ( k >= 0x80 ) {
	if ( k == ShiftCode ) {
	    shift = !shift;
	    need_repaint = TRUE;
	} else if ( k == AltCode ){
	    alt = !alt;
	    need_repaint = TRUE;
	} else if ( k == CapsCode ) {
	    lock = !lock;
	    need_repaint = TRUE;
	} else if ( k == CtrlCode ) {
	    ctrl = !ctrl;
	    need_repaint = TRUE;
	} else if ( k == 0224 /* Expand */ ) {
	    useLargeKeys = !useLargeKeys;
	    resizeEvent(0);
	    repaint( TRUE ); // need it to clear first
	} else if ( k == 0225 /* Opti/Toggle */ ) {
	    useOptiKeys = !useOptiKeys;
	    resizeEvent(0);
	    repaint( TRUE ); // need it to clear first
	} else {
	    qkeycode = specialM[ k - 0x80 ].qcode;
	    unicode = specialM[ k - 0x80 ].unicode;
	}
    } else {
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
/*
	qk = QWSServer::keyMap()[k].key_code;
	if ( qk != Key_unknown ) {
		if ( ctrl )
		    u = QWSServer::keyMap()[k].ctrl_unicode;
		else if ( shift^lock )
		    u = QWSServer::keyMap()[k].shift_unicode;
		else 
		    u = QWSServer::keyMap()[k].unicode;
	}
*/
	char shifted = k;
	if ( !isalpha( k ) ) {
	    // ### Fixme, bad code, needs improving, whole thing needs to
	    // be re-coded to get rid of the way it did things with scancodes etc
	    for ( unsigned i = 0; i < sizeof(shiftMap)/sizeof(ShiftMap); i++ )
		if ( shiftMap[i].normal == k )
		    shifted = shiftMap[i].shifted;
	} else {
	    shifted = toupper( k );
	}
	QChar tempChar( shift^lock ? shifted : k );
	unicode = tempChar.unicode();
#endif
    }
    if  ( unicode != -1 ) {
	modifiers = (shift ? Qt::ShiftButton : 0) | (ctrl ? Qt::ControlButton : 0) |
		  (alt ? Qt::AltButton : 0);
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
	emit key( unicode, qkeycode, modifiers, true, false );
	repeatTimer->start( 500 );
#endif
	need_repaint = shift || alt || ctrl;
	shift = alt = ctrl = FALSE;
	//qDebug( "pressed %d -> %04x ('%c')", k, u, u&0xffff < 256 ? u&0xff : 0 );

	KeyboardConfig *dc = picks->dc;

	if (dc) {
	    if (qkeycode == Qt::Key_Backspace) {
		dc->input.remove(dc->input.last()); // remove last input
		dc->decBackspaces();
	    } else if ( k == 0226 || qkeycode == Qt::Key_Return ||
		        qkeycode == Qt::Key_Space ||
			QChar(unicode).isPunct() ) {
		dc->input.clear();
		dc->resetBackspaces();
	    } else {
		dc->add(QString(QChar(unicode)));
		dc->incBackspaces();
	    }
	}

	picks->repaint();

    }
    pressedKey = k;
    if ( need_repaint ) {
	repaint( FALSE );
    } else {
	QPainter p(this);
	drawKeyboard( p, pressedKey );
    }
    pressTid = startTimer(80);
    pressed = TRUE;
}


void Keyboard::mouseReleaseEvent(QMouseEvent*)
{
    if ( pressTid == 0 )
	clearHighlight();
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    if ( unicode != -1 ) {
	emit key( unicode, qkeycode, modifiers, false, false );
	repeatTimer->stop();
    }
#endif
    pressed = FALSE;
}

void Keyboard::timerEvent(QTimerEvent* e)
{
    if ( e->timerId() == pressTid ) {
	killTimer(pressTid);
	pressTid = 0;
	if ( !pressed )
	    clearHighlight();
    }
}

void Keyboard::repeat()
{
  
  repeatTimer->start( 200 );
  emit key( unicode, qkeycode, modifiers, true, true );
}

void Keyboard::clearHighlight()
{
    if ( pressedKey >= 0 ) {
	int tmp = pressedKey;
	pressedKey = -1;
	QPainter p(this);
	drawKeyboard( p, tmp );
    }
}


QSize Keyboard::sizeHint() const
{
    QFontMetrics fm=fontMetrics();
    int keyHeight = fm.lineSpacing()+2;

    if (useOptiKeys)
    	keyHeight += 1;
    
    return QSize( 320, keyHeight * 5 + picks->sizeHint().height() + 1 );
}


void Keyboard::resetState()
{
    picks->resetState();
}
