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

#ifndef _mwindow_h
#define _mwindow_h

#include <qmainwindow.h>

#include <qpe/applnk.h>

#include <qaction.h>

namespace Opie {
namespace Ui {
    class OWidgetStack;
    class OFileSelector;
}
}

namespace XINE {
    class Lib;
}

class PlaylistView;
class QToolBar;
class QPopupMenu;
class QMenuBar;
class AudioWidget;
class VideoWidget;
class PlaylistItem;

class PMainWindow : public QMainWindow {
    Q_OBJECT
public:
    static QString appName() { return QString::fromLatin1("opie-mediaplayer3" ); }
    PMainWindow(QWidget*, const char*, WFlags );
    virtual ~PMainWindow();

public slots:
    virtual void slotVideoclicked();
protected:
    static const int stack_list = 0;
    static const int stack_file = 1;
    static const int stack_audio = 2;
    static const int stack_video = 3;
    Opie::Ui::OWidgetStack *m_stack;
    Opie::Ui::OFileSelector*m_sfl;
    PlaylistView*m_playList;
    AudioWidget*m_AudioPlayer;
    VideoWidget*m_VideoPlayer;

    QAction*a_appendFiles,*a_removeFiles,*a_showPlaylist,*a_playAction,*a_deleteItem,*a_stopAction;
    QAction*a_playNext,*a_playPrevious,*a_ShowFull,*a_stopPlay;
    QActionGroup*playersGroup;
    QToolBar*m_toolBar;
    QPopupMenu *fileMenu,*dispMenu,*playMenu;
    QMenuBar*m_menuBar;
    XINE::Lib*m_PlayLib;

    void setupActions();
    void setupToolBar();
    void setupMenu();
    void checkLib();
    void setupVideo(bool full);
    void hideVideo();

    bool m_playing:1;
    int m_CurrentPos;
    PlaylistItem*m_LastItem;

protected slots:
    virtual void fileSelected(const DocLnk&);
    virtual void slotAppendFiles();
    virtual void slotRemoveFiles();
    virtual void slotShowList();
    virtual void slotPlayList();
    virtual void slotPlayNext();
    virtual void slotPlayPrevious();
    virtual void slotPlayCurrent();
    virtual void slotStopped();
    virtual void slotCheckPos();
    virtual void slotToggleFull(bool);
    virtual void slotUserStop();
    virtual void slotGoNext();

signals:
    void sigPos(int);
};

#endif
