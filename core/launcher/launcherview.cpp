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

#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
#include <qpe/qpedebug.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>
#include <qpe/menubutton.h>
#include <qpe/mimetype.h>
#include <qpe/resource.h>
#include <qpe/qpetoolbar.h>
//#include <qtopia/private/palmtoprecord.h>

#include <qtimer.h>
#include <qtextstream.h>
#include <qdict.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qhbox.h>
#include <qiconview.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qtoolbutton.h>
#include <qimage.h>


class BgPixmap
{
public:
    BgPixmap( const QPixmap &p ) : pm(p), ref(1) {}
    QPixmap pm;
    int ref;
};

enum BusyIndicatorType {
	BIT_Normal = 0,
	BIT_Blinking
};

static QMap<QString,BgPixmap*> *bgCache = 0;

class LauncherIconView : public QIconView {
public:
    LauncherIconView( QWidget* parent, const char* name=0 ) :
	QIconView(parent,name),
	tf(""),
	cf(0),
	bsy(0),
	bigIcns(TRUE),
	bgColor(white)
    {
	sortmeth = Name;
	hidden.setAutoDelete(TRUE);
	ike = FALSE;
	busytimer = 0;
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
    
    void setBusyIndicatorType ( BusyIndicatorType t ) { busyType = t; }

    QPixmap* busyPixmap() const { return (QPixmap*)&bpm[::abs(busystate)]; }
    QIconViewItem* busyItem() const { return bsy; }
    void setBigIcons( bool bi ) { bigIcns = bi; }

    void updateCategoriesAndMimeTypes();

    void doAutoScroll()
    {
	// We don't want rubberbanding (yet)
    }

    void setBusy(bool on)
    {
	QIconViewItem *c = on ? currentItem() : 0;
	
	if ( bsy != c ) {
	    QIconViewItem *oldbsy = bsy;
	    bsy = c;
	
	    if ( oldbsy )
		oldbsy-> repaint ( );
	
	    if ( bsy ) {
		QPixmap *src = bsy-> QIconViewItem::pixmap();
	    	for ( int i = 0; i <= 5; i++ ) {
		    QImage img = src->convertToImage();
		    QRgb* rgb;
		    int count;
		    if ( img.depth() == 32 ) {
			rgb = (QRgb*)img.bits();
			count = img.bytesPerLine()/sizeof(QRgb)*img.height();
		    } else {
			rgb = img.colorTable();
			count = img.numColors();
		    }
		    int rc, gc, bc;
		    int bs = ::abs ( i * 10 ) + 25;
		    colorGroup().highlight().rgb( &rc, &gc, &bc );
		    rc = rc * bs / 100;
		    gc = gc * bs / 100;
		    bc = bc * bs / 100;
		
		    for ( int r = 0; r < count; r++, rgb++ ) {
			int ri = rc + qRed   ( *rgb ) * ( 100 - bs ) / 100;
			int gi = gc + qGreen ( *rgb ) * ( 100 - bs ) / 100;
			int bi = bc + qBlue  ( *rgb ) * ( 100 - bs ) / 100;
			int ai = qAlpha ( *rgb );
			*rgb = qRgba ( ri, gi, bi, ai );
		    }

		    bpm [i].convertFromImage( img );
		}
		if ( busyType == BIT_Blinking ) {
		    busystate = 0;	
		    if ( busytimer )
			killTimer ( busytimer );
		    busytimer = startTimer ( 200 );
		}
		else
		    busystate = 3;
		timerEvent ( 0 );
	    }
	    else {
		killTimer ( busytimer );
		busytimer = 0;
	    }
	}
    }

    virtual void timerEvent ( QTimerEvent *te )
    {
	if ( !te || ( te-> timerId ( ) == busytimer )) {
	    if ( bsy ) {
		busystate++;
		if ( busystate > 5 )
			busystate = -4;
			
		QScrollView::updateContents ( bsy-> pixmapRect ( false ));
	    }
	}
    }

    bool inKeyEvent() const { return ike; }
    void keyPressEvent(QKeyEvent* e)
    {
	ike = TRUE;
	if ( e->key() == Key_F33 /* OK button */ || e->key() == Key_Space )
	    returnPressed(currentItem());
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

    void setBackgroundPixmap( const QPixmap &pm ) {
	if ( pm. isNull ( )) {
	    bgPixmap = pm;
	}
	else {    
	    // This is need for bg images with alpha channel	
	
	    QPixmap tmp ( pm. size ( ), pm. depth ( ));
    	
	    QPainter p ( &tmp );
	    p. fillRect ( 0, 0, pm. width ( ), pm. height ( ), bgColor. isValid ( ) ? bgColor : white );
	    p. drawPixmap ( 0, 0, pm );    	
	    p. end ( );
    
	   bgPixmap = tmp;
	}
    }

    void setBackgroundColor( const QColor &c ) {
	bgColor = c;
    }

    void drawBackground( QPainter *p, const QRect &r )
    {
	if ( !bgPixmap.isNull() ) {
	    //p-> fillRect ( r, bgColor );
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

    QString getAllDocLinkInfo() const;

protected:

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

private:
    QList<AppLnk> hidden;
    QDict<void> mimes;
    QDict<void> cats;
    SortMethod sortmeth;
    QRegExp tf;
    int cf;
    QIconViewItem* bsy;
    bool ike;
    bool bigIcns;
    QPixmap bgPixmap;
    QPixmap bpm [6];
    QColor bgColor;
    int busytimer;
    int busystate;
    BusyIndicatorType busyType;
};


bool LauncherView::bsy=FALSE;

void LauncherView::setBusy(bool on)
{
    icons->setBusy(on);
}

class LauncherItem : public QIconViewItem
{
public:
    LauncherItem( QIconView *parent, AppLnk* applnk, bool bigIcon=TRUE );
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
    }

    virtual QPixmap* pixmap () const
    {
	const LauncherIconView* liv = (LauncherIconView*)iconView();
	if ( (const LauncherItem *)liv->busyItem() == this )
	    return liv->busyPixmap();
	return QIconViewItem::pixmap();
    }

protected:
    AppLnk* app;
};


LauncherItem::LauncherItem( QIconView *parent, AppLnk *applnk, bool bigIcon )
    : QIconViewItem( parent, applnk->name(),
           bigIcon ? applnk->bigPixmap() :applnk->pixmap() ),
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
    hidden.setAutoDelete(FALSE);
    QList<AppLnk> links=hidden;
    hidden.clear();
    hidden.setAutoDelete(TRUE);
    LauncherItem* item = (LauncherItem*)firstItem();
    while (item) {
	links.append(item->takeAppLnk());
	item = (LauncherItem*)item->nextItem();
    }
    viewport()->setUpdatesEnabled( FALSE );
    clear();
    QListIterator<AppLnk> it(links);
    AppLnk* l;
    while ((l=it.current())) {
	addItem(l,FALSE);
	++it;
    }
    viewport()->setUpdatesEnabled( TRUE );
    if ( resort && !autoArrange() )
	sort();
}

bool LauncherIconView::removeLink(const QString& linkfile)
{
    LauncherItem* item = (LauncherItem*)firstItem();
    AppLnk* l;
    bool did = FALSE;
    DocLnk dl(linkfile);
    while (item) {
	l = item->appLnk();
	if ( ( l->linkFileKnown() && ( l->linkFile() == linkfile ))
	  || ( l->fileKnown() && ( l->file() == linkfile ))
	  || ( dl.fileKnown() && l->fileKnown() && ( dl.file() == l->file() )) ) {
	    delete item;
	    did = TRUE;
	}
	item = (LauncherItem*)item->nextItem();
    }
    QListIterator<AppLnk> it(hidden);
    while ((l=it.current())) {
	++it;
	if ( ( l->linkFileKnown() && ( l->linkFile() == linkfile ))
	  || ( l->file() == linkfile )
	  || ( dl.fileKnown() && ( dl.file() == l->file() )) ) {
	    hidden.removeRef(l);
	   did = TRUE;
	}
    }
    return did;
}

static QString docLinkInfo(const Categories& cats, DocLnk* doc)
{
    QString contents;

    QFileInfo fi( doc->file() );
    if ( !fi.exists() )
	return contents;

    if ( doc->linkFileKnown() ) {
	QString lfn = doc->linkFile();
	QFile f( lfn );
	if ( f.open( IO_ReadOnly ) ) {
	    QTextStream ts( &f );
	    ts.setEncoding( QTextStream::UnicodeUTF8 );
	    contents += ts.read();
	    f.close();
	    goto calcsize;
	}
    }

    contents += "[Desktop Entry]\n";
    contents += "Categories = " // No tr
	+ cats.labels("Document View", doc->categories()).join(";") + "\n"; // No tr
    contents += "File = "+doc->file()+"\n"; // No tr
    contents += "Name = "+doc->name()+"\n"; // No tr
    contents += "Type = "+doc->type()+"\n"; // No tr

calcsize:
    contents += QString("Size = %1\n").arg( fi.size() ); // No tr
    return contents;
}

QString LauncherIconView::getAllDocLinkInfo() const
{
    QString contents;
    LauncherItem* item = (LauncherItem*)firstItem();
    Categories cats;
    while (item) {
	DocLnk* doc = (DocLnk*)item->appLnk();
	contents += docLinkInfo(cats,doc);
	item = (LauncherItem*)item->nextItem();
    }
    QListIterator<AppLnk> it(hidden);
    DocLnk* doc;
    while ((doc=(DocLnk*)it.current())) {
	contents += docLinkInfo(cats,doc);
	++it;
    }
    return contents;
}

//===========================================================================

LauncherView::LauncherView( QWidget* parent, const char* name, WFlags fl )
    : QVBox( parent, name, fl )
{
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
}

void LauncherView::setToolsEnabled(bool y)
{
    if ( !y != !tools ) {
	if ( y ) {
	    tools = new QHBox(this);

	    // Type filter
	    typemb = new QComboBox(tools);

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
    disconnect( typemb, SIGNAL(activated(int)),
	        this, SLOT(showType(int)) );
    disconnect( catmb, SIGNAL(signalSelected(int)),
	        this, SLOT(showCategory(int)) );

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
	types += tr("%1 files").arg(t);
    }
    types << tr("All types of file");
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

    Categories cats( 0 );
    cats.load( categoryFileName() );
    QArray<int> vl( 0 );
    catmb->setCategories( vl, "Document View", // No tr
	tr("Document View") );
    catmb->setRemoveCategoryEdit( TRUE );
    catmb->setAllCategories( TRUE );

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

void LauncherView::setBackgroundType( BackgroundType t, const QString &val )
{
    if ( !bgCache )
	bgCache = new QMap<QString,BgPixmap*>;
    if ( bgCache->contains( bgName ) )
	(*bgCache)[bgName]->ref--;

    switch ( t ) {
	case Ruled: {
	    bgName = QString("Ruled_%1").arg(colorGroup().background().name()); // No tr
	    QPixmap bg;
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
	    icons->setBackgroundPixmap( bg );
	    break;
	}

	case SolidColor:
	    icons->setBackgroundPixmap( QPixmap() );
	    if ( val.isEmpty() ) {
		icons->setBackgroundColor( colorGroup().base() );
	    } else {
		icons->setBackgroundColor( val );
	    }
	    bgName = "";
	    break;

	case Image:
	    bgName = val;
	    if ( bgCache->contains( bgName ) ) {
		(*bgCache)[bgName]->ref++;
		icons->setBackgroundPixmap( (*bgCache)[bgName]->pm );
	    } else {
		qDebug( "Loading image: %s", val.latin1() );
		QPixmap bg( Resource::loadPixmap( val ) );
		if ( bg.isNull() ) {
		    QImageIO imgio;
		    imgio.setFileName( bgName );
		    QSize ds = qApp->desktop()->size();
		    QString param( "Scale( %1, %2, ScaleMin )" ); // No tr
		    imgio.setParameters( param.arg(ds.width()).arg(ds.height()).latin1() );
		    imgio.read();
		    bg = imgio.image();
		}
		bgCache->insert( bgName, new BgPixmap(bg) );
		icons->setBackgroundPixmap( bg );
	    }
	    break;
    }

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

    bgType = t;
    icons->viewport()->update();
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

QString LauncherView::getAllDocLinkInfo() const
{
    return icons->getAllDocLinkInfo();
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
	if (QFile(it.current()->file()).exists() || ( it.current()->file().left(4) == "http" )) {
	    icons->addItem(*it,FALSE);
	}
	else {
	    //maybe insert some .desktop file deletion code later
	    //maybe dir specific
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


void LauncherView::setBusyIndicatorType ( const QString &type )
{
    if ( type. lower ( ) == "blink" )
	icons-> setBusyIndicatorType ( BIT_Blinking );
    else
	icons-> setBusyIndicatorType ( BIT_Normal );
}
