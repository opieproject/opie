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

#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/fileselector.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/resource.h>
#include <qaction.h>
#include <qimage.h>
#include <qfile.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlistbox.h>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>

#include "playlistselection.h"
#include "playlistwidget.h"
#include "mediaplayerstate.h"

#include <stdlib.h>


extern MediaPlayerState *mediaPlayerState;


class PlayListWidgetPrivate {
public:
    QToolButton *tbPlay;
    QToolButton *tbFull;
    QToolButton *tbLoop;
    QToolButton *tbScale;
    QToolButton *tbShuffle;

    QFrame *playListFrame;
    FileSelector *files;
    PlayListSelection *selectedFiles;
    bool setDocumentUsed;
    DocLnk *current;
};


class ToolButton : public QToolButton {
public:
    ToolButton( QWidget *parent, const char *name, const QString& icon, QObject *handler, const QString& slot, bool t = FALSE )
	: QToolButton( parent, name ) {
	setTextLabel( name );
	setPixmap( Resource::loadPixmap( icon ) );
	setAutoRaise( TRUE );
	setFocusPolicy( QWidget::NoFocus );
	setToggleButton( t );
	connect( this, t ? SIGNAL( toggled(bool) ) : SIGNAL( clicked() ), handler, slot );
	QPEMenuToolFocusManager::manager()->addWidget( this );
    }
};


class MenuItem : public QAction {
public:
    MenuItem( QWidget *parent, const QString& text, QObject *handler, const QString& slot )
	: QAction( text, QString::null, 0, 0 ) {
	connect( this, SIGNAL( activated() ), handler, slot );
	addTo( parent );
    }
};


PlayListWidget::PlayListWidget( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl ) {

    d = new PlayListWidgetPrivate;
    d->setDocumentUsed = FALSE;
    d->current = NULL;

    setBackgroundMode( PaletteButton );

    setCaption( tr("MediaPlayer") );
    setIcon( Resource::loadPixmap( "MPEGPlayer" ) );

    setToolBarsMovable( FALSE );

    // Create Toolbar
    QPEToolBar *toolbar = new QPEToolBar( this );
    toolbar->setHorizontalStretchable( TRUE );

    // Create Menubar
    QPEMenuBar *menu = new QPEMenuBar( toolbar );
    menu->setMargin( 0 );

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setLabel( tr( "Play Operations" ) );
#ifdef BUTTONS_ON_TOOLBAR
    d->tbPlay    = new ToolButton( bar, tr( "Play" ),       "mpegplayer/play",    mediaPlayerState, SLOT(setPlaying(bool)), TRUE );
    d->tbShuffle = new ToolButton( bar, tr( "Randomize" ),  "mpegplayer/shuffle", mediaPlayerState, SLOT(setShuffled(bool)), TRUE );
#endif
    d->tbLoop    = new ToolButton( bar, tr( "Loop" ),       "mpegplayer/loop",    mediaPlayerState, SLOT(setLooping(bool)), TRUE );
    d->tbFull    = new ToolButton( bar, tr( "Fullscreen" ), "fullscreen",         mediaPlayerState, SLOT(setFullscreen(bool)), TRUE );
    d->tbScale   = new ToolButton( bar, tr( "Scale" ),      "mpegplayer/scale",   mediaPlayerState, SLOT(setScaled(bool)), TRUE );

    QPopupMenu *pmPlayList = new QPopupMenu( this );
    menu->insertItem( tr( "PlayList" ), pmPlayList );
    new MenuItem( pmPlayList, tr( "Toggle PlayList" ),	   mediaPlayerState, SLOT( togglePlaylist() ) );
    new MenuItem( pmPlayList, tr( "Clear List" ),          this,             SLOT( clearList() ) );
    new MenuItem( pmPlayList, tr( "Add all music files" ), this,             SLOT( addAllMusicToList() ) );
    new MenuItem( pmPlayList, tr( "Add all video files" ), this,             SLOT( addAllVideoToList() ) );
    new MenuItem( pmPlayList, tr( "Add all files" ),       this,             SLOT( addAllToList() ) );
#ifdef CAN_SAVE_LOAD_PLAYLISTS
    new MenuItem( pmPlayList, tr( "Save PlayList" ),       this,             SLOT( saveList() ) );
    new MenuItem( pmPlayList, tr( "Load PlayList" ),       this,             SLOT( loadList() ) );
#endif

    QVBox *vbox5 = new QVBox( this ); vbox5->setBackgroundMode( PaletteButton );

    // Add the playlist area
    QVBox *vbox3 = new QVBox( vbox5 ); vbox3->setBackgroundMode( PaletteButton );
    d->playListFrame = vbox3;

    QLabel *plString = new QLabel( tr(" PlayList"), vbox3 );    
    plString->setBackgroundMode( QButton::PaletteButton );
    plString->setFont( QFont( "Helvetica", 8, QFont::Bold ) );

    QHBox *hbox2 = new QHBox( vbox3 ); hbox2->setBackgroundMode( PaletteButton );
    d->selectedFiles = new PlayListSelection( hbox2 );
    QVBox *vbox1 = new QVBox( hbox2 ); vbox1->setBackgroundMode( PaletteButton );

#ifndef BUTTONS_ON_TOOLBAR
    d->tbPlay = new ToolButton( vbox1, tr( "Play" ), "mpegplayer/play", mediaPlayerState, SLOT(setPlaying(bool)), TRUE );
    QVBox *stretch1 = new QVBox( vbox1 ); stretch1->setBackgroundMode( PaletteButton ); // add stretch
#endif
    new ToolButton( vbox1, tr( "Move Up" ),   "mpegplayer/up",   d->selectedFiles, SLOT(moveSelectedUp()) );
    new ToolButton( vbox1, tr( "Remove" ),    "mpegplayer/cut",  d->selectedFiles, SLOT(removeSelected()) );
    new ToolButton( vbox1, tr( "Move Down" ), "mpegplayer/down", d->selectedFiles, SLOT(moveSelectedDown()) );
    QVBox *stretch2 = new QVBox( vbox1 ); stretch2->setBackgroundMode( PaletteButton ); // add stretch
#ifndef BUTTONS_ON_TOOLBAR
    d->tbShuffle = new ToolButton( vbox1, tr( "Randomize" ), "mpegplayer/shuffle", mediaPlayerState, SLOT(setShuffled(bool)), TRUE );
#endif

    // add the library area
    QVBox *vbox4 = new QVBox( vbox5 ); vbox4->setBackgroundMode( PaletteButton );

    QLabel *libString = new QLabel( tr(" Media Library"), vbox4 );    
    libString->setBackgroundMode( QButton::PaletteButton );
    libString->setFont( QFont( "Helvetica", 8, QFont::Bold ) );

    QHBox *hbox6 = new QHBox( vbox4 ); hbox6->setBackgroundMode( PaletteButton );
    d->files = new FileSelector( "video/*;audio/*", hbox6, "Find Media Files", FALSE, FALSE );
    d->files->setBackgroundMode( PaletteButton );
    QVBox *vbox7 = new QVBox( hbox6 ); vbox7->setBackgroundMode( PaletteButton );

#ifdef SIDE_BUTTONS
    new ToolButton( vbox7, tr( "Add to Playlist" ),      "mpegplayer/add_to_playlist",      d->selectedFiles, SLOT(addSelected()) );
    new ToolButton( vbox7, tr( "Remove from Playlist" ), "mpegplayer/remove_from_playlist", d->selectedFiles, SLOT(removeSelected()) );
    QVBox *stretch3 = new QVBox( vbox1 ); stretch3->setBackgroundMode( PaletteButton ); // add stretch
#endif

    connect( d->files, SIGNAL( fileSelected( const DocLnk & ) ), this, SLOT( addToSelection( const DocLnk & ) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ),    d->tbPlay,    SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( loopingToggled( bool ) ),    d->tbLoop,    SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( shuffledToggled( bool ) ),   d->tbShuffle, SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( fullscreenToggled( bool ) ), d->tbFull,    SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( scaledToggled( bool ) ),     d->tbScale,   SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( fullscreenToggled( bool ) ), d->tbScale,   SLOT( setEnabled( bool ) ) );
    connect( mediaPlayerState, SIGNAL( playlistToggled( bool ) ),   this,         SLOT( setPlaylist( bool ) ) );

    setCentralWidget( vbox5 );

    Config cfg( "MediaPlayer" );
    readConfig( cfg );

    initializeStates();
}


PlayListWidget::~PlayListWidget() {
    Config cfg( "MediaPlayer" );
    writeConfig( cfg );

    if ( d->current )
	delete d->current;
    delete d;
}


void PlayListWidget::initializeStates() {
    d->tbPlay->setOn( mediaPlayerState->playing() );
    d->tbLoop->setOn( mediaPlayerState->looping() );
    d->tbShuffle->setOn( mediaPlayerState->shuffled() );
    d->tbFull->setOn( mediaPlayerState->fullscreen() );
    d->tbScale->setOn( mediaPlayerState->scaled() );
    d->tbScale->setEnabled( mediaPlayerState->fullscreen() );
    setPlaylist( mediaPlayerState->playlist() );
}


void PlayListWidget::readConfig( Config& cfg ) {
    cfg.setGroup("PlayList");
    
    int noOfFiles = cfg.readNumEntry("NumberOfFiles", 0 );
    
    for ( int i = 0; i < noOfFiles; i++ ) {
	QString entryName;
	entryName.sprintf( "File%i", i + 1 );
	QString linkFile = cfg.readEntry( entryName );
	DocLnk lnk( linkFile );
	if ( lnk.isValid() )
            d->selectedFiles->addToSelection( lnk );

    }
}


void PlayListWidget::writeConfig( Config& cfg ) const {
    cfg.setGroup("PlayList");

    int noOfFiles = 0;

    d->selectedFiles->first();
    do {
	const DocLnk *lnk = d->selectedFiles->current();
	if ( lnk ) {
	    QString entryName;
	    entryName.sprintf( "File%i", noOfFiles + 1 );
	    cfg.writeEntry( entryName, lnk->linkFile() );
	    // if this link does exist, add it so we have the file
	    // next time...
	    if ( !QFile::exists( lnk->linkFile() ) ) {
		// the way writing lnks doesn't really check for out
		// of disk space, but check it anyway.
		if ( !lnk->writeLink() ) {
		    QMessageBox::critical( 0, tr("Out of space"),
					   tr( "There was a problem saving "
					       "the playlist.\n"
					       "Your playlist "
					       "may be missing some entries\n"
					       "the next time you start it." )
					   );
		}
	    }		   
	    noOfFiles++;
	}
    } while ( d->selectedFiles->next() );
    
    cfg.writeEntry("NumberOfFiles", noOfFiles );
}


void PlayListWidget::addToSelection( const DocLnk& lnk ) {
    d->setDocumentUsed = FALSE;
    if ( mediaPlayerState->playlist() )
        d->selectedFiles->addToSelection( lnk );
    else
	mediaPlayerState->setPlaying( TRUE );
}


void PlayListWidget::clearList() {
    while ( first() )
	d->selectedFiles->removeSelected();
}


void PlayListWidget::addAllToList() {
    DocLnkSet files;
    Global::findDocuments(&files, "video/*;audio/*");
    QListIterator<DocLnk> dit( files.children() );
    for ( ; dit.current(); ++dit )
        d->selectedFiles->addToSelection( **dit );
}


void PlayListWidget::addAllMusicToList() {
    DocLnkSet files;
    Global::findDocuments(&files, "audio/*");
    QListIterator<DocLnk> dit( files.children() );
    for ( ; dit.current(); ++dit )
        d->selectedFiles->addToSelection( **dit );
}


void PlayListWidget::addAllVideoToList() {
    DocLnkSet files;
    Global::findDocuments(&files, "video/*");
    QListIterator<DocLnk> dit( files.children() );
    for ( ; dit.current(); ++dit )
        d->selectedFiles->addToSelection( **dit );
}


void PlayListWidget::setDocument(const QString& fileref) {
    if ( fileref.isNull() ) {
	QMessageBox::critical( 0, tr( "Invalid File" ), tr( "There was a problem in getting the file." ) );
	return;
    }
    if ( mediaPlayerState->playlist() )
	addToSelection( DocLnk( fileref ) );
    else {
	d->setDocumentUsed = TRUE;
	if ( d->current )
	    delete d->current;
	d->current = new DocLnk( fileref );
    }
    mediaPlayerState->setPlaying( FALSE );
    mediaPlayerState->setPlaying( TRUE );
}


void PlayListWidget::setActiveWindow() {
    // When we get raised we need to ensure that it switches views
    char origView = mediaPlayerState->view();
    mediaPlayerState->setView( 'l' ); // invalidate
    mediaPlayerState->setView( origView ); // now switch back
}


void PlayListWidget::useSelectedDocument() {
    d->setDocumentUsed = FALSE;
}


const DocLnk *PlayListWidget::current() {
    if ( mediaPlayerState->playlist() )
	return d->selectedFiles->current();
    else if ( d->setDocumentUsed && d->current ) {
	return d->current;
    } else
	return d->files->selected();
}


bool PlayListWidget::prev() {
    if ( mediaPlayerState->playlist() ) {
	if ( mediaPlayerState->shuffled() ) {
	    const DocLnk *cur = current();
	    int j = 1 + (int)(97.0 * rand() / (RAND_MAX + 1.0));
	    for ( int i = 0; i < j; i++ ) {
		if ( !d->selectedFiles->next() )
		    d->selectedFiles->first();
	    }
	    if ( cur == current() )
		if ( !d->selectedFiles->next() )
		    d->selectedFiles->first();
	    return TRUE;
	} else {
	    if ( !d->selectedFiles->prev() ) {
		if ( mediaPlayerState->looping() ) {
		    return d->selectedFiles->last();
		} else {
		    return FALSE;
		}
	    }
	    return TRUE;
	}
    } else {
	return mediaPlayerState->looping();
    }
}


bool PlayListWidget::next() {
    if ( mediaPlayerState->playlist() ) {
	if ( mediaPlayerState->shuffled() ) {
	    return prev();
	} else {
	    if ( !d->selectedFiles->next() ) {
		if ( mediaPlayerState->looping() ) {
		    return d->selectedFiles->first();
		} else {
		    return FALSE;
		}
	    }
	    return TRUE;
	}
    } else {
	return mediaPlayerState->looping();
    }
}


bool PlayListWidget::first() {
    if ( mediaPlayerState->playlist() )
	return d->selectedFiles->first();
    else
	return mediaPlayerState->looping();
}


bool PlayListWidget::last() {
    if ( mediaPlayerState->playlist() )
	return d->selectedFiles->last();
    else
	return mediaPlayerState->looping();
}


void PlayListWidget::saveList() {
    QString filename;
//  pseudo code
//  filename = QLineEdit->getText();    
    Config cfg( filename + ".playlist" );
    writeConfig( cfg );
}


void PlayListWidget::loadList() {
    QString filename;
//  pseudo code
//  filename = FileSelector->openFile( "*.playlist" );
    Config cfg( filename + ".playlist" );
    readConfig( cfg );
}


void PlayListWidget::setPlaylist( bool shown ) {
    if ( shown ) 
	d->playListFrame->show();
    else
	d->playListFrame->hide();
}


void PlayListWidget::setView( char view ) {
    if ( view == 'l' )
	showMaximized();
    else
	hide();
}

