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

static const uchar *const letterMapShift[] = {
    (const uchar *const)"ZVCHWK",
    (const uchar *const)"FITALY",
    (const uchar *const)"  NE  ",
    (const uchar *const)"GDORSB",
    (const uchar *const)"QJUMPX",
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

using namespace JumpX;

Keyboard::Keyboard(QWidget* parent, const char* name, WFlags f) :
    QFrame(parent, name, f),
    shift(0), paren(0), ctrl(0), alt(0),
    pressedKeyUnicode(0), pressedKeyQcode(0), pressedMod(0),
    isnoncont(false),
    slideKeyUnicodeH(0), slideKeyQcodeH(0), slideKeyUnicodeV(0), slideKeyQcodeV(0),
    enableMouseTracking(false), slidePix(NULL), slidePixH(NULL), slidePixV(NULL),
    releasedPix(NULL), pressedPix(NULL)
{
    //setPalette(QPalette(QColor(240,240,230))); // Beige!

    releasedPlain = releasedShift = releasedParen = Resource::loadPixmap("jumpx/released");
    pressedPlain  = pressedShift  = pressedParen  = Resource::loadPixmap("jumpx/pressed");
    pressedDigit = Resource::loadPixmap("jumpx/pressed");

    QPixmap tmp;

    tmp = Resource::loadPixmap("jumpx/releasedShift");
    bitBlt(&releasedShift, letterx1, 0, &tmp);

    tmp = Resource::loadPixmap("jumpx/releasedParen");
    bitBlt(&releasedParen, specialx1, 0, &tmp);

    tmp = Resource::loadPixmap("jumpx/pressedShift");
    bitBlt(&pressedShift, letterx1, 0, &tmp);

    tmp = Resource::loadPixmap("jumpx/pressedParen");
    bitBlt(&pressedParen, specialx1, 0, &tmp);

    tmp = Resource::loadPixmap("jumpx/pressedDigit");
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
        if ( shift )
            temp = QChar( letterMapShift[row][column] );
        else
            temp = QChar( letterMap[row][column] );
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
