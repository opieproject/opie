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

#include "launcherview.h"

#include <qtopia/qpeapplication.h>
#include <qtopia/applnk.h>
#include <qtopia/qpedebug.h>
#include <qtopia/private/categories.h>
#include <qtopia/categoryselect.h>
#include <qtopia/menubutton.h>
#include <qtopia/mimetype.h>
#include <qtopia/resource.h>
#include <qtopia/qpetoolbar.h>
//#include <qtopia/private/palmtoprecord.h>

#include <qtimer.h>
#include <qtextstream.h>
#include <qdict.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qhbox.h>
#include <qiconview.h>
#include <qwidgetstack.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qtoolbutton.h>
#include <qimage.h>
#include <qlabel.h>
#include <qobjectlist.h>


// These define how the busy icon is animated and highlighted
#define BRIGHTEN_BUSY_ICON
//#define ALPHA_FADE_BUSY_ICON
//#define USE_ANIMATED_BUSY_ICON_OVERLAY
#define BOUNCE_BUSY_ICON


class BgPixmap
{
public:
    BgPixmap( const QPixmap &p ) : pm(p), ref(1) {}
    QPixmap pm;
    int ref;
};


static QMap<QString,BgPixmap*> *bgCache = 0;

static void cleanup_cache()
{
    QMap<QString,BgPixmap*>::Iterator it = bgCache->begin();
    while ( it != bgCache->end() ) {
	QMap<QString,BgPixmap*>::Iterator curr = it;
	++it;
	delete (*curr);
	bgCache->remove( curr );
    }
    delete bgCache;
    bgCache = 0;
}


class LauncherItem : public QIconViewItem
{
public:
    LauncherItem( QIconView *parent, AppLnk* applnk, bool bigIcon=TRUE );
    ~LauncherItem();

    AppLnk *appLnk() const { return app; }
    AppLnk *takeAppLnk() { AppLnk* r=app; app=0; return r; }

    void animateIcon();
    void resetIcon();

    virtual int compare ( QIconViewItem * i ) const;
    void paintItem( QPainter *p, const QColorGroup &cg );

    void setBusyIndicatorType ( BusyIndicatorType t ) { busyType = t; }
protected:
    bool isBigIcon;
    int iteration;
    AppLnk* app;
private:
    void paintAnimatedIcon( QPainter *p );
    BusyIndicatorType busyType;
};


class LauncherIconView : public QIconView {
public:
    LauncherIconView( QWidget* parent, const char* name=0 ) :
	QIconView(parent,name),
	tf(""),
	cf(0),
	bsy(0),
	busyTimer(0),
	bigIcns(TRUE),
	bgColor(white)
    {
	sortmeth = Name;
	hidden.setAutoDelete(TRUE);
	ike = FALSE;
	calculateGrid( Bottom );
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
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
    QPixmap busyPixmap() const { return busyPix; }
#endif
    void setBigIcons( bool bi ) {
	bigIcns = bi;
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
	busyPix.resize(0,0);
#endif
    }

    void updateCategoriesAndMimeTypes();
    void setBusyIndicatorType ( BusyIndicatorType t ) { busyType = t; }
     void doAutoScroll()
    {
	// We don't want rubberbanding (yet)
    }

    void setBusy(bool on)
    {
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
	if ( busyPix.isNull() ) {
	    int size = ( bigIcns ) ? AppLnk::bigIconSize() : AppLnk::smallIconSize();
	    busyPix.convertFromImage( Resource::loadImage( "busy" ).smoothScale( size * 16, size ) );
	}
#endif

	if ( on ) {
	    busyTimer = startTimer( 100 );
	} else {
	    if ( busyTimer ) {
	        killTimer( busyTimer );
		busyTimer = 0;
	    }
	}

	LauncherItem *c = on ? (LauncherItem*)currentItem() : 0;

	if ( bsy != c ) {
	    LauncherItem *oldBusy = bsy;
	    bsy = c;
	    if ( oldBusy )  {
		oldBusy->resetIcon();
                    }
	    if ( bsy )   {
                                bsy->setBusyIndicatorType( busyType ) ;
		bsy->animateIcon();
                    }
	}
    }

    bool inKeyEvent() const { return ike; }
    void keyPressEvent(QKeyEvent* e)
    {
	ike = TRUE;
	if ( e->key() == Key_F33 /* OK button */ || e->key() == Key_Space ) {
	    if ( (e->state() & ShiftButton) )
		emit mouseButtonPressed(ShiftButton, currentItem(), QPoint() );
	    else
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
	    QString k;
	    k = maj.left(12) == "application/" ? maj : maj.left(sl);
	    mimes.replace(k,(void*)1);
	}
    }

    void setBackgroundOrigin( QWidget::BackgroundOrigin ) {
    }

    void setBackgroundPixmap( const QPixmap &pm ) {
	bgPixmap = pm;
    }

    void setBackgroundColor( const QColor &c ) {
	bgColor = c;
    }

    void drawBackground( QPainter *p, const QRect &r )
    {
	if ( !bgPixmap.isNull() ) {
           p->drawTiledPixmap( r, bgPixmap,
                   QPoint( (r.x() + contentsX()) % bgPixmap.width(),
                           (r.y() + contentsY()) % bgPixmap.height() ) );
	} else {
	    p->fillRect( r, bgColor );
	}
    }

    void setItemTextPos( ItemTextPos pos )
    {
	calculateGrid( pos );
	QIconView::setItemTextPos( pos );
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
	    return a->name().lower().compare(b->name().lower());
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

    void timerEvent( QTimerEvent *te )
    {
	if ( te->timerId() == busyTimer ) {
	    if ( bsy )
		bsy->animateIcon();
	} else {
	    QIconView::timerEvent( te );
	}
    }

    void styleChange( QStyle &old )
    {
	QIconView::styleChange( old );
	calculateGrid( itemTextPos() );
    }

    void calculateGrid( ItemTextPos pos )
    {
	int dw = QApplication::desktop()->width();
	int viewerWidth = dw-style().scrollBarExtent().width();
	if ( pos == Bottom ) {
	    int cols = 3;
	    if ( viewerWidth <= 200 )
		cols = 2;
	    else if ( viewerWidth >= 400 )
		 cols = viewerWidth/96;
	    setSpacing( 4 );
	    setGridX( (viewerWidth-(cols+1)*spacing())/cols );
	    setGridY( fontMetrics().height()*2+24 );
	} else {
	    int cols = 2;
	    if ( viewerWidth < 150 )
		cols = 1;
	    else if ( viewerWidth >= 400 )
		 cols = viewerWidth/150;
	    setSpacing( 2 );
	    setGridX( (viewerWidth-(cols+1)*spacing())/cols );
	    setGridY( fontMetrics().height()+2 );
	}
    }

    void focusInEvent( QFocusEvent * ) {}
    void focusOutEvent( QFocusEvent * ) {}

private:
    QList<AppLnk> hidden;
    QDict<void> mimes;
    QDict<void> cats;
    SortMethod sortmeth;
    QRegExp tf;
    int cf;
    LauncherItem* bsy;
    int busyTimer;
    bool ike;
    bool bigIcns;
    QPixmap bgPixmap;
    QColor bgColor;
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
    QPixmap busyPix;
#endif
  BusyIndicatorType busyType;
};


bool LauncherView::bsy=FALSE;

void LauncherView::setBusy(bool on)
{
    icons->setBusy(on);
}

void LauncherView::setBusyIndicatorType( const QString& type ) {
    if ( type. lower ( ) == "animated" )
	icons->setBusyIndicatorType(  BIT_Animated  ) ;
    else
	icons->setBusyIndicatorType(  BIT_Normal  ) ;
}

LauncherItem::LauncherItem( QIconView *parent, AppLnk *applnk, bool bigIcon )
    : QIconViewItem( parent, applnk->name(),
           bigIcon ? applnk->bigPixmap() :applnk->pixmap() ),
	isBigIcon( bigIcon ),
	iteration(0),
	app(applnk) // Takes ownership
{
}

LauncherItem::~LauncherItem()
{
    LauncherIconView* liv = (LauncherIconView*)iconView();
    if ( liv->busyItem() == this )
	liv->setBusy(FALSE);
    delete app;
}

int LauncherItem::compare ( QIconViewItem * i ) const
{
    LauncherIconView* view = (LauncherIconView*)iconView();
    return view->compare(app,((LauncherItem *)i)->appLnk());
}

void LauncherItem::paintItem( QPainter *p, const QColorGroup &cg )
{
    LauncherIconView* liv = (LauncherIconView*)iconView();
    QBrush oldBrush( liv->itemTextBackground() );
    QColorGroup mycg( cg );
    if ( liv->currentItem() == this ) {
	liv->setItemTextBackground( cg.brush( QColorGroup::Highlight ) );
	mycg.setColor( QColorGroup::Text, cg.color( QColorGroup::HighlightedText ) );
    }

    QIconViewItem::paintItem(p,mycg);

    // Paint animation overlay
    if ( liv->busyItem() == this )
	paintAnimatedIcon(p);

    if ( liv->currentItem() == this )
	liv->setItemTextBackground( oldBrush );
}



void LauncherItem::paintAnimatedIcon( QPainter *p )
{
    LauncherIconView* liv = (LauncherIconView*)iconView();
    int pic = iteration % 16;
    int w = pixmap()->width(), h = pixmap()->height();
    QPixmap dblBuf( w, h + 4 );
    QPainter p2( &dblBuf );
    int x1, y1;
    if ( liv->itemTextPos() == QIconView::Bottom ) {
	x1 = x() + (width() - w) / 2 - liv->contentsX();
	y1 = y() - liv->contentsY();
    } else {
	x1 = x() - liv->contentsX();
	y1 = y() + (height() - h) / 2 - liv->contentsY();
    }
    y1 -= 2;
    p2.translate(-x1,-y1);
    liv->drawBackground( &p2, QRect(x1,y1,w,h+4) );
    int bounceY = 2;
#ifdef BOUNCE_BUSY_ICON
    if ( busyType == BIT_Animated ) {
       bounceY = 4 - ((iteration+2)%8);
       bounceY = bounceY < 0 ? -bounceY : bounceY;
    }
#endif
    p2.drawPixmap( x1, y1 + bounceY, *pixmap() );
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
    p2.drawPixmap( x1, y1 + bounceY, liv->busyPixmap(), w * pic, 0, w, h );
#endif
    p->drawPixmap( x1, y1, dblBuf );
}

void LauncherItem::animateIcon()
{
    LauncherIconView* liv = (LauncherIconView*)iconView();

    if ( liv->busyItem() != this || !app )
	return;

    // Highlight the icon
    if ( iteration == 0 ) {
	QPixmap src = isBigIcon ? app->bigPixmap() : app->pixmap();
	QImage img = src.convertToImage();
	QRgb *rgb;
	int count;
	if ( img.depth() == 32 ) {
	    rgb = (QRgb*)img.bits();
	    count = img.bytesPerLine()/sizeof(QRgb)*img.height();
	} else {
	    rgb = img.colorTable();
	    count = img.numColors();
	}
	for ( int r = 0; r < count; r++, rgb++ ) {
#if defined(BRIGHTEN_BUSY_ICON)
	    QColor c(*rgb);
	    int h, s, v;
	    c.hsv(&h,&s,&v);
	    c.setHsv(h,QMAX(s-24,0),QMIN(v+48,255));
	    *rgb = qRgba(c.red(),c.green(),c.blue(),qAlpha(*rgb));
#elif defined(ALPHA_FADE_BUSY_ICON)
	    *rgb = qRgba(qRed(*rgb),qGreen(*rgb),qBlue(*rgb),qAlpha(*rgb)/2);
#endif
	}
	src.convertFromImage( img );
	setPixmap( src );
    }

    iteration++;

    // Paint animation overlay
    QPainter p( liv->viewport() );
    paintAnimatedIcon( &p );
}

void LauncherItem::resetIcon()
{
    iteration = 0;
    setPixmap( isBigIcon ? app->bigPixmap() : app->pixmap() );
}

//===========================================================================

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
	(void) new LauncherItem( this, app, bigIcns );
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
    viewport()->setUpdatesEnabled( FALSE );
    hidden.setAutoDelete(FALSE);
    QList<AppLnk> links=hidden;
    hidden.clear();
    hidden.setAutoDelete(TRUE);
    LauncherItem* item = (LauncherItem*)firstItem();
    while (item) {
	links.append(item->takeAppLnk());
	item = (LauncherItem*)item->nextItem();
    }
    clear();
    QListIterator<AppLnk> it(links);
    AppLnk* l;
    while ((l=it.current())) {
	addItem(l,FALSE);
	++it;
    }
    if ( resort && !autoArrange() )
	sort();
    viewport()->setUpdatesEnabled( TRUE );
}

bool LauncherIconView::removeLink(const QString& linkfile)
{
    LauncherItem* item = (LauncherItem*)firstItem();
    AppLnk* l;
    bool did = FALSE;
    DocLnk dl(linkfile);
    while (item) {
	l = item->appLnk();
	LauncherItem *nextItem = (LauncherItem *)item->nextItem();
	if (  l->linkFileKnown() && l->linkFile() == linkfile
		|| l->fileKnown() && (
		    l->file() == linkfile
		    || dl.isValid() && dl.file() == l->file() ) ) {
	    delete item;
	    did = TRUE;
	}
	item = nextItem;
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

//===========================================================================

LauncherView::LauncherView( QWidget* parent, const char* name, WFlags fl )
    : QVBox( parent, name, fl )
{
    catmb = 0;
    icons = new LauncherIconView( this );
    setFocusProxy(icons);
    QPEApplication::setStylusOperation( icons->viewport(), QPEApplication::RightOnHold );

    icons->setItemsMovable( FALSE );
    icons->setAutoArrange( TRUE );
    icons->setSorting( TRUE );
    icons->setFrameStyle( QFrame::NoFrame );
    icons->setMargin( 0 );
    icons->setSelectionMode( QIconView::NoSelection );
    icons->setBackgroundMode( PaletteBase );
    icons->setResizeMode( QIconView::Fixed );
    vmode = (ViewMode)-1;
    setViewMode( Icon );

    connect( icons, SIGNAL(mouseButtonClicked(int, QIconViewItem *, const QPoint&)),
		   SLOT(itemClicked(int, QIconViewItem *)) );
    connect( icons, SIGNAL(selectionChanged()),
		   SLOT(selectionChanged()) );
    connect( icons, SIGNAL(returnPressed(QIconViewItem *)),
		   SLOT(returnPressed(QIconViewItem *)) );
    connect( icons, SIGNAL(mouseButtonPressed(int, QIconViewItem *, const QPoint&)),
		   SLOT(itemPressed(int, QIconViewItem *)) );

    tools = 0;
    setBackgroundType( Ruled, QString::null );
}

LauncherView::~LauncherView()
{
    if ( bgCache && bgCache->contains( bgName ) )
	(*bgCache)[bgName]->ref--;
}

void LauncherView::hideIcons()
{
    icons->hide();
}

void LauncherView::setToolsEnabled(bool y)
{
    if ( !y != !tools ) {
	if ( y ) {
	    tools = new QHBox(this);

	    // Type filter
	    typemb = new QComboBox(tools);
	    QSizePolicy p = typemb->sizePolicy();
	    p.setHorData(QSizePolicy::Expanding);
	    typemb->setSizePolicy(p);

	    // Category filter
	    updateTools();
	    tools->show();

	    // Always show vscrollbar
	    icons->setVScrollBarMode( QScrollView::AlwaysOn );
	} else {
	    delete tools;
	    tools = 0;
	}
    }
}

void LauncherView::updateTools()
{
    disconnect( typemb, SIGNAL(activated(int)),
	        this, SLOT(showType(int)) );
    if ( catmb ) disconnect( catmb, SIGNAL(signalSelected(int)),
	        this, SLOT(showCategory(int)) );

    // ### I want to remove this
    icons->updateCategoriesAndMimeTypes();

    QString prev;

    // Type filter
    QStringList types;
    typelist = icons->mimeTypes();
    for (QStringList::ConstIterator it = typelist.begin(); it!=typelist.end(); ++it) {
	QString t = *it;
	if ( t.left(12) == "application/" ) {
	    MimeType mt(t);
	    const AppLnk* app = mt.application();
	    if ( app )
		t = app->name();
	    else
		t = t.mid(12);
	} else {
	    t[0] = t[0].upper();
	}
	types += t;
    }
    types << tr("All types");
    prev = typemb->currentText();
    typemb->clear();
    typemb->insertStringList(types);
    for (int i=0; i<typemb->count(); i++) {
	if ( typemb->text(i) == prev ) {
	    typemb->setCurrentItem(i);
	    break;
	}
    }
    if ( prev.isNull() )
	typemb->setCurrentItem(typemb->count()-1);

    int pcat = catmb ? catmb->currentCategory() : -2;
    if ( !catmb )
	catmb = new CategorySelect(tools);
    Categories cats( 0 );
    cats.load( categoryFileName() );
    QArray<int> vl( 0 );
    catmb->setCategories( vl, "Document View", // No tr
	tr("Document View") );
    catmb->setRemoveCategoryEdit( TRUE );
    catmb->setAllCategories( TRUE );
    catmb->setCurrentCategory(pcat);

    // if type has changed we need to redisplay
    if ( typemb->currentText() != prev )
	showType( typemb->currentItem() );

    connect(typemb, SIGNAL(activated(int)), this, SLOT(showType(int)));
    connect(catmb, SIGNAL(signalSelected(int)), this, SLOT(showCategory(int)));
}

void LauncherView::sortBy(int s)
{
    icons->setSortMethod((LauncherIconView::SortMethod)s);
}

void LauncherView::showType(int t)
{
    if ( t >= (int)typelist.count() ) {
	icons->setTypeFilter("",TRUE);
    } else {
	QString ty = typelist[t];
	if ( !ty.contains('/') )
	    ty += "/*";
	icons->setTypeFilter(ty,TRUE);
    }
}

void LauncherView::showCategory( int c )
{
    icons->setCategoryFilter( c, TRUE );
}

void LauncherView::setViewMode( ViewMode m )
{
    if ( vmode != m ) {
	bool bigIcons = m == Icon;
	icons->viewport()->setUpdatesEnabled( FALSE );
	icons->setBigIcons( bigIcons );
	switch ( m ) {
	    case List:
		icons->setItemTextPos( QIconView::Right );
		break;
	    case Icon:
		icons->setItemTextPos( QIconView::Bottom );
		break;
	}
	icons->hideOrShowItems( FALSE );
	icons->viewport()->setUpdatesEnabled( TRUE );
	vmode = m;
    }
}

//
// User images may require scaling.
//
QImage LauncherView::loadBackgroundImage(QString &bgName)
{
    QImageIO imgio;
    QSize   ds = qApp->desktop()->size();   // should be launcher, not desktop
    bool    further_scaling = TRUE;

    imgio.setFileName( bgName );
    imgio.setParameters("GetHeaderInformation");

    if (imgio.read() == FALSE) {
	return imgio.image();
    }

    if (imgio.image().width() < ds.width() &&
	    imgio.image().height() < ds.height()) {
	further_scaling = FALSE;
    }

    if (!imgio.image().bits()) {
	//
	// Scale and load.  Note we don't scale up.
	//
	QString param( "Scale( %1, %2, ScaleMin )" ); // No tr
	imgio.setParameters(further_scaling ?
	    param.arg(ds.width()).arg(ds.height()).latin1() :
	    "");
	imgio.read();
    } else {
	if (further_scaling) {
	    int	t1 = imgio.image().width() * ds.height();
	    int t2 = imgio.image().height() * ds.width();
	    int dsth = ds.height();
	    int dstw = ds.width();

	    if (t1 > t2) {
		dsth = t2 / imgio.image().width();
	    } else {
		dstw = t1 / imgio.image().height();
	    }

	    //
	    // Loader didn't scale for us.  Do it manually.
	    //
	    return imgio.image().smoothScale(dstw, dsth);
	}
    }

    return imgio.image();
}

void LauncherView::setBackgroundType( BackgroundType t, const QString &val )
{
    if ( !bgCache ) {
	bgCache = new QMap<QString,BgPixmap*>;
	qAddPostRoutine( cleanup_cache );
    }

    if ( bgCache->contains( bgName ) )
	(*bgCache)[bgName]->ref--;
    bgName = "";

    QPixmap bg;

    switch ( t ) {
	case Ruled: {
	    bgName = QString("Ruled_%1").arg(colorGroup().background().name()); // No tr
	    if ( bgCache->contains( bgName ) ) {
		(*bgCache)[bgName]->ref++;
		bg = (*bgCache)[bgName]->pm;
	    } else {
		bg.resize( width(), 9 );
		QPainter painter( &bg );
		for ( int i = 0; i < 3; i++ ) {
		    painter.setPen( white );
		    painter.drawLine( 0, i*3, width()-1, i*3 );
		    painter.drawLine( 0, i*3+1, width()-1, i*3+1 );
		    painter.setPen( colorGroup().background().light(105) );
		    painter.drawLine( 0, i*3+2, width()-1, i*3+2 );
		}
		painter.end();
		bgCache->insert( bgName, new BgPixmap(bg) );
	    }
	    break;
	}

	case Image:
	    if (!val.isEmpty()) {
		bgName = val;
		if ( bgCache->contains( bgName ) ) {
		    (*bgCache)[bgName]->ref++;
		    bg = (*bgCache)[bgName]->pm;
		} else {
		    QString imgFile = bgName;
		    bool tile = FALSE;
		    if ( imgFile[0]!='/' || !QFile::exists(imgFile) ) {
			imgFile = Resource::findPixmap( imgFile );
			tile = TRUE;
		    }
		    QImage img = loadBackgroundImage(imgFile);


		    if ( img.depth() == 1 )
			img = img.convertDepth(8);
		    img.setAlphaBuffer(FALSE);
		    bg.convertFromImage(img);
		    bgCache->insert( bgName, new BgPixmap(bg) );
		}
	    }
	    break;

	case SolidColor:
	default:
	    break;
    }

    const QObjectList *list = queryList( "QWidget", 0, FALSE );
    QObject *obj;
    for ( QObjectListIt it( *list ); (obj=it.current()); ++it ) {
	if ( obj->isWidgetType() ) {
	    QWidget *w = (QWidget*)obj;
	    w->setBackgroundPixmap( bg );
	    if ( bgName.isEmpty() ) {
		// Solid Color
		if ( val.isEmpty() )
		    w->setBackgroundColor( colorGroup().base() );
		else
		    w->setBackgroundColor( val );
	    } else {
		// Ruled or Image pixmap
		w->setBackgroundOrigin( ParentOrigin );
	    }
	}
    }
    delete list;

    bgType = t;
    icons->viewport()->update();

    QTimer::singleShot( 1000, this, SLOT(flushBgCache()) );
}

void LauncherView::setTextColor( const QColor &tc )
{
    textCol = tc;
    QColorGroup cg = icons->colorGroup();
    cg.setColor( QColorGroup::Text, tc );
    icons->setPalette( QPalette(cg,cg,cg) );
    icons->viewport()->update();
}

void LauncherView::setViewFont( const QFont &f )
{
    icons->setFont( f );
    icons->hideOrShowItems( FALSE );
}

void LauncherView::clearViewFont()
{
    icons->unsetFont();
    icons->hideOrShowItems( FALSE );
}

void LauncherView::resizeEvent(QResizeEvent *e)
{
    QVBox::resizeEvent( e );
    if ( e->size().width() != e->oldSize().width() )
	sort();
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
	else if ( btn == ShiftButton )
	    emit rightPressed( appLnk );
	item->setSelected(FALSE);
    }
}

void LauncherView::removeAllItems()
{
    icons->clear();
}

bool LauncherView::removeLink(const QString& linkfile)
{
    return icons->removeLink(linkfile);
}

void LauncherView::setSortEnabled( bool v )
{
    icons->setSorting( v );
    if ( v )
	sort();
}

void LauncherView::setUpdatesEnabled( bool u )
{
    icons->setUpdatesEnabled( u );
}

void LauncherView::sort()
{
    icons->sort();
}

void LauncherView::addItem(AppLnk* app, bool resort)
{
    icons->addItem(app,resort);
}

void LauncherView::paletteChange( const QPalette &p )
{
    icons->unsetPalette();
    QVBox::paletteChange( p );
    if ( bgType == Ruled )
	setBackgroundType( Ruled, QString::null );
    QColorGroup cg = icons->colorGroup();
    cg.setColor( QColorGroup::Text, textCol );
    icons->setPalette( QPalette(cg,cg,cg) );
}

void LauncherView::fontChanged(const QFont&)
{
    qDebug("LauncherView::fontChanged()");
    icons->hideOrShowItems( FALSE );
}

void LauncherView::relayout(void)
{
    icons->hideOrShowItems(FALSE);
}

void LauncherView::flushBgCache()
{
    if ( !bgCache )
	return;
    // remove unreferenced backgrounds.
    QMap<QString,BgPixmap*>::Iterator it = bgCache->begin();
    while ( it != bgCache->end() ) {
	QMap<QString,BgPixmap*>::Iterator curr = it;
	++it;
	if ( (*curr)->ref == 0 ) {
	    delete (*curr);
	    bgCache->remove( curr );
	}
    }
}

