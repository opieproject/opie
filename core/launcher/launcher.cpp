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

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/mimetype.h>
#include <qpe/storage.h>
#include <qpe/palmtoprecord.h>

#include <qdir.h>
#include <qwindowsystem_qws.h>
#include <qtimer.h>
#include <qcombobox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qstyle.h>
#include <qpushbutton.h>
#include <qtabbar.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qframe.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qtextstream.h>

#include "launcherview.h"
#include "launcher.h"
#include "syncdialog.h"
#include "desktop.h"
#include <qpe/lnkproperties.h>
#include "mrulist.h"
#include "qrsync.h"
#include <stdlib.h>
#include <unistd.h>

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <stdio.h>
#include <sys/vfs.h>
#include <mntent.h>
#endif

//#define SHOW_ALL

CategoryTabWidget::CategoryTabWidget( QWidget* parent ) :
    QVBox( parent )
{
    categoryBar = 0;
    stack = 0;
}

void CategoryTabWidget::prevTab()
{
    if ( categoryBar ) {
	int n = categoryBar->count();
	int tab = categoryBar->currentTab();
	if ( tab >= 0 )
            categoryBar->setCurrentTab( (tab - 1 + n)%n );
    }
}

void CategoryTabWidget::nextTab()
{
    if ( categoryBar ) {
	int n = categoryBar->count();
        int tab = categoryBar->currentTab();
	categoryBar->setCurrentTab( (tab + 1)%n );
    }
}

void CategoryTabWidget::addItem( const QString& linkfile )
{
    int i=0;
    AppLnk *app = new AppLnk(linkfile);
    if ( !app->isValid() ) {
	delete app;
	return;
    }
    if ( !app->file().isEmpty() ) {
	// A document
	delete app;
	app = new DocLnk(linkfile);
	((LauncherView*)(stack->widget(ids.count()-1)))->addItem(app);
	return;
    }
    for ( QStringList::Iterator it=ids.begin(); it!=ids.end(); ++it) {
	if ( !(*it).isEmpty() ) {
	    QRegExp tf(*it,FALSE,TRUE);
	    if ( tf.match(app->type()) >= 0 ) {
		((LauncherView*)stack->widget(i))->addItem(app);
		return;
	    }
	    i++;
	}
    }
}

void CategoryTabWidget::initializeCategories(AppLnkSet* rootFolder,
	AppLnkSet* docFolder, const QList<FileSystem> &fs)
{
    delete categoryBar;
    categoryBar = new CategoryTabBar( this );
    QPalette pal = categoryBar->palette();
    pal.setColor( QColorGroup::Light, pal.color(QPalette::Active,QColorGroup::Shadow) );
    pal.setColor( QColorGroup::Background, pal.active().background().light(110) );
    categoryBar->setPalette( pal );

    delete stack;
    stack = new QWidgetStack(this);
    tabs=0;

    ids.clear();

    QStringList types = rootFolder->types();
    for ( QStringList::Iterator it=types.begin(); it!=types.end(); ++it) {
	if ( !(*it).isEmpty() ) {
	    newView(*it,rootFolder->typePixmap(*it),rootFolder->typeName(*it));
	}
    }
    QListIterator<AppLnk> it( rootFolder->children() );
    AppLnk* l;
    while ( (l=it.current()) ) {
	if ( l->type() == "Separator" ) {
	    rootFolder->remove(l);
	    delete l;
	} else {
	    int i=0;
	    for ( QStringList::Iterator it=types.begin(); it!=types.end(); ++it) {
		if ( *it == l->type() )
		    ((LauncherView*)stack->widget(i))->addItem(l,FALSE);
		i++;
	    }
	}
	++it;
    }
    rootFolder->detachChildren();
    for (int i=0; i<tabs; i++)
	((LauncherView*)stack->widget(i))->sort();

    // all documents
    docview = newView( QString::null, Resource::loadPixmap("DocsIcon"), tr("Documents"));
    docview->populate( docFolder, QString::null );
    docFolder->detachChildren();
    docview->setFileSystems(fs);
    docview->setToolsEnabled(TRUE);

    connect( categoryBar, SIGNAL(selected(int)), stack, SLOT(raiseWidget(int)) );

    ((LauncherView*)stack->widget(0))->setFocus();

    categoryBar->show();
    stack->show();
}

void CategoryTabWidget::updateDocs(AppLnkSet* docFolder, const QList<FileSystem> &fs)
{
    docview->populate( docFolder, QString::null );
    docFolder->detachChildren();
    docview->setFileSystems(fs);
    docview->updateTools();
}

LauncherView* CategoryTabWidget::newView( const QString& id, const QPixmap& pm, const QString& label )
{
    LauncherView* view = new LauncherView( stack );
    connect( view, SIGNAL(clicked(const AppLnk*)),
	    this, SIGNAL(clicked(const AppLnk*)));
    connect( view, SIGNAL(rightPressed(AppLnk*)),
	    this, SIGNAL(rightPressed(AppLnk*)));
    ids.append(id);
    categoryBar->addTab( new QTab( pm, label ) );
    stack->addWidget( view, tabs++ );
    return view;
}

void CategoryTabWidget::updateLink(const QString& linkfile)
{
    int i=0;
    LauncherView* view;
    while ((view = (LauncherView*)stack->widget(i++))) {
	if ( view->removeLink(linkfile) )
	    break;
    }
    addItem(linkfile);
    docview->updateTools();
}

void CategoryTabWidget::paletteChange( const QPalette &p )
{
    QVBox::paletteChange( p );
    QPalette pal = palette();
    pal.setColor( QColorGroup::Light, pal.color(QPalette::Active,QColorGroup::Shadow) );
    pal.setColor( QColorGroup::Background, pal.active().background().light(110) );
    categoryBar->setPalette( pal );
    categoryBar->update();
}

void CategoryTabWidget::setBusy(bool on)
{
    if ( on )
	((LauncherView*)stack->visibleWidget())->setBusy(TRUE);
    else
	for (int i=0; i<tabs; i++)
	    ((LauncherView*)stack->widget(i))->setBusy(FALSE);
}


CategoryTabBar::CategoryTabBar( QWidget *parent, const char *name )
    : QTabBar( parent, name )
{
    setFocusPolicy( NoFocus );
    connect( this, SIGNAL( selected(int) ), this, SLOT( layoutTabs() ) );
}

CategoryTabBar::~CategoryTabBar()
{
}

void CategoryTabBar::layoutTabs()
{
    if ( !count() )
	return;

//    int percentFalloffTable[] = { 100, 70, 40, 12, 6, 3, 1, 0 };
    int hiddenTabWidth = -12;
    int middleTab = currentTab();
    int hframe, vframe, overlap;
    style().tabbarMetrics( this, hframe, vframe, overlap );
    QFontMetrics fm = fontMetrics();
    int x = 0;
    QRect r;
    QTab *t;
    int available = width()-1;
    int required = 0;
    for ( int i = 0; i < count(); i++ ) {
	t = tab(i);
	// if (( i < (middleTab - 1) ) || ( i > (middleTab + 1) )) {
	if ( i != middleTab ) {
	    // required += hiddenTabWidth + hframe - overlap;
	    available -= hiddenTabWidth + hframe - overlap;
	    if ( t->iconSet() != 0 )
		available -= t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
	} else {
	    required += fm.width( t->text() ) + hframe - overlap;
	    if ( t->iconSet() != 0 )
		required += t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
	}
    }
    for ( int i = 0; i < count(); i++ ) {
	t = tab(i);
	// if (( i < (middleTab - 1) ) || ( i > (middleTab + 1) )) {
	if ( i != middleTab ) {
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

	    // t->setRect( QRect(x, 0, w * available/required, h) );
	    t->setRect( QRect(x, 0, available, h) );
	    x += t->rect().width() - overlap;
	    r = r.unite( t->rect() );
	}
    }

    QRect rr = tab(count()-1)->rect();
    rr.setRight(width()-1);
    tab(count()-1)->setRect( rr );

    for ( t = tabList()->first(); t; t = tabList()->next() ) {
       QRect tr = t->rect();
       tr.setHeight( r.height() );
       t->setRect( tr );
    }

    update();
}


void CategoryTabBar::paint( QPainter * p, QTab * t, bool selected ) const
{
#if QT_VERSION >= 300
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
#if QT_VERSION >= 300
			    t->identifier() == keyboardFocusTab()
#else
			    t->identitifer() == keyboardFocusTab()
#endif
		);
}


void CategoryTabBar::paintLabel( QPainter* p, const QRect&,
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

//---------------------------------------------------------------------------

Launcher::Launcher( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    setCaption( tr("Launcher") );

    syncDialog = 0;

    // we have a pretty good idea how big we'll be
    setGeometry( 0, 0, qApp->desktop()->width(), qApp->desktop()->height() );

    tabs = 0;
    rootFolder = 0;
    docsFolder = 0;

    tabs = new CategoryTabWidget( this );
    tabs->setMaximumWidth( qApp->desktop()->width() );
    setCentralWidget( tabs );

    connect( tabs, SIGNAL(selected(const QString&)),
	this, SLOT(viewSelected(const QString&)) );
    connect( tabs, SIGNAL(clicked(const AppLnk*)),
	this, SLOT(select(const AppLnk*)));
    connect( tabs, SIGNAL(rightPressed(AppLnk*)),
	this, SLOT(properties(AppLnk*)));

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel* sysChannel = new QCopChannel( "QPE/System", this );
    connect( sysChannel, SIGNAL(received(const QCString &, const QByteArray &)),
             this, SLOT(systemMessage( const QCString &, const QByteArray &)) );
#endif

    storage = new StorageInfo( this );
    connect( storage, SIGNAL( disksChanged() ), SLOT( storageChanged() ) );

    updateTabs();

    preloadApps();

    in_lnk_props = FALSE;
    got_lnk_change = FALSE;
}

Launcher::~Launcher()
{
}

static bool isVisibleWindow(int wid)
{
    const QList<QWSWindow> &list = qwsServer->clientWindows();
    QWSWindow* w;
    for (QListIterator<QWSWindow> it(list); (w=it.current()); ++it) {
	if ( w->winId() == wid )
	    return !w->isFullyObscured();
    }
    return FALSE;
}

void Launcher::showMaximized()
{
    if ( isVisibleWindow( winId() ) )
	doMaximize();
    else
	QTimer::singleShot( 20, this, SLOT(doMaximize()) );
}

void Launcher::doMaximize()
{
    QMainWindow::showMaximized();
}

void Launcher::updateMimeTypes()
{
    MimeType::clear();
    updateMimeTypes(rootFolder);
}

void Launcher::updateMimeTypes(AppLnkSet* folder)
{
    for ( QListIterator<AppLnk> it( folder->children() ); it.current(); ++it ) {
	AppLnk *app = it.current();
	if ( app->type() == "Folder" )
	    updateMimeTypes((AppLnkSet *)app);
	else {
	    MimeType::registerApp(*app);
	}
    }
}

void Launcher::loadDocs()
{
    delete docsFolder;
    docsFolder = new DocLnkSet;
    Global::findDocuments(docsFolder);
}

void Launcher::updateTabs()
{
    MimeType::updateApplications(); // ### reads all applnks twice

    delete rootFolder;
    rootFolder = new AppLnkSet( MimeType::appsFolderName() );

    loadDocs();

    tabs->initializeCategories(rootFolder, docsFolder, storage->fileSystems());
}

void Launcher::updateDocs()
{
    loadDocs();
    tabs->updateDocs(docsFolder,storage->fileSystems());
}

void Launcher::viewSelected(const QString& s)
{
    setCaption( s + tr(" - Launcher") );
}

void Launcher::nextView()
{
    tabs->nextTab();
}


void Launcher::select( const AppLnk *appLnk )
{
    if ( appLnk->type() == "Folder" ) {
	// Not supported: flat is simpler for the user
    } else {
	if ( appLnk->exec().isNull() ) {
	    QMessageBox::information(this,tr("No application"),
		tr("<p>No application is defined for this document."
		"<p>Type is %1.").arg(appLnk->type()));
	    return;
	}
	tabs->setBusy(TRUE);
	emit executing( appLnk );
	appLnk->execute();
    }
}

void Launcher::externalSelected(const AppLnk *appLnk)
{
    tabs->setBusy(TRUE);
    emit executing( appLnk );
}

void Launcher::properties( AppLnk *appLnk )
{
    if ( appLnk->type() == "Folder" ) {
	// Not supported: flat is simpler for the user
    } else {
	in_lnk_props = TRUE;
	got_lnk_change = FALSE;
	LnkProperties prop(appLnk);
	connect(&prop, SIGNAL(select(const AppLnk *)), this, SLOT(externalSelected(const AppLnk *)));
	prop.showMaximized();
	prop.exec();
	in_lnk_props = FALSE;
	if ( got_lnk_change ) {
	    updateLink(lnk_change);
	}
    }
}

void Launcher::updateLink(const QString& link)
{
    if (link.isNull())
	updateTabs();
    else if (link.isEmpty())
	updateDocs();
    else
	tabs->updateLink(link);
}

void Launcher::systemMessage( const QCString &msg, const QByteArray &data)
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "linkChanged(QString)" ) {
	QString link;
	stream >> link;
	if ( in_lnk_props ) {
	    got_lnk_change = TRUE;
	    lnk_change = link;
	} else {
	    updateLink(link);
	}
    } else if ( msg == "busy()" ) {
	emit busy();
    } else if ( msg == "notBusy(QString)" ) {
	QString app;
	stream >> app;
	tabs->setBusy(FALSE);
	emit notBusy(app);
    } else if ( msg == "mkdir(QString)" ) {
	QString dir;
	stream >> dir;
	if ( !dir.isEmpty() )
	    mkdir( dir );
    } else if ( msg == "rdiffGenSig(QString,QString)" ) {
	QString baseFile, sigFile;
	stream >> baseFile >> sigFile;
	QRsync::generateSignature( baseFile, sigFile );
    } else if ( msg == "rdiffGenDiff(QString,QString,QString)" ) {
	QString baseFile, sigFile, deltaFile;
	stream >> baseFile >> sigFile >> deltaFile;
	QRsync::generateDiff( baseFile, sigFile, deltaFile );
    } else if ( msg == "rdiffApplyPatch(QString,QString)" ) {
	QString baseFile, deltaFile;
	stream >> baseFile >> deltaFile;
	if ( !QFile::exists( baseFile ) ) {
	    QFile f( baseFile );
	    f.open( IO_WriteOnly );
	    f.close();
	}
	QRsync::applyDiff( baseFile, deltaFile );
	QCopEnvelope e( "QPE/Desktop", "patchApplied(QString)" );
	e << baseFile;
    } else if ( msg == "rdiffCleanup()" ) {
	mkdir( "/tmp/rdiff" );
	QDir dir;
	dir.setPath( "/tmp/rdiff" );
	QStringList entries = dir.entryList();
	for ( QStringList::Iterator it = entries.begin(); it != entries.end(); ++it )
	    dir.remove( *it );
    } else if ( msg == "sendHandshakeInfo()" ) {
	QString home = getenv( "HOME" );
	QCopEnvelope e( "QPE/Desktop", "handshakeInfo(QString,bool)" );
	e << home;
	int locked = (int) Desktop::screenLocked();
	e << locked;
    } else if ( msg == "sendCardInfo()" ) {
	QCopEnvelope e( "QPE/Desktop", "cardInfo(QString)" );
	const QList<FileSystem> &fs = storage->fileSystems();
	QListIterator<FileSystem> it ( fs );
	QString s;
	QString homeDir = getenv("HOME");
	QString hardDiskHome;
	for ( ; it.current(); ++it ) {
	    if ( (*it)->isRemovable() )
		s += (*it)->name() + "=" + (*it)->path() + "/Documents "
		     + QString::number( (*it)->availBlocks() * (*it)->blockSize() )
		     + " " + (*it)->options() + ";";
	    else if ( (*it)->disk() == "/dev/mtdblock1" ||
		      (*it)->disk() == "/dev/mtdblock/1" )
		s += (*it)->name() + "=" + homeDir + "/Documents "
		     + QString::number( (*it)->availBlocks() * (*it)->blockSize() )
		     + " " + (*it)->options() + ";";
	    else if ( (*it)->name().contains( "Hard Disk") &&
		      homeDir.contains( (*it)->path() ) &&
		      (*it)->path().length() > hardDiskHome.length() )
		hardDiskHome =
		    (*it)->name() + "=" + homeDir + "/Documents "
		    + QString::number( (*it)->availBlocks() * (*it)->blockSize() )
		    + " " + (*it)->options() + ";";
	}
	if ( !hardDiskHome.isEmpty() )
	    s += hardDiskHome;

	e << s;
    } else if ( msg == "sendSyncDate(QString)" ) {
	QString app;
	stream >> app;
	Config cfg( "qpe" );
	cfg.setGroup("SyncDate");
	QCopEnvelope e( "QPE/Desktop", "syncDate(QString,QString)" );
	e  << app  << cfg.readEntry( app );
	//qDebug("QPE/System sendSyncDate for %s: response %s", app.latin1(),
	//cfg.readEntry( app ).latin1() );
    } else if ( msg == "setSyncDate(QString,QString)" ) {
	QString app, date;
	stream >> app >> date;
	Config cfg( "qpe" );
	cfg.setGroup("SyncDate");
	cfg.writeEntry( app, date );
	//qDebug("setSyncDate(QString,QString) %s %s", app.latin1(), date.latin1());
    } else if ( msg == "startSync(QString)" ) {
	QString what;
	stream >> what;
	delete syncDialog; syncDialog = 0;
	syncDialog = new SyncDialog( this, "syncProgress", FALSE,
				     WStyle_Tool | WStyle_Customize |
				     Qt::WStyle_StaysOnTop );
	syncDialog->showMaximized();
	syncDialog->whatLabel->setText( "<b>" + what + "</b>" );
	connect( syncDialog->buttonCancel, SIGNAL( clicked() ),
		 SLOT( cancelSync() ) );	
    }
    else if ( msg == "stopSync()") {
	delete syncDialog; syncDialog = 0;
    } else if ( msg == "getAllDocLinks()" ) {
	loadDocs();

	QString contents;

	for ( QListIterator<DocLnk> it( docsFolder->children() ); it.current(); ++it ) {
	    DocLnk *doc = it.current();
	    QString lfn = doc->linkFile();
	    QFileInfo fi( doc->file() );
	    if ( !fi.exists() )
		continue;



	    QFile f( lfn );
	    if ( f.open( IO_ReadOnly ) ) {
		QTextStream ts( &f );
		ts.setEncoding( QTextStream::UnicodeUTF8 );
		contents += ts.read();
		f.close();
	    } else {
		contents += "[Desktop Entry]\n";
		contents += "Categories = " + Qtopia::Record::idsToString( doc->categories() ) + "\n";
		contents += "File = "+doc->file()+"\n";
		contents += "Name = "+doc->name()+"\n";
		contents += "Type = "+doc->type()+"\n";
	    }
	    contents += QString("Size = %1\n").arg( fi.size() );
	}

	//qDebug( "sending length %d", contents.length() );
	QCopEnvelope e( "QPE/Desktop", "docLinks(QString)" );
	e << contents;
	
 	//qDebug( "================ \n\n%s\n\n===============",
	//contents.latin1() );

	delete docsFolder;
	docsFolder = 0;
    }
}

void Launcher::cancelSync()
{
    QCopEnvelope e( "QPE/Desktop", "cancelSync()" );
}

void Launcher::storageChanged()
{
    if ( in_lnk_props ) {
	got_lnk_change = TRUE;
	lnk_change = "";
    } else {
	updateDocs();
    }
}


bool Launcher::mkdir(const QString &localPath)
{
    QDir fullDir(localPath);
    if (fullDir.exists())
	return true;

    // at this point the directory doesn't exist
    // go through the directory tree and start creating the direcotories
    // that don't exist; if we can't create the directories, return false

    QString dirSeps = "/";
    int dirIndex = localPath.find(dirSeps);
    QString checkedPath;

    // didn't find any seps; weird, use the cur dir instead
    if (dirIndex == -1) {
	//qDebug("No seperators found in path %s", localPath.latin1());
	checkedPath = QDir::currentDirPath();
    }

    while (checkedPath != localPath) {
	// no more seperators found, use the local path
	if (dirIndex == -1)
	    checkedPath = localPath;
	else {
	    // the next directory to check
	    checkedPath = localPath.left(dirIndex) + "/";
	    // advance the iterator; the next dir seperator
	    dirIndex = localPath.find(dirSeps, dirIndex+1);
	}

	QDir checkDir(checkedPath);
	if (!checkDir.exists()) {
	    //qDebug("mkdir making dir %s", checkedPath.latin1());

	    if (!checkDir.mkdir(checkedPath)) {
		qDebug("Unable to make directory %s", checkedPath.latin1());
		return FALSE;
	    }
	}

    }
    return TRUE;
}

void Launcher::preloadApps()
{
    Config cfg("Launcher");
    cfg.setGroup("Preload");
    QStringList apps = cfg.readListEntry("Apps",',');
    for (QStringList::ConstIterator it=apps.begin(); it!=apps.end(); ++it) {
	QCopEnvelope e("QPE/Application/"+(*it).local8Bit(), "enablePreload()");
    }
}
