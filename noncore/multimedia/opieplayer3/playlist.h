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
#ifndef __playlist_h
#define __playlist_h

#include <qpe/applnk.h>

#include <qlistview.h>
#include <qstringlist.h>

namespace XINE {
    class Lib;
}

class PlaylistView;

class PlaylistItem:public QListViewItem
{
public:
    PlaylistItem(const DocLnk&aLink,PlaylistView*parent);
    PlaylistItem(const DocLnk&aLink,PlaylistView*parent,PlaylistItem*after);
    virtual ~PlaylistItem();

    const DocLnk&Lnk()const{return m_Content;}
    void Video(bool y);
    bool isVideo()const{return m_video;}

protected:
    DocLnk m_Content;
    bool m_video:1;
};

typedef QValueList<PlaylistItem*> t_itemlist;

class PlaylistView:public QListView
{
    Q_OBJECT
public:
    PlaylistView( QWidget *parent = 0, const char *name = 0);
    virtual ~PlaylistView();
    XINE::Lib*getXine();
    PlaylistItem* currentItem()const;
    void setCurrentItem(PlaylistItem*);
    PlaylistItem* nextItem(PlaylistItem*parent)const;
    PlaylistItem* prevItem(PlaylistItem*parent)const;

    void removeFromList(PlaylistItem*Item);

    enum itemcolumns{
        COL_ICON=0,
        COL_FILE=1,
        COL_TIME=2,
        COL_ARTIST=3,
        COL_ALBUM=4,
        COL_TITLE=5,
        COL_TYPE=6,
        COL_SIZE=7
    };
    void addFile(const QString&aFile,const QString&aName="");
signals:
    void contentChanged(int);

public slots:
    virtual void slotAddFile(const DocLnk&);
    virtual void slotAppendDir();
    virtual void slotOpenM3u();
    virtual void slotSaveAsM3u();

protected:
    void checkLib();
    QStringList columnLabels;
    XINE::Lib*m_Infolib;
    PlaylistItem*m_lastItem;
    t_itemlist m_items;
    QString m_lastDir;
};

#endif
