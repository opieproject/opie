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

#ifndef PLAY_LIST_WIDGET_GUI_H
#define PLAY_LIST_WIDGET_GUI_H

#include <qmainwindow.h>
#include <qpe/applnk.h>
#include <qpe/resource.h>
#include <qpe/qpemenubar.h>

#include <qtabwidget.h>
#include <qpe/fileselector.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qslider.h>
#include <qlcdnumber.h>

class PlayListWidgetPrivate;
class PlayListSelection;
class MediaPlayerState;
class PlayListFileView;

class Config;
class QPEToolBar;
class QListViewItem;
class QListView;
class QPoint;
class QAction;
class QLabel;

class PlayListWidgetPrivate {
public:
    QToolButton *tbPlay, *tbFull, *tbLoop,  *tbShuffle, *tbAddToList,  *tbRemoveFromList, *tbMoveUp, *tbMoveDown, *tbRemove;
    QFrame *playListFrame;
    PlayListSelection *selectedFiles;
    bool setDocumentUsed;
};


class ToolButton : public QToolButton {
    Q_OBJECT
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

class PlayListWidgetGui : public QMainWindow {
    Q_OBJECT
public:
    PlayListWidgetGui( MediaPlayerState &_mediaPlayerState, QWidget* parent=0, const char* name=0 );
    ~PlayListWidgetGui();

protected:
    QTabWidget * tabWidget;
    PlayListFileView *audioView, *videoView;
    QListView *playlistView;
    QLabel *libString;
    QPopupMenu *pmView ;
    QPopupMenu *gammaMenu;
    QSlider *gammaSlider;
    QLCDNumber *gammaLCD;
    bool fromSetDocument;
    bool insanityBool;
    QString setDocFileRef;
    // retrieve the current playlist entry (media file link)
    QPushButton *tbDeletePlaylist;
    int selected;
    QPopupMenu *pmPlayList;
    FileSelector*  playLists;
    QPopupMenu *skinsMenu;
    PlayListWidgetPrivate *d; // Private implementation data
    QVBox *vbox1;
    QVBox *vbox5;
    QPEToolBar *bar;
    QWidget *playListTab;
    void setActiveWindow(); // need to handle this to show the right view
    void setView( char );

    MediaPlayerState &mediaPlayerState;
};

#endif

