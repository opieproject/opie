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
#include "pickboardpicks.h"
#include "pickboardcfg.h"

#include <qpe/global.h>

#include <qpainter.h>
#include <qlist.h>
#include <qbitmap.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qdialog.h>
#include <qscrollview.h>
#include <qpopupmenu.h>
#include <qhbuttongroup.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#ifdef QWS
#include <qwindowsystem_qws.h>
#endif

void PickboardPicks::doMenu()
{
    QWidget* cause = (QWidget*)sender(); // evil

    QPopupMenu popup(this);
    config()->fillMenu(popup);

    QPoint pos = cause->mapToGlobal(cause->rect().topRight());
    QSize sz = popup.sizeHint();
    pos.ry() -= sz.height();
    pos.rx() -= sz.width();
    popup.move(pos);
    config()->doMenu(popup.exec());
}

static const char *BS_xpm[] = {
"5 7 2 1",
"a c #000000",
". c None",
"...aa",
"..aaa",
".aaaa",
"aaaaa",
".aaaa",
"..aaa",
"...aa",
};
static const char *Del_xpm[] = {
"14 7 2 1",
"a c #000000",
". c None",
"aaa..aaaa.a...",
"a..a.a....a...",
"a..a.a....a...",
"a..a.aaa..a...",
"a..a.a....a...",
"a..a.a....a...",
"aaa..aaaa.aaaa"
};
static const char *Home_xpm[] = {
"20 7 2 1",
"a c #000000",
". c None",
"a..a..aa..a...a.aaaa",
"a..a.a..a.aa.aa.a...",
"a..a.a..a.a.a.a.a...",
"aaaa.a..a.a.a.a.aaa.",
"a..a.a..a.a...a.a...",
"a..a.a..a.a...a.a...",
"a..a..aa..a...a.aaaa"
};
static const char *PgUp_xpm[] = {
"20 7 2 1",
"a c #000000",
". c None",
"aaa.......a..a......",
"a..a......a..a......",
"a..a.aa...a..a.aaa..",
"aaa.a.....a..a.a..a.",
"a...a.aa..a..a.aaa..",
"a...a..a..a..a.a....",
"a....aaa...aa..a...."
};
static const char *PgDn_xpm[] = {
"20 7 2 1",
"a c #000000",
". c None",
"aaa.......aaa.......",
"a..a......a..a......",
"a..a.aa...a..a.a..a.",
"aaa.a.....a..a.aa.a.",
"a...a.aa..a..a.a.aa.",
"a...a..a..a..a.a..a.",
"a....aaa..aaa..a..a."
};
static const char *End_xpm[] = {
"14 7 2 1",
"a c #000000",
". c None",
"aaaa.a..a.aaa.",
"a....aa.a.a..a",
"a....a.aa.a..a",
"aaa..a..a.a..a",
"a....a..a.a..a",
"a....a..a.a..a",
"aaaa.a..a.aaa."
};
static const char *Enter_xpm[] = {
"14 7 2 1",
"a c #000000",
". c None",
".............a",
".............a",
"..a..........a",
".aa.........a.",
"aaaaaaaaaaaa..",
".aa...........",
"..a..........."
};
static const char *Esc_xpm[] = {
"14 7 2 1",
"a c #000000",
". c None",
"aaaa..aa...aa.",
"a....a..a.a..a",
"a....a....a...",
"aaa...aa..a...",
"a.......a.a...",
"a....a..a.a..a",
"aaaa..aa...aa."
};
static const char *Ins_xpm[] = {
"13 7 2 1",
"a c #000000",
". c None",
"aaa.a..a..aa.",
".a..aa.a.a..a",
".a..a.aa.a...",
".a..a..a..aa.",
".a..a..a....a",
".a..a..a.a..a",
"aaa.a..a..aa."
};
static const char *Up_xpm[] = {
"7 7 2 1",
"a c #000000",
". c None",
"...a...",
"..aaa..",
".a.a.a.",
"a..a..a",
"...a...",
"...a...",
"...a..."
};
static const char *Left_xpm[] = {
"7 7 2 1",
"a c #000000",
". c None",
"...a...",
"..a....",
".a.....",
"aaaaaaa",
".a.....",
"..a....",
"...a..."
};
static const char *Down_xpm[] = {
"7 7 2 1",
"a c #000000",
". c None",
"...a...",
"...a...",
"...a...",
"a..a..a",
".a.a.a.",
"..aaa..",
"...a..."
};
static const char *Right_xpm[] = {
"7 7 2 1",
"a c #000000",
". c None",
"...a...",
"....a..",
".....a.",
"aaaaaaa",
".....a.",
"....a..",
"...a..."
};
static const char *BackTab_xpm[] = {
"8 7 2 1",
"a c #000000",
". c None",
"a.......",
"a..a....",
"a.aa....",
"aaaaaaaa",
"a.aa....",
"a..a....",
"a......."
};
static const char *Tab_xpm[] = {
"8 7 2 1",
"a c #000000",
". c None",
".......a",
"....a..a",
"....aa.a",
"aaaaaaaa",
"....aa.a",
"....a..a",
".......a"
};
static const char *Space_xpm[] = {
"9 9 2 1",
"a c #000000",
". c None",
"aaaaaaaaa",
"a.......a",
"a.......a",
"a.......a",
"a.......a",
"a.......a",
"a.......a",
"a.......a",
"aaaaaaaaa"
};

PickboardPicks::PickboardPicks(QWidget* parent, const char* name, WFlags f ) :
    QFrame(parent,name,f)
{
}

void PickboardPicks::initialise(void)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
    mode = 0;

    DictFilterConfig* dc = new DictFilterConfig(this);
    dc->addSet("ABC");
    dc->addSet("DEF");
    dc->addSet("GHI");
    dc->addSet("JKL");
    dc->addSet("MNO");
    dc->addSet("PQR");
    dc->addSet("STU");
    dc->addSet("VWX");
    dc->addSet("YZ-'");
    dc->addMode("123");
    dc->addMode("@*!?");
    dc->addMode("KEY");
    dc->addMode("Space");
    dc->addMode("Back");
    dc->addMode("Enter");
    dc->addMode("Shift");
    configs.append(dc);

    CharStringConfig* number = new CharStringConfig(this);
    number->addChar("0");
    number->addChar("1");
    number->addChar("2");
    number->addChar("3");
    number->addChar("4");
    number->addChar("5");
    number->addChar("6");
    number->addChar("7");
    number->addChar("8");
    number->addChar("9");
    number->addChar("."); // #### or "," in some locales
    configs.append(number);

    CharConfig* punc = new CharConfig(this);

    punc->addChar(0,"\"");
    punc->addChar(0,"`");
    punc->addChar(0,"'");
    punc->addChar(0,"\253");
    punc->addChar(0,"\273");
    punc->addChar(0,"\277");
    punc->addChar(1,"(");
    punc->addChar(1,")");
    punc->addChar(1,"[");
    punc->addChar(1,"]");
    punc->addChar(1,"{");
    punc->addChar(1,"}");

    punc->addChar(0,"+");
    punc->addChar(0,"-");
    punc->addChar(0,"*");
    punc->addChar(0,"/");
    punc->addChar(0,"=");
    punc->addChar(0,"_");
    punc->addChar(0,"$");
    punc->addChar(0,"&");
    punc->addChar(1,"|");
    punc->addChar(1,"@");
    punc->addChar(1,"\\");
    punc->addChar(1,"#");
    punc->addChar(1,"^");
    punc->addChar(1,"~");
    punc->addChar(1,"<");
    punc->addChar(1,">");

    punc->addChar(0,".");
    punc->addChar(0,"?");
    punc->addChar(0,"!");
    punc->addChar(0,",");
    punc->addChar(0,";");
    punc->addChar(1,":");
    punc->addChar(1,"\267");
    punc->addChar(1,"\277");
    punc->addChar(1,"\241");
    punc->addChar(1,"\367");

    punc->addChar(0,"$");
    punc->addChar(0,"\242");
    punc->addChar(0,"\245");
    punc->addChar(1,"\243");
    punc->addChar(1,"\244");
    punc->addChar(1,"\260");

    configs.append(punc);

    KeycodeConfig* keys = new KeycodeConfig(this);
    keys->addKey(0,QPixmap(Esc_xpm),Key_Escape);
    keys->addKey(0,QPixmap(BS_xpm),Key_Backspace);
    keys->addGap(0,10);

    keys->addKey(0,QPixmap(Ins_xpm),Key_Insert);
    keys->addKey(0,QPixmap(Home_xpm),Key_Home);
    keys->addKey(0,QPixmap(PgUp_xpm),Key_PageUp);

    keys->addGap(0,25);
    keys->addKey(0,QPixmap(Up_xpm),Key_Up);
    keys->addGap(0,15);

    keys->addKey(1,QPixmap(BackTab_xpm),Key_Tab);
    keys->addGap(1,3);
    keys->addKey(1,QPixmap(Tab_xpm),Key_Tab);
    keys->addGap(1,10);

    keys->addKey(1,QPixmap(Del_xpm),Key_Delete);
    keys->addGap(1,2);
    keys->addKey(1,QPixmap(End_xpm),Key_End);
    keys->addGap(1,3);
    keys->addKey(1,QPixmap(PgDn_xpm),Key_PageDown);

    keys->addGap(1,10);
    keys->addKey(1,QPixmap(Left_xpm),Key_Left);
    keys->addKey(1,QPixmap(Down_xpm),Key_Down);
    keys->addKey(1,QPixmap(Right_xpm),Key_Right);

    keys->addGap(1,13);
    keys->addKey(1,QPixmap(Space_xpm),Key_Space);

    keys->addGap(0,10);
    keys->addKey(0,QPixmap(Enter_xpm),Key_Return);

    configs.append(keys);
}

PickboardPicks::~PickboardPicks()
{
}

QSize PickboardPicks::sizeHint() const
{
    return QSize(240,fontMetrics().lineSpacing()*2+3);
}

void PickboardPicks::drawContents(QPainter* p)
{
    config()->draw(p);
}

void PickboardPicks::mousePressEvent(QMouseEvent* e)
{
    config()->pickPoint(e->pos(),TRUE);
}

void PickboardPicks::mouseDoubleClickEvent(QMouseEvent* e)
{
    config()->pickPoint(e->pos(),TRUE);
}

void PickboardPicks::mouseReleaseEvent(QMouseEvent* e)
{
    config()->pickPoint(e->pos(),FALSE);
}

void PickboardPicks::setMode(int m)
{
    mode = m;
}

void PickboardPicks::resetState()
{
    config()->doMenu(100);
}
