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

#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/fileselector.h>
#include <qpe/qpeapplication.h>

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
#include <qtabwidget.h>
#include <qlistview.h>
#include <qpoint.h>
//#include <qtimer.h>

#include "playlistselection.h"
#include "playlistwidget.h"
#include "mediaplayerstate.h"

#include <stdlib.h>

#define BUTTONS_ON_TOOLBAR
#define SIDE_BUTTONS
#define CAN_SAVE_LOAD_PLAYLISTS

extern MediaPlayerState *mediaPlayerState;

// class myFileSelector {

// };
class PlayListWidgetPrivate {
public:
    QToolButton *tbPlay, *tbFull, *tbLoop, *tbScale, *tbShuffle, *tbAddToList,  *tbRemoveFromList, *tbMoveUp, *tbMoveDown, *tbRemove;
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
//    menuTimer = new QTimer( this ,"menu timer"),
//     connect( menuTimer, SIGNAL( timeout() ), SLOT( addSelected() ) );

    setBackgroundMode( PaletteButton );

    setCaption( tr("OpiePlayer") );
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
    d->tbAddToList =  new ToolButton( bar, tr( "Add to Playlist" ),"mpegplayer/add_to_playlist",this , SLOT(addSelected()) );
    d->tbRemoveFromList = new ToolButton( bar, tr( "Remove from Playlist" ), "mpegplayer/remove_from_playlist", this , SLOT(removeSelected()) );
    d->tbPlay    = new ToolButton( bar, tr( "Play" ),"mpegplayer/play", mediaPlayerState, SLOT( setPlaying(bool)), TRUE );
    d->tbShuffle = new ToolButton( bar, tr( "Randomize" ), "mpegplayer/shuffle", mediaPlayerState, SLOT(setShuffled(bool)), TRUE );
    d->tbLoop    = new ToolButton( bar, tr( "Loop" ),"mpegplayer/loop", mediaPlayerState, SLOT(setLooping(bool)), TRUE );

    QPopupMenu *pmPlayList = new QPopupMenu( this );
    menu->insertItem( tr( "File" ), pmPlayList );
    new MenuItem( pmPlayList, tr( "Clear List" ),          this,             SLOT( clearList() ) );
    new MenuItem( pmPlayList, tr( "Add all audio files" ), this,             SLOT( addAllMusicToList() ) );
    new MenuItem( pmPlayList, tr( "Add all video files" ), this,             SLOT( addAllVideoToList() ) );
    new MenuItem( pmPlayList, tr( "Add all files" ),       this,             SLOT( addAllToList() ) );
    new MenuItem( pmPlayList, tr( "Save PlayList" ),       this,             SLOT( saveList() ) );
    new MenuItem( pmPlayList, tr( "Load PlayList" ),       this,             SLOT( loadList() ) );

    QPopupMenu *pmView = new QPopupMenu( this );
    menu->insertItem( tr( "View" ), pmView );

    fullScreenButton = new QAction(tr("Full Screen"), Resource::loadPixmap("fullscreen"), QString::null, 0, this, 0);
    connect( fullScreenButton, SIGNAL(activated()), mediaPlayerState, SLOT(toggleFullscreen()) );
    fullScreenButton->addTo(pmView);
    scaleButton = new QAction(tr("Scale"), Resource::loadPixmap("mpegplayer/scale"), QString::null, 0, this, 0);
    connect( scaleButton, SIGNAL(activated()), mediaPlayerState, SLOT(toggleScaled()) );
    scaleButton->addTo(pmView);

    QVBox *vbox5 = new QVBox( this ); vbox5->setBackgroundMode( PaletteButton );
    QVBox *vbox4 = new QVBox( vbox5 ); vbox4->setBackgroundMode( PaletteButton );

    QHBox *hbox6 = new QHBox( vbox4 ); hbox6->setBackgroundMode( PaletteButton );
    
    tabWidget = new QTabWidget( hbox6, "tabWidget" ); 
    tabWidget->setTabShape(QTabWidget::Triangular);

    QWidget *pTab;
    pTab = new QWidget( tabWidget, "pTab" );
    playlistView = new QListView( pTab, "Videoview" );
    playlistView->setMinimumSize(236,260);
    tabWidget->insertTab( pTab,"Playlist");

    // Add the playlist area

    QVBox *vbox3 = new QVBox( pTab ); vbox3->setBackgroundMode( PaletteButton );
    d->playListFrame = vbox3;
    d->playListFrame ->setMinimumSize(235,260);

    QHBox *hbox2 = new QHBox( vbox3 ); hbox2->setBackgroundMode( PaletteButton );

    d->selectedFiles = new PlayListSelection( hbox2);
    QVBox *vbox1 = new QVBox( hbox2 ); vbox1->setBackgroundMode( PaletteButton );

     QVBox *stretch1 = new QVBox( vbox1 ); stretch1->setBackgroundMode( PaletteButton ); // add stretch
     new ToolButton( vbox1, tr( "Move Up" ),   "mpegplayer/up",   d->selectedFiles, SLOT(moveSelectedUp()) );
     new ToolButton( vbox1, tr( "Remove" ),    "mpegplayer/cut",  d->selectedFiles, SLOT(removeSelected()) );
     new ToolButton( vbox1, tr( "Move Down" ), "mpegplayer/down", d->selectedFiles, SLOT(moveSelectedDown()) );
     QVBox *stretch2 = new QVBox( vbox1 ); stretch2->setBackgroundMode( PaletteButton ); // add stretch

    QWidget *aTab;
    aTab = new QWidget( tabWidget, "aTab" );
    audioView = new QListView( aTab, "Audioview" );
    audioView->setMinimumSize(233,260);
    audioView->addColumn( "Title",150);
    audioView->addColumn("Size", 45);
    audioView->addColumn("Media",35);
    audioView->setColumnAlignment(1, Qt::AlignRight);
    audioView->setColumnAlignment(2, Qt::AlignRight);
    tabWidget->insertTab(aTab,"Audio");
//    audioView
     Global::findDocuments(&files, "audio/*");
     QListIterator<DocLnk> dit( files.children() );
     QString storage;
     for ( ; dit.current(); ++dit ) {
         QListViewItem * newItem;
         if(dit.current()->file().find("/mnt/cf") != -1 ) storage="CF";
         else if(dit.current()->file().find("/mnt/hda") != -1 ) storage="CF";
         else if(dit.current()->file().find("/mnt/card") != -1 ) storage="SD";
         else storage="RAM";
         if ( QFile( dit.current()->file()).exists() ) {
         newItem= /*(void)*/ new QListViewItem( audioView, dit.current()->name(), QString::number( QFile( dit.current()->file()).size() ), storage);
         newItem->setPixmap(0, Resource::loadPixmap( "mpegplayer/musicfile" ));
         }
     }
// videowidget
     
    QWidget *vTab;
    vTab = new QWidget( tabWidget, "vTab" );
    videoView = new QListView( vTab, "Videoview" );
    videoView->setMinimumSize(233,260);
    
    videoView->addColumn("Title",150);
    videoView->addColumn("Size",45);
    videoView->addColumn("Media",35);
    videoView->setColumnAlignment(1, Qt::AlignRight);
    videoView->setColumnAlignment(2, Qt::AlignRight);

    tabWidget->insertTab( vTab,"Video");

    Global::findDocuments(&vFiles, "video/*");
    QListIterator<DocLnk> Vdit( vFiles.children() );
    for ( ; Vdit.current(); ++Vdit ) {
         if( Vdit.current()->file().find("/mnt/cf") != -1 ) storage="CF";
         else if( Vdit.current()->file().find("/mnt/hda") != -1 ) storage="CF";
         else if( Vdit.current()->file().find("/mnt/card") != -1 ) storage="SD";
         else storage="RAM";
        QListViewItem * newItem;
         if ( QFile( Vdit.current()->file()).exists() ) {
        newItem= /*(void)*/ new QListViewItem( videoView, Vdit.current()->name(), QString::number( QFile( Vdit.current()->file()).size() ), storage);
        newItem->setPixmap(0, Resource::loadPixmap( "mpegplayer/videofile" ));
         }
     }
    // add the library area
    QPEApplication::setStylusOperation( this, QPEApplication::RightOnHold );

//      connect( audioView, SIGNAL( clicked( QListViewItem *) ), this, SLOT( playIt( QListViewItem *) ) );
//      connect( videoView, SIGNAL( clicked( QListViewItem *) ), this, SLOT( playIt( QListViewItem *) ) );

   connect( audioView, SIGNAL( clicked( QListViewItem *) ), this, SLOT( addToSelection( QListViewItem *) ) );
   connect( videoView, SIGNAL( clicked( QListViewItem *) ), this, SLOT( addToSelection( QListViewItem *) ) );

   connect( audioView, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint&, int ) ),
            this, SLOT( addToSelection( QListViewItem *, const QPoint&, int )) );
   connect( videoView, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint&, int ) ),
            this, SLOT( addToSelection( QListViewItem *, const QPoint&, int )) );


   connect( tabWidget, SIGNAL (currentChanged(QWidget*)),this,SLOT(tabChanged(QWidget*)));
  //   connect( d->files, SIGNAL( fileSelected( const DocLnk & ) ), this, SLOT( addToSelection( const DocLnk & ) ) );
  //   connect( d->files, SIGNAL( fileSelected( const DocLnk & ) ), this, SLOT( addToSelection( const DocLnk & ) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ),    d->tbPlay,    SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( loopingToggled( bool ) ),    d->tbLoop,    SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( shuffledToggled( bool ) ),   d->tbShuffle, SLOT( setOn( bool ) ) );

//     connect( mediaPlayerState, SIGNAL( fullscreenToggled( bool ) ), fullScreenButton,    SLOT( setOn( bool ) ) );
//      connect( mediaPlayerState, SIGNAL( scaledToggled( bool ) ),     scaleButton,   SLOT( setEnabled( bool ) ) );
//      connect( mediaPlayerState, SIGNAL( fullscreenToggled( bool ) ), fullScreenButton,   SLOT( setEnabled( bool ) ) );

    connect( mediaPlayerState, SIGNAL( playlistToggled( bool ) ),   this,         SLOT( setPlaylist( bool ) ) );

   connect( d->selectedFiles, SIGNAL( clicked( QListViewItem *) ), this, SLOT( playIt( QListViewItem *) ) );
//   connect( d->selectedFiles, SIGNAL( fileSelected( const DocLnk & ) ), this, SLOT( addToSelection( const DocLnk & ) ) );

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
//     d->tbFull->setOn( mediaPlayerState->fullscreen() );
//     d->tbScale->setOn( mediaPlayerState->scaled() );
//     d->tbScale->setEnabled( mediaPlayerState->fullscreen() );
//    setPlaylist( mediaPlayerState->playlist() );
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


void PlayListWidget::addToSelection( QListViewItem *it ,const QPoint & p, int index) {
    qDebug("add");
//        d->selectedFiles->addToSelection( lnk );
 //    }
//     else
//   mediaPlayerState->setPlaying( TRUE );
//
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

void PlayListWidget::addSelected() {
//    QMessageBox::message("Note","Bozo the clown thinks\nthere's something actually here");
    int tabPage=tabWidget->currentPageIndex();
      switch (tabPage) {
        case 0: //playlist
          break;
        case 1: { //audio
            addToSelection( audioView->selectedItem() );
                }           
            break;
        case 2: { // video
            addToSelection( videoView->selectedItem() );
                }
            break;
      };
}

void PlayListWidget::removeSelected() {
    d->selectedFiles->removeSelected( );
}


void PlayListWidget::playIt( QListViewItem *it) {
//   d->setDocumentUsed = FALSE;
        mediaPlayerState->setPlaying(true);
}

void PlayListWidget::addToSelection( QListViewItem *it) {
   d->setDocumentUsed = FALSE;
    
   if(it) {
    qDebug("add to selection");
    int tabPage=tabWidget->currentPageIndex();
    switch (tabPage) {
      case 1: {
          QListIterator<DocLnk> dit( files.children() );
          for ( ; dit.current(); ++dit ) {
              if( dit.current()->name() == it->text(0)) {
                  d->selectedFiles->addToSelection(  **dit );
              }
          }
      }
          break;
      case 2: {
          QListIterator<DocLnk> dit( vFiles.children() );
          for ( ; dit.current(); ++dit ) {
              qDebug(dit.current()->name());
              if( dit.current()->name() == it->text(0)) {
                  d->selectedFiles->addToSelection(  **dit );
              }
          }
      }
          break;
      case 0:
        break;
    };
    tabWidget->setCurrentPage(0);
//        mediaPlayerState->setPlaying( TRUE );
   }
}

void PlayListWidget::tabChanged(QWidget *widg) {

    int tabPage=tabWidget->currentPageIndex();
      switch (tabPage) {
      case 0:
      {
           d->tbRemoveFromList->setEnabled(TRUE);
           d->tbAddToList->setEnabled(FALSE);
      }
          break;
        case 1:
        {
           d->tbRemoveFromList->setEnabled(FALSE);
          d->tbAddToList->setEnabled(TRUE);
        }
            break;
        case 2:
        {
          d->tbRemoveFromList->setEnabled(FALSE);
          d->tbAddToList->setEnabled(TRUE);
        }
            break;
      };
}

// void PlayListWidget::cancelMenuTimer() {
//     if( menuTimer->isActive() )
//     menuTimer->stop();
// }

// void PlayListWidget::showFileMenu() {

// }

// void PlayListWidget::contentsMousePressEvent( QMouseEvent * e )
// {
// //    QListView::contentsMousePressEvent( e );
//     menuTimer->start( 750, TRUE );
// }


// void PlayListWidget::contentsMouseReleaseEvent( QMouseEvent * e )
// {
// //    QListView::contentsMouseReleaseEvent( e );
//     menuTimer->stop();
// }
// // void PlayListWidget::setFullScreen() {
//  mediaPlayerState->toggleFullscreen( );
// }

// void PlayListWidget::setScaled() {
//  mediaPlayerState->toggleScaled();
// }
