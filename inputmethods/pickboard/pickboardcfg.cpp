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

#include "pickboardcfg.h"
#include "pickboardpicks.h"

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
#include <qwindowsystem_qws.h>

static const char * pickboard_help =
    "<h1>The Pickboard</h1>"
    "<i>The smallest and fastest way to type.</i>"
    "<p>"
    "Enter a word by tapping letter-groups and picking the word."
    "<br>Enter spaces with \"Space\", or other keys through \"KEY\"."
    "<br>Use \"Shift\" to capitalize words that are not normally capitalized."
    "<br>Press \"Shift\" twice for an all-capitals word."
    "<br>Add custom words by picking them, then selecting \"Add...\" from the menu on the right."
    ;

const int intermatchmargin=5;


PickboardConfig::~PickboardConfig() { }

void PickboardConfig::updateRows(int from, int to)
{
    if ( from != to ) { // (all)
	parent->update();
    } else {
	QFontMetrics fm = parent->fontMetrics();
	parent->update(QRect(0,1+fm.descent() + from * fm.lineSpacing(), parent->width(),
	    fm.lineSpacing()));
    }
}

void PickboardConfig::updateItem(int r, int)
{
    updateRows(r,r);
}

void PickboardConfig::changeMode(int m)
{
    parent->setMode(m);
}
void PickboardConfig::generateText(const QString& s)
{
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    for (int i=0; i<(int)s.length(); i++) {
	parent->emitKey(s[i].unicode(), 0, 0, true, false);
	parent->emitKey(s[i].unicode(), 0, 0, false, false);
    }
#endif   
}
void PickboardConfig::generateKey( int k )
{
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    parent->emitKey(0, k, 0, true, false);
    parent->emitKey(0, k, 0, false, false);
#endif
}

void PickboardConfig::pickPoint(const QPoint& p, bool press)
{
    if ( press ) {
	int ls=parent->height()/nrows;
	int y=0;
	pressx = -1;
	for (int r=0; r<nrows; r++) {
	    if ( p.y() >= y && p.y() < y+ls ) {
		pressrow = r;
		pressx = p.x();
		pickInRow( pressrow, pressx, TRUE );
		return;
	    }
	    y += ls;
	}
    } else if ( pressx >= 0 ) {
	pickInRow( pressrow, pressx, FALSE );
	pressx = -1;
    }
}

void PickboardConfig::fillMenu(QPopupMenu& menu)
{
    menu.insertItem("Reset",100);
    menu.insertSeparator();
    menu.insertItem("Help",1);
}

void PickboardConfig::doMenu(int i)
{
    switch (i) {
	case 100:
	    if ( parent->currentMode() ) {
		changeMode(0);
		updateRows(0,1);
	    }
	    break;
	case 1: {
	    QMessageBox help("Pickboard Help", pickboard_help,
		QMessageBox::NoIcon, 1, 0, 0);
	    help.showMaximized();
	    help.exec();
	}
    }
}

void StringConfig::draw(QPainter* p)
{
    QFontMetrics fm = p->fontMetrics();

    for (int r=0; r<nrows; r++) {
	p->translate(0,fm.lineSpacing());
	p->setPen(rowColor(r));

	int tw=0;
	QString s;
	int i=0;
	for (; !(s=text(r,i)).isNull(); ++i) {
	    int w = fm.width(s);
	    tw += w;
	}
	bool spread = spreadRow(r);// && parent->width() > tw;
	int xw = spread ? (parent->width()-tw)/(i-1) : 3;
	int x = spread ? (parent->width()-tw-xw*(i-1))/2 : 2;

	i=0;
	for (; !(s=text(r,i)).isNull(); ++i) {
	    int w = fm.width(s)+xw;
	    if ( highlight(r,i) ) {
		p->fillRect(x-xw/2,1+fm.descent()-fm.lineSpacing(),w,fm.lineSpacing(),Qt::black);
		p->setPen(Qt::white);
	    }else{
		p->setPen(Qt::black);
	    }
	    p->drawText(x,-fm.descent()-1,s);
	    x += w;
	}
    }
}

void StringConfig::pickInRow(int r, int xpos, bool press)
{
    QFontMetrics fm = parent->fontMetrics();

    int tw=0;
    QString s;
    int i=0;
    for (; !(s=text(r,i)).isNull(); ++i) {
	int w = fm.width(s);
	tw += w;
    }
    bool spread = spreadRow(r) && parent->width() > tw;
    int xw = spread ? (parent->width()-tw)/(i-1) : 3;
    int x = spread ? (parent->width()-tw-xw*(i-1))/2 : 2;

    i=0;
    for (; !(s=text(r,i)).isNull(); ++i) {
	int x2 = x + fm.width(s)+xw;
	if ( xpos >= x && xpos < x2 ) {
	    pick(press, r, i);
	    return;
	}
	x = x2;
    }
}

void StringConfig::updateItem(int r, int item)
{
    QFontMetrics fm = parent->fontMetrics();

    int y = r * fm.lineSpacing();

    int tw=0;
    QString s;
    int i=0;
    for (; !(s=text(r,i)).isNull(); ++i) {
	int w = fm.width(s);
	tw += w;
    }
    bool spread = spreadRow(r) && parent->width() > tw;
    int xw = spread ? (parent->width()-tw)/(i-1) : 3;
    int x = spread ? (parent->width()-tw-xw*(i-1))/2 : 2;

    i=0;
    for (; !(s=text(r,i)).isNull(); ++i) {
	int w = fm.width(s)+xw;
	if ( i == item ) {
	    parent->update(QRect(x-xw/2,y+1+fm.descent(),w,fm.lineSpacing()));
	    return;
	}
	x += w;
    }
}

bool StringConfig::highlight(int,int) const
{
    return FALSE;
}

LetterButton::LetterButton(const QChar& letter, QWidget* parent) :
	QPushButton(letter,parent)
{
    setToggleButton(TRUE);
    setAutoDefault(FALSE);
    connect(this,SIGNAL(clicked()),this,SLOT(toggleCase()));
    skip=TRUE;
}

void LetterButton::toggleCase()
{
    if ( skip ) {
	// Don't toggle case the first time
	skip=FALSE;
	return;
    }

    QChar ch = text()[0];
    QChar nch = ch.lower();
    if ( ch == nch )
	nch = ch.upper();
    setText(nch);
}

LetterChoice::LetterChoice(QWidget* parent, const QString& set) :
    QButtonGroup(parent)
{
    QHBoxLayout *l = new QHBoxLayout(this);
    setFrameStyle(0);
    setExclusive(TRUE);
    for (int i=0; i<(int)set.length(); i++) {
	LetterButton* b = new LetterButton(set[i],this);
	l->addWidget(b,1,AlignCenter);
	connect(b,SIGNAL(clicked()),this,SLOT(change()));
    }
}

void LetterChoice::change()
{
    LetterButton* b = (LetterButton*)sender();
    ch = b->text()[0];
    emit changed();
}


PickboardAdd::PickboardAdd(QWidget* owner, const QStringList& setlist) :
    QDialog( owner, 0, TRUE )
{
    QVBoxLayout* l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);

    QScrollView *sv = new QScrollView(this);
    sv->setResizePolicy(QScrollView::AutoOneFit);
    setMaximumHeight(200); // ### QDialog shouldn't allow us to be bigger than the screen
    QVBox *letters = new QVBox(sv);
    letters->setSpacing(0);
    lc = new LetterChoice*[setlist.count()];
    nlc = (int)setlist.count();
    for (int i=0; i<nlc; i++) {
	lc[i] = new LetterChoice(letters,setlist[i]);
	connect(lc[i],SIGNAL(changed()),this,SLOT(checkAllDone()));
    }
    sv->addChild(letters);
    QHBox* hb = new QHBox(this);
    hb->setSpacing(0);
    yes = new QPushButton("OK",hb);
    yes->setEnabled(FALSE);
    QPushButton *no = new QPushButton("Cancel",hb);
    connect(yes, SIGNAL(clicked()), this, SLOT(accept()));
    connect(no, SIGNAL(clicked()), this, SLOT(reject()));
}

PickboardAdd::~PickboardAdd()
{
    delete [] lc;
}

QString PickboardAdd::word() const
{
    QString str;
    for (int i=0; i<nlc; i++) {
	str += lc[i]->choice();
    }
    return str;
}

bool PickboardAdd::exec()
{
    QPoint pos = parentWidget()->mapToGlobal(QPoint(0,0));
    pos.ry() -= height();
    if ( QDialog::exec() ) {
	Global::addWords(QStringList(word()));
	return TRUE;
    } else {
	return FALSE;
    }
}

void PickboardAdd::checkAllDone()
{
    if ( !yes->isEnabled() ) {
	for (int i=0; i<nlc; i++) {
	    if ( lc[i]->choice().isNull() )
		return;
	}
	yes->setEnabled(TRUE);
    }
}


void DictFilterConfig::doMenu(int i)
{
    switch (i) {
      case 300:
	if ( input.count() == 0 ) {
	    QMessageBox::information(0, "Adding Words",
		"To add words, pick the letters,\nthen "
		"open the Add dialog. In that\ndialog, tap "
		"the correct letters\nfrom the list "
		"(tap twice for\ncapitals).");
	} else {
	    PickboardAdd add(parent,capitalize(input));
	    if ( add.exec() )
		generateText(add.word());
	    input.clear();
	    matches.clear();
	    updateRows(0,0);
	}
	break;
      case 100:
	if ( !input.isEmpty() ) {
	    input.clear();
	    matches.clear();
	    StringConfig::doMenu(i);
	    updateRows(0,1);
	    break;
	} // else fall through
      default:
	StringConfig::doMenu(i);
    }
    shift = 0;
    lit0 = -1;
}

QString DictFilterConfig::text(int r, int i)
{
    QStringList l = r ? sets : input.isEmpty() ? othermodes : matches;
    return i < (int)l.count() ?
	(input.isEmpty() ? l[i] : capitalize(l[i]))
	: QString::null;
}

bool DictFilterConfig::spreadRow(int r)
{
    return r ? TRUE : input.isEmpty() ? TRUE : FALSE;
}

QStringList DictFilterConfig::capitalize(const QStringList& l)
{
    switch ( shift ) {
      case 1: {
	QStringList r;
	QStringList::ConstIterator it = l.begin();
	r.append((*it).upper());
	for (++it; it != l.end(); ++it)
	    r.append(*it);
	return r;
      } case 2: {
	QStringList r;
	for (QStringList::ConstIterator it = l.begin(); it != l.end(); ++it)
	    r.append((*it).upper());
	return r;
      }
    }
    return l;
}

QString DictFilterConfig::capitalize(const QString& s)
{
    switch ( shift ) {
	case 1: {
	    QString u = s;
	    u[0] = u[0].upper();
	    return u;
	    break;
	} case 2:
	    return s.upper();
	    break;
    }
    return s;
}

void DictFilterConfig::pick(bool press, int row, int item)
{
    if ( row == 0 ) {
	if ( press ) {
	    if ( input.isEmpty() ) {
		lit0 = item;
		if ( othermodes[item] == "Space" ) {
		    updateItem(row,item);
		    generateText(" ");
		} else if ( othermodes[item] == "Back" ) {
		    updateItem(row,item);
		    generateKey(Qt::Key_Backspace);
		} else if ( othermodes[item] == "Enter" ) {
		    updateItem(row,item);
		    generateKey(Qt::Key_Return);
		} else if ( othermodes[item] == "Shift" ) {
		    updateItem(row,item);
		    shift = (shift+1)%3;
		}
	    }
	} else {
	    if ( !input.isEmpty() ) {
		input.clear();
		if ( item>=0 ) {
		    generateText(capitalize(matches[item]));
		}
		shift = 0;
		matches.clear();
		updateRows(0,0);
	    } else if ( item < 3 ) {
		lit0 = -1;
		changeMode(item+1); // I'm mode 0! ####
		updateRows(0,1);
	    }
	    if ( lit0 >= 0 ) {
		if ( !shift || othermodes[lit0] != "Shift" ) {
		    updateItem(0,lit0); 
		    lit0 = -1;
		}
	    }
	}
    } else {
	lit0 = -1;
	if ( press && item >= 0 ) {
	    lit1 = item;
	    add(sets[item]);
	    updateItem(1,item); 
	    updateRows(0,0);
	} else {
	    updateItem(1,lit1); 
	    lit1 = -1;
	}
    }
}

bool DictFilterConfig::scanMatch(const QString& set, const QChar& l) const
{
    return set == "?" || set == "*" || set.contains(l);
}

//static int visit=0;
//static int lvisit=0;

void DictFilterConfig::scan(const QDawg::Node* n, int ipos, const QString& str, int length, bool extend)
{
    if ( n ) {
	do {
//visit++;
	    bool pastend = ipos >= (int)input.count();
	    if ( pastend && extend || !pastend && scanMatch(input[ipos],n->letter().lower()) ) {
		if ( length>1 ) {
		    if ( !pastend && input[ipos] == "*" ) {
			scan(n->jump(),ipos+1,str+n->letter(),length-1,FALSE);
			scan(n->jump(),ipos,str+n->letter(),length,FALSE);
		    } else {
			scan(n->jump(),ipos+1,str+n->letter(),length-1,extend);
		    }
		} else {
		    if ( n->isWord() ) {
			matches.append(str+n->letter());
		    }
		}
	    }
	    n = n->next();
	} while (n);
    }
}

void DictFilterConfig::scanLengths(const QDawg::Node* n, int ipos, int& length_bitarray)
{
    if ( n ) {
	do {
//lvisit++;
	    bool pastend = ipos >= (int)input.count();
	    if ( pastend || scanMatch(input[ipos],n->letter().lower()) ) {
		scanLengths(n->jump(),ipos+1,length_bitarray);
		if ( n->isWord() )
		    length_bitarray |= (1<<(ipos+1));
	    }
	    n = n->next();
	} while (n);
    }
}

void DictFilterConfig::add(const QString& set)
{
    QFontMetrics fm = parent->fontMetrics();
    input.append(set.lower());
    matches.clear();
//visit=0;
//lvisit=0;
    int length_bitarray = 0;
    if ( input.count() > 4 ) {
	scanLengths(Global::addedDawg().root(),0,length_bitarray);
	scanLengths(Global::fixedDawg().root(),0,length_bitarray);
    } else {
	length_bitarray = 0xffffffff;
    }
    for (int len=input.count(); len<22 /* 32 */; ++len) {
	if ( length_bitarray & (1<<len) ) {
	    scan(Global::addedDawg().root(),0,"",len,TRUE);
	    scan(Global::fixedDawg().root(),0,"",len,TRUE);
	    int x = 2;
	    for (QStringList::Iterator it=matches.begin(); it!=matches.end(); ++it) {
		x += fm.width(*it)+intermatchmargin;
		if ( x >= parent->width() ) {
//qDebug("%d+%d visits",lvisit,visit);
		    return; // RETURN - No point finding more
		}
	    }
	}
	if ( len == 1 && input.count() == 1 ) {
	    // Allow all single-characters to show as "matches"
	    for ( int i=0; i<(int)set.length(); i++ ) {
		QChar ch = set[i].lower();
		matches.append(ch);
	    }
	}
    }
//qDebug("%d+%d visits",lvisit,visit);
}

bool DictFilterConfig::highlight(int r,int c) const
{
    return r == 0 ? c == lit0 : c == lit1;
}


void DictFilterConfig::addSet(const QString& s)
{
    sets.append(s);
}

void DictFilterConfig::addMode(const QString& s)
{
    othermodes.append(s);
}

void DictFilterConfig::fillMenu(QPopupMenu& menu)
{
    menu.insertItem("Add...",300);
    StringConfig::fillMenu(menu);
}

QValueList<QPixmap> KeycodeConfig::row(int i)
{
    return i ? keypm2 : keypm1;
}

void KeycodeConfig::pickInRow(int r, int xpos, bool press)
{
    QValueList<QPixmap> pl = row(r);
    QValueList<QPixmap>::Iterator it;
    int item=0;
    int x=xmarg;
    for (it=pl.begin(); it!=pl.end(); ++it) {
	int x2 = x + (*it).width();
	if ( (*it).height() > 1 )
	    x2 += xw;
	if ( xpos >= x && xpos < x2 ) {
	    pick(press, r, item);
	    return;
	}
	x = x2;
	item++;
    }
}

void KeycodeConfig::pick(bool press, int row, int item)
{
    if ( !press ) {
	if ( item >= 0 ) {
	    int k = row == 0 ? keys1[item] : keys2[item];
	    if ( k )
		generateKey(k);
	}
	changeMode(0);
	updateRows(0,1);
    }
}

void KeycodeConfig::draw(QPainter* p)
{
    int y=3;
    QValueList<QPixmap>::Iterator it;
    for (int r=0; r<nrows; r++) {
	QValueList<QPixmap> pl = row(r);
	int x = xmarg;
	for (it=pl.begin(); it!=pl.end(); ++it) {
	    if ( (*it).height() == 1 ) {
		// just a gap
		x += (*it).width();
	    } else {
		p->drawPixmap(x,y,*it);
		x += (*it).width()+xw;
	    }
	}
	y += parent->height()/nrows;
    }
}


void KeycodeConfig::addKey(int r, const QPixmap& pm, int code)
{
    if ( r == 0 ) {
	keypm1.append(pm);
	keys1.append(code);
    } else {
	keypm2.append(pm);
	keys2.append(code);
    }
}
void KeycodeConfig::addGap(int r, int w)
{
    QBitmap pm(w,1); // ick.
    addKey(r,pm,0);
}

QString CharConfig::text(int r, int i)
{
    QStringList l = r ? chars2 : chars1;
    return i < (int)l.count() ? l[i] : QString::null;
}
bool CharConfig::spreadRow(int)
{
    return TRUE;
}

void CharConfig::pick(bool press, int row, int item)
{
    if ( !press ) {
	if ( item >= 0 ) {
	    generateText(row == 0 ? chars1[item] : chars2[item]);
	}
	changeMode(0);
	updateRows(0,1);
    }
}

void CharConfig::addChar(int r, const QString& s)
{
    if ( r ) chars2.append(s); else chars1.append(s);
}

QString CharStringConfig::text(int r, int i)
{
    QStringList l = r ? chars : QStringList(input);
    return i < (int)l.count() ? l[i] : QString::null;
}

bool CharStringConfig::spreadRow(int i)
{
    return i ? TRUE : FALSE;
}

void CharStringConfig::pick(bool press, int row, int item)
{
    if ( row == 0 ) {
	if ( !press ) {
	    if ( item>=0 ) {
		generateText(input);
	    }
	    input = "";
	    changeMode(0);
	    updateRows(0,1);
	}
    } else {
	if ( press && item >= 0 ) {
	    input.append(chars[item]);
	    updateRows(0,0);
	}
    }
}

void CharStringConfig::addChar(const QString& s)
{
    chars.append(s);
}

void CharStringConfig::doMenu(int i)
{
    if ( i == 100 ) {
	input = "";
	updateRows(0,0);
    }

    StringConfig::doMenu(i);
}

