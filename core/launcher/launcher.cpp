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

#include "startmenu.h"
#include "taskbar.h"
#include "serverinterface.h"
#include "launcherview.h"
#include "launcher.h"
#include "server.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qtopia/global.h>
#ifdef Q_WS_QWS
#include <qtopia/qcopenvelope_qws.h>
#endif
#include <qtopia/resource.h>
#include <qtopia/applnk.h>
#include <qtopia/config.h>
#include <qtopia/qpeapplication.h>
#include <qtopia/mimetype.h>
#include <qtopia/private/categories.h>
#define QTOPIA_INTERNAL_FSLP
#include <qtopia/lnkproperties.h>

/* QT */
#include <qdir.h>
#ifdef Q_WS_QWS
#include <qkeyboard_qws.h>
#include <qwindowsystem_qws.h>
#endif
#include <qtimer.h>
#include <qcombobox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qstyle.h>
#include <qpushbutton.h>
#include <qtabbar.h>
#include <qwidgetstack.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qframe.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qtextstream.h>
#include <qpopupmenu.h>

/* STD */
#include <stdlib.h>
#include <assert.h>
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <unistd.h>
#include <stdio.h>
#include <sys/vfs.h>
#include <mntent.h>
#endif


static bool isVisibleWindow( int );
//===========================================================================

LauncherTabWidget::LauncherTabWidget( Launcher* parent ) :
    QVBox( parent ), docview( 0 )
{
    docLoadingWidgetEnabled = false;
    docLoadingWidget = 0;
    docLoadingWidgetProgress = 0;
    launcher = parent;
    categoryBar = new LauncherTabBar( this );
    QPalette pal = categoryBar->palette();
    pal.setColor( QColorGroup::Light, pal.color(QPalette::Active,QColorGroup::Shadow) );
    pal.setColor( QColorGroup::Background, pal.active().background().light(110) );
    categoryBar->setPalette( pal );
    stack = new QWidgetStack(this);
    connect( categoryBar, SIGNAL(selected(int)), this, SLOT(raiseTabWidget()) );
    categoryBar->show();
    stack->show();

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel *channel = new QCopChannel( "QPE/Launcher", this );
    connect( channel, SIGNAL(received(const QCString&,const QByteArray&)),
             this, SLOT(launcherMessage(const QCString&,const QByteArray&)) );
    connect( qApp, SIGNAL(appMessage(const QCString&,const QByteArray&)),
	     this, SLOT(appMessage(const QCString&,const QByteArray&)));
#endif

    createDocLoadingWidget();
}

void LauncherTabWidget::createDocLoadingWidget()
{
    // Construct the 'doc loading widget' shown when finding documents

    // ### LauncherView class needs changing to be more generic so
    // this widget can change its background similar to the iconviews
    // so the background for this matches
    docLoadingWidget = new LauncherView( stack );
    docLoadingWidget->hideIcons();
    QVBox *docLoadingVBox = new QVBox( docLoadingWidget );

    docLoadingVBox->setSpacing( 20 );
    docLoadingVBox->setMargin( 10 );

    QWidget *space1 = new QWidget( docLoadingVBox );
    docLoadingVBox->setStretchFactor( space1, 1 );

    QLabel *waitPixmap = new QLabel( docLoadingVBox );
    waitPixmap->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, waitPixmap->sizePolicy().hasHeightForWidth() ) );
    waitPixmap->setPixmap( Resource::loadPixmap( "bigwait" ) );
    waitPixmap->setAlignment( int( QLabel::AlignCenter ) );

    Config cfg( "Launcher" );
    cfg.setGroup( "DocTab" );
    bool docTabEnabled = cfg.readBoolEntry( "Enable", true );

    QLabel *textLabel = new QLabel( docLoadingVBox );
    textLabel->setAlignment( int( QLabel::AlignCenter ) );
    docLoadingWidgetProgress = new QProgressBar( docLoadingVBox );
    docLoadingWidgetProgress->setProgress( 0 );
    docLoadingWidgetProgress->setCenterIndicator( TRUE );
    docLoadingWidgetProgress->setBackgroundMode( NoBackground ); // No flicker
    setProgressStyle();

    if ( docTabEnabled )
    {
        textLabel->setText( tr( "<b>Finding Documents...</b>" ) );
    }
    else
    {
        textLabel->setText( tr( "<b>The Documents Tab<p>has been disabled.<p>"
                                "Use Settings->Launcher->DocTab<p>to reenable it.</b></center>" ) );
        docLoadingWidgetProgress->hide();
        docLoadingWidgetEnabled = true;
    }

    QWidget *space2 = new QWidget( docLoadingVBox );
    docLoadingVBox->setStretchFactor( space2, 1 );

    cfg.setGroup( "Tab Documents" ); // No tr
    setTabViewAppearance( docLoadingWidget, cfg );

    stack->addWidget( docLoadingWidget, 0 );
}

void LauncherTabWidget::initLayout()
{
    layout()->activate();
    docView()->setFocus();
    categoryBar->showTab("Documents");
}

void LauncherTabWidget::appMessage(const QCString& message, const QByteArray&)
{
    if ( message == "nextView()" )
	categoryBar->nextTab();
}

void LauncherTabWidget::raiseTabWidget()
{
    if ( categoryBar->currentView() == docView()
	 && docLoadingWidgetEnabled ) {
	stack->raiseWidget( docLoadingWidget );
	docLoadingWidget->updateGeometry();
    } else {
	stack->raiseWidget( categoryBar->currentView() );
    }
}

void LauncherTabWidget::tabProperties()
{
    LauncherView *view = categoryBar->currentView();
    QPopupMenu *m = new QPopupMenu( this );
    m->insertItem( tr("Icon View"), LauncherView::Icon );
    m->insertItem( tr("List View"), LauncherView::List );
    m->setItemChecked( (int)view->viewMode(), TRUE );
    int rv = m->exec( QCursor::pos() );
    if ( rv >= 0 && rv != view->viewMode() ) {
	view->setViewMode( (LauncherView::ViewMode)rv );
    }

    delete m;
}

void LauncherTabWidget::deleteView( const QString& id )
{
    LauncherTab *t = categoryBar->launcherTab(id);
    if ( t ) {
	stack->removeWidget( t->view );
	delete t->view;
	categoryBar->removeTab( t );
    }
}

LauncherView* LauncherTabWidget::newView( const QString& id, const QPixmap& pm, const QString& label )
{
    LauncherView* view = new LauncherView( stack );
    connect( view, SIGNAL(clicked(const AppLnk*)),
	    this, SIGNAL(clicked(const AppLnk*)));
    connect( view, SIGNAL(rightPressed(AppLnk*)),
	    this, SIGNAL(rightPressed(AppLnk*)));

    int n = categoryBar->count();
    stack->addWidget( view, n );

    LauncherTab *tab = new LauncherTab( id, view, pm, label );
    categoryBar->insertTab( tab, n-1 );

    if ( id == "Documents" )
	docview = view;

    odebug << "inserting " << id << " at " << n-1 << "" << oendl;

    Config cfg("Launcher");
    setTabAppearance( tab, cfg );

    cfg.setGroup( "GUI" );
    view->setBusyIndicatorType( cfg.readEntry( "BusyType", QString::null ) );

    return view;
}

LauncherView *LauncherTabWidget::view( const QString &id )
{
    LauncherTab *t = categoryBar->launcherTab(id);
    if ( !t )
	return 0;
    return t->view;
}

LauncherView *LauncherTabWidget::docView()
{
    return docview;
}

void LauncherTabWidget::setLoadingWidgetEnabled( bool v )
{
    if ( v != docLoadingWidgetEnabled && docLoadingWidget ) {
	docLoadingWidgetEnabled = v;
	raiseTabWidget();
    }
}

void LauncherTabWidget::setLoadingProgress( int percent )
{
    docLoadingWidgetProgress->setProgress( (percent / 4) * 4 );
}

// ### this function could more to LauncherView
void LauncherTabWidget::setTabViewAppearance( LauncherView *v, Config &cfg )
{
    // View
    QString view = cfg.readEntry( "View", "Icon" );
    if ( view == "List" ) // No tr
	v->setViewMode( LauncherView::List );
    QString bgType = cfg.readEntry( "BackgroundType", "Image" );
    if ( bgType == "Image" ) { // No tr
	QString pm = cfg.readEntry( "BackgroundImage", "launcher/opie-background" );
	v->setBackgroundType( LauncherView::Image, pm );
    } else if ( bgType == "SolidColor" ) {
	QString c = cfg.readEntry( "BackgroundColor" );
	v->setBackgroundType( LauncherView::SolidColor, c );
    } else {
	v->setBackgroundType( LauncherView::Ruled, QString::null );
    }
    QString textCol = cfg.readEntry( "TextColor" );
    if ( textCol.isEmpty() )
	v->setTextColor( QColor() );
    else
	v->setTextColor( QColor(textCol) );
//    bool customFont = cfg.readBoolEntry( "CustomFont", FALSE );


    QStringList font = cfg.readListEntry( "Font", ',' );
    if ( font.count() == 4 )
        v->setViewFont( QFont(font[0], font[1].toInt(), font[2].toInt(), font[3].toInt()!=0) );

    // ### FIXME TabColor TabTextColor

}

// ### Could move to LauncherTab
void LauncherTabWidget::setTabAppearance( LauncherTab *tab, Config &cfg )
{
    cfg.setGroup( QString( "Tab %1" ).arg(tab->type) ); // No tr

    setTabViewAppearance( tab->view, cfg );

    // Tabs
    QString tabCol = cfg.readEntry( "TabColor" );
    if ( tabCol.isEmpty() )
	tab->bgColor = QColor();
    else
	tab->bgColor = QColor(tabCol);
    QString tabTextCol = cfg.readEntry( "TabTextColor" );
    if ( tabTextCol.isEmpty() )
	tab->fgColor = QColor();
    else
	tab->fgColor = QColor(tabTextCol);
}

void LauncherTabWidget::paletteChange( const QPalette &p )
{
    QVBox::paletteChange( p );
    QPalette pal = palette();
    pal.setColor( QColorGroup::Light, pal.color(QPalette::Active,QColorGroup::Shadow) );
    pal.setColor( QColorGroup::Background, pal.active().background().light(110) );
    categoryBar->setPalette( pal );
    categoryBar->update();
}

void LauncherTabWidget::styleChange( QStyle & )
{
    QTimer::singleShot( 0, this, SLOT(setProgressStyle()) );
}

void LauncherTabWidget::setProgressStyle()
{
    if (docLoadingWidgetProgress) {
	docLoadingWidgetProgress->setFrameShape( QProgressBar::Box );
	docLoadingWidgetProgress->setFrameShadow( QProgressBar::Plain );
	docLoadingWidgetProgress->setMargin( 1 );
	docLoadingWidgetProgress->setLineWidth( 1 );
    }
}

void LauncherTabWidget::setBusy(bool on)
{
    if ( on )
	currentView()->setBusy(TRUE);
    else {
	for ( int i = 0; i < categoryBar->count(); i++ ) {
	    LauncherView *view = ((LauncherTab *)categoryBar->tab(i))->view;
	    view->setBusy( FALSE );
	}
    }
}

void LauncherTabWidget::setBusyIndicatorType( const QString& str ) {
    for (int i = 0; i < categoryBar->count(); i++ ) {
        LauncherView* view = static_cast<LauncherTab*>( categoryBar->tab(i) )->view;
        view->setBusyIndicatorType( str );
    }
}

LauncherView *LauncherTabWidget::currentView(void)
{
    return (LauncherView*)stack->visibleWidget();
}



void LauncherTabWidget::launcherMessage( const QCString &msg, const QByteArray &data)
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "setTabView(QString,int)" ) {
	QString id;
	stream >> id;
	int mode;
	stream >> mode;
	if ( view(id) )
	    view(id)->setViewMode( (LauncherView::ViewMode)mode );
    } else if ( msg == "setTabBackground(QString,int,QString)" ) {
	QString id;
	stream >> id;
	int mode;
	stream >> mode;
	QString pixmapOrColor;
	stream >> pixmapOrColor;
	if ( view(id) )
	    view(id)->setBackgroundType( (LauncherView::BackgroundType)mode, pixmapOrColor );
	if ( id == "Documents" )
	    docLoadingWidget->setBackgroundType( (LauncherView::BackgroundType)mode, pixmapOrColor );
    } else if ( msg == "setTextColor(QString,QString)" ) {
	QString id;
	stream >> id;
	QString color;
	stream >> color;
	if ( view(id) )
	    view(id)->setTextColor( QColor(color) );
	if ( id == "Documents" )
	    docLoadingWidget->setTextColor( QColor(color) );
    } else if ( msg == "setFont(QString,QString,int,int,int)" ) {
	QString id;
	stream >> id;
	QString fam;
	stream >> fam;
	int size;
	stream >> size;
	int weight;
	stream >> weight;
	int italic;
	stream >> italic;
	if ( view(id) ) {
	    if ( !fam.isEmpty() ) {
		view(id)->setViewFont( QFont(fam, size, weight, italic!=0) );
        odebug << "setFont: " << fam << ", " << size << ", " << weight << ", " << italic << "" << oendl;
	    } else {
		view(id)->clearViewFont();
	    }
	}
    }else if ( msg == "setBusyIndicatorType(QString)" ) {
        QString type;
        stream >> type;
        setBusyIndicatorType( type );
    }else if ( msg == "home()" ) {
        if ( isVisibleWindow( static_cast<QWidget*>(parent())->winId() ) ) {
            if (categoryBar)
                categoryBar->nextTab();
        }else
            static_cast<QWidget*>(parent())->raise();
    }
}



//---------------------------------------------------------------------------

Launcher::Launcher()
    : QMainWindow( 0, "PDA User Interface", QWidget::WStyle_Customize | QWidget::WGroupLeader )
{
    tabs = 0;
    tb = 0;
    Config cfg( "Launcher" );
    cfg.setGroup( "DocTab" );
    docTabEnabled = cfg.readBoolEntry( "Enable", true );
}

void Launcher::createGUI()
{
    setCaption( tr("Launcher") );

    // we have a pretty good idea how big we'll be
    setGeometry( 0, 0, qApp->desktop()->width(), qApp->desktop()->height() );

    tb = new TaskBar;
    tabs = new LauncherTabWidget( this );
    setCentralWidget( tabs );

    ServerInterface::dockWidget( tb, ServerInterface::Bottom );
    tb->show();

    qApp->installEventFilter( this );


    connect( qApp, SIGNAL(symbol()), this, SLOT(toggleSymbolInput()) );
    connect( qApp, SIGNAL(numLockStateToggle()), this, SLOT(toggleNumLockState()) );
    connect( qApp, SIGNAL(capsLockStateToggle()), this, SLOT(toggleCapsLockState()) );

    connect( tb, SIGNAL(tabSelected(const QString&)),
	this, SLOT(showTab(const QString&)) );
    connect( tabs, SIGNAL(selected(const QString&)),
	this, SLOT(viewSelected(const QString&)) );
    connect( tabs, SIGNAL(clicked(const AppLnk*)),
	this, SLOT(select(const AppLnk*)));
    connect( tabs, SIGNAL(rightPressed(AppLnk*)),
	this, SLOT(properties(AppLnk*)));

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel* sysChannel = new QCopChannel( "QPE/System", this );
    connect( sysChannel, SIGNAL(received(const QCString&,const QByteArray&)),
             this, SLOT(systemMessage(const QCString&,const QByteArray&)) );
#endif

    // all documents
    QImage img( Resource::loadImage( "DocsIcon" ) );
    QPixmap pm;
    pm = img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
    // It could add this itself if it handles docs

    tabs->newView("Documents", pm, tr("Documents") )->setToolsEnabled( TRUE );

    QTimer::singleShot( 0, tabs, SLOT( initLayout() ) );
    qApp->setMainWidget( this );
    QTimer::singleShot( 500, this, SLOT( makeVisible() ) );
}

Launcher::~Launcher()
{
    if ( tb )
	destroyGUI();
}

 bool Launcher::requiresDocuments() const
 {
    Config cfg( "Launcher" );
    cfg.setGroup( "DocTab" );
    return cfg.readBoolEntry( "Enable", true );
}

void Launcher::makeVisible()
{
    showMaximized();
}

void Launcher::destroyGUI()
{
    delete tb;
    tb = 0;
    delete tabs;
    tabs =0;
}

bool Launcher::eventFilter( QObject*, QEvent *ev )
{
#ifdef QT_QWS_CUSTOM
    if ( ev->type() == QEvent::KeyPress ) {
	QKeyEvent *ke = (QKeyEvent *)ev;
	if ( ke->key() == Qt::Key_F11 ) { // menu key
	    QWidget *active = qApp->activeWindow();
	    if ( active && active->isPopup() )
		active->close();
	    else {
		Global::terminateBuiltin("calibrate"); // No tr
		tb->launchStartMenu();
	    }
	    return TRUE;
	}
    }
#else
    Q_UNUSED(ev);
#endif
    return FALSE;
}

void Launcher::toggleSymbolInput()
{
    tb->toggleSymbolInput();
}

void Launcher::toggleNumLockState()
{
    tb->toggleNumLockState();
}

void Launcher::toggleCapsLockState()
{
    tb->toggleCapsLockState();
}

static bool isVisibleWindow(int wid)
{
#ifdef Q_WS_QWS
    const QList<QWSWindow> &list = qwsServer->clientWindows();
    QWSWindow* w;
    for (QListIterator<QWSWindow> it(list); (w=it.current()); ++it) {
	if ( w->winId() == wid )
	    return !w->isFullyObscured();
    }
#endif
    return FALSE;
}

void Launcher::viewSelected(const QString& s)
{
    setCaption( s + tr(" - Launcher") );
}

void Launcher::showTab(const QString& id)
{
    tabs->categoryBar->showTab(id);
    raise();
}

void Launcher::select( const AppLnk *appLnk )
{
    if ( appLnk->type() == "Folder" ) { // No tr
	// Not supported: flat is simpler for the user
    } else {
	if ( appLnk->exec().isNull() ) {
	    int i = QMessageBox::information(this,tr("No application"),
		tr("<p>No application is defined for this document."
		"<p>Type is %1.").arg(appLnk->type()), tr("OK"), tr("View as text"), 0, 0, 1);

            /* ### Fixme */
	    if ( i == 1 )
		Global::execute("textedit",appLnk->file());

	    return;
	}
	tabs->setBusy(TRUE);
	emit executing( appLnk );
	appLnk->execute();
    }
}

void Launcher::properties( AppLnk *appLnk )
{
    if ( appLnk->type() == "Folder" ) { // No tr
	// Not supported: flat is simpler for the user
    } else {
/* ### libqtopia FIXME also moving docLnks... */
	LnkProperties prop(appLnk,0 );

	QPEApplication::execDialog( &prop );
    }
}

void Launcher::storageChanged( const QList<FileSystem> &fs )
{
    // ### update combo boxes if we had a combo box for the storage type
}

void Launcher::systemMessage( const QCString &msg, const QByteArray &data)
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "busy()" ) {
	tb->startWait();
    } else if ( msg == "notBusy(QString)" ) {
	QString app;
	stream >> app;
	tabs->setBusy(FALSE);
	tb->stopWait(app);
    } else if (msg == "applyStyle()") {
	tabs->currentView()->relayout();
    }
}

// These are the update functions from the server
void Launcher::typeAdded( const QString& type, const QString& name,
				    const QPixmap& pixmap, const QPixmap& )
{
    tabs->newView( type, pixmap, name );
    ids.append( type );
    /* this will be called in applicationScanningProgress with value 100! */
//    tb->refreshStartMenu();

    static bool first = TRUE;
    if ( first ) {
	first = FALSE;
        tabs->categoryBar->showTab(type);
    }

    tabs->view( type )->setUpdatesEnabled( FALSE );
    tabs->view( type )->setSortEnabled( FALSE );
}

void Launcher::typeRemoved( const QString& type )
{
    tabs->view( type )->removeAllItems();
    tabs->deleteView( type );
    ids.remove( type );
    /* this will be called in applicationScanningProgress with value 100! */
//    tb->refreshStartMenu();
}

void Launcher::applicationAdded( const QString& type, const AppLnk& app )
{
    if ( app.type() == "Separator" )  // No tr
	return;

    LauncherView *view = tabs->view( type );
    if ( view )
	view->addItem( new AppLnk( app ), FALSE );
    else
	qWarning("addAppLnk: No view for type %s. Can't add app %s!",
				  type.latin1(),app.name().latin1()  );

    MimeType::registerApp( app );
}

void Launcher::applicationRemoved( const QString& type, const AppLnk& app )
{
    LauncherView *view = tabs->view( type );
    if ( view )
	view->removeLink( app.linkFile() );
    else
    owarn << "removeAppLnk: No view for " << type << "!" << oendl;
}

void Launcher::allApplicationsRemoved()
{
    MimeType::clear();
    for ( QStringList::ConstIterator it=ids.begin(); it!= ids.end(); ++it)
	tabs->view( (*it) )->removeAllItems();
}

void Launcher::documentAdded( const DocLnk& doc )
{
    tabs->docView()->addItem( new DocLnk( doc ), FALSE );
}

void Launcher::showLoadingDocs()
{
    tabs->docView()->hide();
}

void Launcher::showDocTab()
{
    if ( tabs->categoryBar->currentView() == tabs->docView() )
	tabs->docView()->show();
}

void Launcher::documentRemoved( const DocLnk& doc )
{
    tabs->docView()->removeLink( doc.linkFile() );
}

void Launcher::documentChanged( const DocLnk& oldDoc, const DocLnk& newDoc )
{
    documentRemoved( oldDoc );
    documentAdded( newDoc );
}

void Launcher::allDocumentsRemoved()
{
    tabs->docView()->removeAllItems();
}

void Launcher::applicationStateChanged( const QString& name, ApplicationState state )
{
    tb->setApplicationState( name, state );
}

void Launcher::applicationScanningProgress( int percent )
{
    switch ( percent ) {
        case 0: {
	    for ( QStringList::ConstIterator it=ids.begin(); it!= ids.end(); ++it) {
		tabs->view( (*it) )->setUpdatesEnabled( FALSE );
		tabs->view( (*it) )->setSortEnabled( FALSE );
	    }
	    break;
        }
        case 100: {
	    for ( QStringList::ConstIterator it=ids.begin(); it!= ids.end(); ++it) {
		tabs->view( (*it) )->setUpdatesEnabled( TRUE );
		tabs->view( (*it) )->setSortEnabled( TRUE );
	    }
            tb->refreshStartMenu();
	    break;
        }
        default:
            break;
    }
}

void Launcher::documentScanningProgress( int percent )
{
    switch ( percent ) {
        case 0: {
	    tabs->setLoadingProgress( 0 );
	    tabs->setLoadingWidgetEnabled( TRUE );
	    tabs->docView()->setUpdatesEnabled( FALSE );
	    tabs->docView()->setSortEnabled( FALSE );
	    break;
        }
        case 100: {
	    tabs->docView()->updateTools();
	    tabs->docView()->setSortEnabled( TRUE );
	    tabs->docView()->setUpdatesEnabled( TRUE );
	    tabs->setLoadingWidgetEnabled( FALSE );
	    break;
        }
        default:
	    tabs->setLoadingProgress( percent );
            break;
    }
}

