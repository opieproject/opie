/**************************************************************************************94x78**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*********************************************************************************************/
#include "keyboard.h"

#include <qpe/resource.h>

//#include <iostream.h>


static const int autorepeatDelaytime = 500; // ms
static const int autorepeatRate      = 20;  // chars per second

static const int mod1x1    =   0;
static const int mod1x2    =  23;
static const int mod1w     =  mod1x2 - mod1x1;

static const int letterx1  =  27;
static const int letterx2  = 129;
static const int letterw   =  17;
static const int letterh   =  14;

static const int num1x1    = 130;
static const int num1x2    = 137;
static const int num1w     = num1x2 - num1x1;

static const int specialx1 = 138;
static const int specialx2 = 170;
static const int specialw  =  16;

static const int num2x1    = 171;
static const int num2x2    = 178;
static const int num2w     = num2x2 - num2x1;

static const int mod2x1    = 179;
static const int mod2x2    = 203;
static const int mod2w     = mod2x2 - mod2x1;

static const int cursorx1  = 207;
static const int cursorw   =  16;

static const int myParenID = -10;


typedef struct mapElement
{
    int    qcode;
    ushort unicode;
};

static const mapElement mod1Map[] = {
    { Qt::Key_Escape, 27 },
    { Qt::Key_Tab,     9 },
    { Qt::Key_Return, 13 },
    { Qt::Key_Alt,     0 },
    { Qt::Key_Control, 0 },
};

static const uchar *const letterMap[] = {
    (const uchar *const)"zvchwk",
    (const uchar *const)"fitaly",
    (const uchar *const)"  ne  ",
    (const uchar *const)"gdorsb",
    (const uchar *const)"qjumpx",
};

static const ushort kletterMap[][6] = {
	{ 0x110c, 0x1112, 0x1109, 0x116d, 0x1167, 0x1163 },
	{ 0x110f, 0x1105, 0x1100, 0x1161, 0x1175, 0x1162 },
	{ ' ',    ' ',    0x110b, 0x1165, ' ',    ' '    },
	{ 0x1110, 0x1103, 0x1102, 0x1169, 0x1173, 0x1166 },
	{ 0x110e, 0x1107, 0x1106, 0x1111, 0x116e, 0x1172 },
};

static const uchar *const letterMapShift[] = {
    (const uchar *const)"ZVCHWK",
    (const uchar *const)"FITALY",
    (const uchar *const)"  NE  ",
    (const uchar *const)"GDORSB",
    (const uchar *const)"QJUMPX",
};

static const ushort kletterMapShift[][6] = {
	{ 0x110d, 0x1112, 0x110a, 0x116d, 0x1167, 0x1163 },
	{ 0x110f, 0x1105, 0x1101, 0x1161, 0x1175, 0x1164 },
	{ ' ',    ' ',    0x110b, 0x1165, ' ',    ' '    },
	{ 0x1110, 0x1104, 0x1102, 0x1169, 0x1173, 0x1168 },
	{ 0x110e, 0x1108, 0x1106, 0x1111, 0x116e, 0x1172 },
};

static const uchar *const num1Map = (const uchar *const)"12345";

static const uchar *const specialMap[] = {
    (const uchar *const)"-+",
    (const uchar *const)"*!",
    (const uchar *const)",'",
    (const uchar *const)".%",
    (const uchar *const)"/$",
};

static const uchar *const specialMapShift[] = {
    (const uchar *const)"_=",
    (const uchar *const)"#?",
    (const uchar *const)";\"",
    (const uchar *const)":|",
    (const uchar *const)"\\&",
};

static const uchar *const specialMapParen[] = {
    (const uchar *const)"()",
    (const uchar *const)"[]",
    (const uchar *const)"{}",
    (const uchar *const)"<>",
    (const uchar *const)"@~",
};

static const uchar *const num2Map = (const uchar *const)"67890";

static const mapElement mod2Map[] = {
    { Qt::Key_Backspace,  8 },
    { Qt::Key_Delete,     0 },
    { Qt::Key_Return,    13 },
    { Qt::Key_Shift,      0 },
    { myParenID,          0 },
};

static const int cursorMap[][2] = {
    { Qt::Key_Home, Qt::Key_PageUp   },
    { Qt::Key_End,  Qt::Key_PageDown },
    { Qt::Key_Up,   Qt::Key_Up       },
    { Qt::Key_Left, Qt::Key_Right    },
    { Qt::Key_Down, Qt::Key_Down     },
};

using namespace KJumpX;

Keyboard::Keyboard(QWidget* parent, const char* name, WFlags f) :
    QFrame(parent, name, f),
    shift(0), paren(0), ctrl(0), alt(0), lang(1), lastKey(0),
    pressedKeyUnicode(0), pressedKeyQcode(0), pressedMod(0),
    isnoncont(false),
    slideKeyUnicodeH(0), slideKeyQcodeH(0), slideKeyUnicodeV(0), slideKeyQcodeV(0),
    enableMouseTracking(false), slidePix(NULL), slidePixH(NULL), slidePixV(NULL),
    releasedPix(NULL), pressedPix(NULL)
{
    //setPalette(QPalette(QColor(240,240,230))); // Beige!

    releasedPlain = releasedShift = releasedParen = Resource::loadPixmap("kjumpx/released");
    pressedPlain  = pressedShift  = pressedParen  = Resource::loadPixmap("kjumpx/pressed");
    pressedDigit = Resource::loadPixmap("kjumpx/pressed");

    QPixmap tmp;

    tmp = Resource::loadPixmap("kjumpx/releasedShift");
    bitBlt(&releasedShift, letterx1, 0, &tmp);

    tmp = Resource::loadPixmap("kjumpx/releasedParen");
    bitBlt(&releasedParen, specialx1, 0, &tmp);

    tmp = Resource::loadPixmap("kjumpx/pressedShift");
    bitBlt(&pressedShift, letterx1, 0, &tmp);

    tmp = Resource::loadPixmap("kjumpx/pressedParen");
    bitBlt(&pressedParen, specialx1, 0, &tmp);

    tmp = Resource::loadPixmap("kjumpx/pressedDigit");
    bitBlt(&pressedDigit, specialx1, 0, &tmp);

    offscreen = QPixmap( releasedPlain );

    releasedPix = &releasedPlain;
    pressedPix  = &pressedPlain;
    slidePix    = &pressedPlain;

    delayTimer = new QTimer(this);
    rateTimer  = new QTimer(this);
    connect( delayTimer, SIGNAL( timeout() ), this, SLOT( delayTimerDone() ) );
    connect( rateTimer,  SIGNAL( timeout() ), this, SLOT( rateTimerDone() ) );
}

void Keyboard::resizeEvent(QResizeEvent*)
{
    //cout << "resizeEvent()" << endl;
}

void Keyboard::paintEvent(QPaintEvent*)
{
    bitBlt(this, 0, 0, &offscreen);
}

void Keyboard::mousePressEvent(QMouseEvent *e)
{
    pressedx = -1;
    pressedKeyUnicode = pressedKeyQcode = pressedMod = 0;

    int x = e->x();
    int y = e->y();

    int row = (y - 1) / letterh;

    if ( x <= mod1x2 ) // mod1
    {
        pressedx = mod1x1;
        pressedy = row * letterh;
        pressedw = mod1w + 1;
        pressedh = letterh + 1;
        if ( row == 2 ) // return
        {
            pressed2x = mod2x1;
            pressed2y = 2 * letterh;
            pressed2w = mod2w + 1;
            pressed2h = letterh + 1;
            isnoncont = true;
        }
        else if ( row == 3 ) // alt
            alt = 1;
        else if ( row == 4 ) // ctrl
            ctrl = 1;
        pressedKeyUnicode = mod1Map[row].unicode;
        pressedKeyQcode   = mod1Map[row].qcode;
    }
    else if ( x >= letterx1 && x <= letterx2 ) // letter
    {
        int column = (x - letterx1 - 1) / letterw;
        QChar temp;
		if (lang == 0) // english
        	if ( shift )
            	temp = QChar( letterMapShift[row][column] );
        	else 
            	temp = QChar( letterMap[row][column] );
		else if (lang == 1) // korean
        	if ( shift )
            	temp = parseKoreanInput( kletterMapShift[row][column] );
        	else 
            	temp = parseKoreanInput( kletterMap[row][column] );

        if ( temp == ' ' ) // space
        {
            if ( column < 3 )
            {
                pressedx  = letterx1;
                pressed2x = letterx1 + letterw * 4;
            }
            else
            {
                pressedx  = letterx1 + letterw * 4;
                pressed2x = letterx1;
            }
            pressedy = pressed2y = row * letterh;
            pressedw = pressed2w = letterw * 2 + 1;
            pressedh = pressed2h = letterh + 1;
            isnoncont = true;
        }
        else
        {
            pressedx = letterx1 + column * letterw;
            pressedy = row * letterh;
            pressedw = letterw + 1;
            pressedh = letterh + 1;
        }
        pressedKeyUnicode = temp.unicode();
        pressedKeyQcode = slideKeyQcodeH = slideKeyQcodeV = temp.upper().unicode();
        if ( temp == ' ' )
        {
            slideKeyUnicodeH = slideKeyUnicodeV = 8;
            slideKeyQcodeH   = slideKeyQcodeV   = Qt::Key_Backspace;
        }
        else if ( temp == temp.lower() )
        {
            slideKeyUnicodeH = slideKeyUnicodeV = temp.upper().unicode();
            slidePixH = slidePixV = &pressedShift;
        }
        else
        {
            slideKeyUnicodeH = slideKeyUnicodeV = temp.lower().unicode();
            slidePixH = slidePixV = &pressedPlain;
        }
        enableMouseTracking = true;
    }
    else if ( x >= num1x1 && x <= num1x2 ) // num1
    {
        pressedx = num1x1;
        pressedy = row * letterh;
        pressedw = num1w + 1;
        pressedh = letterh + 1;
        QChar temp = QChar( num1Map[row] );
        pressedKeyUnicode = pressedKeyQcode = temp.unicode();
    }
    else if ( x >= specialx1 && x <= specialx2 ) // special
    {
        int column = (x - specialx1 - 1) / specialw;
        pressedx = specialx1 + column * specialw;
        pressedy = row * letterh;
        pressedw = specialw + 1;
        pressedh = letterh + 1;
        QChar temp;
        if ( shift )
            temp = QChar( specialMapShift[row][column] );
        else if ( paren )
            temp = QChar( specialMapParen[row][column] );
        else
            temp = QChar( specialMap[row][column] );
        pressedKeyUnicode = pressedKeyQcode = temp.unicode();
        slideKeyUnicodeH = slideKeyQcodeH = slideKeyUnicodeV = slideKeyQcodeV =
            QChar('0').unicode() + ( 5 * column + row + 1 ) % 10;
        slidePixH = slidePixV = &pressedDigit;
        if ( shift )
        {
            slideKeyUnicodeV = slideKeyQcodeV =
                QChar( specialMap[row][column] ).unicode();
            slidePixV = &pressedPlain;
        }
        else if ( !(shift || paren) )
        {
            slideKeyUnicodeV = slideKeyQcodeV =
                QChar( specialMapShift[row][column] ).unicode();
            slidePixV = &pressedShift;
        }
        enableMouseTracking = true;
    }
    else if ( x >= num2x1 && x <= num2x2 ) // num2
    {
        pressedx = num2x1;
        pressedy = row * letterh;
        pressedw = num2w + 1;
        pressedh = letterh + 1;
        QChar temp = QChar( num2Map[row] );
        pressedKeyUnicode = pressedKeyQcode = temp.unicode();
    }
    else if ( x >= mod2x1 && x <= mod2x2 ) // mod2
    {
        pressedx = mod2x1;
        pressedy = row * letterh;
        pressedw = mod2w + 1;
        pressedh = letterh + 1;
        if ( row == 2 ) // return
        {
            pressed2x = mod1x1;
            pressed2y = 2 * letterh;
            pressed2w = mod2w + 1;
            pressed2h = letterh + 1;
            isnoncont = true;
        }
        pressedKeyUnicode = mod2Map[row].unicode;
        pressedKeyQcode   = mod2Map[row].qcode;

        if ( row == 3 ) // shift
        {
            paren = 0;
            switch ( shift )
            {
            case 0:
            {
                shift = 1;
                releasedPix = &releasedShift;
                pressedPix = &pressedShift;
                bitBlt( &offscreen, 0, 0, releasedPix );
                break;
            }
            case 1:
            {
                shift = 2;
                break;
            }
            case 2:
            {
                shift = 0;
                releasedPix = &releasedPlain;
                pressedPix = &pressedPlain;
                bitBlt( &offscreen, 0, 0, releasedPix );
                break;
            }
            }
        }
        else if ( row == 4 ) // parenthesis
        {
            shift = 0;
            switch ( paren )
            {
            case 0:
            {
                paren = 1;
                releasedPix = &releasedParen;
                pressedPix = &pressedParen;
                bitBlt( &offscreen, 0, 0, releasedPix );
                break;
            }
            case 1:
            {
                paren = 2;
                break;
            }
            case 2:
            {
                paren = 0;
                releasedPix = &releasedPlain;
                pressedPix = &pressedPlain;
                bitBlt( &offscreen, 0, 0, releasedPix );
                break;
            }
            }
        }
    }
    else if ( x >= cursorx1 ) // cursor
    {
        int column = (x - cursorx1 - 1) / cursorw;
        if ( row == 2 || row == 4 )
            pressedx = cursorx1 + cursorw / 2;
        else
            pressedx = cursorx1 + column * cursorw;
        pressedy = row * letterh;
        pressedw = cursorw + 1;
        pressedh = letterh + 1;
        pressedKeyQcode = cursorMap[row][column];
    }

    pressedMod = ( shift ? Qt::ShiftButton : 0 ) |
                 ( ctrl ? Qt::ControlButton : 0 ) |
                 ( alt ? Qt::AltButton : 0 );

	lastKey = pressedKeyUnicode;

    emit key( pressedKeyUnicode, pressedKeyQcode, pressedMod, true, false );
    delayTimer->start( autorepeatDelaytime, true );

    if ( pressedx == -1 )
        return;

    bitBlt( &offscreen, pressedx, pressedy,
            pressedPix, pressedx, pressedy, pressedw, pressedh );
    if ( isnoncont )
        bitBlt( &offscreen, pressed2x, pressed2y,
                pressedPix, pressed2x, pressed2y, pressed2w, pressed2h );

    repaint( false );
}

void Keyboard::mouseReleaseEvent(QMouseEvent*)
{
    //cout << pressedx << " " << pressedy << " " << pressedw << " " << pressedh << endl;

    delayTimer->stop();
    rateTimer->stop();
    enableMouseTracking = false;

    if ( pressedx == -1 )
        return;

    if ( shift == 2 && pressedKeyQcode == Qt::Key_Shift )
        return;
    if ( paren == 2 && pressedKeyQcode == myParenID )
        return;

    if ( shift == 1 && pressedKeyQcode != Qt::Key_Shift )
    {
        shift = 0;
        releasedPix = &releasedPlain;
        pressedPix = &pressedPlain;
        bitBlt( &offscreen, 0, 0, releasedPix );
    }

    if ( paren == 1 && pressedKeyQcode != myParenID )
    {
        paren = 0;
        releasedPix = &releasedPlain;
        pressedPix = &pressedPlain;
        bitBlt( &offscreen, 0, 0, releasedPix );
    }

    if ( alt && pressedKeyQcode != Qt::Key_Alt )
        alt = 0;
    if ( ctrl && pressedKeyQcode != Qt::Key_Control )
        ctrl = 0;

    bitBlt( &offscreen, pressedx, pressedy,
            releasedPix, pressedx, pressedy, pressedw, pressedh );

    if ( isnoncont )
    {
        isnoncont = false;
        bitBlt( &offscreen, pressed2x, pressed2y,
                releasedPix, pressed2x, pressed2y, pressed2w, pressed2h );
    }

    repaint( false );
}

void Keyboard::mouseMoveEvent(QMouseEvent *e)
{
    if ( !enableMouseTracking )
        return;

    if ( e->x() < pressedx || e->x() >= pressedx + pressedw )
    {
        pressedKeyUnicode = slideKeyUnicodeH;
        pressedKeyQcode = slideKeyQcodeH;
        slidePix = slidePixH;
    }
    else if ( e->y() < pressedy || e->y() >= pressedy + pressedh )
    {
        pressedKeyUnicode = slideKeyUnicodeV;
        pressedKeyQcode = slideKeyQcodeV;
        slidePix = slidePixV;
    }
    else
        return;

    enableMouseTracking = false;

    delayTimer->stop();
    rateTimer->stop();

    bitBlt( &offscreen, pressedx, pressedy,
            slidePix, pressedx, pressedy, pressedw, pressedh );

    emit key( 8, Qt::Key_Backspace, pressedMod, true, false );
    emit key( pressedKeyUnicode, pressedKeyQcode, pressedMod, true, false );
    delayTimer->start( autorepeatDelaytime, true );

    repaint( false );
}

void Keyboard::delayTimerDone()
{
    emit key( pressedKeyUnicode, pressedKeyQcode, pressedMod, true, true );
    rateTimer->start( 1000/autorepeatRate, false );
}

void Keyboard::rateTimerDone()
{
    emit key( pressedKeyUnicode, pressedKeyQcode, pressedMod, true, true );
}

QSize Keyboard::sizeHint() const
{
    return offscreen.size();
}

void Keyboard::resetState()
{
    //cout << "resetState()" << endl;
}

/*
 *
 * TODO
 * one major problem with this implementation is that you can't move the
 * cursor after inputing korean chars, otherwise it will eat up and replace
 * the char before the cursor you move to. fix that
 *
 * make a kor/eng swaping key
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 * how korean input works
 *
 * all following chars means unicode char value and are in hex
 *
 * 초음 = schar (start char)
 * 중음 = mchar (middle char)
 * 끝음 = echar (end char)
 *
 * there are 19 schars. unicode position is at 1100 - 1112
 * there are 21 mchars. unicode position is at 1161 - 1175
 * there are 27 echars. unicode position is at 11a8 - 11c2
 *
 * the map with everything combined is at ac00 - d7a3
 *
 * to find a combination of schar + mchar in the map, lookup 
 * ((schar - 0x1100) * 587) + ((mchar - 0x1161) * 27) + (echar - 0x11a8) + 0xac00)
 *
 */

QChar Keyboard::parseKoreanInput (ushort c) {

	static ushort schar, mchar, echar;

	if ((lastKey < 0x1100 || 0x11c2 < lastKey) && (lastKey < 0xac00 || 0xd7a3 < lastKey)
					&& !(lastKey == 0 && (shift || paren || ctrl || alt))) {

		//printf ("reset...\n");
		schar = 0, mchar = 0, echar = 0;
	}

	//printf ("in %x %x %x %x %x\n", schar, mchar, echar, c, lastKey);
	if ( 0x1100 <= c && c <= 0x1112 ) { // schar or echar was input

		if (schar == 0 || (schar != 0 && mchar == 0)) {
			schar = c; mchar = 0; echar = 0;
			return QChar(c);
		}
		else if (mchar != 0) {

			if (echar == 0) {

				if (!(echar = constoe(c))) {

					schar = c; mchar = 0; echar = 0; 
					return QChar(c);
				}

			}
			else { // must figure out what the echar is

				if (echar == 0x11a8) { // ㄱ

					if (c == 0x1100) 		echar = 0x11a9; // ㄱ + ㄱ
					else if (c == 0x1109) 	echar = 0x11aa; // ㄱ + ㅅ
					else { 
						schar = c; mchar = 0; echar = 0; 
						return QChar(c);
					}

				} else if (echar == 0x11ab) { // ㄴ

					if (c == 0x110c)  		echar = 0x11ac; // ㄴ + ㅈ
					else if (c == 0x1112)  	echar = 0x11ad; // ㄴ + ㅎ
					else {
						schar = c; mchar = 0; echar = 0; 
						return QChar(c);
					}

				} else if (echar == 0x11af) { // ㄹ

					if (c == 0x1100) 		echar = 0x11b0; // ㄹ + ㄱ
					else if (c == 0x1106) 	echar = 0x11b1; // ㄹ + ㅁ
					else if (c == 0x1107) 	echar = 0x11b2; // ㄹ + ㅂ
					else if (c == 0x1109) 	echar = 0x11b3; // ㄹ + ㅅ
					else if (c == 0x1110) 	echar = 0x11b4; // ㄹ + ㅌ
					else if (c == 0x1111) 	echar = 0x11b5; // ㄹ + ㅍ
					else if (c == 0x1112) 	echar = 0x11b6; // ㄹ + ㅎ
					else {
						schar = c; mchar = 0; echar = 0; 
						return QChar(c);
					}

				} else if (echar == 0x11b8) { // ㅂ

					if (c == 0x1109) 		echar = 0x11b9; // ㅂ + ㅅ
					else {
						schar = c; mchar = 0; echar = 0; 
						return QChar(c);
					}

				} else if (echar == 0x11ba) { // ㅅ 

					if (c == 0x1109) 		echar = 0x11bb; // ㅅ + ㅅ 
					else {
						schar = c; mchar = 0; echar = 0; 
						return QChar(c);
					}

				} else { // if any other char, cannot combine chars

					schar = c; mchar = 0; echar = 0;
					return QChar(c);
				}

				lastKey  = echar;
			}
		}

	}
	else if (0x1161 <= c && c <= 0x1175) { // mchar was input

		if (schar != 0 && mchar == 0) { mchar = c; }

		else if (schar != 0 && mchar != 0 && echar == 0) {

			switch (mchar) {
				case 0x1169:
					if (c == 0x1161) mchar = 0x116a;
					else if (c == 0x1162) mchar = 0x116b;
					else if (c == 0x1175) mchar = 0x116c;
					else {
						schar = 0; mchar = 0; echar = 0;
						return QChar(c);
					}
					break;
				case 0x116e:
					if (c == 0x1165) mchar = 0x116f;
					else if (c == 0x1166) mchar = 0x1170;
					else if (c == 0x1175) mchar = 0x1171;
					else {
						schar = 0; mchar = 0; echar = 0;
						return QChar(c);
					}
					break;
				case 0x1173:
					if (c == 0x1175) mchar = 0x1174;
					else {
						schar = 0; mchar = 0; echar = 0;
						return QChar(c);
					}
					break;
				default: 
					schar = 0; mchar = 0; echar = 0;
					return QChar(c);
			}
		}
		else if (schar != 0 && mchar != 0 && echar != 0) {

			emit key( 8, Qt::Key_Backspace, 0, true, false );

			ushort prev = 0;
			switch (echar) {
				/*
				case 0x11a9:
					prev = combineKoreanChars(schar, mchar, 0x11a8);
					schar = 0x1100; 
					break;
				*/
				case 0x11aa:
					prev = combineKoreanChars(schar, mchar, 0x11a8);
					schar = 0x1109;
					break;
				case 0x11ac:
					prev = combineKoreanChars(schar, mchar, 0x11ab);
					schar = 0x110c; 
					break;
				case 0x11ad:
					prev = combineKoreanChars(schar, mchar, 0x11ab);
					schar = 0x1112;
					break;
				case 0x11b0:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1100; 
					break;
				case 0x11b1:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1106;
					break;
				case 0x11b2:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1107; 
					break;
				case 0x11b3:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1109;
					break;
				case 0x11b4:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1110;
					break;
				case 0x11b9:
					prev = combineKoreanChars(schar, mchar, 0x11b8);
					schar = 0x1109;
					break;
				/*
				case 0x11bb:
					prev = combineKoreanChars(schar, mchar, 0x11ba);
					schar = 0x1109; 
					break;
				*/
				default: 

					if (constoe(echar)) {

						prev = combineKoreanChars(schar, mchar, 0);
						schar = constoe(echar); 
					} 
					break;
			}
			
			emit key( prev, prev, 0, true, false );

			mchar = c; echar = 0;

			return QChar(combineKoreanChars(schar, mchar, 0));
		
		} 
		else {
			schar = 0; mchar = 0; echar = 0;
			return QChar(c);
		}

	} 
	else if (c == ' ') return QChar(c);


	// and now... finally delete previous char, and return new char
	emit key( 8, Qt::Key_Backspace, 0, true, false );

	//printf ("out %x %x %x %x\n", schar, mchar, echar, c);


	return QChar (combineKoreanChars( schar, mchar, echar));

}

ushort Keyboard::combineKoreanChars(const ushort s, const ushort m, const ushort e) {

	return ((s - 0x1100) * 588) + ((m - 0x1161) * 28) + (e ? e - 0x11a7 : 0) + 0xac00;

}

ushort Keyboard::constoe(const ushort c) {

	// converts schars to echars if possible

	if (0x1100 <= c && c <= 0x1112) { // schar to echar

		switch (c) {
			case 0x1100: return 0x11a8;
			case 0x1101: return 0x11a9;
			case 0x1102: return 0x11ab;
			case 0x1103: return 0x11ae;
			case 0x1105: return 0x11af;
			case 0x1106: return 0x11b7;
			case 0x1107: return 0x11b8;
			case 0x1109: return 0x11ba;
			case 0x110a: return 0x11bb;
			case 0x110b: return 0x11bc;
			case 0x110c: return 0x11bd;
			case 0x110e: return 0x11be;
			case 0x110f: return 0x11bf;
			case 0x1110: return 0x11c0;
			case 0x1111: return 0x11c1;
			case 0x1112: return 0x11c2;
			default: return 0;

		}

	} else { //echar to schar

		switch (c) {
			case 0x11a8: return 0x1100;
			case 0x11a9: return 0x1101;
			case 0x11ab: return 0x1102;
			case 0x11ae: return 0x1103;
			case 0x11af: return 0x1105;
			case 0x11b7: return 0x1106;
			case 0x11b8: return 0x1107;
			case 0x11ba: return 0x1109;
			case 0x11bb: return 0x110a;
			case 0x11bc: return 0x110b;
			case 0x11bd: return 0x110c;
			case 0x11be: return 0x110e;
			case 0x11bf: return 0x110f;
			case 0x11c0: return 0x1110;
			case 0x11c1: return 0x1111;
			case 0x11c2: return 0x1112;
			default: return 0;

		}

	}
}
