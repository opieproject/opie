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
#include "documentlist.h"
#include "serverinterface.h"

#include <opie2/oglobal.h>

#include <qtopia/config.h>
#include <qtopia/mimetype.h>
#include <qtopia/resource.h>
#include <qtopia/private/categories.h>
#include <qtopia/qpeapplication.h>
#include <qtopia/applnk.h>
#include <qtopia/storage.h>
#ifdef Q_WS_QWS
#include <qtopia/qcopenvelope_qws.h>
#endif

#include <qtimer.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qdir.h>
#include <qpainter.h>
#include <qimage.h>
#include <qcopchannel_qws.h>
#include <qlistview.h>
#include <qlist.h>
#include <qpixmap.h>


AppLnkSet *DocumentList::appLnkSet = 0;

static const int MAX_SEARCH_DEPTH = 10;


class DocumentListPrivate : public QObject {
    Q_OBJECT
public:
    DocumentListPrivate( ServerInterface *gui );
    ~DocumentListPrivate();

    void initialize();

    const QString nextFile();
    const DocLnk *iterate();
    bool store( DocLnk* dl );
    void estimatedPercentScanned();

    DocLnkSet dls;
    QDict<void> reference;
    QDictIterator<void> *dit;
    enum { Find, RemoveKnownFiles, MakeUnknownFiles, Done } state;

    QStringList docPaths;
    unsigned int docPathsSearched;

    int searchDepth;
    QDir *listDirs[MAX_SEARCH_DEPTH];
    const QFileInfoList *lists[MAX_SEARCH_DEPTH];
    unsigned int listPositions[MAX_SEARCH_DEPTH];

    StorageInfo *storage;

    int tid;

    ServerInterface *serverGui;

    bool needToSendAllDocLinks;
    bool sendAppLnks;
    bool sendDocLnks;
    bool scanDocs;
};


/*
 * scandocs will be read from Config
 */
DocumentList::DocumentList( ServerInterface *serverGui, bool /*scanDocs*/,
			    QObject *parent, const char *name )
 : QObject( parent, name )
{
    appLnkSet = new AppLnkSet( MimeType::appsFolderName() );
    d = new DocumentListPrivate( serverGui );
    d->needToSendAllDocLinks = false;

    Config cfg( "Launcher" );
    cfg.setGroup( "DocTab" );
    d->scanDocs = cfg.readBoolEntry( "Enable", true );
    qDebug( "DocumentList::DocumentList() : scanDocs = %d", d->scanDocs );

    QTimer::singleShot( 10, this, SLOT( startInitialScan() ) );
}

void DocumentList::startInitialScan()
{
    reloadAppLnks();
    reloadDocLnks();
}

DocumentList::~DocumentList()
{
    delete appLnkSet;
    delete d;
}


void DocumentList::add( const DocLnk& doc )
{
    if ( d->serverGui && QFile::exists( doc.file() ) )
	d->serverGui->documentAdded( doc );
}


void DocumentList::start()
{
    resume();
}


void DocumentList::pause()
{
    //qDebug("pause %i", d->tid);
    killTimer( d->tid );
    d->tid = 0;
}


void DocumentList::resume()
{
    if ( d->tid == 0 ) {
	d->tid = startTimer( 20 );
	//qDebug("resumed %i", d->tid);
    }
}

/*
void DocumentList::resend()
{
    // Re-emits all the added items to the list (firstly letting everyone know to
    // clear what they have as it is being sent again)
    pause();
    emit allRemoved();
    QTimer::singleShot( 5, this, SLOT( resendWorker() ) );
}


void DocumentList::resendWorker()
{
    const QList<DocLnk> &list = d->dls.children();
    for ( QListIterator<DocLnk> it( list ); it.current(); ++it )
	add( *(*it) );
    resume();
}
*/

void DocumentList::rescan()
{
    //qDebug("rescan");
    pause();
    d->initialize();
    resume();
}


void DocumentList::timerEvent( QTimerEvent *te )
{
    if ( te->timerId() == d->tid ) {
	// Do 3 at a time
	for (int i = 0; i < 3; i++ ) {
	    const DocLnk *lnk = d->iterate();
	    if ( lnk ) {
		add( *lnk );
	    } else {
		// stop when done
		pause();
		if ( d->serverGui )
		    d->serverGui->documentScanningProgress( 100 );
		if ( d->needToSendAllDocLinks )
		    sendAllDocLinks();
		break;
	    }
	}
    }
}


void DocumentList::reloadAppLnks()
{
    if ( d->sendAppLnks && d->serverGui ) {
	d->serverGui->applicationScanningProgress( 0 );
	d->serverGui->allApplicationsRemoved();
    }

    delete appLnkSet;
    appLnkSet = new AppLnkSet( MimeType::appsFolderName() );

    if ( d->sendAppLnks && d->serverGui ) {
	static QStringList prevTypeList;
	QStringList types = appLnkSet->types();
	for ( QStringList::Iterator ittypes=types.begin(); ittypes!=types.end(); ++ittypes) {
	    if ( !(*ittypes).isEmpty() ) {
		if ( !prevTypeList.contains(*ittypes) ) {
		    QString name = appLnkSet->typeName(*ittypes);
		    QPixmap pm = appLnkSet->typePixmap(*ittypes);
		    QPixmap bgPm = appLnkSet->typeBigPixmap(*ittypes);

		    if (pm.isNull()) {
			QImage img( Resource::loadImage( "UnknownDocument" ) );
			pm = img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
			bgPm = img.smoothScale( AppLnk::bigIconSize(), AppLnk::bigIconSize() );
		    }

		    //qDebug("adding type %s", (*ittypes).latin1());

		    // ### our current launcher expects docs tab to be last
		    d->serverGui->typeAdded( *ittypes, name.isNull() ? (*ittypes) : name, pm, bgPm );
		}
		prevTypeList.remove(*ittypes);
	    }
	}
	for ( QStringList::Iterator ittypes=prevTypeList.begin(); ittypes!=prevTypeList.end(); ++ittypes) {
	    //qDebug("removing type %s", (*ittypes).latin1());
	    d->serverGui->typeRemoved(*ittypes);
	}
	prevTypeList = types;
    }

    QListIterator<AppLnk> itapp( appLnkSet->children() );
    AppLnk* l;
    while ( (l=itapp.current()) ) {
	++itapp;
	if ( d->sendAppLnks && d->serverGui )
	    d->serverGui->applicationAdded( l->type(), *l );
    }

    if ( d->sendAppLnks && d->serverGui )
	d->serverGui->applicationScanningProgress( 100 );
}

void DocumentList::reloadDocLnks()
{
    if ( !d->scanDocs )
	return;

    if ( d->sendDocLnks && d->serverGui ) {
	d->serverGui->documentScanningProgress( 0 );
	d->serverGui->allDocumentsRemoved();
    }

    rescan();
}

void DocumentList::linkChanged( QString arg )
{
    //qDebug( "linkchanged( %s )", arg.latin1() );

    if ( arg.isNull() || OGlobal::isAppLnkFileName( arg ) ) {
	reloadAppLnks();
    } else {

	const QList<DocLnk> &list = d->dls.children();
	QListIterator<DocLnk> it( list );
	while ( it.current() ) {
	    DocLnk *doc = it.current();
	    ++it;
	    if ( ( doc->linkFileKnown() && doc->linkFile() == arg )
		|| ( doc->fileKnown() && doc->file() == arg ) ) {
		//qDebug( "found old link" );
		DocLnk* dl = new DocLnk( arg );
		// add new one if it exists and matches the mimetype
		if ( d->store( dl ) ) {
		    // Existing link has been changed, send old link ref and a ref
		    // to the new link
		    //qDebug( "change case" );
		    if ( d->serverGui )
			d->serverGui->documentChanged( *doc, *dl );

		} else {
		    // Link has been removed or doesn't match the mimetypes any more
		    // so we aren't interested in it, so take it away from the list
		    //qDebug( "removal case" );
		    if ( d->serverGui )
			d->serverGui->documentRemoved( *doc );

		}
		d->dls.remove( doc ); // remove old link from docLnkSet
		delete doc;
		return;
	    }
	}
	// Didn't find existing link, must be new
	DocLnk* dl = new DocLnk( arg );
	if ( d->store( dl ) ) {
	    // Add if it's a link we are interested in
	    //qDebug( "add case" );
	    add( *dl );
	}

    }
}

void DocumentList::restoreDone()
{
    reloadAppLnks();
    reloadDocLnks();
}

void DocumentList::storageChanged()
{
    // ### can implement better
    reloadAppLnks();
    reloadDocLnks();
    // ### Optimization opportunity
    // Could be a bit more intelligent and somehow work out which
    // mtab entry has changed and then only scan that and add and remove
    // links appropriately.
//    rescan();
}

void DocumentList::sendAllDocLinks()
{
    if ( d->tid != 0 ) {
	// We are in the middle of scanning, set a flag so
	// we do this when we finish our scanning
	d->needToSendAllDocLinks = true;
	return;
    }

    QString contents;
    Categories cats;
    for ( QListIterator<DocLnk> it( d->dls.children() ); it.current(); ++it ) {
	DocLnk *doc = it.current();
	QFileInfo fi( doc->file() );
	if ( !fi.exists() )
	    continue;

	bool fake = !doc->linkFileKnown();
	if ( !fake ) {
	    QFile f( doc->linkFile() );
	    if ( f.open( IO_ReadOnly ) ) {
		QTextStream ts( &f );
		ts.setEncoding( QTextStream::UnicodeUTF8 );
		contents += ts.read();
		f.close();
	    } else
		fake = TRUE;
	}
	if (fake) {
	    contents += "[Desktop Entry]\n"; // No tr
	    contents += "Categories = " + // No tr
		cats.labels("Document View",doc->categories()).join(";") + "\n"; // No tr
	    contents += "Name = "+doc->name()+"\n"; // No tr
	    contents += "Type = "+doc->type()+"\n"; // No tr
	}
	contents += "File = "+doc->file()+"\n"; // No tr // (resolves path)
	contents += QString("Size = %1\n").arg( fi.size() ); // No tr
    }

    //qDebug( "sending length %d", contents.length() );
#ifndef QT_NO_COP
    QCopEnvelope e( "QPE/Desktop", "docLinks(QString)" );
    e << contents;
#endif
    //qDebug( "================ \n\n%s\n\n===============", contents.latin1() );

    d->needToSendAllDocLinks = false;
}
























DocumentListPrivate::DocumentListPrivate( ServerInterface *gui )
{
    storage = new StorageInfo( this );
    serverGui = gui;
    if ( serverGui ) {
	sendAppLnks = serverGui->requiresApplications();
	sendDocLnks = serverGui->requiresDocuments();
    } else {
	sendAppLnks = false;
	sendDocLnks = false;
    }
    for ( int i = 0; i < MAX_SEARCH_DEPTH; i++ ) {
	listDirs[i] = 0;
	lists[i] = 0;
	listPositions[i] = 0;
    }
    initialize();
    tid = 0;
}


void DocumentListPrivate::initialize()
{
    // Reset
    dls.clear();
    docPaths.clear();
    reference.clear();

    QDir docDir( QPEApplication::documentDir() );
    if ( docDir.exists() )
	docPaths += QPEApplication::documentDir();
    int i = 1;
    const QList<FileSystem> &fs = storage->fileSystems();
    QListIterator<FileSystem> it( fs );
    for ( ; it.current(); ++it )
	if ( (*it)->isRemovable() ) {
	    docPaths += (*it)->path();
	    i++;
	}

    for ( int i = 0; i < MAX_SEARCH_DEPTH; i++ ) {
	if ( listDirs[i] ) {
	    delete listDirs[i];
	    listDirs[i] = 0;
	}
	lists[i] = 0;
	listPositions[i] = 0;
    }

    docPathsSearched = 0;
    searchDepth = -1;
    state = Find;
    dit = 0;
}


DocumentListPrivate::~DocumentListPrivate()
{
    for ( int i = 0; i < MAX_SEARCH_DEPTH; i++ )
	if ( listDirs[i] )
	    delete listDirs[i];
    delete dit;
}


void DocumentListPrivate::estimatedPercentScanned()
{
    double overallProgress = 0.0;
    double levelWeight = 75.0;

    int topCount = docPaths.count();
    if ( topCount > 1 ) {
	levelWeight = levelWeight / topCount;
	overallProgress += (docPathsSearched - 1) * levelWeight;
    }

    for ( int d = 0; d <= searchDepth; d++ ) {
	if ( listDirs[d] ) {
	    int items = lists[d]->count();
	    if ( items > 1 ) {
		levelWeight = levelWeight / items;
		// Take in to account "." and ".."
		overallProgress += (listPositions[d] - 3) * levelWeight;
	    }
	} else {
	    break;
	}
    }

    // qDebug( "overallProgress: %f", overallProgress );

    if ( serverGui )
	serverGui->documentScanningProgress( (int)overallProgress );
}


const QString DocumentListPrivate::nextFile()
{
    while ( TRUE ) {
	while ( searchDepth < 0 ) {
	    // go to next base path
	    if ( docPathsSearched >= docPaths.count() ) {
		// end of base paths
		return QString::null;
	    } else {
		QDir dir( docPaths[docPathsSearched] );
		// qDebug("now using base path: %s", docPaths[docPathsSearched].latin1() );
		docPathsSearched++;
		if ( !dir.exists( ".Qtopia-ignore" ) ) {
		    listDirs[0] = new QDir( dir );
		    lists[0] = listDirs[0]->entryInfoList();
		    listPositions[0] = 0;
		    searchDepth = 0;
		}
	    }
	}

	const QFileInfoList *fil = lists[searchDepth];
	QFileInfoList *fl = (QFileInfoList *)fil;
	unsigned int pos = listPositions[searchDepth];

	if ( pos >= fl->count() ) {
	    // go up a depth
	    delete listDirs[searchDepth];
	    listDirs[searchDepth] = 0;
	    lists[searchDepth] = 0;
	    listPositions[searchDepth] = 0;
	    searchDepth--;
	} else {
	    const QFileInfo *fi = fl->at(pos);
	    listPositions[searchDepth]++;
	    QString bn = fi->fileName();
	    if ( bn[0] != '.' ) {
		if ( fi->isDir()  ) {
		    if ( bn != "CVS" && bn != "Qtopia" && bn != "QtPalmtop" ) {
			// go down a depth
			QDir dir( fi->filePath() );
			// qDebug("now going in to path: %s", bn.latin1() );
			if ( !dir.exists( ".Qtopia-ignore" ) ) {
			    if ( searchDepth < MAX_SEARCH_DEPTH - 1) {
				searchDepth++;
				listDirs[searchDepth] = new QDir( dir );
				lists[searchDepth] = listDirs[searchDepth]->entryInfoList();
				listPositions[searchDepth] = 0;
			    }
			}
		    }
		} else {
		    estimatedPercentScanned();
		    return fl->at(pos)->filePath();
		}
	    }
	}
    }

    return QString::null;
}


bool DocumentListPrivate::store( DocLnk* dl )
{
    // if ( dl->fileKnown() && !dl->file().isEmpty() ) {
    if ( dl && dl->fileKnown() ) {
	dls.add( dl ); // store
	return TRUE;
    }

    // don't store - delete
    delete dl;
    return FALSE;
}


#define MAGIC_NUMBER	((void*)2)

const DocLnk *DocumentListPrivate::iterate()
{
    if ( state == Find ) {
	//qDebug("state Find");
	QString file = nextFile();
	while ( !file.isNull() ) {
	    if ( file.right(8) == ".desktop" ) { // No tr
		DocLnk* dl = new DocLnk( file );
		if ( store(dl) )
		    return dl;
	    } else {
		reference.insert( file, MAGIC_NUMBER );
	    }
	    file = nextFile();
	}
	state = RemoveKnownFiles;

	if ( serverGui )
	    serverGui->documentScanningProgress( 75 );
    }

    static int iterationI;
    static int iterationCount;

    if ( state == RemoveKnownFiles ) {
	//qDebug("state RemoveKnownFiles");
	const QList<DocLnk> &list = dls.children();
	for ( QListIterator<DocLnk> it( list ); it.current(); ++it ) {
	    reference.remove( (*it)->file() );
	    // ### does this need to be deleted?
	}
	dit = new QDictIterator<void>(reference);
	state = MakeUnknownFiles;

	iterationI = 0;
	iterationCount = dit->count();
    }

    if ( state == MakeUnknownFiles ) {
	//qDebug("state MakeUnknownFiles");
	for (void* c; (c=dit->current()); ++(*dit) ) {
	    if ( c == MAGIC_NUMBER ) {
		DocLnk* dl = new DocLnk;
		QFileInfo fi( dit->currentKey() );
		dl->setFile( fi.filePath() );
		dl->setName( fi.baseName() );
		if ( store(dl) ) {
		    ++*dit;
		    iterationI++;
		    if ( serverGui )
			serverGui->documentScanningProgress( 75 + (25*iterationI)/iterationCount );
		    return dl;
		}
	    }
	    iterationI++;
	}

	delete dit;
	dit = 0;
	state = Done;
    }

    //qDebug("state Done");
    return NULL;
}


#include "documentlist.moc"



