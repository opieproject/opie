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
#include <qapplication.h>
#include <qstyle.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#include "qcopenvelope_qws.h"
#include "qpedecoration_qws.h"
#include <qdialog.h>
#include <qdrawutil.h>
#include <qgfx_qws.h>
#include "qpeapplication.h"
#include "resource.h"
#include "global.h"
#include <qfile.h>
#include <qsignal.h>

#include <stdlib.h>

extern QRect qt_maxWindowRect; 

//#define MINIMIZE_HELP_HACK  // use minimize button when not a dialog

//#define WHATSTHIS_MODE

#ifndef QT_NO_QWS_QPE_WM_STYLE

#ifndef QT_NO_IMAGEIO_XPM

/* XPM */
static const char * const qpe_close_xpm[] = {
"16 16 3 1",
"       c None",
".      c #FFFFFF",
"+      c #000000",
"                ",
"                ",
"      .....     ",
"    ..+++++..   ",
"   .+++++++++.  ",
"   .+..+++..+.  ",
"  .++...+...++. ",
"  .+++.....+++. ",
"  .++++...++++. ",
"  .+++.....+++. ",
"  .++...+...++. ",
"   .+..+++..+.  ",
"   .+++++++++.  ",
"    ..+++++..   ",
"      .....     ",
"                "};

/* XPM */
static const char * const qpe_accept_xpm[] = {
"16 16 3 1",
"       c None",
".      c #FFFFFF",
"+      c #000000",
"                ",
"                ",
"      .....     ",
"    ..+++++..   ",
"   .+++++++++.  ",
"   .+++++++++.  ",
"  .+++++++..++. ",
"  .++.+++...++. ",
"  .+...+...+++. ",
"  .+......++++. ",
"  .++....+++++. ",
"   .++..+++++.  ",
"   .+++++++++.  ",
"    ..+++++..   ",
"      .....     ",
"                "};

#endif // QT_NO_IMAGEIO_XPM

class TLWidget : public QWidget
{
public:
    QWSManager *manager()
    {
	return topData()->qwsManager;
    }

    QTLWExtra *topExtra()
    {
	return topData();
    }
};

QPEManager::QPEManager( QPEDecoration *d, QObject *parent )
    : QObject( parent ), decoration( d ), helpState(0)
{
    wtTimer = new QTimer( this );
    connect( wtTimer, SIGNAL(timeout()), this, SLOT(whatsThisTimeout()) );
}


void QPEManager::updateActive()
{
    QWidget *newActive = qApp->activeWindow();
    if ( (QWidget*)active == newActive )
	return;

    if ( active ) {
	((TLWidget *)(QWidget*)active)->manager()->removeEventFilter( this );
    }

    if ( newActive && ((TLWidget *)newActive)->manager() ) {
	active = newActive;
	((TLWidget *)(QWidget*)active)->manager()->installEventFilter( this );
    } else {
	active = 0;
    }
}

int QPEManager::pointInQpeRegion( QWidget *w, const QPoint &p )
{
    if ( decoration->region( w, w->geometry(),
	    (QWSDecoration::Region)QPEDecoration::Help ).contains(p) ) {
	return QPEDecoration::Help;
    }

    return QWSDecoration::None;
}

bool QPEManager::eventFilter( QObject *o, QEvent *e )
{
    QWSManager *mgr = (QWSManager *)o;
    QWidget *w = mgr->widget();
    switch ( e->type() ) {
	case QEvent::MouseButtonPress:
	    {
		if ( QWhatsThis::inWhatsThisMode() ) {
		    QWhatsThis::leaveWhatsThisMode();
		    return true;
		}
		pressTime = QTime::currentTime();
		QPoint p = ((QMouseEvent*)e)->globalPos();
		if ( pointInQpeRegion( w, p ) == QPEDecoration::Help ) {
		    helpState = QWSButton::Clicked|QWSButton::MouseOver;
		    drawButton( w, QPEDecoration::Help, helpState );
		    return true;
		}
	    }
	    break;
	case QEvent::MouseButtonRelease:
	    if ( helpState & QWSButton::Clicked ) {
		helpState = 0;
		drawButton( w, QPEDecoration::Help, helpState );
		QPoint p = ((QMouseEvent*)e)->globalPos();
		if ( pointInQpeRegion( w, p ) == QPEDecoration::Help ) {
#ifdef WHATSTHIS_MODE
		    if ( pressTime.msecsTo( QTime::currentTime() ) > 250 ) {
			decoration->help( w );
		    } else {
			QWhatsThis::enterWhatsThisMode();
			wtTimer->start( 200 );
		    }
#else
		    decoration->help( w );
#endif
		}
		return true;
	    }
	    break;
	case QEvent::MouseMove:
	    if ( helpState & QWSButton::Clicked ) {
		int oldState = helpState;
		QPoint p = ((QMouseEvent*)e)->globalPos();
		if ( pointInQpeRegion( w, p ) == QPEDecoration::Help )
		    helpState = QWSButton::Clicked|QWSButton::MouseOver;
		else
		    helpState = 0;
		if ( helpState != oldState )
		    drawButton( w, QPEDecoration::Help, helpState );
	    }
	    break;
	default:
	    break;
    }
    return QObject::eventFilter( o, e );
}

void QPEManager::drawButton( QWidget *w, QPEDecoration::QPERegion r, int state )
{
    QPainter painter(w);
    QRegion rgn = ((TLWidget *)w)->topExtra()->decor_allocated_region;
    painter.internalGfx()->setWidgetDeviceRegion( rgn );
    painter.setClipRegion(decoration->region(w, w->rect(),QWSDecoration::All));
    decoration->paintButton( &painter, w, (QWSDecoration::Region)r, state );
}

void QPEManager::whatsThisTimeout()
{
    if ( !QWhatsThis::inWhatsThisMode() ) {
	wtTimer->stop();
	if ( active )
	    drawButton( active, QPEDecoration::Help, 0 );
    } else if ( active ) {
	static int state = 0;
	if ( !state )
	    state = QWSButton::Clicked|QWSButton::MouseOver;
	else
	    state = 0;
	drawButton( active, QPEDecoration::Help, state );
    }
}

//=========

class HackWidget : public QWidget
{
public:
    bool needsOk() { return (getWState() & WState_Reserved1 ); }
};

//===========================================================================

QPEDecoration::QPEDecoration()
    : QWSDefaultDecoration()
{
    imageOk = Resource::loadImage( "OKButton" );
    imageClose = Resource::loadImage( "CloseButton" );
    imageHelp = Resource::loadImage( "HelpButton" );
    helpFile = QString(qApp->argv()[0]) + ".html";
    QString lang = getenv( "LANG" );
    helpExists = QFile::exists( QPEApplication::qpeDir() + "/help/" + lang + "/html/" + helpFile );
    if ( !helpExists )
	helpExists = QFile::exists( QPEApplication::qpeDir() + "/help/en/html/" + helpFile );
#ifndef MINIMIZE_HELP_HACK
    qpeManager = new QPEManager( this );
#else
    qpeManager = 0;
#endif
}

QPEDecoration::~QPEDecoration()
{
    delete qpeManager;
}

const char **QPEDecoration::menuPixmap()
{
    return (const char **)0;
}

const char **QPEDecoration::closePixmap()
{
    return (const char **)qpe_close_xpm;
}

const char **QPEDecoration::minimizePixmap()
{
    return (const char **)qpe_accept_xpm;
}

const char **QPEDecoration::maximizePixmap()
{
    return (const char **)0;
}

const char **QPEDecoration::normalizePixmap()
{
    return (const char **)0;
}

int QPEDecoration::getTitleHeight(const QWidget *)
{
    return 15;
}

/*
    If rect is empty, no frame is added. (a hack, really)
*/
QRegion QPEDecoration::region(const QWidget *widget, const QRect &rect, QWSDecoration::Region type)
{
    int titleHeight = getTitleHeight(widget);
//    int titleWidth = getTitleWidth(widget);
//    int bw = rect.isEmpty() ? 0 : BORDER_WIDTH;
    QRegion region;

#ifndef MINIMIZE_HELP_HACK
    qpeManager->updateActive();
#endif

    switch ((int)type) {
	case Menu:
	case Maximize:
	    break;
	case Minimize: {
	    if ( ((HackWidget *)widget)->needsOk() || 
		 (widget->inherits( "QDialog" ) && !widget->inherits( "QMessageBox" ) ) ) {
		QRect r(rect.right() - imageOk.width(),
		    rect.top() - titleHeight - 2,
		    imageOk.width(), titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	    }
#ifdef MINIMIZE_HELP_HACK
	    else if ( helpExists ) {
		QRect r;
		r = QRect(rect.right() - imageClose.width() - imageHelp.width(),
			  rect.top() - titleHeight - 2,
			  imageHelp.width(), titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	    }
#endif
	}
	break;
	case Close: {
	    if ( ((HackWidget *)widget)->needsOk() || 
		 (widget->inherits( "QDialog" ) && !widget->inherits( "QMessageBox" ) ) ) {
		QRect r;
		r = QRect(rect.right() - imageOk.width() - imageClose.width(),
		    rect.top() - titleHeight - 2,
		    imageClose.width(), titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	    } else {
		QRect r(rect.right() - imageClose.width(),
		    rect.top() - titleHeight - 2,
		    imageClose.width(), titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	    }
	    break;
	}
    	case Title: {
	    if ( widget->isMaximized() ) {
		region = QRegion();
	    } else {
		int btnWidth = imageClose.width();
		if ( ((HackWidget *)widget)->needsOk() || 
		     (widget->inherits( "QDialog" ) && !widget->inherits( "QMessageBox" ) ) )
		    btnWidth += imageOk.width();
		QRect r(rect.left() + 1, rect.top() - titleHeight,
			rect.width() - btnWidth - 2, titleHeight);
		if (r.width() > 0)
		    region = r;
	    }
	    break;
	}
	case Help: {
	    if ( helpExists ) {
		QRect r;
		int l = rect.right() - imageClose.width() - imageHelp.width();
		if ( ((HackWidget *)widget)->needsOk() || 
		     (widget->inherits( "QDialog" ) && !widget->inherits( "QMessageBox" ) ) )
		    l -= imageOk.width();
		r = QRect(l, rect.top() - titleHeight - 2,
			  imageHelp.width(), titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	    }
	    break;
	}
	case Top:
	case Left:
	case Right:
	case Bottom:
	case TopLeft:
	case TopRight:
	case BottomLeft:
	case BottomRight:
	    if ( widget->isMaximized() ) {
		region = QRegion();
		break;
	    }
	    //else fallthrough!!!
	case All:
	default:
	    region = QWSDefaultDecoration::region(widget, rect, type);
	    break;
    }

    return region;
}

void QPEDecoration::paint(QPainter *painter, const QWidget *widget)
{
#ifndef QT_NO_STYLE
//    QStyle &style = QApplication::style();
#endif

    int titleWidth = getTitleWidth(widget);
    int titleHeight = getTitleHeight(widget);

    QRect rect(widget->rect());

    // Border rect
    QRect br( rect.left() - BORDER_WIDTH,
                rect.top() - BORDER_WIDTH - titleHeight,
                rect.width() + 2 * BORDER_WIDTH,
                rect.height() + BORDER_WIDTH + BOTTOM_BORDER_WIDTH + titleHeight );

    // title bar rect
    QRect tr;

    tr = QRect( rect.left(), rect.top() - titleHeight, rect.width(), titleHeight );

    QRegion oldClip = painter->clipRegion();
    painter->setClipRegion( oldClip - QRegion( tr ) );	// reduce flicker

#ifndef QT_NO_PALETTE
    // const QColorGroup &cg = QApplication::palette().active();
    const QColorGroup &cg = widget->palette().active();

    qDrawWinPanel(painter, br.x(), br.y(), br.width(),
		  br.height() - 4, cg, FALSE,
		  &cg.brush(QColorGroup::Background));

    painter->setClipRegion( oldClip );

    if (titleWidth > 0) {
	QBrush titleBrush;
	QPen   titlePen;
	QPen   titleLines;
	int    titleLeft = titleHeight + 4;

	if (widget == qApp->activeWindow()) {
	    titleBrush = cg.brush(QColorGroup::Highlight);
	    titlePen   = cg.color(QColorGroup::HighlightedText);
	} else {
	    titleBrush = cg.brush(QColorGroup::Background);
	    titlePen   = cg.color(QColorGroup::Text);
	}

	titleLines = titleBrush.color().dark(150);

#define CLAMP(x, y)	    ( ((x) > (y)) ? (y) : (x) )

	    titleLeft = rect.left() + 5;
	    painter->setPen( cg.midlight() );
	    painter->drawLine( rect.left() - BORDER_WIDTH + 2,
				rect.bottom() + 1, rect.right() + BORDER_WIDTH - 2,
				rect.bottom() + 1 );

	    fillTitle( painter, widget, rect.left() - 2,
			rect.top() - titleHeight - 2,
			rect.width() + 3, titleHeight + 2 );
/*
	    painter->fillRect(rect.left() - 2,
			    rect.top() - titleHeight - 2,
			    rect.width() + 3, titleHeight + 2,
			    titleBrush);

	    painter->setPen( titleLines );
	    for ( int i = rect.top() - titleHeight - 2; i < rect.top(); i += 2 )
		painter->drawLine( rect.left() - 2, i, rect.left() + rect.width() + 2, i );
*/
	painter->setPen(titlePen);
	QFont f( QApplication::font() );
	f.setWeight( QFont::Bold );
	painter->setFont(f);
	painter->drawText( titleLeft, -titleHeight,
	 		rect.width() - titleHeight - 10, titleHeight-1,
			QPainter::AlignVCenter, widget->caption());
    }

#endif //QT_NO_PALETTE

#ifndef MINIMIZE_HELP_HACK
    paintButton( painter, widget, (QWSDecoration::Region)Help, 0 );
#endif
}

void QPEDecoration::paintButton(QPainter *painter, const QWidget *w,
			QWSDecoration::Region type, int state)
{
#ifndef QT_NO_PALETTE
#ifndef QT_NO_STYLE
//    QStyle &style = QApplication::style();
#endif
    const QColorGroup &cg = w->palette().active();

    QRect brect(region(w, w->rect(), type).boundingRect());

//    int xoff=2;
//    int yoff=2;

//    const QPixmap *pm=pixmapFor(w,type,state & QWSButton::On, xoff, yoff);

    const QImage *img = 0;

	switch ((int)type) {
	    case Close:
		img = &imageClose;
		break;
	    case Minimize:
		if ( ((HackWidget *)w)->needsOk() || 
		     (w->inherits( "QDialog" ) && !w->inherits( "QMessageBox" ) ) )
		    img = &imageOk;
		else if ( helpExists )
		    img = &imageHelp;
		break;
	    case Help:
		img = &imageHelp;
		break;
	    default:
		return;
	}

	QBrush titleBrush;

	if (w == qApp->activeWindow()) {
	    titleBrush = cg.brush(QColorGroup::Highlight);
	} else {
	    titleBrush = cg.brush(QColorGroup::Background);
	}

	fillTitle( painter, w, brect.x(), brect.y(), brect.width()+1,
		    brect.height()+1 );
	if ((state & QWSButton::MouseOver) && (state & QWSButton::Clicked)) {
	    if (img) painter->drawImage(brect.x()+1, brect.y()+3, *img);
	} else {
	    if (img) painter->drawImage(brect.x(), brect.y()+2, *img);
	}

#endif

}

void QPEDecoration::fillTitle( QPainter *p, const QWidget *widget,
				int x, int y, int w, int h )
{
    QBrush titleBrush;
    QPen   topLine;
    QPen   titleLines;

    const QColorGroup &cg = widget->palette().active();

    if (widget == qApp->activeWindow()) {
	titleBrush = cg.brush(QColorGroup::Highlight);
	titleLines = titleBrush.color().dark();
	topLine = titleBrush.color().light();
    } else {
	titleBrush = cg.brush(QColorGroup::Background);
	titleLines = titleBrush.color();
	topLine = titleBrush.color();
    }

    p->fillRect( x, y, w, h, titleBrush);

    p->setPen( topLine );
    p->drawLine( x, y+1, x+w-1, y+1 );
    p->setPen( titleLines );
    for ( int i = y; i < y+h; i += 2 )
	p->drawLine( x, i, x+w-1, i );
}

//#define QPE_DONT_SHOW_TITLEBAR

void QPEDecoration::maximize( QWidget *widget )
{
#ifdef QPE_DONT_SHOW_TITLEBAR    
    if ( !widget->inherits( "QDialog" ) ) {
	widget->setGeometry( qt_maxWindowRect );
    } else 
#endif	
	{
	QWSDecoration::maximize( widget );
    }
}

#ifndef QT_NO_DIALOG
class HackDialog : public QDialog
{
public:
    void acceptIt() {
	if ( isA( "QMessageBox" ) )
	    qApp->postEvent( this, new QKeyEvent( QEvent::KeyPress, Key_Enter, '\n', 0, "\n" ) );
	else
	    accept();
    }
};
#endif


void QPEDecoration::minimize( QWidget *widget )
{
#ifndef QT_NO_DIALOG
    // We use the minimize button as an "accept" button.
    if ( widget->inherits( "QDialog" ) ) {
        HackDialog *d = (HackDialog *)widget;
        d->acceptIt();
    } 
#endif
    else if ( ((HackWidget *)widget)->needsOk() ) {
	QSignal s;
	s.connect( widget, SLOT( accept() ) );
	s.activate();
    } else {
	help( widget );
    }
}

void QPEDecoration::help( QWidget * )
{
    if ( helpExists ) {
	Global::execute( "helpbrowser", helpFile );
    }
}


/*
#ifndef QT_NO_POPUPMENU
QPopupMenu *QPEDecoration::menu(QWSManager*, const QWidget*, const QPoint&)
{
    return 0;
}
#endif
*/




#endif // QT_NO_QWS_QPE_WM_STYLE
