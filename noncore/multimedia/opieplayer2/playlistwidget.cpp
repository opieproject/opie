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
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
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

#include <qdir.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtextstream.h>

#include "playlistselection.h"
#include "playlistwidget.h"
#include "mediaplayerstate.h"
#include "inputDialog.h"

//only needed for the random play
#include <stdlib.h>

#include "audiowidget.h"
#include "videowidget.h"

extern MediaPlayerState *mediaPlayerState;


PlayListWidget::PlayListWidget( QWidget* parent, const char* name, WFlags fl )
    : PlayListWidgetGui( parent, name, fl ) {

    d->tbAddToList =  new ToolButton( bar, tr( "Add to Playlist" ), "opieplayer2/add_to_playlist",
                                      this , SLOT(addSelected() ) );
    d->tbRemoveFromList = new ToolButton( bar, tr( "Remove from Playlist" ), "opieplayer2/remove_from_playlist",
                                          this , SLOT(removeSelected() ) );
    d->tbPlay    = new ToolButton( bar, tr( "Play" ), "opieplayer2/play",
                                   this , SLOT( btnPlay( bool) ), TRUE );
    d->tbShuffle = new ToolButton( bar, tr( "Randomize" ),"opieplayer2/shuffle",
                                   mediaPlayerState, SLOT( setShuffled( bool ) ), TRUE );
    d->tbLoop    = new ToolButton( bar, tr( "Loop" ), "opieplayer2/loop",
                                   mediaPlayerState, SLOT( setLooping( bool ) ), TRUE );

    (void)new MenuItem( pmPlayList, tr( "Clear List" ), this, SLOT( clearList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all audio files" ), this, SLOT( addAllMusicToList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all video files" ), this, SLOT( addAllVideoToList() ) );
    (void)new MenuItem( pmPlayList, tr( "Add all files" ), this, SLOT( addAllToList() ) );
    pmPlayList->insertSeparator(-1);
    (void)new MenuItem( pmPlayList, tr( "Save PlayList" ), this, SLOT( saveList() ) );
    (void)new MenuItem( pmPlayList, tr( "Export playlist to m3u" ), this, SLOT(writem3u() ) );
    pmPlayList->insertSeparator(-1);
    (void)new MenuItem( pmPlayList, tr( "Open File or URL" ), this,SLOT( openFile() ) );
    pmPlayList->insertSeparator(-1);
    (void)new MenuItem( pmPlayList, tr( "Rescan for Audio Files" ), this,SLOT( scanForAudio() ) );
    (void)new MenuItem( pmPlayList, tr( "Rescan for Video Files" ), this,SLOT( scanForVideo() ) );

    pmView->insertItem(  Resource::loadPixmap("fullscreen") , tr( "Full Screen"), mediaPlayerState, SLOT( toggleFullscreen() ) );

    Config cfg( "OpiePlayer" );
    bool b= cfg.readBoolEntry("FullScreen", 0);
    mediaPlayerState->setFullscreen(  b );
    pmView->setItemChecked( -16, b );

    (void)new ToolButton( vbox1, tr( "Move Up" ),   "opieplayer2/up",   d->selectedFiles, SLOT(moveSelectedUp() ) );
    (void)new ToolButton( vbox1, tr( "Remove" ),    "opieplayer2/cut",  d->selectedFiles, SLOT(removeSelected() ) );
    (void)new ToolButton( vbox1, tr( "Move Down" ), "opieplayer2/down", d->selectedFiles, SLOT(moveSelectedDown() ) );
    QVBox *stretch2 = new QVBox( vbox1 );

    connect( tbDeletePlaylist, ( SIGNAL( released() ) ), SLOT( deletePlaylist() ) );
    connect( pmView, SIGNAL( activated( int ) ), this, SLOT( pmViewActivated( int ) ) );
    connect( skinsMenu, SIGNAL( activated( int ) ) , this, SLOT( skinsMenuActivated( int ) ) );
    connect( d->selectedFiles, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int) ),
             this,SLOT( playlistViewPressed( int, QListViewItem *, const QPoint&, int ) ) );
    connect( audioView, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int ) ),
             this,SLOT( viewPressed( int, QListViewItem *, const QPoint&, int ) ) );
    connect( audioView, SIGNAL( returnPressed( QListViewItem *) ),
             this,SLOT( playIt( QListViewItem *) ) );
    connect( audioView, SIGNAL( doubleClicked( QListViewItem *) ), this, SLOT( addToSelection( QListViewItem *) ) );
    connect( videoView, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int) ),
             this,SLOT( viewPressed( int, QListViewItem *, const QPoint&, int) ) );
    connect( videoView, SIGNAL( returnPressed( QListViewItem *) ),
             this,SLOT( playIt( QListViewItem *) ) );
    connect( videoView, SIGNAL( doubleClicked( QListViewItem *) ), this, SLOT( addToSelection( QListViewItem *) ) );
    connect( playLists, SIGNAL( fileSelected( const DocLnk &) ), this, SLOT( loadList( const DocLnk & ) ) );
    connect( tabWidget, SIGNAL ( currentChanged(QWidget*) ), this, SLOT( tabChanged( QWidget* ) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ), d->tbPlay,  SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( loopingToggled( bool ) ), d->tbLoop, SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( shuffledToggled( bool ) ), d->tbShuffle, SLOT( setOn( bool ) ) );
    connect( mediaPlayerState, SIGNAL( playlistToggled( bool ) ), this, SLOT( setPlaylist( bool ) ) );
    connect( d->selectedFiles, SIGNAL( doubleClicked( QListViewItem *) ), this, SLOT( playIt( QListViewItem *) ) );

    readConfig( cfg );
    QString currentPlaylist = cfg.readEntry( "CurrentPlaylist", "" );
    loadList(DocLnk( currentPlaylist ) );
    setCaption( tr( "OpiePlayer: " ) + currentPlaylist );

    // see which skins are installed
    populateSkinsMenu();
    initializeStates();
}


PlayListWidget::~PlayListWidget() {
/* fixing symptoms and not sources is entirely stupid - zecke */
//  Config cfg( "OpiePlayer" );
//  writeConfig( cfg );
    if ( d->current ) {
        delete d->current;
    }
    delete d;
}


void PlayListWidget::initializeStates() {
    d->tbPlay->setOn( mediaPlayerState->playing() );
    d->tbLoop->setOn( mediaPlayerState->looping() );
    d->tbShuffle->setOn( mediaPlayerState->shuffled() );
    setPlaylist( true );
}


void PlayListWidget::readConfig( Config& cfg ) {
    cfg.setGroup( "PlayList" );
    QString currentString = cfg.readEntry( "current", "" );
    int noOfFiles = cfg.readNumEntry( "NumberOfFiles", 0 );
    for ( int i = 0; i < noOfFiles; i++ ) {
        QString entryName;
        entryName.sprintf( "File%i", i + 1 );
        QString linkFile = cfg.readEntry( entryName );
        if( QFileInfo( linkFile ).exists() ) {
            DocLnk lnk( linkFile );
            if ( QFileInfo( lnk.file() ).exists() || linkFile.find( "http" , 0, TRUE) != -1) {
                d->selectedFiles->addToSelection( lnk );
            }
        }
    }
    d->selectedFiles->setSelectedItem( currentString );
}


void PlayListWidget::writeConfig( Config& cfg ) const {
    d->selectedFiles->writeCurrent( cfg );
    cfg.setGroup( "PlayList" );
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
    }
    while ( d->selectedFiles->next() );
    cfg.writeEntry("NumberOfFiles", noOfFiles );
}


void PlayListWidget::addToSelection( const DocLnk& lnk ) {
    d->setDocumentUsed = FALSE;
    if ( mediaPlayerState->playlist() ) {
        if( QFileInfo( lnk.file() ).exists() || lnk.file().left(4) == "http" )
            d->selectedFiles->addToSelection( lnk );
    }
    else
        mediaPlayerState->setPlaying( TRUE );
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
        m.insertItem( tr( "Play" ), this, SLOT( playSelected() ));
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
        m.insertItem( tr( "Play Selected" ), this, SLOT( playSelected() ));
        m.insertItem( tr( "Remove" ), this, SLOT( removeSelected() ));
        m.exec( QCursor::pos() );
    }
    break;
    }
}


void PlayListWidget::addAllToList() {
    DocLnkSet filesAll;
    Global::findDocuments(&filesAll, "video/*;audio/*");
    QListIterator<DocLnk> Adit( filesAll.children() );
    for ( ; Adit.current(); ++Adit ) {
        if( QFileInfo( Adit.current()->file() ).exists() ) {
            d->selectedFiles->addToSelection( **Adit );
        }
    }
}


void PlayListWidget::addAllMusicToList() {
    QListIterator<DocLnk> dit( files.children() );
    for ( ; dit.current(); ++dit ) {
        if( QFileInfo(dit.current()->file() ).exists() ) {
            d->selectedFiles->addToSelection( **dit );
        }
    }
}


void PlayListWidget::addAllVideoToList() {
    QListIterator<DocLnk> dit( vFiles.children() );
    for ( ; dit.current(); ++dit ) {
        if( QFileInfo( dit.current()->file() ).exists() ) {
            d->selectedFiles->addToSelection( **dit );
        }
    }
}


void PlayListWidget::setDocument( const QString& fileref ) {
    //qDebug( fileref );
    fromSetDocument = TRUE;
    if ( fileref.isNull() ) {
        QMessageBox::critical( 0, tr( "Invalid File" ), tr( "There was a problem in getting the file." ) );
        return;
    }

    if( fileref.find( "m3u", 0, TRUE) != -1 ) { //is m3u
        readm3u( fileref );
    } else if( fileref.find( "pls", 0, TRUE) != -1 ) { //is pls
        readPls( fileref );
    } else if( fileref.find( "playlist", 0, TRUE) != -1 ) {//is playlist
        clearList();
        loadList( DocLnk( fileref ) );
        d->selectedFiles->first();
    } else {
        clearList();
        addToSelection( DocLnk( fileref ) );
        d->setDocumentUsed = TRUE;
        mediaPlayerState->setPlaying( FALSE );
        qApp->processEvents();
        mediaPlayerState->setPlaying( TRUE );
        qApp->processEvents();
        setCaption( tr("OpiePlayer") );
    }
}


void PlayListWidget::useSelectedDocument() {
    d->setDocumentUsed = FALSE;
}


const DocLnk *PlayListWidget::current() { // this is fugly
    switch ( tabWidget->currentPageIndex() ) {
    case 0: //playlist
    {
//      qDebug("playlist");
        if ( mediaPlayerState->playlist() ) {
            return d->selectedFiles->current();
        } else if ( d->setDocumentUsed && d->current ) {
            return d->current;
        } else {
            return d->files->selected();
        }
    }
    break;
    case 1://audio
    {
//      qDebug("audioView");
        QListIterator<DocLnk> dit( files.children() );
        for ( ; dit.current(); ++dit ) {
            if( dit.current()->name() == audioView->currentItem()->text( 0 ) && !insanityBool ) {
                insanityBool = TRUE;
                return dit;
            }
        }
    }
    break;
    case 2: // video
    {
//      qDebug("videoView");
        QListIterator<DocLnk> Vdit( vFiles.children() );
        for ( ; Vdit.current(); ++Vdit ) {
            if( Vdit.current()->name() == videoView->currentItem()->text( 0 ) && !insanityBool) {
                insanityBool = TRUE;
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
                if ( !d->selectedFiles->next() ) {
                    d->selectedFiles->first();
                }
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
    InputDialog *fileDlg = 0l;
    fileDlg = new InputDialog(this,tr("Save Playlist"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        if ( d->current )
            delete d->current;
        filename = fileDlg->text();//+".playlist";
        //         qDebug("saving playlist "+filename+".playlist");
        Config cfg( filename +".playlist");
        writeConfig( cfg );

        DocLnk lnk;
        lnk.setFile(QDir::homeDirPath()+"/Settings/"+filename+".playlist.conf"); //sets File property
        lnk.setType("playlist/plain");// hey is this a REGISTERED mime type?!?!? ;D
        lnk.setIcon("opieplayer2/playlist2");
        lnk.setName( filename); //sets file name
        // qDebug(filename);
        if(!lnk.writeLink()) {
            qDebug("Writing doclink did not work");
        }
    }
    Config config( "OpiePlayer" );
    config.writeEntry("CurrentPlaylist",filename);
    setCaption(tr("OpiePlayer: ")+filename);
    d->selectedFiles->first();
    if(fileDlg) {
        delete fileDlg;
    }
}

void PlayListWidget::loadList( const DocLnk & lnk) {
    QString name= lnk.name();
    // qDebug("currentList is "+name);
    if( name.length()>1) {
        setCaption("OpiePlayer: "+name);
        // qDebug("load list "+ name+".playlist");
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
    if ( shown ) {
        d->playListFrame->show();
    } else {
        d->playListFrame->hide();
    }
}


void PlayListWidget::addSelected() {

    Config cfg( "OpiePlayer" );
    cfg.setGroup("PlayList");
    QString currentPlaylist = cfg.readEntry("CurrentPlaylist","");
    //    int noOfFiles = cfg.readNumEntry("NumberOfFiles", 0 );

    switch (tabWidget->currentPageIndex()) {
    case 0: //playlist
        break;
    case 1: { //audio
        QListViewItemIterator it(  audioView  );
        // iterate through all items of the listview
        for ( ; it.current(); ++it ) {
            if ( it.current()->isSelected() ) {
                QListIterator<DocLnk> dit( files.children() );
                for ( ; dit.current(); ++dit ) {
                    if( dit.current()->name() == it.current()->text(0) ) {
                        if(QFileInfo( dit.current()->file()).exists())
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
                        if(QFileInfo( dit.current()->file()).exists())
                            d->selectedFiles->addToSelection(  **dit );
                    }
                }
        videoView->setSelected( it.current(),FALSE);
            }
        }
        tabWidget->setCurrentPage(0);
    }
    break;
    };
}


void PlayListWidget::removeSelected() {
    d->selectedFiles->removeSelected( );
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
        switch (tabWidget->currentPageIndex()) {
        case 1: {
            QListIterator<DocLnk> dit( files.children() );
            for ( ; dit.current(); ++dit ) {
                if( dit.current()->name() == it->text(0)) {
                    if(QFileInfo( dit.current()->file()).exists())
                        d->selectedFiles->addToSelection(  **dit );
                }
      }
        }
            break;
        case 2: {
            QListIterator<DocLnk> dit( vFiles.children() );
            for ( ; dit.current(); ++dit ) {
                if( dit.current()->name() == it->text(0)) {
                    if(QFileInfo( dit.current()->file()).exists())
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


void PlayListWidget::tabChanged(QWidget *) {

    switch ( tabWidget->currentPageIndex()) {
    case 0:
    {
        if( !tbDeletePlaylist->isHidden() )  {
            tbDeletePlaylist->hide();
        }
        d->tbRemoveFromList->setEnabled(TRUE);
        d->tbAddToList->setEnabled(FALSE);
    }
    break;
    case 1:
    {
        audioView->clear();
        populateAudioView();

        if( !tbDeletePlaylist->isHidden() ) {
            tbDeletePlaylist->hide();
        }
        d->tbRemoveFromList->setEnabled(FALSE);
        d->tbAddToList->setEnabled(TRUE);
    }
    break;
    case 2:
    {
        videoView->clear();
        populateVideoView();
        if( !tbDeletePlaylist->isHidden() ) {
            tbDeletePlaylist->hide();
        }
        d->tbRemoveFromList->setEnabled(FALSE);
        d->tbAddToList->setEnabled(TRUE);
    }
    break;
    case 3:
    {
        if( tbDeletePlaylist->isHidden() ) {
            tbDeletePlaylist->show();
        }
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
      mediaPlayerState->setPlaying(b);
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


void PlayListWidget::playSelected() {
    btnPlay( TRUE);
}


void PlayListWidget::scanForAudio() {
//  qDebug("scan for audio");
  files.detachChildren();
  QListIterator<DocLnk> sdit( files.children() );
  for ( ; sdit.current(); ++sdit ) {
    delete sdit.current();
  }
  Global::findDocuments(&files, "audio/*");
  audioScan = TRUE;
}

void PlayListWidget::scanForVideo() {
//  qDebug("scan for video");
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
    if(!audioScan) {
        scanForAudio();
    }

    QListIterator<DocLnk> dit( files.children() );
    QListIterator<FileSystem> it ( fs );

    QString storage;
    for ( ; dit.current(); ++dit ) {
        for( ; it.current(); ++it ){
            const QString name = (*it)->name();
            const QString path = (*it)->path();
            if(dit.current()->file().find(path) != -1 ) {
                storage = name;
            }
        }

        QListViewItem * newItem;
        if ( QFile( dit.current()->file()).exists()  ) {
            // qDebug(dit.current()->name());
            newItem= /*(void)*/ new QListViewItem( audioView, dit.current()->name(),
                                                   QString::number( QFile( dit.current()->file()).size() ), storage);
            newItem->setPixmap( 0, Resource::loadPixmap( "opieplayer2/musicfile" ) );
        }
    }
}


void PlayListWidget::populateVideoView() {
    videoView->clear();
    StorageInfo storageInfo;
    const QList<FileSystem> &fs = storageInfo.fileSystems();

    if(!videoScan ) {
        scanForVideo();
    }

    QListIterator<DocLnk> Vdit( vFiles.children() );
    QListIterator<FileSystem> it ( fs );
    videoView->clear();
    QString storage;
    for ( ; Vdit.current(); ++Vdit ) {
        for( ; it.current(); ++it ) {
            const QString name = (*it)->name();
            const QString path = (*it)->path();
            if( Vdit.current()->file().find(path) != -1 ) {
                storage=name;
            }
        }

        QListViewItem * newItem;
        if ( QFile( Vdit.current()->file() ).exists() ) {
            newItem= /*(void)*/ new QListViewItem( videoView, Vdit.current()->name(),
                                                   QString::number( QFile( Vdit.current()->file() ).size() ), storage );
            newItem->setPixmap(0, Resource::loadPixmap( "opieplayer2/videofile" ) );
        }
    }
}


void PlayListWidget::openFile() {
    QString filename, name;
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Open file or URL"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        filename = fileDlg->text();

        qDebug( "Selected filename is " + filename );
        if( filename.right( 3 ) == "m3u" ) {
            readm3u( filename );
        } else if( filename.right(3) == "pls" ) {
            readPls( filename );
        } else {
            /* FIXME ....... AUDIO/X-MPEGURL is bad*/
            DocLnk lnk;
            lnk.setName( filename ); //sets file name
            lnk.setFile( filename ); //sets File property
            //qWarning( "Mimetype: " + MimeType( QFile::encodeName(filename) ).id() );
            lnk.setType( MimeType( QFile::encodeName(filename) ).id() );
            lnk.setExec( "opieplayer" );
            lnk.setIcon( "opieplayer2/MPEGPlayer" );

            if( !lnk.writeLink() ) {
                qDebug( "Writing doclink did not work" );
            }
            d->selectedFiles->addToSelection(  lnk );
        }
    }
    if( fileDlg ) {
        delete fileDlg;
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

void PlayListWidget::readm3u( const QString &filename ) {
    qDebug( "m3u filename is " + filename );
    QFile f( filename );

    if( f.open( IO_ReadOnly ) ) {
        QTextStream t(&f);
        QString s;//, first, second;
        int i=0;
        while ( !t.atEnd() ) {
            s=t.readLine();

            if( s.find( "#", 0, TRUE) == -1 ) {
                if( s.find( " ",  0, TRUE) == -1 )  { // not sure if this is neede since cf uses vfat
                    if( s.left(2) == "E:" || s.left(2) == "P:" ) {
                        s = s.right( s.length() -2 );
                        DocLnk lnk( s );
                        QFileInfo f( s );
                        QString name = f.baseName();
                        name = name.right( name.length() - name.findRev( "\\", -1, TRUE )  -1 );
                        lnk.setName( name );
                        s=s.replace( QRegExp( "\\" ), "/" );
                        lnk.setFile( s );
                        lnk.writeLink();
                        qDebug( "add " + name);
                        d->selectedFiles->addToSelection( lnk );
                    } else { // is url
                        s.replace( QRegExp( "%20" )," " );
                        DocLnk lnk( s );
                        QString name;
                        if( name.left( 4 ) == "http" ) {
                            name = s.right( s.length() - 7 );
                        } else {
                            name = s;
                        }
                        lnk.setName( name );
                        if( s.at( s.length() - 4) == '.' ) {
                            lnk.setFile( s );
                        } else {
                            lnk.setFile( s + "/" );
                        }
                        lnk.setType( "audio/x-mpegurl" );
                        lnk.writeLink();
                        d->selectedFiles->addToSelection( lnk );
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
    fileDlg = new InputDialog( this, tr( "Save m3u Playlist " ), TRUE, 0);
    fileDlg->exec();
    QString filename, list;
    if( fileDlg->result() == 1 ) {
        filename = fileDlg->text();
        qDebug( filename );
        int noOfFiles = 0;
        d->selectedFiles->first();
        do {
            // we dont check for existance because of url's
            //         qDebug(d->selectedFiles->current()->file());

            // so maybe we should do some net checking to ,-)
            list += d->selectedFiles->current()->file() + "\n";
            noOfFiles++;
        }
        while ( d->selectedFiles->next() );
        qDebug( list );
        if( filename.left( 1) != "/" ) {
            filename=QPEApplication::documentDir() + "/" + filename;
        }
        if( filename.right( 3 ) != "m3u" ) {
            filename=filename+".m3u";
        }
        QFile f( filename );
        f.open( IO_WriteOnly );
        f.writeBlock( list, list.length() );
        f.close();
    }
    if( fileDlg ) {
        delete fileDlg;
    }
}

void PlayListWidget::readPls( const QString &filename ) {

    qDebug( "pls filename is " + filename );
    QFile f( filename );

    if( f.open( IO_ReadOnly ) ) {
        QTextStream t( &f );
        QString s;//, first, second;
        int i = 0;
        while ( !t.atEnd() ) {
            s = t.readLine();
            if( s.left(4)  == "File" ) {
                s = s.right( s.length() - 6 );
                s.replace( QRegExp( "%20" )," ");
                qDebug( "adding " + s + " to playlist" );
                // numberofentries=2
                // File1=http
                // Title
                // Length
                // Version
                // File2=http
                s = s.replace( QRegExp( "\\" ), "/" );
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
                if( s.at( s.length() - 4) == '.') // if this is probably a file
                    lnk.setFile( s );
                else { //if its a url
                    if( name.right( 1 ).find( '/' ) == -1) {
                        s += "/";
                    }
                    lnk.setFile( s );
                }
                lnk.setType( "audio/x-mpegurl" );

                //qDebug("DocLnk add "+name);
                d->selectedFiles->addToSelection( lnk );
            }
        }
        i++;
    }
}

void PlayListWidget::pmViewActivated(int index) {
//  qDebug("%d", index);
    switch(index) {
    case -16:
    {
        mediaPlayerState->toggleFullscreen();
        bool b=mediaPlayerState->fullscreen();
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
        qDebug(  fi->fileName() );
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
    for( uint i = defaultSkinIndex; i > defaultSkinIndex - skinsMenu->count(); i-- ) {
        skinsMenu->setItemChecked( i, FALSE );
    }
    skinsMenu->setItemChecked( item, TRUE );

    Config cfg( "OpiePlayer" );
    cfg.setGroup("Options");
    cfg.writeEntry("Skin", skinsMenu->text( item ) );
}
