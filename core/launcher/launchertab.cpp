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
#include "launchertab.h"
#include <qapplication.h>


LauncherTabBar::LauncherTabBar( QWidget *parent, const char *name )
    : QTabBar( parent, name )
{
    setFocusPolicy( NoFocus );
    connect( this, SIGNAL( selected(int) ), this, SLOT( layoutTabs() ) );
}

LauncherTabBar::~LauncherTabBar()
{
}

void LauncherTabBar::insertTab( LauncherTab *t, int index )
{
    if ( index < 0 )
	items.append( t );
    else
	items.insert( (uint)index, t );
    tabs.insert( t->type, t );
    QTabBar::insertTab( t, index );
}

void LauncherTabBar::removeTab( QTab *tab )
{
    LauncherTab *t = (LauncherTab *)tab;
    tabs.remove( t->type );
    items.remove( t );
    QTabBar::removeTab( t );
}

void LauncherTabBar::prevTab()
{
    int n = count();
    int tab = currentTab();
    if ( tab >= 0 )
	setCurrentTab( (tab - 1 + n)%n );
}

void LauncherTabBar::nextTab()
{
    int n = count();
    int tab = currentTab();
    setCurrentTab( (tab + 1)%n );
}

void LauncherTabBar::showTab( const QString& id )
{
    setCurrentTab( tabs[id] );
}

void LauncherTabBar::layoutTabs()
{
    if ( !count() )
	return;

    int available = width()-1;

    QFontMetrics fm = fontMetrics();
    int hiddenTabWidth = -12;
    LauncherTab *current = currentLauncherTab();
    int hframe, vframe, overlap;
    style().tabbarMetrics( this, hframe, vframe, overlap );
    int x = 0;
    QRect r;
    LauncherTab *t;
    QListIterator< LauncherTab > it( items );
    int required = 0;
    int eventabwidth = (width()-1)/count();
    enum Mode { HideBackText, Pack, Even } mode=Even;
    for (it.toFirst(); it.current(); ++it ) {
	t = it.current();
	if ( !t )
	    continue;
	int iw = fm.width( t->text() ) + hframe - overlap;
	if ( t != current ) {
	    available -= hiddenTabWidth + hframe - overlap;
	    if ( t->iconSet() != 0 )
		available -= t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
	}
	if ( t->iconSet() != 0 )
	    iw += t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
	required += iw;
	// As space gets tight, packed looks better than even. "10" must be at least 0.
	if ( iw >= eventabwidth-10 )
	    mode = Pack;
    }
    if ( mode == Pack && required > width()-1 )
	mode = HideBackText;
    for ( it.toFirst(); it.current(); ++it ) {
	t = it.current();
	if ( !t )
	    continue;
	if ( mode != HideBackText ) {
	    int w = fm.width( t->text() );
	    int ih = 0;
	    if ( t->iconSet() != 0 ) {
		w += t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
		ih = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	    }
	    int h = QMAX( fm.height(), ih );
	    h = QMAX( h, QApplication::globalStrut().height() );

	    h += vframe;
	    w += hframe;

	    QRect totr(x, 0,
		mode == Even ? eventabwidth : w * (width()-1)/required, h);
	    t->setRect(totr);
	    x += totr.width() - overlap;
	    r = r.unite(totr);
	} else if ( t != current ) {
	    int w = hiddenTabWidth;
	    int ih = 0;
	    if ( t->iconSet() != 0 ) {
		w += t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
		ih = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	    }
	    int h = QMAX( fm.height(), ih );
	    h = QMAX( h, QApplication::globalStrut().height() );

	    h += vframe;
	    w += hframe;

	    t->setRect( QRect(x, 0, w, h) );
	    x += t->rect().width() - overlap;
	    r = r.unite( t->rect() );
	} else {
	    int ih = 0;
	    if ( t->iconSet() != 0 ) {
		ih = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	    }
	    int h = QMAX( fm.height(), ih );
	    h = QMAX( h, QApplication::globalStrut().height() );

	    h += vframe;

	    t->setRect( QRect(x, 0, available, h) );
	    x += t->rect().width() - overlap;
	    r = r.unite( t->rect() );
	}
    }

    t = it.toLast();
    if (t) {
	QRect rr = t->rect();
	rr.setRight(width()-1);
	t->setRect( rr );
    }

    for (it.toFirst(); it.current(); ++it ) {
	t = it.current();
	QRect tr = t->rect();
	tr.setHeight( r.height() );
	t->setRect( tr );
    }

    update();
}

void LauncherTabBar::paint( QPainter * p, QTab * t, bool selected ) const
{
    LauncherTabBar *that = (LauncherTabBar *) this;
    LauncherTab *ct = (LauncherTab *)t;
    QPalette pal = palette();
    bool setPal = FALSE;
    if ( ct->bgColor.isValid() ) {
	pal.setColor( QPalette::Active, QColorGroup::Background, ct->bgColor );
	pal.setColor( QPalette::Active, QColorGroup::Button, ct->bgColor );
	pal.setColor( QPalette::Inactive, QColorGroup::Background, ct->bgColor );
	pal.setColor( QPalette::Inactive, QColorGroup::Button, ct->bgColor );
	that->setUpdatesEnabled( FALSE );
	that->setPalette( pal );
	setPal = TRUE;
    }
#if QT_VERSION >= 0x030000
    QStyle::SFlags flags = QStyle::Style_Default;
    if ( selected )
        flags |= QStyle::Style_Selected;
    style().drawControl( QStyle::CE_TabBarTab, p, this, t->rect(),
                         colorGroup(), flags, QStyleOption(t) );
#else
    style().drawTab( p, this, t, selected );
#endif

    QRect r( t->rect() );
    QFont f( font() );
    if ( selected )
	f.setBold( TRUE );
    p->setFont( f );

    if ( ct->fgColor.isValid() ) {
	pal.setColor( QPalette::Active, QColorGroup::Foreground, ct->fgColor );
	pal.setColor( QPalette::Inactive, QColorGroup::Foreground, ct->fgColor );
	that->setUpdatesEnabled( FALSE );
	that->setPalette( pal );
	setPal = TRUE;
    }
    int iw = 0;
    int ih = 0;
    if ( t->iconSet() != 0 ) {
	iw = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width() + 2;
	ih = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
    }
    int w = iw + p->fontMetrics().width( t->text() ) + 4;
    int h = QMAX(p->fontMetrics().height() + 4, ih );
    paintLabel( p, QRect( r.left() + (r.width()-w)/2 - 3,
			    r.top() + (r.height()-h)/2, w, h ), t,
#if QT_VERSION >= 0x030000
			    t->identifier() == keyboardFocusTab()
#else
			    t->identitifer() == keyboardFocusTab()
#endif
		);
    if ( setPal ) {
	that->unsetPalette();
	that->setUpdatesEnabled( TRUE );
    }
}

void LauncherTabBar::paintLabel( QPainter* p, const QRect&,
			  QTab* t, bool has_focus ) const
{
    QRect r = t->rect();
    //    if ( t->id != currentTab() )
    //r.moveBy( 1, 1 );
    //
    if ( t->iconSet() ) {
	// the tab has an iconset, draw it in the right mode
	QIconSet::Mode mode = (t->isEnabled() && isEnabled()) ? QIconSet::Normal : QIconSet::Disabled;
	if ( mode == QIconSet::Normal && has_focus )
	    mode = QIconSet::Active;
	QPixmap pixmap = t->iconSet()->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	int pixh = pixmap.height();
	p->drawPixmap( r.left() + 6, r.center().y() - pixh / 2 + 1, pixmap );
	r.setLeft( r.left() + pixw + 5 );
    }

    QRect tr = r;

    if ( r.width() < 20 )
	return;

    if ( t->isEnabled() && isEnabled()  ) {
#if defined(_WS_WIN32_)
	if ( colorGroup().brush( QColorGroup::Button ) == colorGroup().brush( QColorGroup::Background ) )
	    p->setPen( colorGroup().buttonText() );
	else
	    p->setPen( colorGroup().foreground() );
#else
	p->setPen( colorGroup().foreground() );
#endif
	p->drawText( tr, AlignCenter | AlignVCenter | ShowPrefix, t->text() );
    } else {
	p->setPen( palette().disabled().foreground() );
	p->drawText( tr, AlignCenter | AlignVCenter | ShowPrefix, t->text() );
    }
}

