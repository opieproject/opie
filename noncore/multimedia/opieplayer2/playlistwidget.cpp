/*
                            This file is part of the Opie Project

                             Copyright (c)  2002,2003 Max Reiss <harlekin@handhelds.org>
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

#include <qtoolbar.h>

#include <opie2/ofiledialog.h>

#include <qmessagebox.h>

#include "playlistselection.h"
#include "playlistwidget.h"
#include "mediaplayer.h"
#include "inputDialog.h"
#include "om3u.h"
#include "playlistfileview.h"

//only needed for the random play
#include <assert.h>

using namespace Opie::Ui;
PlayListWidget::PlayListWidget(QWidget* parent, const char* name, WFlags fl )
    : PlayListWidgetGui( parent, "playList" ) , currentFileListView( 0 )
{
    mediaPlayerState = new MediaPlayerState(0, "mediaPlayerState" );
    m_mp = new MediaPlayer(*this, *mediaPlayerState, 0, "mediaPlayer");



    d->tbAddToList =  new ToolButton( bar, tr( "Add to Playlist" ),
                                      "opieplayer2/add_to_playlist",
                                      this , SLOT(addSelected() ) );
    d->tbRemoveFromList = new ToolButton( bar, tr( "Remove from Playlist" ),
                                          "opieplayer2/remove_from_playlist",
                                          this , SLOT(removeSelected() ) );
    d->tbPlay    = new ToolButton( bar, tr( "Play" ), "opieplayer2/play",
                                   this , SLOT( btnPlay(bool) ), TRUE );
    d->tbShuffle = new ToolButton( bar, tr( "Randomize" ),"opieplayer2/shuffle",
                                   mediaPlayerState, SLOT( setShuffled(bool) ), TRUE );
    d->tbLoop    = new ToolButton( bar, tr( "Loop" ), "opieplayer2/loop",
                                   mediaPlayerState, SLOT( setLooping(bool) ), TRUE );

    (void)new MenuItem( pmPlayList, tr( "Clear List" ), this, SLOT( clearList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all audio files" ),
                        this, SLOT( addAllMusicToList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all video files" ),
                        this, SLOT( addAllVideoToList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all files" ),
                        this, SLOT( addAllToList() ) );
    pmPlayList->insertSeparator(-1);
    (void)new MenuItem( pmPlayList, tr( "Add File" ),
                        this,SLOT( openFile() ) );
    (void)new MenuItem( pmPlayList, tr("Add URL"),
                        this,SLOT( openURL() ) );
    pmPlayList->insertSeparator(-1);
    (void)new MenuItem( pmPlayList, tr( "Save Playlist" ),
                        this, SLOT(writem3u() ) );
    pmPlayList->insertSeparator(-1);
    (void)new MenuItem( pmPlayList, tr( "Rescan for Audio Files" ),
                        audioView, SLOT( scanFiles() ) );
    (void)new MenuItem( pmPlayList, tr( "Rescan for Video Files" ),
                        videoView, SLOT( scanFiles() ) );

    pmView->insertItem(  Resource::loadPixmap("fullscreen") , tr( "Full Screen"),
                         mediaPlayerState, SLOT( toggleFullscreen() ) );

    Config cfg( "OpiePlayer" );
    bool b= cfg.readBoolEntry("FullScreen", 0);
    mediaPlayerState->setFullscreen(  b );
    pmView->setItemChecked( -16, b );

    (void)new ToolButton( vbox1, tr( "Move Up" ), "opieplayer2/up",
                          d->selectedFiles, SLOT(moveSelectedUp() ) );
    (void)new ToolButton( vbox1, tr( "Remove" ), "opieplayer2/cut",
                          d->selectedFiles, SLOT(removeSelected() ) );
    (void)new ToolButton( vbox1, tr( "Move Down" ), "opieplayer2/down",
                          d->selectedFiles, SLOT(moveSelectedDown() ) );
    QVBox *stretch2 = new QVBox( vbox1 );

    connect( tbDeletePlaylist, ( SIGNAL( released() ) ),
             SLOT( deletePlaylist() ) );
    connect( pmView, SIGNAL( activated(int) ),
             this, SLOT( pmViewActivated(int) ) );
    connect( skinsMenu, SIGNAL( activated(int) ) ,
             this, SLOT( skinsMenuActivated(int) ) );
    connect( d->selectedFiles, SIGNAL( mouseButtonPressed(int,QListViewItem*,const QPoint&,int) ),
             this,SLOT( playlistViewPressed(int,QListViewItem*,const QPoint&,int) ) );
    connect( audioView, SIGNAL( mouseButtonPressed(int,QListViewItem*,const QPoint&,int) ),
             this,SLOT( viewPressed(int,QListViewItem*,const QPoint&,int) ) );
    connect( audioView, SIGNAL( returnPressed(QListViewItem*) ),
             this,SLOT( playIt(QListViewItem*) ) );
    connect( audioView, SIGNAL( doubleClicked(QListViewItem*) ),
             this, SLOT( addToSelection(QListViewItem*) ) );
    connect( videoView, SIGNAL( mouseButtonPressed(int,QListViewItem*,const QPoint&,int) ),
             this,SLOT( viewPressed(int,QListViewItem*,const QPoint&,int) ) );
    connect( videoView, SIGNAL( returnPressed(QListViewItem*) ),
             this,SLOT( playIt(QListViewItem*) ) );
    connect( videoView, SIGNAL( doubleClicked(QListViewItem*) ),
             this, SLOT( addToSelection(QListViewItem*) ) );
    connect( playLists, SIGNAL( fileSelected(const DocLnk&) ),
             this, SLOT( loadList(const DocLnk&) ) );
    connect( tabWidget, SIGNAL ( currentChanged(QWidget*) ),
             this, SLOT( tabChanged(QWidget*) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled(bool) ),
             d->tbPlay,  SLOT( setOn(bool) ) );
    connect( mediaPlayerState, SIGNAL( loopingToggled(bool) ),
             d->tbLoop, SLOT( setOn(bool) ) );
    connect( mediaPlayerState, SIGNAL( shuffledToggled(bool) ),
             d->tbShuffle, SLOT( setOn(bool) ) );
    connect( d->selectedFiles, SIGNAL( doubleClicked(QListViewItem*) ),
             this, SLOT( playIt(QListViewItem*) ) );
    connect ( gammaSlider,  SIGNAL( valueChanged(int) ),
              mediaPlayerState,  SLOT( setVideoGamma(int) ) );

    connect( this, SIGNAL(skinSelected() ),
             m_mp, SLOT( reloadSkins() ) );

    // see which skins are installed
    populateSkinsMenu();
    initializeStates();

    channel = new QCopChannel( "QPE/Application/opieplayer2", this );
    connect( channel, SIGNAL(received(const QCString&,const QByteArray&)),
        this, SLOT( qcopReceive(const QCString&,const QByteArray&)) );


    cfg.setGroup("PlayList");
    QString currentPlaylist = cfg.readEntry( "CurrentPlaylist", "default");
    loadList(DocLnk(  currentPlaylist ) );

    tabWidget->showPage( playListTab );
}


PlayListWidget::~PlayListWidget() {
    delete d;
    delete m_mp;
}


void PlayListWidget::initializeStates() {
    d->tbPlay->setOn( mediaPlayerState->isPlaying() );
    d->tbLoop->setOn( mediaPlayerState->isLooping() );
    d->tbShuffle->setOn( mediaPlayerState->isShuffled() );
    d->playListFrame->show();
}

void PlayListWidget::writeDefaultPlaylist() {

    Config config( "OpiePlayer" );
    config.setGroup( "PlayList" );
    QString filename=QPEApplication::documentDir() + "/default.m3u";
    QString currentString = config.readEntry( "CurrentPlaylist", filename);
    if( currentString == filename) {
      Om3u *m3uList;
      //    odebug << "<<<<<<<<<<<<<default>>>>>>>>>>>>>>>>>>>" << oendl; 
      if( d->selectedFiles->first() ) {
        m3uList = new Om3u(filename, IO_ReadWrite | IO_Truncate);
        do {
          //        odebug << d->selectedFiles->current()->file() << oendl; 
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
  Config cfg( "OpiePlayer" );
  cfg.setGroup("PlayList");
  cfg.writeEntry("CurrentPlaylist","default");
  setCaption("OpiePlayer");
}

void PlayListWidget::viewPressed( int mouse, QListViewItem *, const QPoint& , int) {
    switch (mouse) {
    case LeftButton:
        break;
    case RightButton:
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
    case LeftButton:
        break;
    case RightButton:
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
    tabWidget->setCurrentPage(0);
    writeCurrentM3u();
    d->selectedFiles->first();
}


void PlayListWidget::setDocument( const QString& fileref ) {
  //    odebug << "<<<<<<<<set document>>>>>>>>>> "+fileref << oendl; 
    fromSetDocument = TRUE;
    QFileInfo fileInfo(fileref);

    if ( !fileInfo.exists() ) {
        QMessageBox::warning( this, tr( "Invalid File" ),
                               tr( "There was a problem in getting the file." ) );
        return;
    }

    clearList();
    QString extension = fileInfo.extension(false);

    if( extension.find( "m3u", 0, false) != -1
       || extension.find( "pls", 0, false) != -1 ) {
        readListFromFile( fileref );
    } else {
        clearList();
        DocLnk lnk;
        lnk.setName( fileInfo.baseName() ); //sets name
        lnk.setFile( fileref ); //sets file name
        addToSelection( lnk );
        writeCurrentM3u();

        d->setDocumentUsed = TRUE;
        mediaPlayerState->setPlaying( FALSE );
        mediaPlayerState->setPlaying( TRUE );
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
    if ( mediaPlayerState->isShuffled() ) {
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
            if ( mediaPlayerState->isLooping() ) {
                return d->selectedFiles->last();
            } else {
                return FALSE;
            }
        }
        return TRUE;
    }
}


bool PlayListWidget::next() {
//odebug << "<<<<<<<<<<<<next()" << oendl; 
    if ( mediaPlayerState->isShuffled() ) {
        return prev();
    } else {
        if ( !d->selectedFiles->next() ) {
            if ( mediaPlayerState->isLooping() ) {
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

    if( name.length()>0) {
        setCaption("OpiePlayer: "+name);
        clearList();
        readListFromFile(lnk.file());
        tabWidget->setCurrentPage(0);
    }
}

void PlayListWidget::addSelected() {
  assert( inFileListMode() );

  QListViewItemIterator it( currentFileListView );
  for ( ; it.current(); ++it )
      if ( it.current()->isSelected() ) {
          QString filename = it.current()->text(3);

          DocLnk lnk;
          lnk.setName( QFileInfo( filename ).baseName() ); //sets name
          lnk.setFile( filename ); //sets file name

          d->selectedFiles->addToSelection( lnk );
      }

  currentFileListView->clearSelection();

  writeCurrentM3u();
}


void PlayListWidget::removeSelected() {
    d->selectedFiles->removeSelected( );
    writeCurrentM3u();
}


void PlayListWidget::playIt( QListViewItem *it) {
    if(!it) return;
    mediaPlayerState->setPlaying(FALSE);
    mediaPlayerState->setPlaying(TRUE);
    d->selectedFiles->unSelect();
}


void PlayListWidget::addToSelection( QListViewItem *it) {
  d->setDocumentUsed = FALSE;

  if(it) {
    if ( currentTab() == CurrentPlayList )
        return;
    DocLnk lnk;
    QString filename;

    filename=it->text(3);
    lnk.setName( QFileInfo(filename).baseName() ); //sets name
    lnk.setFile( filename ); //sets file name
    d->selectedFiles->addToSelection(  lnk);

    writeCurrentM3u();
//    tabWidget->setCurrentPage(0);

  }
}


void PlayListWidget::tabChanged(QWidget *) {

    d->tbPlay->setEnabled( true );

    disconnect( audioView, SIGNAL( itemsSelected(bool) ),
                d->tbPlay, SLOT( setEnabled(bool) ) );
    disconnect( videoView, SIGNAL( itemsSelected(bool) ),
                d->tbPlay, SLOT( setEnabled(bool) ) );

    currentFileListView = 0;

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

        connect( audioView, SIGNAL( itemsSelected(bool) ),
                 d->tbPlay, SLOT( setEnabled(bool) ) );

        d->tbPlay->setEnabled( audioView->hasSelection() );

        currentFileListView = audioView;
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

        connect( videoView, SIGNAL( itemsSelected(bool) ),
                 d->tbPlay, SLOT( setEnabled(bool) ) );

        d->tbPlay->setEnabled( videoView->hasSelection() );

        currentFileListView = videoView;
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
    mediaPlayerState->setPlaying(b);
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

bool PlayListWidget::inFileListMode() const
{
    TabType tab = currentTab();
    return tab == AudioFiles || tab == VideoFiles;
}

void PlayListWidget::openURL() {
    // http://66.28.164.33:2080
    // http://somafm.com/star0242.m3u
    QString filename, name;
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Add  URL"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        filename = fileDlg->text();
        odebug << "Selected filename is " + filename << oendl; 
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

            lnk.setName( filename ); //sets name
            lnk.setFile( filename ); //sets file name

//            lnk.setIcon("opieplayer2/musicfile");

            d->selectedFiles->addToSelection(  lnk );
            writeCurrentM3u();
            d->selectedFiles->setSelectedItem( lnk.name());
        }
        else if( filename.right( 3) == "m3u" ||  filename.right(3) == "pls" ) {
            readListFromFile( filename );
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


void PlayListWidget::openFile() {

    QString filename, name;

    Config cfg( "OpiePlayer" );
    cfg.setGroup("Dialog");
    MimeTypes types;
    QStringList audio, video, all;
    audio << "audio/*";
    audio << "playlist/plain";
    audio << "audio/x-ogg";
    audio << "audio/x-mpegurl";

    video << "video/*";
    video << "playlist/plain";

    all += audio;
    all += video;
    types.insert("All Media Files", all );
    types.insert("Audio",  audio );
    types.insert("Video", video );

    QString str = OFileDialog::getOpenFileName( 1,
                                                cfg.readEntry("LastDirectory",QPEApplication::documentDir()),"",
                                                types, 0 );

    if(str.left(2) == "//")  {
        str=str.right(str.length()-1);
    }
    cfg.writeEntry( "LastDirectory" ,QFileInfo( str ).dirPath() );

    if( !str.isEmpty() ) {

        odebug << "Selected filename is " + str << oendl; 
        filename = str;
        DocLnk lnk;

        if( filename.right( 3) == "m3u" ||  filename.right(3) == "pls" ) {
            readListFromFile( filename );
        } else {
            lnk.setName( QFileInfo(filename).baseName() ); //sets name
            lnk.setFile( filename ); //sets file name
            d->selectedFiles->addToSelection(  lnk );
            writeCurrentM3u();
            d->selectedFiles->setSelectedItem( lnk.name() );
        }
    }
}


void PlayListWidget::readListFromFile( const QString &filename ) {
    odebug << "read list filename " + filename << oendl; 
    QFileInfo fi(filename);
    Om3u *m3uList;
    QString s, name;
    m3uList = new Om3u( filename, IO_ReadOnly );
    if(fi.extension(false).find("m3u",0,false) != -1 )
        m3uList->readM3u();
    else if(fi.extension(false).find("pls",0,false) != -1 )
        m3uList->readPls();

    DocLnk lnk;
    for ( QStringList::ConstIterator it = m3uList->begin(); it != m3uList->end(); ++it ) {
        s = *it;
        //        odebug << s << oendl; 
        if(s.left(4)=="http") {
          lnk.setName( s ); //sets file name
          lnk.setIcon("opieplayer2/musicfile");
          lnk.setFile( s ); //sets file name

        }  else { //is file
          lnk.setName( QFileInfo(s).baseName());
          if(s.left(1) != "/")  {

            lnk.setFile( QFileInfo(filename).dirPath()+"/"+s);
          } else {
            lnk.setFile( s);
          }
        }
        d->selectedFiles->addToSelection( lnk );
    }
    Config config( "OpiePlayer" );
    config.setGroup( "PlayList" );

    config.writeEntry("CurrentPlaylist",filename);
    config.write();
    currentPlayList=filename;

    m3uList->close();
    delete m3uList;

    d->selectedFiles->setSelectedItem( s);
    setCaption(tr("OpiePlayer: ")+ QFileInfo(filename).baseName());

}

//  writes current playlist to current m3u file */
 void PlayListWidget::writeCurrentM3u() {
   odebug << "writing to current m3u" << oendl; 
   Config cfg( "OpiePlayer" );
   cfg.setGroup("PlayList");
   QString currentPlaylist = cfg.readEntry("CurrentPlaylist","default");

   Om3u *m3uList;
   m3uList = new Om3u( currentPlaylist, IO_ReadWrite | IO_Truncate );
   if( d->selectedFiles->first()) {

       do {
           //      odebug << "add writeCurrentM3u " +d->selectedFiles->current()->file() << oendl; 
           m3uList->add( d->selectedFiles->current()->file() );
       }
       while ( d->selectedFiles->next() );
       //    odebug << "<<<<<<<<<<<<>>>>>>>>>>>>>>>>>" << oendl; 
       m3uList->write();
      m3uList->close();
   }
   delete m3uList;

 }

  /*
 writes current playlist to m3u file */
void PlayListWidget::writem3u() {
    //InputDilog *fileDlg;
    //fileDlg = new InputDialog( this, tr( "Save m3u Playlist " ), TRUE, 0);
    //fileDlg->exec();

    Config cfg( "OpiePlayer" );
    cfg.setGroup("Dialog");
    MimeTypes types;
    QStringList audio, video, all;
    audio << "audio/*";
    audio << "playlist/plain";
    audio << "audio/x-mpegurl";

    video << "video/*";
    video << "playlist/plain";

    all += audio;
    all += video;
    types.insert("All Media Files", all );
    types.insert("Audio",  audio );
    types.insert("Video", video );

    QString str = OFileDialog::getOpenFileName( 1,
                  cfg.readEntry("LastDirectory",QPEApplication::documentDir()),"",
                  types, 0 );
    if(str.left(2) == "//") str=str.right(str.length()-1);
    cfg.writeEntry("LastDirectory" ,QFileInfo(str).dirPath());


    QString name, filename, list;
    Om3u *m3uList;

    if( !str.isEmpty() ) {
        name = str;
        //       name = fileDlg->text();
//        odebug << filename << oendl; 
        if( name.find("/",0,true) != -1) {// assume they specify a file path
            filename = name;
            name = name.right(name.length()- name.findRev("/",-1,true) - 1 );
        }
        else //otherwise dump it somewhere noticable
            filename = QPEApplication::documentDir() + "/" + name;

        if( filename.right( 3 ) != "m3u" ) //needs filename extension
            filename += ".m3u";

        if( d->selectedFiles->first()) { //ramble through playlist view
            m3uList = new Om3u( filename, IO_ReadWrite | IO_Truncate);

            do {
            m3uList->add( d->selectedFiles->current()->file());
          }
          while ( d->selectedFiles->next() );
          //    odebug << list << oendl; 
          m3uList->write();
          m3uList->close();
          delete m3uList;

          //delete fileDlg;

          DocLnk lnk;
          lnk.setFile( filename);
          lnk.setIcon("opieplayer2/playlist2");
          lnk.setName( name); //sets file name

          // odebug << filename << oendl; 
          Config config( "OpiePlayer" );
          config.setGroup( "PlayList" );

          config.writeEntry("CurrentPlaylist",filename);
          currentPlayList=filename;

          if(!lnk.writeLink()) {
            odebug << "Writing doclink did not work" << oendl; 
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
    //           odebug << "Play" << oendl; 
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
//  odebug << "" << index << "" << oendl; 
    switch(index) {
    case -16:
    {
        mediaPlayerState->toggleFullscreen();
        bool b=mediaPlayerState->isFullscreen();
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
//        odebug << fi->fileName() << oendl; 
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
    return currentFileListView->currentItem()->text( 3 );
}


void PlayListWidget::qcopReceive(const QCString &msg, const QByteArray &data) {
   odebug << "qcop message "+msg << oendl; 
   QDataStream stream ( data, IO_ReadOnly );
   if ( msg == "play()" ) { //plays current selection
      btnPlay( true);
   } else if ( msg == "stop()" ) {
      mediaPlayerState->setPlaying( false);
   } else if ( msg == "togglePause()" ) {
      mediaPlayerState->togglePaused();
   } else if ( msg == "next()" ) { //select next in list
      mediaPlayerState->setNext();
   } else if ( msg == "prev()" ) { //select previous in list
      mediaPlayerState->setPrev();
   } else if ( msg == "toggleLooping()" ) { //loop or not loop
      mediaPlayerState->toggleLooping();
   } else if ( msg == "toggleShuffled()" ) { //shuffled or not shuffled
      mediaPlayerState->toggleShuffled();
   } else if ( msg == "volUp()" ) { //volume more
//       emit moreClicked();
//       emit moreReleased();
   } else if ( msg == "volDown()" ) { //volume less
//       emit lessClicked();
//       emit lessReleased();
   } else if ( msg == "play(QString)" ) { //play this now
      QString file;
      stream >> file;
      setDocument( (const QString &) file);
   } else if ( msg == "add(QString)" ) { //add to playlist
      QString file;
      stream >> file;
      QFileInfo fileInfo(file);
      DocLnk lnk;
      lnk.setName( fileInfo.baseName() ); //sets name
      lnk.setFile( file ); //sets file name
      addToSelection( lnk );
   } else if ( msg == "rem(QString)" ) { //remove from playlist
      QString file;
      stream >> file;

   }

}
