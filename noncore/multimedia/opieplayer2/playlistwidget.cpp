/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qpe/qpetoolbar.h>
#include <qpe/qpeapplication.h>
#include <qpe/storage.h>
#include <qpe/mimetype.h>
#include <qpe/global.h>
#include <qpe/resource.h>

#include <qdatetime.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtextstream.h>

#include "playlistselection.h"
#include "playlistwidget.h"
#include "mediaplayerstate.h"
#include "inputDialog.h"
#include "om3u.h"
#include "playlistfileview.h"

//only needed for the random play
#include <stdlib.h>
#include <assert.h>

#include "audiowidget.h"
#include "videowidget.h"

extern MediaPlayerState *mediaPlayerState;
// extern AudioWidget *audioUI;
// extern VideoWidget *videoUI;

PlayListWidget::PlayListWidget( MediaPlayerState &mediaPlayerState, QWidget* parent, const char* name )
    : PlayListWidgetGui( mediaPlayerState, parent, name ) {

    d->tbAddToList =  new ToolButton( bar, tr( "Add to Playlist" ),
                                      "opieplayer2/add_to_playlist",
                                      this , SLOT(addSelected() ) );
    d->tbRemoveFromList = new ToolButton( bar, tr( "Remove from Playlist" ),
                                          "opieplayer2/remove_from_playlist",
                                          this , SLOT(removeSelected() ) );
    d->tbPlay    = new ToolButton( bar, tr( "Play" ), "opieplayer2/play",
                                   this , SLOT( btnPlay( bool) ), TRUE );
    d->tbShuffle = new ToolButton( bar, tr( "Randomize" ),"opieplayer2/shuffle",
                                   &mediaPlayerState, SLOT( setShuffled( bool ) ), TRUE );
    d->tbLoop    = new ToolButton( bar, tr( "Loop" ), "opieplayer2/loop",
                                   &mediaPlayerState, SLOT( setLooping( bool ) ), TRUE );

    (void)new MenuItem( pmPlayList, tr( "Clear List" ), this, SLOT( clearList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all audio files" ),
                        this, SLOT( addAllMusicToList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all video files" ),
                        this, SLOT( addAllVideoToList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all files" ),
                        this, SLOT( addAllToList() ) );
    pmPlayList->insertSeparator(-1);
//     (void)new MenuItem( pmPlayList, tr( "Save PlayList" ),
//                         this, SLOT( saveList() ) );
    (void)new MenuItem( pmPlayList, tr( "Save Playlist" ),
                        this, SLOT(writem3u() ) );
    pmPlayList->insertSeparator(-1);
    (void)new MenuItem( pmPlayList, tr( "Open File or URL" ),
                        this,SLOT( openFile() ) );
    pmPlayList->insertSeparator(-1);
    (void)new MenuItem( pmPlayList, tr( "Rescan for Audio Files" ),
                        audioView, SLOT( scanFiles() ) );
    (void)new MenuItem( pmPlayList, tr( "Rescan for Video Files" ),
                        videoView, SLOT( scanFiles() ) );

    pmView->insertItem(  Resource::loadPixmap("fullscreen") , tr( "Full Screen"),
                         &mediaPlayerState, SLOT( toggleFullscreen() ) );

    Config cfg( "OpiePlayer" );
    bool b= cfg.readBoolEntry("FullScreen", 0);
    mediaPlayerState.setFullscreen(  b );
    pmView->setItemChecked( -16, b );

    (void)new ToolButton( vbox1, tr( "Move Up" ),   "opieplayer2/up",
                          d->selectedFiles, SLOT(moveSelectedUp() ) );
    (void)new ToolButton( vbox1, tr( "Remove" ),    "opieplayer2/cut",
                          d->selectedFiles, SLOT(removeSelected() ) );
    (void)new ToolButton( vbox1, tr( "Move Down" ), "opieplayer2/down",
                          d->selectedFiles, SLOT(moveSelectedDown() ) );
    //    QVBox *stretch2 = new QVBox( vbox1 );

    connect( tbDeletePlaylist, ( SIGNAL( released() ) ),
             SLOT( deletePlaylist() ) );
    connect( pmView, SIGNAL( activated( int ) ),
             this, SLOT( pmViewActivated( int ) ) );
    connect( skinsMenu, SIGNAL( activated( int ) ) ,
             this, SLOT( skinsMenuActivated( int ) ) );
    connect( d->selectedFiles, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int) ),
             this,SLOT( playlistViewPressed( int, QListViewItem *, const QPoint&, int ) ) );
    connect( audioView, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int ) ),
             this,SLOT( viewPressed( int, QListViewItem *, const QPoint&, int ) ) );
    connect( audioView, SIGNAL( returnPressed( QListViewItem *) ),
             this,SLOT( playIt( QListViewItem *) ) );
    connect( audioView, SIGNAL( doubleClicked( QListViewItem *) ),
             this, SLOT( addToSelection( QListViewItem *) ) );
    connect( videoView, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int) ),
             this,SLOT( viewPressed( int, QListViewItem *, const QPoint&, int) ) );
    connect( videoView, SIGNAL( returnPressed( QListViewItem *) ),
             this,SLOT( playIt( QListViewItem *) ) );
    connect( videoView, SIGNAL( doubleClicked( QListViewItem *) ),
             this, SLOT( addToSelection( QListViewItem *) ) );
    connect( playLists, SIGNAL( fileSelected( const DocLnk &) ),
             this, SLOT( loadList( const DocLnk & ) ) );
    connect( tabWidget, SIGNAL ( currentChanged(QWidget*) ),
             this, SLOT( tabChanged( QWidget* ) ) );
    connect( &mediaPlayerState, SIGNAL( playingToggled( bool ) ),
             d->tbPlay,  SLOT( setOn( bool ) ) );
    connect( &mediaPlayerState, SIGNAL( loopingToggled( bool ) ),
             d->tbLoop, SLOT( setOn( bool ) ) );
    connect( &mediaPlayerState, SIGNAL( shuffledToggled( bool ) ),
             d->tbShuffle, SLOT( setOn( bool ) ) );
    connect( d->selectedFiles, SIGNAL( doubleClicked( QListViewItem *) ),
             this, SLOT( playIt( QListViewItem *) ) );
    connect ( gammaSlider,  SIGNAL( valueChanged( int ) ),
              &mediaPlayerState,  SLOT( setVideoGamma( int ) ) );

    // see which skins are installed
    populateSkinsMenu();
    initializeStates();

    cfg.setGroup("PlayList");
    QString currentPlaylist = cfg.readEntry( "CurrentPlaylist", "default");
    loadList(DocLnk(  currentPlaylist ) );
}


PlayListWidget::~PlayListWidget() {
    delete d;
}


void PlayListWidget::initializeStates() {
    d->tbPlay->setOn( mediaPlayerState.isPlaying() );
    d->tbLoop->setOn( mediaPlayerState.isLooping() );
    d->tbShuffle->setOn( mediaPlayerState.isShuffled() );
    d->playListFrame->show();
}

void PlayListWidget::writeDefaultPlaylist() {

    Config config( "OpiePlayer" );
    config.setGroup( "PlayList" );
    QString filename=QPEApplication::documentDir() + "/default.m3u";
    QString currentString = config.readEntry( "CurrentPlaylist", filename);
    if( currentString == filename) {
      Om3u *m3uList;
      //    qDebug("<<<<<<<<<<<<<default>>>>>>>>>>>>>>>>>>>");
      if( d->selectedFiles->first() ) {
        m3uList = new Om3u(filename, IO_ReadWrite | IO_Truncate);
        do {
          //        qDebug(d->selectedFiles->current()->file());
          m3uList->add( d->selectedFiles->current()->file() );
        }
        while ( d->selectedFiles->next() );

        m3uList->write();
        m3uList->close();
        delete m3uList;
        
      }
    }
}

void PlayListWidget::addToSelection( const DocLnk& lnk ) {
    d->setDocumentUsed = FALSE;
    if( QFileInfo( lnk.file() ).exists() ||
        lnk.file().left(4) == "http" ) {
        d->selectedFiles->addToSelection( lnk );
    }
//          writeCurrentM3u();          
}


void PlayListWidget::clearList() {
    while ( first() ) {
        d->selectedFiles->removeSelected();
    }
}

void PlayListWidget::viewPressed( int mouse, QListViewItem *, const QPoint& , int) {
    switch (mouse) {
    case 1:
        break;
    case 2:
    {
        QPopupMenu  m;
        m.insertItem( tr( "Play Selected" ), this, SLOT( playSelected() ));
        m.insertItem( tr( "Add to Playlist" ), this, SLOT( addSelected() ));
        m.exec( QCursor::pos() );
    }
        break;
    }
}


void PlayListWidget::playlistViewPressed( int mouse, QListViewItem *, const QPoint& , int ) {
    switch (mouse) {
    case 1:
        break;
    case 2:
    {
        QPopupMenu  m;
        m.insertItem( tr( "Play" ), this, SLOT( playSelected() ));
        m.insertItem( tr( "Remove" ), this, SLOT( removeSelected() ));
        m.exec( QCursor::pos() );
    }
    break;
    }
}


void PlayListWidget::addAllToList() {

//   QTime t;
//   t.start();

  audioView->populateView();

  QListViewItemIterator audioIt( audioView );
  DocLnk lnk;
  QString filename;
  // iterate through all items of the listview
  for ( ; audioIt.current(); ++audioIt ) {
    filename = audioIt.current()->text(3);
    lnk.setName( QFileInfo(filename).baseName() ); //sets name
    lnk.setFile( filename ); //sets file name
    d->selectedFiles->addToSelection(  lnk);
  }      

  videoView->populateView();

  QListViewItemIterator videoIt( videoView );
  for ( ; videoIt.current(); ++videoIt ) {
    filename = videoIt.current()->text(3);
    lnk.setName( QFileInfo(filename).baseName() ); //sets name
    lnk.setFile( filename ); //sets file name
    d->selectedFiles->addToSelection(  lnk);
  }      

  //      d->selectedFiles->addToSelection(  );
      //      if ( it.current()->isSelected() )
      //        lst->append( audioIt.current() );
  //    }
    

//       if(!audioScan)
//       scanForAudio();
//     if(!videoScan) 
//       scanForVideo();

//      DocLnkSet filesAll;
//      Global::findDocuments(&filesAll, "video/*;"+audioMimes);
//      QListIterator<DocLnk> Adit( filesAll.children() );
//      for ( ; Adit.current(); ++Adit ) {
//          if( QFileInfo( Adit.current()->file() ).exists() ) {
//              d->selectedFiles->addToSelection( **Adit );
//          }
//      }

  //     qDebug("elapsed time %d", t.elapsed() );

    tabWidget->setCurrentPage(0);
    
    writeCurrentM3u();
    d->selectedFiles->first();
}


void PlayListWidget::addAllMusicToList() {

  audioView->populateView();

  QListViewItemIterator audioIt( audioView );
  DocLnk lnk;
  QString filename;
  // iterate through all items of the listview
  for ( ; audioIt.current(); ++audioIt ) {
    filename = audioIt.current()->text(3);
    lnk.setName( QFileInfo(filename).baseName() ); //sets name
    lnk.setFile( filename ); //sets file name
    d->selectedFiles->addToSelection(  lnk);
  }      

    /*    if(!audioScan)
        scanForAudio();
    QListIterator<DocLnk> dit( files.children() );
    for ( ; dit.current(); ++dit ) {
        if( QFileInfo(dit.current()->file() ).exists() ) {
            d->selectedFiles->addToSelection( **dit );
        }
    }
    */
    tabWidget->setCurrentPage(0);
    writeCurrentM3u();
    d->selectedFiles->first();
}


void PlayListWidget::addAllVideoToList() {

  videoView->populateView();

  QListViewItemIterator videoIt( videoView );
  DocLnk lnk;
  QString filename;
  for ( ; videoIt.current(); ++videoIt ) {
    filename = videoIt.current()->text(3);
    lnk.setName( QFileInfo(filename).baseName() ); //sets name
    lnk.setFile( filename ); //sets file name
    d->selectedFiles->addToSelection(  lnk);
  }      


  /*    if(!videoScan)
        scanForVideo();
    QListIterator<DocLnk> dit( vFiles.children() );
    for ( ; dit.current(); ++dit ) {
        if( QFileInfo( dit.current()->file() ).exists() ) {
            d->selectedFiles->addToSelection( **dit );
        }
    }
*/    
    tabWidget->setCurrentPage(0);
    writeCurrentM3u();          
    d->selectedFiles->first();
}


void PlayListWidget::setDocument( const QString& fileref ) {
  //    qDebug( "<<<<<<<<set document>>>>>>>>>> "+fileref );
    fromSetDocument = TRUE;
    if ( fileref.isNull() ) {
        QMessageBox::warning( this, tr( "Invalid File" ),
                               tr( "There was a problem in getting the file." ) );
        return;
    }

    clearList();
    if( fileref.find( "m3u", 0, TRUE) != -1  ) { //is m3u
        readm3u( fileref );
    } else if( DocLnk( fileref).file().find( "m3u", 0, TRUE) != -1 ) {
        readm3u( DocLnk( fileref).file() );
    } else if( fileref.find( "pls", 0, TRUE) != -1 ) { //is pls
        readPls( fileref );
    } else if( DocLnk( fileref).file().find( "pls", 0, TRUE) != -1 ) {
        readPls( DocLnk( fileref).file() );
    } else {
        clearList();
        addToSelection( DocLnk( fileref ) );
        writeCurrentM3u();          
        
        d->setDocumentUsed = TRUE;
        mediaPlayerState.setPlaying( FALSE );
        mediaPlayerState.setPlaying( TRUE );
    }
}


void PlayListWidget::useSelectedDocument() {
    d->setDocumentUsed = FALSE;
}


const DocLnk *PlayListWidget::current() const { // this is fugly
    assert( currentTab() == CurrentPlayList );

    const DocLnk *lnk = d->selectedFiles->current();
    if ( !lnk ) {
        d->selectedFiles->first();
        lnk = d->selectedFiles->current();
    }
    assert( lnk );
    return lnk;
}


bool PlayListWidget::prev() {
    if ( mediaPlayerState.isShuffled() ) {
        const DocLnk *cur = current();
        int j = 1 + (int)(97.0 * rand() / (RAND_MAX + 1.0));
        for ( int i = 0; i < j; i++ ) {
            if ( !d->selectedFiles->next() )
                d->selectedFiles->first();
        }
        if ( cur == current() )
            if ( !d->selectedFiles->next() ) {
                d->selectedFiles->first();
            }
        return TRUE;
    } else {
        if ( !d->selectedFiles->prev() ) {
            if ( mediaPlayerState.isLooping() ) {
                return d->selectedFiles->last();
            } else {
                return FALSE;
            }
        }
        return TRUE;
    }
}


bool PlayListWidget::next() {
//qDebug("<<<<<<<<<<<<next()");
    if ( mediaPlayerState.isShuffled() ) {
        return prev();
    } else {
        if ( !d->selectedFiles->next() ) {
            if ( mediaPlayerState.isLooping() ) {
                return d->selectedFiles->first();
            } else {
                return FALSE;
            }
        }
        return TRUE;
    }
}


bool PlayListWidget::first() {
    return d->selectedFiles->first();
}


bool PlayListWidget::last() {
    return d->selectedFiles->last();
}


 void PlayListWidget::saveList() {
   writem3u();
 }


void PlayListWidget::loadList( const DocLnk & lnk) {
    QString name = lnk.name();
    //  qDebug("<<<<<<<<<<<<<<<<<<<<<<<<currentList is "+name);

    if( name.length()>0) {
        setCaption("OpiePlayer: "+name);
//       qDebug("<<<<<<<<<<<<load list "+ lnk.file());
        clearList();
        readm3u(lnk.file());
        tabWidget->setCurrentPage(0);
    }
}

void PlayListWidget::addSelected() {
  assert( inFileListMode() );

  QListView *fileListView = currentFileListView();
  QListViewItemIterator it( fileListView );
  for ( ; it.current(); ++it )
      if ( it.current()->isSelected() ) {
          QString filename = it.current()->text(3);

          DocLnk lnk;
          lnk.setName( QFileInfo( filename ).baseName() ); //sets name
          lnk.setFile( filename ); //sets file name

          d->selectedFiles->addToSelection( lnk );
      }

  fileListView->clearSelection();

  tabWidget->setCurrentPage( 0 );
  writeCurrentM3u();
}


void PlayListWidget::removeSelected() {
    d->selectedFiles->removeSelected( );
    writeCurrentM3u();          
}


void PlayListWidget::playIt( QListViewItem *it) {
    if(!it) return;
    mediaPlayerState.setPlaying(FALSE);
    mediaPlayerState.setPlaying(TRUE);
    d->selectedFiles->unSelect();
}


void PlayListWidget::addToSelection( QListViewItem *it) {
  d->setDocumentUsed = FALSE;

  if(it) {
    if ( currentTab() == CurrentPlayList )
        return;
    //      case 1: {
    DocLnk lnk;
    QString filename;

    filename=it->text(3);
    lnk.setName( QFileInfo(filename).baseName() ); //sets name
    lnk.setFile( filename ); //sets file name
    d->selectedFiles->addToSelection(  lnk);
 
    writeCurrentM3u();          
    tabWidget->setCurrentPage(0);
        
  }
}


void PlayListWidget::tabChanged(QWidget *) {

    d->tbPlay->setEnabled( true );

    switch ( currentTab() ) {
    case CurrentPlayList:
    {
        if( !tbDeletePlaylist->isHidden() )  {
            tbDeletePlaylist->hide();
        }
        d->tbRemoveFromList->setEnabled(TRUE);
        d->tbAddToList->setEnabled(FALSE);

        d->tbPlay->setEnabled( !d->selectedFiles->isEmpty() );
    }
    break;
    case AudioFiles:
    {
        audioView->populateView();

        if( !tbDeletePlaylist->isHidden() ) {
            tbDeletePlaylist->hide();
        }
        d->tbRemoveFromList->setEnabled(FALSE);
        d->tbAddToList->setEnabled(TRUE);
    }
    break;
    case VideoFiles:
    {
        videoView->populateView();
        if( !tbDeletePlaylist->isHidden() ) {
            tbDeletePlaylist->hide();
        }
        d->tbRemoveFromList->setEnabled(FALSE);
        d->tbAddToList->setEnabled(TRUE);
    }
    break;
    case PlayLists:
    {
        if( tbDeletePlaylist->isHidden() ) {
            tbDeletePlaylist->show();
        }
        playLists->reread();
        d->tbAddToList->setEnabled(FALSE);

        d->tbPlay->setEnabled( false );
    }
    break;
    };
}


void PlayListWidget::btnPlay(bool b) {
//    mediaPlayerState->setPlaying(false);
    mediaPlayerState.setPlaying(b);
    insanityBool=FALSE;
}

void PlayListWidget::deletePlaylist() {
  switch( QMessageBox::information( this, (tr("Remove Playlist?")),
                                    (tr("You really want to delete\nthis playlist?")),
                                    (tr("Yes")), (tr("No")), 0 )){
  case 0: // Yes clicked,
    QFile().remove(playLists->selectedDocument().file());
    QFile().remove(playLists->selectedDocument().linkFile());
    playLists->reread();
    break;
  case 1: // Cancel
    break;
  };
}


void PlayListWidget::playSelected() {
    btnPlay( TRUE);
}

QListView *PlayListWidget::currentFileListView() const
{
    switch ( currentTab() ) {
        case AudioFiles: return audioView;
        case VideoFiles: return videoView;
        default: assert( false );
    }
    return 0;
}

bool PlayListWidget::inFileListMode() const
{
    TabType tab = currentTab();
    return tab == AudioFiles || tab == VideoFiles;
}

void PlayListWidget::openFile() {
                // http://66.28.164.33:2080
                // http://somafm.com/star0242.m3u
    QString filename, name;
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Open file or URL"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        filename = fileDlg->text();
        qDebug( "Selected filename is " + filename );
        //            Om3u *m3uList;
            DocLnk lnk;
            Config cfg( "OpiePlayer" );
            cfg.setGroup("PlayList");

        if(filename.left(4) == "http") {
            QString m3uFile, m3uFilePath;
            if(filename.find(":",8,TRUE) != -1) { //found a port
                m3uFile = filename.left( filename.find( ":",8,TRUE));
                m3uFile = m3uFile.right(  7);
            } else if(filename.left(4) == "http"){
                m3uFile=filename;
                m3uFile = m3uFile.right( m3uFile.length() - 7);
            } else{
                m3uFile=filename;
            }

//            qDebug("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"+ m3uFile);
            lnk.setName( filename ); //sets name
            lnk.setFile( filename ); //sets file name

//            lnk.setIcon("opieplayer2/musicfile");

            d->selectedFiles->addToSelection(  lnk );
            writeCurrentM3u();
            d->selectedFiles->setSelectedItem( lnk.name());
        }
        else if( filename.right( 3) == "m3u" ) {
            readm3u( filename );

        } else if( filename.right(3) == "pls" ) {
            readPls( filename );
        } else {
            lnk.setName( QFileInfo(filename).baseName() ); //sets name
            lnk.setFile( filename ); //sets file name
            d->selectedFiles->addToSelection(  lnk);
            writeCurrentM3u();
            d->selectedFiles->setSelectedItem( lnk.name());
        }
    }
            
    delete fileDlg;
}

/*
reads m3u and shows files/urls to playlist widget */
void PlayListWidget::readm3u( const QString &filename ) { 
    qDebug( "read m3u filename " + filename );

    Om3u *m3uList;
    QString s, name;
    m3uList = new Om3u( filename, IO_ReadOnly );
    m3uList->readM3u();
    DocLnk lnk;
    for ( QStringList::ConstIterator it = m3uList->begin(); it != m3uList->end(); ++it ) {
        s = *it;
//          qDebug("reading "+ s);
        if(s.left(4)=="http") {
          lnk.setName( s ); //sets file name
          lnk.setIcon("opieplayer2/musicfile");
          lnk.setFile( s ); //sets file name
            
        }  else {
          //               if( QFileInfo( s ).exists() ) {
          lnk.setName( QFileInfo(s).baseName());
          //                 if(s.right(4) == '.')   {//if regular file
          if(s.left(1) != "/")  { 
            //            qDebug("set link "+QFileInfo(filename).dirPath()+"/"+s);
            lnk.setFile( QFileInfo(filename).dirPath()+"/"+s);
//            lnk.setIcon(MimeType(s).pixmap() );
//            lnk.setIcon("SoundPlayer");
          } else {
            //            qDebug("set link2 "+s);
            lnk.setFile( s);
//            lnk.setIcon(MimeType(s).pixmap() );
//            lnk.setIcon("SoundPlayer");
          }
        }
        d->selectedFiles->addToSelection( lnk );
    }
    Config config( "OpiePlayer" );
    config.setGroup( "PlayList" );

    config.writeEntry("CurrentPlaylist",filename);
    config.write();
    currentPlayList=filename;

//    m3uList->write();
    m3uList->close();
    delete m3uList;

    d->selectedFiles->setSelectedItem( s);
    setCaption(tr("OpiePlayer: ")+ QFileInfo(filename).baseName());
    
}

/*
reads pls and adds files/urls to playlist  */
void PlayListWidget::readPls( const QString &filename ) {

    qDebug( "pls filename is " + filename );
    Om3u *m3uList;
    QString s, name;
    m3uList = new Om3u( filename, IO_ReadOnly );
    m3uList->readPls();

    for ( QStringList::ConstIterator it = m3uList->begin(); it != m3uList->end(); ++it ) {
        s = *it;
        //        s.replace( QRegExp( "%20" )," " );
        DocLnk lnk( s );
        QFileInfo f( s );
        QString name = f.baseName();

        if( name.left( 4 ) == "http" ) {
            name = s.right( s.length() - 7);
        }  else {
            name = s;
        }

        name = name.right( name.length() - name.findRev( "\\", -1, TRUE) - 1 );

        lnk.setName( name );
        if( s.at( s.length() - 4) == '.') {// if this is probably a file
            lnk.setFile( s );
         } else { //if its a url
//             if( name.right( 1 ).find( '/' ) == -1) {
//                 s += "/";
//             }
            lnk.setFile( s );
        }
        lnk.setType( "audio/x-mpegurl" );

        lnk.writeLink();
        d->selectedFiles->addToSelection( lnk );
    }

    m3uList->close();
    delete m3uList;
}

/*
 writes current playlist to current m3u file */
void PlayListWidget::writeCurrentM3u() {
  qDebug("writing to current m3u");
  Config cfg( "OpiePlayer" );
  cfg.setGroup("PlayList");
  QString currentPlaylist = cfg.readEntry("CurrentPlaylist","default");

  Om3u *m3uList;
  m3uList = new Om3u( currentPlaylist, IO_ReadWrite | IO_Truncate );
  if( d->selectedFiles->first()) {

      do {
          //      qDebug( "add writeCurrentM3u " +d->selectedFiles->current()->file());
          m3uList->add( d->selectedFiles->current()->file() );
      }
      while ( d->selectedFiles->next() );
      //    qDebug( "<<<<<<<<<<<<>>>>>>>>>>>>>>>>>" );
      m3uList->write();
      m3uList->close();
  }
  delete m3uList;
  
}

  /*
 writes current playlist to m3u file */
void PlayListWidget::writem3u() {
    InputDialog *fileDlg;
    fileDlg = new InputDialog( this, tr( "Save m3u Playlist " ), TRUE, 0);
    fileDlg->exec();
    QString name, filename, list;
    Om3u *m3uList;

    if( fileDlg->result() == 1 ) {
        name = fileDlg->text();
//        qDebug( filename );

        if( name.left( 1) != "/" ) {
            filename = QPEApplication::documentDir() + "/" + name;
        }

        if( name.right( 3 ) != "m3u" ) {
            filename = QPEApplication::documentDir() + "/" +name+".m3u";
        }

        if( d->selectedFiles->first()) {
          m3uList = new Om3u(filename, IO_ReadWrite);

          do {
            m3uList->add( d->selectedFiles->current()->file());
          }
          while ( d->selectedFiles->next() );
          //    qDebug( list );
          m3uList->write();
          m3uList->close();
          delete m3uList;
        
          delete fileDlg;

          DocLnk lnk;
          lnk.setFile( filename);
          lnk.setIcon("opieplayer2/playlist2");
          lnk.setName( name); //sets file name

          // qDebug(filename);
          Config config( "OpiePlayer" );
          config.setGroup( "PlayList" );
    
          config.writeEntry("CurrentPlaylist",filename);
          currentPlayList=filename;

          if(!lnk.writeLink()) {
            qDebug("Writing doclink did not work");
          }

          setCaption(tr("OpiePlayer: ") + name);
        }
    }
}

void PlayListWidget::keyReleaseEvent( QKeyEvent *e ) {
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
    //          doBlank();
    break;
  case Key_F13: //mail
    //           doUnblank();
    break;
  case Key_Q: //add to playlist
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
    //          playSelected(); puh
    break;
  case Key_1:
    tabWidget->setCurrentPage( 0 );
    break;
  case Key_2:
    tabWidget->setCurrentPage( 1 );
    break;
  case Key_3:
    tabWidget->setCurrentPage( 2 );
    break;
  case Key_4:
    tabWidget->setCurrentPage( 3 );
    break;
  case Key_Down:
    if ( !d->selectedFiles->next() )
      d->selectedFiles->first();
    break;
  case Key_Up:
    if ( !d->selectedFiles->prev() )
      //        d->selectedFiles->last();
      break;
  }
}

void PlayListWidget::pmViewActivated(int index) {
//  qDebug("%d", index);
    switch(index) {
    case -16:
    {
        mediaPlayerState.toggleFullscreen();
        bool b=mediaPlayerState.isFullscreen();
        pmView->setItemChecked( index, b);
        Config cfg( "OpiePlayer" );
        cfg.writeEntry( "FullScreen", b );
    }
    break;
    };
}

void PlayListWidget::populateSkinsMenu() {
    int item = 0;
    defaultSkinIndex = 0;
    QString skinName;
    Config cfg( "OpiePlayer" );
    cfg.setGroup("Options" );
    QString skin = cfg.readEntry( "Skin", "default" );

    QDir skinsDir( QPEApplication::qpeDir() + "/pics/opieplayer2/skins" );
    skinsDir.setFilter( QDir::Dirs );
    skinsDir.setSorting(QDir::Name );
    const QFileInfoList *skinslist = skinsDir.entryInfoList();
    QFileInfoListIterator it( *skinslist );
    QFileInfo *fi;
    while ( ( fi = it.current() ) ) {
        skinName =  fi->fileName();
//        qDebug(  fi->fileName() );
        if( skinName != "." &&  skinName != ".." && skinName !="CVS" )  {
            item = skinsMenu->insertItem( fi->fileName() ) ;
        }
        if( skinName == "default" ) {
            defaultSkinIndex = item;
        }
        if( skinName == skin ) {
            skinsMenu->setItemChecked( item, TRUE );
        }
        ++it;
    }
}

void PlayListWidget::skinsMenuActivated( int item ) {
    for(unsigned int i = defaultSkinIndex; i > defaultSkinIndex - skinsMenu->count(); i-- ) {
        skinsMenu->setItemChecked( i, FALSE );
    }
    skinsMenu->setItemChecked( item, TRUE );

    {
        Config cfg( "OpiePlayer" );
        cfg.setGroup("Options");
        cfg.writeEntry("Skin", skinsMenu->text( item ) );
    }

    emit skinSelected();
}

PlayListWidget::TabType PlayListWidget::currentTab() const
{
    static const TabType indexToTabType[ TabTypeCount ] =
    { CurrentPlayList, AudioFiles, VideoFiles, PlayLists };

    int index = tabWidget->currentPageIndex();
    assert( index < TabTypeCount && index >= 0 );

    return indexToTabType[ index ];
}

PlayListWidget::Entry PlayListWidget::currentEntry() const
{
    if ( currentTab() == CurrentPlayList ) {
        const DocLnk *lnk = current();
        return Entry( lnk->name(), lnk->file() );
    }

    return Entry( currentFileListPathName() );
}

QString PlayListWidget::currentFileListPathName() const {
    return currentFileListView()->currentItem()->text( 3 );
}
