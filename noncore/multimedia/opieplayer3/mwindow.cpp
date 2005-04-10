/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "mwindow.h"
#include "playlist.h"
#include "audiowidget.h"
#include "videowidget.h"
#include "../opieplayer2/lib.h"

#include <opie2/oapplicationfactory.h>
#include <opie2/owidgetstack.h>
#include <opie2/ofileselector.h>
#include <opie2/odebug.h>

#include <qpe/resource.h>

#include <qfileinfo.h>
#include <qfile.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtimer.h>

OPIE_EXPORT_APP( Opie::Core::OApplicationFactory<PMainWindow>)

using namespace Opie::Ui;

PMainWindow::PMainWindow(QWidget*w, const char*name, WFlags f)
    : QMainWindow(w,name,f)
{
    setCaption( QObject::tr("Opie Mediaplayer 3" ) );
    setupActions();
    setupToolBar();
    setupMenu();

    m_stack = new OWidgetStack(this);
    m_stack->forceMode(Opie::Ui::OWidgetStack::SmallScreen);
    setCentralWidget(m_stack );
    m_playList = new PlaylistView(m_stack,"playlist");
    m_stack->addWidget(m_playList,stack_list);

    m_sfl = new OFileSelector("video/*;audio/*",m_stack);
    m_stack->addWidget(m_sfl,stack_file);
    connect(m_sfl, SIGNAL(fileSelected(const DocLnk &)), m_playList, SLOT(slotAddFile(const DocLnk &)));
    m_AudioPlayer = new AudioWidget(m_stack);
    m_stack->addWidget(m_AudioPlayer,stack_audio);
    connect(this,SIGNAL(sigPos(int)),m_AudioPlayer,SLOT(updatePos(int)));
    m_VideoPlayer = new VideoWidget(m_stack);
    m_stack->addWidget(m_VideoPlayer,stack_video);
    connect(this,SIGNAL(sigPos(int)),m_VideoPlayer,SLOT(updatePos(int)));
    connect(m_VideoPlayer,SIGNAL(videoclicked()),this,SLOT(slotVideoclicked()));

    m_stack->raiseWidget(stack_list);
    m_PlayLib = 0;
    m_LastItem = 0;
}

void PMainWindow::checkLib()
{
    if (m_PlayLib == 0) {
        m_PlayLib = new XINE::Lib(XINE::Lib::InitializeImmediately);
        m_PlayLib->ensureInitialized();
        connect(m_PlayLib,SIGNAL(stopped()),this,SLOT(slotStopped()));
    }
}

PMainWindow::~PMainWindow()
{
    if (m_PlayLib) {
        m_PlayLib->stop();
        delete m_PlayLib;
    }
}

void PMainWindow::fileSelected(const DocLnk&)
{
}

void PMainWindow::slotAppendFiles()
{
    m_stack->raiseWidget(m_sfl);
}

void PMainWindow::slotShowList()
{
    m_stack->raiseWidget(m_playList);
}

void PMainWindow::slotPlayList()
{
    if (m_playing && m_LastItem && m_PlayLib) {
        if (!m_LastItem->isVideo()) {
            m_stack->raiseWidget(stack_audio);
        } else {
            m_stack->raiseWidget(stack_video);
        }
        return;
    }

    m_LastItem = m_playList->currentItem();
    if (!m_LastItem) {
        slotPlayNext();
        return;
    }
    //m_VideoPlayer->setFullScreen(a_ShowFull->isOn());
    slotPlayCurrent();
}

void PMainWindow::hideVideo()
{
    if (m_VideoPlayer->isVisible() && a_ShowFull->isOn()) {
        m_VideoPlayer->showNormal();
        m_VideoPlayer->hide();
    }
}

void PMainWindow::slotPlayCurrent()
{
    if (!m_LastItem) {
        if (m_PlayLib) m_PlayLib->stop();
        hideVideo();
        m_stack->raiseWidget(stack_list);
        return;
    }
    checkLib();
    m_CurrentPos = 0;
    m_playList->setCurrentItem(m_LastItem);
    odebug << "Pos: " << m_PlayLib->currentTime() << oendl;
    if (!m_LastItem->isVideo()) {
        hideVideo();
        m_playing = true;
        QTimer::singleShot( 500, this, SLOT( slotCheckPos() ) );
        m_stack->raiseWidget(stack_audio);
        m_AudioPlayer->playFile(m_LastItem->Lnk(),m_PlayLib);
    } else {
        m_playing = true;
        QTimer::singleShot( 500, this, SLOT( slotCheckPos() ) );
        setupVideo(a_ShowFull->isOn());
        m_VideoPlayer->playFile(m_LastItem->Lnk(),m_PlayLib);
    }
}

void PMainWindow::slotPlayNext()
{
    PlaylistItem*item = m_playList->nextItem(m_LastItem);
    if (!item) return;
    m_LastItem = item;
    slotPlayCurrent();
}

void PMainWindow::slotGoNext()
{
    m_LastItem = m_playList->nextItem(m_LastItem);
    slotPlayCurrent();
}

void PMainWindow::slotPlayPrevious()
{
    PlaylistItem*item = m_playList->prevItem(m_LastItem);
    if (!item) return;
    m_LastItem = item;
    slotPlayCurrent();
}

void PMainWindow::slotUserStop()
{
    if (!m_playing || !m_PlayLib) return;
    m_playing = false;
    m_PlayLib->stop();
    hideVideo();
    m_stack->raiseWidget(stack_list);
}

void PMainWindow::slotStopped()
{
    if (!m_playing) return;
    m_playing = false;
    slotGoNext();
}

void PMainWindow::slotCheckPos()
{
    if (!m_playing) return;
    emit sigPos(m_PlayLib->currentTime());
    QTimer::singleShot( 1000, this, SLOT( slotCheckPos() ) );
}

void PMainWindow::slotRemoveFiles()
{
    slotUserStop();
    PlaylistItem* Item = m_playList->currentItem();
    m_stack->raiseWidget(stack_list);
    m_playList->removeFromList(Item);
}

void PMainWindow::setupActions()
{
    a_appendFiles = new QAction(tr("Open file(s)"),Resource::loadIconSet( "opieplayer2/add_to_playlist" ), 0, 0, this, 0, false );
    connect(a_appendFiles,SIGNAL(activated()),this,SLOT(slotAppendFiles()));
    a_showPlaylist = new QAction(tr("Show playlist"),Resource::loadIconSet( "txt" ), 0, 0, this, 0, false );
    connect(a_showPlaylist,SIGNAL(activated()),this,SLOT(slotShowList()));
    a_removeFiles = new QAction(tr("Remove file"),Resource::loadIconSet( "opieplayer2/remove_from_playlist" ), 0, 0, this, 0, false );
    connect(a_removeFiles,SIGNAL(activated()),this,SLOT(slotRemoveFiles()));

    playersGroup = new QActionGroup(this,"playgroup",false);

    a_playAction = new QAction(tr("Play list"),Resource::loadIconSet( "opieplayer2/play" ), 0, 0, this, 0, false );
    connect(a_playAction,SIGNAL(activated()),this,SLOT(slotPlayList()));
    a_playNext = new QAction(tr("Play next in list"),Resource::loadIconSet( "fastforward" ), 0, 0, this, 0, false );
    connect(a_playNext,SIGNAL(activated()),this,SLOT(slotPlayNext()));
    a_playPrevious = new QAction(tr("Play previous in list"),Resource::loadIconSet( "fastback" ), 0, 0, this, 0, false );
    connect(a_playPrevious,SIGNAL(activated()),this,SLOT(slotPlayPrevious()));
    a_ShowFull = new QAction(tr("Show videos fullscreen"),Resource::loadIconSet( "fullscreen" ), 0, 0, this, 0, true );
    connect(a_ShowFull,SIGNAL(toggled(bool)),this,SLOT(slotToggleFull(bool)));
    a_stopPlay = new QAction(tr("Show videos fullscreen"),Resource::loadIconSet( "stop" ), 0, 0, this, 0, false );
    connect(a_stopPlay,SIGNAL(activated()),this,SLOT(slotUserStop()));

    playersGroup->insert(a_playPrevious);
    playersGroup->insert(a_playAction);
    playersGroup->insert(a_stopPlay);
    playersGroup->insert(a_playNext);
    //playersGroup->insert(a_ShowFull);
}

void PMainWindow::setupToolBar()
{
    setToolBarsMovable( false );
    m_toolBar = new QToolBar( this );
    //m_menuBar = new QMenuBar(m_toolBar);
    m_menuBar=menuBar();
    addToolBar(m_toolBar);
    m_toolBar->setHorizontalStretchable( true );
    a_appendFiles->addTo(m_toolBar);
    a_removeFiles->addTo(m_toolBar);
    a_showPlaylist->addTo(m_toolBar);
    a_ShowFull->addTo(m_toolBar);
    playersGroup->addTo(m_toolBar);
}

void PMainWindow::setupVideo(bool full)
{
    if (full) {
        m_VideoPlayer->setBackgroundColor(black);
        m_VideoPlayer->reparent(0, WStyle_Customize | WStyle_NoBorderEx, QPoint(0,0));
        m_VideoPlayer->setGeometry(0,0,qApp->desktop()->size().width(),qApp->desktop()->size().height());
        m_VideoPlayer->showFullScreen();
    } else {
        m_VideoPlayer->hide();
        m_stack->addWidget(m_VideoPlayer,stack_video);
        m_stack->raiseWidget(stack_video);
    }
    m_VideoPlayer->fullScreen(full);
    m_VideoPlayer->disconnect(this);
    connect(m_VideoPlayer,SIGNAL(videoclicked()),this,SLOT(slotVideoclicked()));
}

void PMainWindow::slotVideoclicked()
{
    odebug << "PMainWindow::slotVideoclicked()" << oendl;
    if (a_ShowFull->isOn()) {
        a_ShowFull->setOn(false);
        slotToggleFull(false);
    }
}

void PMainWindow::slotToggleFull(bool how)
{
    if (m_PlayLib && m_VideoPlayer->isVisible() && m_PlayLib->isShowingVideo()) {
        m_PlayLib->pause(true);
        setupVideo(how);
        m_PlayLib->pause(false);
    }
}

void PMainWindow::setupMenu()
{
    fileMenu = new QPopupMenu( m_menuBar );
    m_menuBar->insertItem( tr( "Playlist" ), fileMenu );
    a_appendFiles->addTo(fileMenu);
    a_removeFiles->addTo(fileMenu);

    dispMenu = new QPopupMenu( m_menuBar );
    m_menuBar->insertItem( tr( "Show" ), dispMenu );
    a_showPlaylist->addTo(dispMenu);
    a_ShowFull->addTo(dispMenu);
    playMenu = new QPopupMenu(m_menuBar);
    m_menuBar->insertItem(tr("Playing"),playMenu);

    playersGroup->addTo(playMenu);
}
