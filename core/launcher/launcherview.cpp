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

#include "launcherview.h"

#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
#include <qpe/qpedebug.h>
#include <qpe/config.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>
#include <qpe/menubutton.h>
#include <qpe/resource.h>
#include <qpe/qpetoolbar.h>

#include <qtimer.h>
#include <qdict.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qhbox.h>
#include <qiconview.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qtoolbutton.h>

class LauncherIconView : public QIconView {
public:
    LauncherIconView( QWidget* parent, const char* name=0 ) :
	QIconView(parent,name),
	tf(""),
	cf(0),
	bsy(0)
    {
	sortmeth = Name;
	hidden.setAutoDelete(TRUE);
	ike = FALSE;
    }

    ~LauncherIconView()
    {
#if 0 // debuggery
	QListIterator<AppLnk> it(hidden);
	AppLnk* l;
	while ((l=it.current())) {
	    ++it;
	    //qDebug("%p: hidden (should remove)",l);
	}
#endif
    }

    QIconViewItem* busyItem() const { return bsy; }

    void updateCategoriesAndMimeTypes();

    void doAutoScroll()
    {
	// We don't want rubberbanding (yet)
    }

    void setBusy(bool on)
    {
	QIconViewItem *c = on ? currentItem() : 0;
	if ( bsy != c ) {
	    QIconViewItem* o = bsy;
	    bsy = c;
	    if ( o ) o->repaint();
	    if ( c ) c->repaint();
	}
    }

    bool inKeyEvent() const { return ike; }
    void keyPressEvent(QKeyEvent* e)
    {
	ike = TRUE;
	if ( e->key() == Key_F33 ) {
	    // "OK" button
	    returnPressed(currentItem());
	}
	QIconView::keyPressEvent(e);
	ike = FALSE;
    }

    void addItem(AppLnk* app, bool resort=TRUE);
    bool removeLink(const QString& linkfile);

    QStringList mimeTypes() const;
    QStringList categories() const;

    void clear()
    {
	mimes.clear();
	cats.clear();
	QIconView::clear();
	hidden.clear();
    }

    void addCatsAndMimes(AppLnk* app)
    {
	// 	QStringList c = app->categories();
	// 	for (QStringList::ConstIterator cit=c.begin(); cit!=c.end(); ++cit) {
	// 	    cats.replace(*cit,(void*)1);
	// 	}
	QString maj=app->type();
	int sl=maj.find('/');
	if (sl>=0) {
	    QString k = maj.left(sl);
	    mimes.replace(k,(void*)1);
	}
    }

    void drawBackground( QPainter *p, const QRect &r )
    {
	Config config("qpe");
	config.setGroup("Appearance");
	QString backgroundImage = config.readEntry("BackgroundImage");

	if (backgroundImage.isNull()) backgroundImage="qpe-background";
	int backgroundMode = QPixmap::defaultDepth() >= 12 ? 1 : 0;
	//int backgroundMode = 2;

	if ( backgroundMode == 1 ) {

	    // Double buffer the background
	    static QPixmap *bg = NULL;
	    static QColor bgColor;

	    if ( (bg == NULL) || (bgColor != colorGroup().button()) ) {
		// Create a new background double buffer
		if (bg == NULL)
		    bg = new QPixmap( width(), height() );
		bgColor = colorGroup().button();
		QPainter painter( bg );

		painter.fillRect( QRect( 0, 0, width(), height() ), colorGroup().background().light(110));
                // Overlay the Qtopia logo in the center
                QImage logo;
		if (QFile::exists(backgroundImage)) {
                logo = QImage(backgroundImage);
		} else {
		logo = Resource::loadImage(backgroundImage );
		}
		if ( !logo.isNull() )
		    painter.drawImage( (width() - logo.width()) / 2,
				       (height() - logo.height()) / 2, logo );
	    }

	    // Draw the double buffer to the widget (it is tiled for when the icon view is large)
	    p->drawTiledPixmap( r, *bg, QPoint( (r.x() + contentsX()) % bg->width(),
						(r.y() + contentsY()) % bg->height() ) );
	} else if ( backgroundMode == 2 ) {
	    static QPixmap *bg = 0;
	    static QColor bgColor;
	    if ( !bg || (bgColor != colorGroup().background()) ) {
		bgColor = colorGroup().background();
		bg = new QPixmap( width(), 9 );
		QPainter painter( bg );
		for ( int i = 0; i < 3; i++ ) {
		    painter.setPen( colorGroup().background().light(130) );
		    painter.drawLine( 0, i*3, width()-1, i*3 );
		    painter.drawLine( 0, i*3+1, width()-1, i*3+1 );
		    painter.setPen( colorGroup().background().light(105) );
		    painter.drawLine( 0, i*3+2, width()-1, i*3+2 );
		}
	    }
	    p->drawTiledPixmap( r, *bg, QPoint( (r.x() + contentsX()) % bg->width(),
						(r.y() + contentsY()) % bg->height() ) );
	} else {
	    p->fillRect( r, QBrush( colorGroup().background().light(110) ) );
	}
    }

    void hideOrShowItems(bool resort);

    void setTypeFilter(const QString& typefilter, bool resort)
    {
	tf = QRegExp(typefilter,FALSE,TRUE);
	hideOrShowItems(resort);
    }

    void setCategoryFilter( int catfilter, bool resort )
    {
	Categories cat;
	cat.load( categoryFileName() );
	QString str;
	if ( catfilter == -2 )
	    cf = 0;
	else
	    cf = catfilter;
	hideOrShowItems(resort);
    }

    enum SortMethod { Name, Date, Type };

    void setSortMethod( SortMethod m )
    {
	if ( sortmeth != m ) {
	    sortmeth = m;
	    sort();
	}
    }

    int compare(const AppLnk* a, const AppLnk* b)
    {
	switch (sortmeth) {
	case Name:
	    return a->name().compare(b->name());
	case Date: {
	    QFileInfo fa(a->linkFileKnown() ? a->linkFile() : a->file());
	    QFileInfo fb(b->linkFileKnown() ? b->linkFile() : b->file());
	    return fa.lastModified().secsTo(fb.lastModified());
	}
	case Type:
	    return a->type().compare(b->type());
	}
	return 0;
    }

protected:

    void styleChange( QStyle &old )
    {
	QIconView::styleChange( old );
	//### duplicated code from LauncherView constructor
	int dw = QApplication::desktop()->width();
	setGridX( (dw-13-style().scrollBarExtent().width())/3 ); // tweaked for 8pt+dw=176 and 10pt+dw=240
    }

private:
    QList<AppLnk> hidden;
    QDict<void> mimes;
    QDict<void> cats;
    SortMethod sortmeth;
    QRegExp tf;
    int cf;
    QIconViewItem* bsy;
    bool ike;

};


bool LauncherView::bsy=FALSE;

void LauncherView::setBusy(bool on)
{
    icons->setBusy(on);
}

class LauncherItem : public QIconViewItem
{
public:
    LauncherItem( QIconView *parent, AppLnk* applnk );
    ~LauncherItem()
    {
	LauncherIconView* liv = (LauncherIconView*)iconView();
	if ( liv->busyItem() == this )
	    liv->setBusy(FALSE);
	delete app;
    }

    AppLnk* appLnk() const { return app; }
    AppLnk* takeAppLnk() { AppLnk* r=app; app=0; return r; }

    virtual int compare ( QIconViewItem * i ) const;

    void paintItem( QPainter *p, const QColorGroup &cg )
    {
	LauncherIconView* liv = (LauncherIconView*)iconView();
	QBrush oldBrush( liv->itemTextBackground() );
	QColorGroup mycg( cg );
	if ( liv->currentItem() == this ) {
	    liv->setItemTextBackground( cg.brush( QColorGroup::Highlight ) );
	    mycg.setColor( QColorGroup::Text, cg.color( QColorGroup::HighlightedText ) );
	}
	QIconViewItem::paintItem(p,mycg);
	if ( liv->currentItem() == this )
	    liv->setItemTextBackground( oldBrush );
	if ( liv->busyItem() == this ) {
	    static QPixmap* busypm=0;
	    if ( !busypm )
		busypm = new QPixmap(Resource::loadPixmap("launching"));
	    p->drawPixmap(x()+(width()-busypm->width())/2, y(),*busypm);
	}
    }

protected:
    AppLnk* app;
};


LauncherItem::LauncherItem( QIconView *parent, AppLnk *applnk )
    : QIconViewItem( parent, applnk->name(), applnk->bigPixmap() ),
	app(applnk) // Takes ownership
{
}

int LauncherItem::compare ( QIconViewItem * i ) const
{
    LauncherIconView* view = (LauncherIconView*)iconView();
    return view->compare(app,((LauncherItem *)i)->appLnk());
}

QStringList LauncherIconView::mimeTypes() const
{
    QStringList r;
    QDictIterator<void> it(mimes);
    while (it.current()) {
	r.append(it.currentKey());
	++it;
    }
    r.sort();
    return r;
}

void LauncherIconView::addItem(AppLnk* app, bool resort)
{
    addCatsAndMimes(app);

    if ( (tf.isEmpty() || tf.match(app->type()) >= 0)
	 && (cf == 0 || app->categories().contains(cf)
	     || cf == -1 && app->categories().count() == 0 ) )
	(void) new LauncherItem( this, app );
    else
	hidden.append(app);
    if ( resort )
	sort();
}

void LauncherIconView::updateCategoriesAndMimeTypes()
{
    mimes.clear();
    cats.clear();
    LauncherItem* item = (LauncherItem*)firstItem();
    while (item) {
	addCatsAndMimes(item->appLnk());
	item = (LauncherItem*)item->nextItem();
    }
    QListIterator<AppLnk> it(hidden);
    AppLnk* l;
    while ((l=it.current())) {
	addCatsAndMimes(l);
	++it;
    }
}

void LauncherIconView::hideOrShowItems(bool resort)
{
    hidden.setAutoDelete(FALSE);
    QList<AppLnk> links=hidden;
    hidden.clear();
    hidden.setAutoDelete(TRUE);
    LauncherItem* item = (LauncherItem*)firstItem();
    while (item) {
	links.append(item->takeAppLnk());
	item = (LauncherItem*)item->nextItem();
    }
    bool oldAutoArrange = autoArrange();
    setAutoArrange( FALSE );
    clear();
    QListIterator<AppLnk> it(links);
    AppLnk* l;
    while ((l=it.current())) {
	addItem(l,FALSE);
	++it;
    }
    if ( resort )
	sort();
    setAutoArrange( oldAutoArrange );
}

bool LauncherIconView::removeLink(const QString& linkfile)
{
    LauncherItem* item = (LauncherItem*)firstItem();
    AppLnk* l;
    bool did = FALSE;
    DocLnk dl(linkfile);
    while (item) {
	l = item->appLnk();
	if (  l->linkFileKnown() && l->linkFile() == linkfile || l->file() == linkfile
		|| dl.isValid() && dl.file() == l->file() ) {
	    delete item;
	    did = TRUE;
	}
	item = (LauncherItem*)item->nextItem();
    }
    QListIterator<AppLnk> it(hidden);
    while ((l=it.current())) {
	++it;
	if ( l->linkFileKnown() && l->linkFile() == linkfile
		|| l->file() == linkfile
		|| dl.isValid() && dl.file() == l->file() ) {
	    hidden.removeRef(l);
	   did = TRUE;
	}
    }
    return did;
}

LauncherView::LauncherView( QWidget* parent, const char* name, WFlags fl )
    : QVBox( parent, name, fl )
{
    icons = new LauncherIconView( this );
    setFocusProxy(icons);
    QPEApplication::setStylusOperation( icons->viewport(), QPEApplication::RightOnHold );

    int dw = QApplication::desktop()->width();
    icons->setItemsMovable( FALSE );
    icons->setAutoArrange( TRUE );
    icons->setSorting( TRUE );
    icons->setGridX( (dw-13-style().scrollBarExtent().width())/3 ); // tweaked for 8pt+dw=176 and 10pt+dw=240
    icons->setGridY( fontMetrics().height()*2+24 );
    icons->setFrameStyle( QFrame::NoFrame );
    icons->setSpacing( 4 );
    icons->setMargin( 0 );
    icons->setSelectionMode( QIconView::Multi );
    icons->setBackgroundMode( PaletteBase );

    connect( icons, SIGNAL(mouseButtonClicked(int, QIconViewItem *, const QPoint&)),
		   SLOT(itemClicked(int, QIconViewItem *)) );
    connect( icons, SIGNAL(selectionChanged()),
		   SLOT(selectionChanged()) );
    connect( icons, SIGNAL(returnPressed(QIconViewItem *)),
		   SLOT(returnPressed(QIconViewItem *)) );
    connect( icons, SIGNAL(mouseButtonPressed(int, QIconViewItem *, const QPoint&)),
		   SLOT(itemPressed(int, QIconViewItem *)) );

    tools = 0;
}

LauncherView::~LauncherView()
{
}

void LauncherView::setToolsEnabled(bool y)
{
    if ( !y != !tools ) {
	if ( y ) {
	    tools = new QHBox(this);

	    // Type filter
	    typemb = new MenuButton(tools);
	    typemb->setLabel(tr("Type: %1"));

	    // Category filter
	    catmb = new CategorySelect(tools);

	    updateTools();
	    tools->show();
	} else {
	    delete tools;
	    tools = 0;
	}
    }
}

void LauncherView::updateTools()
{
    disconnect( typemb, SIGNAL(selected(const QString&)),
	        this, SLOT(showType(const QString&)) );
    disconnect( catmb, SIGNAL(signalSelected(int)),
	        this, SLOT(showCategory(int)) );

    icons->updateCategoriesAndMimeTypes();

    QString prev;

    // Type filter
    QStringList types;
    types << tr("All");
    types << "--";
    types += icons->mimeTypes();
    prev = typemb->currentText();
    typemb->clear();
    typemb->insertItems(types);
    typemb->select(prev);

    Categories cats( 0 );
    cats.load( categoryFileName() );
    QArray<int> vl( 0 );
    catmb->setCategories( vl, "Document View", tr("Document View") );
    catmb->setRemoveCategoryEdit( TRUE );
    catmb->setAllCategories( TRUE );

    connect(typemb, SIGNAL(selected(const QString&)), this, SLOT(showType(const QString&)));
    connect(catmb, SIGNAL(signalSelected(int)), this, SLOT(showCategory(int)));
}

void LauncherView::sortBy(int s)
{
    icons->setSortMethod((LauncherIconView::SortMethod)s);
}

void LauncherView::showType(const QString& t)
{
    if ( t == tr("All") ) {
	icons->setTypeFilter("",TRUE);
    } else {
	icons->setTypeFilter(t+"/*",TRUE);
    }
}

void LauncherView::showCategory( int c )
{
    icons->setCategoryFilter( c, TRUE );
}

void LauncherView::resizeEvent(QResizeEvent *e)
{
    QVBox::resizeEvent( e );
    if ( e->size().width() != e->oldSize().width() )
	sort();
}

void LauncherView::populate( AppLnkSet *folder, const QString& typefilter )
{
    icons->clear();
    internalPopulate( folder, typefilter );
}

void LauncherView::selectionChanged()
{
    QIconViewItem* item = icons->currentItem();
    if ( item && item->isSelected() ) {
	AppLnk *appLnk = ((LauncherItem *)item)->appLnk();
	if ( icons->inKeyEvent() ) // not for mouse press
	    emit clicked( appLnk );
	item->setSelected(FALSE);
    }
}

void LauncherView::returnPressed( QIconViewItem *item )
{
    if ( item ) {
	AppLnk *appLnk = ((LauncherItem *)item)->appLnk();
	emit clicked( appLnk );
    }
}

void LauncherView::itemClicked( int btn, QIconViewItem *item )
{
    if ( item ) {
	AppLnk *appLnk = ((LauncherItem *)item)->appLnk();
	if ( btn == LeftButton ) {
	    // Make sure it's the item we execute that gets highlighted
	    icons->setCurrentItem( item );
	    emit clicked( appLnk );
	}
	item->setSelected(FALSE);
    }
}

void LauncherView::itemPressed( int btn, QIconViewItem *item )
{
    if ( item ) {
	AppLnk *appLnk = ((LauncherItem *)item)->appLnk();
	if ( btn == RightButton )
	    emit rightPressed( appLnk );
/*
	else if ( btn == LeftButton )
	    emit clicked( appLnk );
*/
	item->setSelected(FALSE);
    }
}

void LauncherView::internalPopulate( AppLnkSet *folder, const QString& typefilter )
{
    QListIterator<AppLnk> it( folder->children() );
    icons->setTypeFilter(typefilter,FALSE);

    while ( it.current() ) {
      // show only the icons for existing files
      if (!QFile(it.current()->file()).exists() )
	{
	  //maybe insert some .desktop file deletion code later
	  //maybe dir specific
	}
      else
	{
	  icons->addItem(*it,FALSE);
	}
	++it;
    }

    icons->sort();
}

bool LauncherView::removeLink(const QString& linkfile)
{
    return icons->removeLink(linkfile);
}

void LauncherView::sort()
{
    icons->sort();
}

void LauncherView::addItem(AppLnk* app, bool resort)
{
    icons->addItem(app,resort);
}

void LauncherView::setFileSystems(const QList<FileSystem> &)
{
    // ### does nothing now...
}
