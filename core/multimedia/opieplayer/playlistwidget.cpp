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
// code added by L. J. Potter Sat 03-02-2002 06:17:54
#define QTOPIA_INTERNAL_FSLP
#include <qpe/qcopenvelope_qws.h>

#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/fileselector.h>
#include <qpe/qpeapplication.h>
#include <qpe/lnkproperties.h>
#include <qpe/storage.h>

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/resource.h>
#include <qaction.h>
#include <qcursor.h>
#include <qimage.h>
#include <qfile.h>
#include <qdir.h>
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
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtextstream.h>

//#include <qtimer.h>

#include "playlistselection.h"
#include "playlistwidget.h"
#include "mediaplayerstate.h"

#include "inputDialog.h"

#include <stdlib.h>
#include "audiowidget.h"
#include "videowidget.h"

#include <unistd.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

// for setBacklight()
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#define BUTTONS_ON_TOOLBAR
#define SIDE_BUTTONS
#define CAN_SAVE_LOAD_PLAYLISTS

extern AudioWidget *audioUI;
extern VideoWidget *videoUI;
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
    fromSetDocument = FALSE;
    insanityBool=FALSE;
    audioScan = FALSE;
    videoScan = FALSE;
//    menuTimer = new QTimer( this ,"menu timer"),
//     connect( menuTimer, SIGNAL( timeout() ), SLOT( addSelected() ) );

    setBackgroundMode( PaletteButton );

    setCaption( tr("OpiePlayer") );
    setIcon( Resource::loadPixmap( "opieplayer/MPEGPlayer" ) );

    setToolBarsMovable( FALSE );

      // Create Toolbar
    QPEToolBar *toolbar = new QPEToolBar( this );
    toolbar->setHorizontalStretchable( TRUE );

      // Create Menubar
    QPEMenuBar *menu = new QPEMenuBar( toolbar );
    menu->setMargin( 0 );

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setLabel( tr( "Play Operations" ) );
//      d->tbPlayCurList =  new ToolButton( bar, tr( "play List" ), "opieplayer/play_current_list",
//                                        this , SLOT( addSelected()) );
    tbDeletePlaylist = new QPushButton( Resource::loadIconSet("trash"),"",bar,"close");
    tbDeletePlaylist->setFlat(TRUE);
    tbDeletePlaylist->setFixedSize(20,20);
    
    d->tbAddToList =  new ToolButton( bar, tr( "Add to Playlist" ), "opieplayer/add_to_playlist",
                                      this , SLOT(addSelected()) );
    d->tbRemoveFromList = new ToolButton( bar, tr( "Remove from Playlist" ), "opieplayer/remove_from_playlist",
                                          this , SLOT(removeSelected()) );
//    d->tbPlay    = new ToolButton( bar, tr( "Play" ), "opieplayer/play", /*this */mediaPlayerState , SLOT(setPlaying(bool) /* btnPlay() */), TRUE );
    d->tbPlay    = new ToolButton( bar, tr( "Play" ), "opieplayer/play",
                                   this , SLOT( btnPlay(bool) ), TRUE );
    d->tbShuffle = new ToolButton( bar, tr( "Randomize" ),"opieplayer/shuffle",
                                   mediaPlayerState, SLOT(setShuffled(bool)), TRUE );
    d->tbLoop    = new ToolButton( bar, tr( "Loop" ),"opieplayer/loop",
                                   mediaPlayerState, SLOT(setLooping(bool)), TRUE );
    tbDeletePlaylist->hide();

    QPopupMenu *pmPlayList = new QPopupMenu( this );
    menu->insertItem( tr( "File" ), pmPlayList );
    new MenuItem( pmPlayList, tr( "Clear List" ), this, SLOT( clearList() ) );
    new MenuItem( pmPlayList, tr( "Add all audio files" ), this, SLOT( addAllMusicToList() ) );
    new MenuItem( pmPlayList, tr( "Add all video files" ), this, SLOT( addAllVideoToList() ) );
    new MenuItem( pmPlayList, tr( "Add all files" ), this, SLOT( addAllToList() ) );
    pmPlayList->insertSeparator(-1);
    new MenuItem( pmPlayList, tr( "Save PlayList" ), this, SLOT( saveList() ) );
    new MenuItem( pmPlayList, tr( "Export playlist to m3u" ), this, SLOT(writem3u() ) );
    pmPlayList->insertSeparator(-1);
    new MenuItem( pmPlayList, tr( "Open File or URL" ), this,SLOT( openFile() ) );
    pmPlayList->insertSeparator(-1);
    new MenuItem( pmPlayList, tr( "Rescan for Audio Files" ), this,SLOT( scanForAudio() ) );
    new MenuItem( pmPlayList, tr( "Rescan for Video Files" ), this,SLOT( scanForVideo() ) );

    QPopupMenu *pmView = new QPopupMenu( this );
    menu->insertItem( tr( "View" ), pmView );

    fullScreenButton = new QAction(tr("Full Screen"), Resource::loadPixmap("fullscreen"), QString::null, 0, this, 0);
    fullScreenButton->addTo(pmView);
    scaleButton = new QAction(tr("Scale"), Resource::loadPixmap("opieplayer/scale"), QString::null, 0, this, 0);
    scaleButton->addTo(pmView);

    QVBox *vbox5 = new QVBox( this ); vbox5->setBackgroundMode( PaletteButton );
    QVBox *vbox4 = new QVBox( vbox5 ); vbox4->setBackgroundMode( PaletteButton );

    QHBox *hbox6 = new QHBox( vbox4 ); hbox6->setBackgroundMode( PaletteButton );
    
    tabWidget = new QTabWidget( hbox6, "tabWidget" );
    tabWidget->setTabShape(QTabWidget::Triangular);
    
    QWidget *pTab;
    pTab = new QWidget( tabWidget, "pTab" );
//      playlistView = new QListView( pTab, "playlistview" );
//    playlistView->setMinimumSize(236,260);
    tabWidget->insertTab( pTab,"Playlist");


      // Add the playlist area

    QVBox *vbox3 = new QVBox( pTab ); vbox3->setBackgroundMode( PaletteButton );
    d->playListFrame = vbox3;
    d->playListFrame ->setMinimumSize(235,260);

    QHBox *hbox2 = new QHBox( vbox3 ); hbox2->setBackgroundMode( PaletteButton );

    d->selectedFiles = new PlayListSelection( hbox2);
    QVBox *vbox1 = new QVBox( hbox2 ); vbox1->setBackgroundMode( PaletteButton );

    QPEApplication::setStylusOperation( d->selectedFiles->viewport(),QPEApplication::RightOnHold);


          
    QVBox *stretch1 = new QVBox( vbox1 ); stretch1->setBackgroundMode( PaletteButton ); // add stretch
    new ToolButton( vbox1, tr( "Move Up" ),   "opieplayer/up",   d->selectedFiles, SLOT(moveSelectedUp()) );
    new ToolButton( vbox1, tr( "Remove" ),    "opieplayer/cut",  d->selectedFiles, SLOT(removeSelected()) );
    new ToolButton( vbox1, tr( "Move Down" ), "opieplayer/down", d->selectedFiles, SLOT(moveSelectedDown()) );
    QVBox *stretch2 = new QVBox( vbox1 ); stretch2->setBackgroundMode( PaletteButton ); // add stretch

    QWidget *aTab;
    aTab = new QWidget( tabWidget, "aTab" );
    audioView = new QListView( aTab, "Audioview" );
    audioView->setMinimumSize(233,260);
    audioView->addColumn( tr("Title"),140);
    audioView->addColumn(tr("Size"), -1);
    audioView->addColumn(tr("Media"),-1);
    audioView->setColumnAlignment(1, Qt::AlignRight);
    audioView->setColumnAlignment(2, Qt::AlignRight);
    audioView->setAllColumnsShowFocus(TRUE);

    audioView->setMultiSelection( TRUE );
    audioView->setSelectionMode( QListView::Extended);
    
    tabWidget->insertTab(aTab,tr("Audio"));

    QPEApplication::setStylusOperation( audioView->viewport(),QPEApplication::RightOnHold);

//    audioView
//     populateAudioView();
// videowidget
     
    QWidget *vTab;
    vTab = new QWidget( tabWidget, "vTab" );
    videoView = new QListView( vTab, "Videoview" );
    videoView->setMinimumSize(233,260);
    
    videoView->addColumn(tr("Title"),140);
    videoView->addColumn(tr("Size"),-1);
    videoView->addColumn(tr("Media"),-1);
    videoView->setColumnAlignment(1, Qt::AlignRight);
    videoView->setColumnAlignment(2, Qt::AlignRight);
    videoView->setAllColumnsShowFocus(TRUE);
    videoView->setMultiSelection( TRUE );
    videoView->setSelectionMode( QListView::Extended);

    QPEApplication::setStylusOperation( videoView->viewport(),QPEApplication::RightOnHold);

    tabWidget->insertTab( vTab,tr("Video"));
//  populateVideoView();

//playlists list
    QWidget *LTab;
    LTab = new QWidget( tabWidget, "LTab" );
    playLists = new FileSelector( "playlist/plain", LTab, "fileselector" , FALSE, FALSE); //buggy
    playLists->setMinimumSize(233,260);
    tabWidget->insertTab(LTab,tr("Lists"));

//      connect( playLists, SIGNAL( newSelected( const DocLnk &) ), this, SLOT( newFile( const DocLnk & ) ) );

// add the library area

//     connect( audioView, SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int)),
//              this, SLOT( fauxPlay( QListViewItem *) ) );
//     connect( videoView, SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int)),
//              this, SLOT( fauxPlay( QListViewItem *)) );

//    connect( audioView, SIGNAL( clicked( QListViewItem *) ), this, SLOT( fauxPlay( QListViewItem *) ) );
//    connect( videoView, SIGNAL( clicked( QListViewItem *) ), this, SLOT( fauxPlay( QListViewItem *) ) );

    connect(tbDeletePlaylist,(SIGNAL(released())),SLOT( deletePlaylist()));
    connect( fullScreenButton, SIGNAL(activated()), mediaPlayerState, SLOT(toggleFullscreen()) );
    connect( scaleButton, SIGNAL(activated()), mediaPlayerState, SLOT(toggleScaled()) );
    connect( d->selectedFiles, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
             this,SLOT( playlistViewPressed(int, QListViewItem *, const QPoint&, int)) );

    connect( audioView, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
             this,SLOT( viewPressed(int, QListViewItem *, const QPoint&, int)) );
    
    connect( audioView, SIGNAL( returnPressed( QListViewItem *)),
             this,SLOT( playIt( QListViewItem *)) );
    connect( audioView, SIGNAL( doubleClicked( QListViewItem *) ), this, SLOT( addToSelection( QListViewItem *) ) );

    connect( videoView, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
             this,SLOT( viewPressed(int, QListViewItem *, const QPoint&, int)) );
    connect( videoView, SIGNAL( returnPressed( QListViewItem *)),
             this,SLOT( playIt( QListViewItem *)) );
    connect( videoView, SIGNAL( doubleClicked( QListViewItem *) ), this, SLOT( addToSelection( QListViewItem *) ) );

    connect( playLists, SIGNAL( fileSelected( const DocLnk &) ), this, SLOT( loadList( const DocLnk & ) ) );


    connect( tabWidget, SIGNAL (currentChanged(QWidget*)),this,SLOT(tabChanged(QWidget*)));

    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ),    d->tbPlay,    SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( loopingToggled( bool ) ),    d->tbLoop,    SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( shuffledToggled( bool ) ),   d->tbShuffle, SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( playlistToggled( bool ) ),   this,         SLOT( setPlaylist( bool ) ) );

    connect( d->selectedFiles, SIGNAL( doubleClicked( QListViewItem *) ), this, SLOT( playIt( QListViewItem *) ) );
//   connect( d->selectedFiles, SIGNAL( fileSelected( const DocLnk & ) ), this, SLOT( addToSelection( const DocLnk & ) ) );

    setCentralWidget( vbox5 );

    Config cfg( "OpiePlayer" );
    readConfig( cfg );
    QString currentPlaylist = cfg.readEntry("CurrentPlaylist","");
//    qDebug("currentList is "+currentPlaylist);
    loadList(DocLnk( currentPlaylist));
    setCaption(tr("OpiePlayer: ")+ currentPlaylist );
    
    initializeStates();
}


PlayListWidget::~PlayListWidget() {
    Config cfg( "OpiePlayer" );
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
    setPlaylist( true);
//     d->selectedFiles->first();

}


void PlayListWidget::readConfig( Config& cfg ) {
    cfg.setGroup("PlayList");
    QString currentString = cfg.readEntry("current", "" );
    int noOfFiles = cfg.readNumEntry("NumberOfFiles", 0 );
    for ( int i = 0; i < noOfFiles; i++ ) {
        QString entryName;
        entryName.sprintf( "File%i", i + 1 );
        QString linkFile = cfg.readEntry( entryName );
        DocLnk lnk( linkFile );
        if ( lnk.isValid() ) {
            d->selectedFiles->addToSelection( lnk );
        }
    }
    d->selectedFiles->setSelectedItem( currentString);
//    d->selectedFiles->setSelectedItem( (const QString &)currentString);
}


void PlayListWidget::writeConfig( Config& cfg ) const {

    d->selectedFiles->writeCurrent( cfg);
    cfg.setGroup("PlayList");
    int noOfFiles = 0;
    d->selectedFiles->first();
    do {
        const DocLnk *lnk = d->selectedFiles->current();
        if ( lnk ) {
            QString entryName;
            entryName.sprintf( "File%i", noOfFiles + 1 );
//            qDebug(entryName);
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
    }
    while ( d->selectedFiles->next() );
    cfg.writeEntry("NumberOfFiles", noOfFiles );
}


void PlayListWidget::addToSelection( const DocLnk& lnk ) {
//    qDebug("add");
//     if( lnk.file().find(" ",0,TRUE) != -1 || lnk.file().find("%20",0,TRUE) != -1) {
//         QMessageBox::message("Note","You are trying to play\na malformed url.");
        
//     } else { 

        d->setDocumentUsed = FALSE;
        if ( mediaPlayerState->playlist() ) {
            if(QFileInfo(lnk.file()).exists() || lnk.file().left(4) == "http" )
                d->selectedFiles->addToSelection( lnk );
        }
        else
            mediaPlayerState->setPlaying( TRUE );
//    }
}


void PlayListWidget::clearList() {
    while ( first() )
        d->selectedFiles->removeSelected();
}


void PlayListWidget::addAllToList() {
    DocLnkSet filesAll;
    Global::findDocuments(&filesAll, "video/*;audio/*");
    QListIterator<DocLnk> Adit( filesAll.children() );
    for ( ; Adit.current(); ++Adit )
        if(QFileInfo(Adit.current()->file()).exists())
            d->selectedFiles->addToSelection( **Adit );
}


void PlayListWidget::addAllMusicToList() {
    QListIterator<DocLnk> dit( files.children() );
    for ( ; dit.current(); ++dit ) 
        if(QFileInfo(dit.current()->file()).exists())
            d->selectedFiles->addToSelection( **dit );
}


void PlayListWidget::addAllVideoToList() {
    QListIterator<DocLnk> dit( vFiles.children() );
    for ( ; dit.current(); ++dit )
        if(QFileInfo( dit.current()->file()).exists())
            d->selectedFiles->addToSelection( **dit );
}


void PlayListWidget::setDocument(const QString& fileref) {
    qDebug(fileref);
    fromSetDocument = TRUE;
    if ( fileref.isNull() ) {
        QMessageBox::critical( 0, tr( "Invalid File" ), tr( "There was a problem in getting the file." ) );
        return;
    }
//    qDebug("setDocument "+fileref);
    if(fileref.find("m3u",0,TRUE) != -1) { //is m3u
        readm3u( fileref);
    }
    else if(fileref.find("pls",0,TRUE) != -1) { //is pls
        readPls( fileref);
    }
    else if(fileref.find("playlist",0,TRUE) != -1) {//is playlist
        clearList();
        loadList(DocLnk(fileref));
        d->selectedFiles->first();
    } else { 
        clearList();
        addToSelection( DocLnk( fileref ) );
        d->setDocumentUsed = TRUE;
        mediaPlayerState->setPlaying( FALSE );
        qApp->processEvents();
        mediaPlayerState->setPlaying( TRUE );
        qApp->processEvents();
        setCaption(tr("OpiePlayer"));
    }
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


const DocLnk *PlayListWidget::current() { // this is fugly

//      if( fromSetDocument) {
//          qDebug("from setDoc");
//          DocLnkSet files;
//          Global::findDocuments(&files, "video/*;audio/*");
//          QListIterator<DocLnk> dit( files.children() );
//          for ( ; dit.current(); ++dit ) {
//              if(dit.current()->linkFile() ==  setDocFileRef) {
//                  qDebug(setDocFileRef);
//                  return dit;
//              }
//          }
//      } else


    switch (tabWidget->currentPageIndex()) {
      case 0: //playlist
      {
          qDebug("playlist");
          if ( mediaPlayerState->playlist() ) {
              return d->selectedFiles->current();
          }
          else if ( d->setDocumentUsed && d->current ) {
              return d->current;
          } else {
              return d->files->selected();
          }
      }
      break;
      case 1://audio
      {
          qDebug("audioView");
          QListIterator<DocLnk> dit( files.children() );
          for ( ; dit.current(); ++dit ) {
              if( dit.current()->name() == audioView->currentItem()->text(0) && !insanityBool) {
                  qDebug("here");
                  insanityBool=TRUE;
                  return dit;
              }
          }
      }           
      break;
      case 2: // video
      {
          qDebug("videoView");
          QListIterator<DocLnk> Vdit( vFiles.children() );
          for ( ; Vdit.current(); ++Vdit ) {
              if( Vdit.current()->name() == videoView->currentItem()->text(0) && !insanityBool) {
                  insanityBool=TRUE;
                  return Vdit;
              }
          }
      }
      break;
    };
    return 0;
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
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Save Playlist"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        if ( d->current )
            delete d->current;
        filename = fileDlg->LineEdit1->text();//+".playlist";
//         qDebug("saving playlist "+filename+".playlist");
        Config cfg( filename +".playlist");
        writeConfig( cfg );

        DocLnk lnk;
//        lnk.setComment( "");
        lnk.setFile(QDir::homeDirPath()+"/Settings/"+filename+".playlist.conf"); //sets File property
        lnk.setType("playlist/plain");// hey is this a REGISTERED mime type?!?!? ;D
        lnk.setIcon("opieplayer/playlist2");
        lnk.setName( filename); //sets file name
//        qDebug(filename);
        if(!lnk.writeLink())
            qDebug("Writing doclink did not work");
    }
    Config config( "OpiePlayer" );
    config.writeEntry("CurrentPlaylist",filename);
    setCaption(tr("OpiePlayer: ")+filename);
    d->selectedFiles->first();
    if(fileDlg)
        delete fileDlg;
}

void PlayListWidget::loadList( const DocLnk & lnk) {
    QString name= lnk.name();
//    qDebug("currentList is "+name);
    if( name.length()>1) {
        setCaption("OpiePlayer: "+name);
//    qDebug("load list "+ name+".playlist");
        clearList();
        Config cfg( name+".playlist");
        readConfig(cfg);

        tabWidget->setCurrentPage(0);

        Config config( "OpiePlayer" );
        config.writeEntry("CurrentPlaylist", name);
//        d->selectedFiles->first();
    }
    
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

    Config cfg( "OpiePlayer" );
    cfg.setGroup("PlayList");
    QString currentPlaylist = cfg.readEntry("CurrentPlaylist","");
    int noOfFiles = cfg.readNumEntry("NumberOfFiles", 0 );

    switch (tabWidget->currentPageIndex()) {
      case 0: //playlist
          break;
      case 1: { //audio
//                 QString entryName;
//                 entryName.sprintf( "File%i", i + 1 );
//                 QString linkFile = cfg.readEntry( entryName );
          QListViewItemIterator it(  audioView  );
            // iterate through all items of the listview
          for ( ; it.current(); ++it ) {
              if ( it.current()->isSelected() ) {
                  QListIterator<DocLnk> dit( files.children() );
                  for ( ; dit.current(); ++dit ) {
                      if( dit.current()->name() == it.current()->text(0) ) {
                          d->selectedFiles->addToSelection(  **dit );
                      }
                  }
                  audioView->setSelected( it.current(),FALSE);
              }
          }
          tabWidget->setCurrentPage(0);
      }           
      break;
      case 2: { // video
          QListViewItemIterator it(  videoView  );
            // iterate through all items of the listview
          for ( ; it.current(); ++it ) {
              if ( it.current()->isSelected() ) {
                  QListIterator<DocLnk> dit( vFiles.children() );
                  for ( ; dit.current(); ++dit ) {
                      if( dit.current()->name() == it.current()->text(0) ) {
                          d->selectedFiles->addToSelection(  **dit );
                      }
                  }

                  videoView->setSelected( it.current(),FALSE);
              }
          }
//             for ( int i = 0; i < noOfFiles; i++ ) {
//                 QString entryName;
//                 entryName.sprintf( "File%i", i + 1 );
//                 QString linkFile = cfg.readEntry( entryName );
//                   if( DocLnk( linkFile).name() == videoView->selectedItem()->text(0) ) {
//                       int result= QMessageBox::warning(this,tr("OpiePlayer"),
//                                   tr("This is all ready in your playlist.\nContinue?"),
//                                   tr("Yes"),tr("No"),0,0,1);
//                       if (result !=0)
//                           return;
//                   }
//             }
//             addToSelection( videoView->selectedItem() );
          tabWidget->setCurrentPage(0);
      }
      break;
    };
}

void PlayListWidget::removeSelected() {
    d->selectedFiles->removeSelected( );
}

void PlayListWidget::playIt( QListViewItem *it) {
//   d->setDocumentUsed = FALSE;
//  mediaPlayerState->curPosition =0;
    qDebug("playIt");
    mediaPlayerState->setPlaying(FALSE);
    mediaPlayerState->setPlaying(TRUE);
    d->selectedFiles->unSelect();
}

void PlayListWidget::addToSelection( QListViewItem *it) {
    d->setDocumentUsed = FALSE;
    
    if(it) {
        switch (tabWidget->currentPageIndex()) {
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
    }
}

void PlayListWidget::tabChanged(QWidget *widg) {

    switch ( tabWidget->currentPageIndex()) {
      case 0:
      {
          if( !tbDeletePlaylist->isHidden())
              tbDeletePlaylist->hide();
          d->tbRemoveFromList->setEnabled(TRUE);
          d->tbAddToList->setEnabled(FALSE);
      }
      break;
      case 1:
      {
          audioView->clear();
          populateAudioView();

          if( !tbDeletePlaylist->isHidden())
              tbDeletePlaylist->hide();
          d->tbRemoveFromList->setEnabled(FALSE);
          d->tbAddToList->setEnabled(TRUE);
      }
      break;
      case 2:
      {
          videoView->clear();
          populateVideoView();
          if( !tbDeletePlaylist->isHidden())
              tbDeletePlaylist->hide();
          d->tbRemoveFromList->setEnabled(FALSE);
          d->tbAddToList->setEnabled(TRUE);
      }
      break;
      case 3:
      {
          if( tbDeletePlaylist->isHidden())
              tbDeletePlaylist->show();
          playLists->reread();
      }
      break;
    };
}

void PlayListWidget::btnPlay(bool b) {

//    mediaPlayerState->setPlaying(b);
    switch ( tabWidget->currentPageIndex()) {
      case 0:
      {
//            if( d->selectedFiles->current()->file().find(" ",0,TRUE) != -1
//                if( d->selectedFiles->current()->file().find("%20",0,TRUE) != -1) {
//                 QMessageBox::message("Note","You are trying to play\na malformed url.");
//            } else { 
              mediaPlayerState->setPlaying(b);
//          }
    }
      break;
      case 1:
      {
          addToSelection( audioView->currentItem() );
          mediaPlayerState->setPlaying(b);
          d->selectedFiles->removeSelected( );
          tabWidget->setCurrentPage(1);
          d->selectedFiles->unSelect();
          insanityBool=FALSE;         
      }//          audioView->clearSelection();
      break;
      case 2:
      {
          addToSelection( videoView->currentItem() );
          mediaPlayerState->setPlaying(b);
          qApp->processEvents();
          d->selectedFiles->removeSelected( );
          tabWidget->setCurrentPage(2);
          d->selectedFiles->unSelect();
          insanityBool=FALSE;
      }//          videoView->clearSelection();
      break;
    };
 
}

void PlayListWidget::deletePlaylist() {
    switch( QMessageBox::information( this, (tr("Remove Playlist?")),
                                      (tr("You really want to delete\nthis playlist?")),
                                      (tr("Yes")), (tr("No")), 0 )){
      case 0: // Yes clicked,
          QFile().remove(playLists->selected()->file());
          QFile().remove(playLists->selected()->linkFile());
          playLists->reread();
          break;
      case 1: // Cancel
          break;
    };
}

void PlayListWidget::viewPressed( int mouse, QListViewItem *item, const QPoint& point, int i)
{
    switch (mouse) {
      case 1:
          break;
      case 2:{

          QPopupMenu  m;
          m.insertItem( tr( "Play" ), this, SLOT( playSelected() ));
          m.insertItem( tr( "Add to Playlist" ), this, SLOT( addSelected() ));
          m.insertSeparator();
          if( QFile(QPEApplication::qpeDir()+"lib/libopie.so").exists() ) 
              m.insertItem( tr( "Properties" ), this, SLOT( listDelete() ));
    
          m.exec( QCursor::pos() );
      }
      break;
    };
}

void PlayListWidget::playSelected()
{
    btnPlay( TRUE);
//    d->selectedFiles->unSelect();
}

void PlayListWidget::playlistViewPressed( int mouse, QListViewItem *item, const QPoint& point, int i)
{
    switch (mouse) {
      case 1:
          
          break;
      case 2:{
          QPopupMenu  m;
          m.insertItem( tr( "Play Selected" ), this, SLOT( playSelected() ));
          m.insertItem( tr( "Remove" ), this, SLOT( removeSelected() ));
//       m.insertSeparator();
//     m.insertItem( tr( "Properties" ), this, SLOT( listDelete() ));
          m.exec( QCursor::pos() );
      }
      break;
    };

}

void PlayListWidget::listDelete() {
    Config cfg( "OpiePlayer" );
    cfg.setGroup("PlayList");
    QString currentPlaylist = cfg.readEntry("CurrentPlaylist","");
    QString file;
    int noOfFiles = cfg.readNumEntry("NumberOfFiles", 0 );
    switch ( tabWidget->currentPageIndex()) {
      case 0: 
          break;
      case 1:
      {
          file = audioView->selectedItem()->text(0);
//           Global::findDocuments(&files, "audio/*");
//           AppLnkSet appFiles;
          QListIterator<DocLnk> dit( files.children() );
          for ( ; dit.current(); ++dit ) {
              if( dit.current()->name() == file) {
//                  qDebug(file);
                  LnkProperties prop( dit.current() );
//  connect(&prop, SIGNAL(select(const AppLnk *)), this, SLOT(externalSelected(const AppLnk *)));
                  prop.showMaximized();
                  prop.exec();
              }
          }
          populateAudioView();
      }
      break;
      case 2:
      {
//           file = videoView->selectedItem()->text(0);
//           for ( int i = 0; i < noOfFiles; i++ ) {
//               QString entryName;
//               entryName.sprintf( "File%i", i + 1 );
//               QString linkFile = cfg.readEntry( entryName );
//                   AppLnk lnk( AppLnk(linkFile));
//               if( lnk.name() == file ) {
//                   LnkProperties prop( &lnk);
// //  connect(&prop, SIGNAL(select(const AppLnk *)), this, SLOT(externalSelected(const AppLnk *)));
//                   prop.showMaximized();
//                   prop.exec();
//               }
//            }
      }
      break;
    };
}

void PlayListWidget::scanForAudio() {
    qDebug("scan for audio");
    files.detachChildren();
    QListIterator<DocLnk> sdit( files.children() );
    for ( ; sdit.current(); ++sdit ) {
        delete sdit.current();
    }
    Global::findDocuments(&files, "audio/*");
    audioScan = TRUE;
}
void PlayListWidget::scanForVideo() {
    qDebug("scan for video");
    vFiles.detachChildren();
    QListIterator<DocLnk> sdit( vFiles.children() );
    for ( ; sdit.current(); ++sdit ) {
        delete sdit.current();
    }
    Global::findDocuments(&vFiles, "video/*");
    videoScan = TRUE;
}

void PlayListWidget::populateAudioView() {

    audioView->clear();
    StorageInfo storageInfo;
    const QList<FileSystem> &fs = storageInfo.fileSystems();
    if(!audioScan) scanForAudio();

    QListIterator<DocLnk> dit( files.children() );
    QListIterator<FileSystem> it ( fs );

    QString storage;
    for ( ; dit.current(); ++dit ) {
        for( ; it.current(); ++it ){
            const QString name = (*it)->name();
            const QString path = (*it)->path();
            if(dit.current()->file().find(path) != -1 ) storage=name;
        }

        QListViewItem * newItem;
        if ( QFile( dit.current()->file()).exists()  ) {
//              qDebug(dit.current()->name());
            newItem= /*(void)*/ new QListViewItem( audioView, dit.current()->name(),
                                                   QString::number( QFile( dit.current()->file()).size() ), storage);
            newItem->setPixmap(0, Resource::loadPixmap( "opieplayer/musicfile" ));
        }
    }

}

void PlayListWidget::populateVideoView() {
    videoView->clear();
    StorageInfo storageInfo;
    const QList<FileSystem> &fs = storageInfo.fileSystems();

    if(!videoScan ) scanForVideo();

    QListIterator<DocLnk> Vdit( vFiles.children() );
    QListIterator<FileSystem> it ( fs );
    videoView->clear();
    QString storage;
    for ( ; Vdit.current(); ++Vdit ) {
        for( ; it.current(); ++it ){
            const QString name = (*it)->name();
            const QString path = (*it)->path();
            if( Vdit.current()->file().find(path) != -1 ) storage=name;
        }

        QListViewItem * newItem;
        if ( QFile( Vdit.current()->file()).exists() ) {
            newItem= /*(void)*/ new QListViewItem( videoView, Vdit.current()->name(),
                                                   QString::number( QFile( Vdit.current()->file()).size() ), storage);
            newItem->setPixmap(0, Resource::loadPixmap( "opieplayer/videofile" ));
        }
    }
}

void PlayListWidget::openFile() {
    QString filename, name;
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Open file or URL"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        filename = fileDlg->LineEdit1->text();
// http://205.188.234.129:8030
// http://66.28.68.70:8000
//        filename.replace(QRegExp("%20")," ");
        if(filename.find(" ",0,TRUE) != -1 || filename.find("%20",0,TRUE) != -1) {
            QMessageBox::message("Note","Spaces in urls are not allowed.");
            return;
        } else {
            qDebug("Selected filename is "+filename);
            if(filename.right(3) == "m3u")
                readm3u( filename);
            else if(filename.right(3) == "pls")
                readPls( filename);
            else {
                DocLnk lnk;

                lnk.setName(filename); //sets file name
                if(filename.right(1) != "/" && filename.right(3) != "mp3" && filename.right(3) != "MP3")
                    filename += "/";
                lnk.setFile(filename); //sets File property

                lnk.setType("audio/x-mpegurl");
                lnk.setExec("opieplayer");
                lnk.setIcon("opieplayer/MPEGPlayer");

                if(!lnk.writeLink()) 
                    qDebug("Writing doclink did not work");
                d->selectedFiles->addToSelection(  lnk);
//     if(fileDlg2)
//         delete fileDlg2;
            }
        }
    }
    if(fileDlg)
        delete fileDlg;
}

void PlayListWidget::keyReleaseEvent( QKeyEvent *e)
{
    switch ( e->key() ) {
////////////////////////////// Zaurus keys
      case Key_F9: //activity
//           if(audioUI->isHidden())
//             audioUI->showMaximized();
          break;
      case Key_F10: //contacts
//           if( videoUI->isHidden())
//             videoUI->showMaximized();
          break;
      case Key_F11: //menu
          break;
      case Key_F12: //home
//           doBlank();
          break;
      case Key_F13: //mail
//           doUnblank();
          break;
      case Key_Q: //add to playlist
          qDebug("Add");
          addSelected();
          break;
      case Key_R: //remove from playlist
          removeSelected();
          break;
//       case Key_P: //play
//           qDebug("Play");
//           playSelected();
//           break;
      case Key_Space:
          qDebug("Play");
//          playSelected(); puh
          break;
      case Key_1:
          tabWidget->setCurrentPage(0);
          break;
      case Key_2:
          tabWidget->setCurrentPage(1);
          break;
      case Key_3:
          tabWidget->setCurrentPage(2);
          break;
      case Key_4:
          tabWidget->setCurrentPage(3);         
          break;
    }
}

void PlayListWidget::keyPressEvent( QKeyEvent *e)
{
//    qDebug("Key press");
//     switch ( e->key() ) {
// ////////////////////////////// Zaurus keys
//       case Key_A: //add to playlist
//           qDebug("Add");
//           addSelected();
//           break;
//       case Key_R: //remove from playlist
//           removeSelected();
//           break;
//       case Key_P: //play
//           qDebug("Play");
//           playSelected();
//           break;
//       case Key_Space:
//           qDebug("Play");
//           playSelected();
//           break;
//     }
}

void PlayListWidget::doBlank() {
    qDebug("do blanking");
    fd=open("/dev/fb0",O_RDWR);
    if (fd != -1) {
        ioctl(fd,FBIOBLANK,1);
//            close(fd);
    }
}

void PlayListWidget::doUnblank() {
      // this crashes opieplayer with a segfault
//      int fd;
//       fd=open("/dev/fb0",O_RDWR);
    qDebug("do unblanking");
    if (fd != -1) {
        ioctl(fd,FBIOBLANK,0);
        close(fd);
    }
    QCopEnvelope h("QPE/System", "setBacklight(int)");
    h <<-3;// v[1]; // -3 Force on
}

void PlayListWidget::readm3u(const QString &filename) {

    qDebug("m3u filename is "+filename);
    QFile f(filename);

    if(f.open(IO_ReadOnly)) {
        QTextStream t(&f);
        QString s;//, first, second;
        int i=0;
        while ( !t.atEnd()) {
//        Lview->insertLine(t.readLine(),-1);
            s=t.readLine();
            if(s.find(" ",0,TRUE) != -1 || s.find("%20",0,TRUE) != -1) {
                QMessageBox::message("Note","Spaces in urls are not allowed.");
            } 
            else if(s.find("#",0,TRUE) == -1) {
                if(s.find(" ",0,TRUE) == -1)  { // not sure if this is neede since cf uses vfat
                    if(s.left(2) == "E:" || s.left(2) == "P:") {
                        s=s.right(s.length()-2);
                        DocLnk lnk( s );
                        QFileInfo f(s);
                        QString name = f.baseName();
                        name = name.right(name.length()-name.findRev("\\",-1,TRUE)-1);
                        lnk.setName( name);
                        s=s.replace( QRegExp("\\"),"/");
                        lnk.setFile( s);
                        lnk.writeLink();
//                         lnk.setIcon(opieplayer/MPEGPlayer);
                        qDebug("add "+name);
                        d->selectedFiles->addToSelection( lnk);
                    } else { // is url

                        s.replace(QRegExp("%20")," ");
                        DocLnk lnk( s);
                        QString name;
                        if(name.left(4)=="http")
                            name = s.right( s.length() - 7);
                        else
                            name=s;
//                         name = name.right(name.length()-name.findRev("\\",-1,TRUE)-1);
                        lnk.setName(name);
                        if(s.at(s.length()-4) == '.')
                            lnk.setFile( s);
                        else
                            lnk.setFile( s+"/");
//                         lnk.setFile( filename);
//                         lnk.setComment( s+"/");
                        lnk.setType("audio/x-mpegurl");
                        lnk.writeLink();
//                          lnk.setIcon( "opieplayer/MPEGPlayer");
//                          qDebug("add "+s);
                        d->selectedFiles->addToSelection( lnk);
                    }
                    i++;
                }
            }
        }
    }
    f.close();
}

void PlayListWidget::writem3u() {

    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Save m3u Playlist "),TRUE, 0);
    fileDlg->exec();
        QString filename,list;
    if( fileDlg->result() == 1 ) {
        filename = fileDlg->LineEdit1->text();
        qDebug(filename);
        int noOfFiles = 0;
        d->selectedFiles->first();
        do {
// we dont check for existance because of url's
//         qDebug(d->selectedFiles->current()->file());
                list += d->selectedFiles->current()->file()+"\n";
                noOfFiles++;
        }
        while ( d->selectedFiles->next() );
        qDebug(list);
        if(filename.left(1) != "/")
            filename=QPEApplication::documentDir()+"/"+filename;
        if(filename.right(3) != "m3u")
            filename=filename+".m3u";
        
        QFile f(filename);
        f.open(IO_WriteOnly);
        f.writeBlock(list, list.length());
        f.close();
    }
    if(fileDlg) delete fileDlg;
}

void PlayListWidget::readPls(const QString &filename) {

    qDebug("pls filename is "+filename);
    QFile f(filename);

    if(f.open(IO_ReadOnly)) {
        QTextStream t(&f);
        QString s;//, first, second;
        int i=0;
        while ( !t.atEnd()) {
            s=t.readLine();
            if(s.left(4) == "File") {
                s=s.right(s.length() - 6);
                s.replace(QRegExp("%20")," ");
                qDebug("adding "+s+" to playlist");
// numberofentries=2
// File1=http
// Title
// Length
// Version
// File2=http
             
                s=s.replace( QRegExp("\\"),"/");
                DocLnk lnk( s );
                QFileInfo f(s);
                QString name = f.baseName();
                if(name.left(4)=="http")
                    name = s.right( s.length() - 7);
                else
                    name=s;
                name = name.right(name.length()-name.findRev("\\",-1,TRUE)-1);
//             QFileInfo f(s);
//             QString name = f.baseName();
//             name = name.left(name.length()-4);
//             name = name.right(name.findRev("/",0,TRUE));
                lnk.setName( name);
                if(s.at(s.length()-4) == '.')
                    lnk.setFile( s);
                else {
                 if( name.right(1).find('/') == -1)
                   s+="/";
                   //                  if(s.right(1) != '/')
                    lnk.setFile( s);

                }
                lnk.setType("audio/x-mpegurl");

                qDebug("DocLnk add "+name);
                d->selectedFiles->addToSelection( lnk);
            }
        }
        i++;
    }
}

