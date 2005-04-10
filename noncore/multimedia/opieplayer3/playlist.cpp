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
#include "playlist.h"
#include "../opieplayer2/lib.h"

#include <opie2/odebug.h>
#include <opie2/oresource.h>

#include <qpe/resource.h>

#include <qfileinfo.h>

PlaylistItem::PlaylistItem(const DocLnk& aLink,PlaylistView*parent)
    :QListViewItem(parent),m_Content(aLink),m_video(false)
{
}

PlaylistItem::PlaylistItem(const DocLnk&aLink,PlaylistView*parent,PlaylistItem*after)
    :QListViewItem(parent,after),m_Content(aLink),m_video(false)
{
}

void PlaylistItem::Video(bool y)
{
    m_video=y;
    if (m_video) {
        setPixmap(0,Opie::Core::OResource::loadPixmap("opieplayer2/videofile"));
    } else {
        setPixmap(0,Opie::Core::OResource::loadPixmap("opieplayer2/musicfile"));
    }
}

PlaylistItem::~PlaylistItem()
{
}

/* PlaylistView Methods */
PlaylistView::PlaylistView(QWidget *parent, const char *name)
    : QListView(parent,name)
{
//    columnLabels << tr("FullName");
    columnLabels << tr("");                 // icon
    columnLabels << tr("File");
    columnLabels << tr("Playtime");
    columnLabels << tr("Artist");
    columnLabels << tr("Album");
    columnLabels << tr("Title");
    columnLabels << tr("Type");
    columnLabels << tr("Size");
    for (QStringList::Iterator it = columnLabels.begin(); it != columnLabels.end(); ++it) {
        addColumn(*it);
    }
    m_Infolib=0;
    setAllColumnsShowFocus(true);
    setSelectionMode(Single);
    setSorting(-1);
    m_lastItem = 0;
}

PlaylistView::~PlaylistView()
{
    if (m_Infolib) delete m_Infolib;
}

void PlaylistView::checkLib()
{
    if (!m_Infolib) {
        m_Infolib = new XINE::Lib(XINE::Lib::InitializeImmediately);
        m_Infolib->ensureInitialized();
    }
}

void PlaylistView::slotAddFile(const DocLnk&aLink)
{
    QFileInfo fileInfo(aLink.file());
    if (!fileInfo.exists()) return;
    checkLib();

    m_Infolib->stop();

    if (m_lastItem) {
        m_lastItem = new PlaylistItem(aLink,this,m_lastItem);
    } else {
        m_lastItem = new PlaylistItem(aLink,this);
    }
    m_lastItem->setExpandable(false);
    m_lastItem->setText(1,aLink.name());
    int i = m_Infolib->setfile(aLink.file());
    odebug << "File set: " << i << oendl;
    if (i<1) {
        return;
    }

    QString codec = m_Infolib->metaInfo(6);
    if (codec.isEmpty()) {
        codec = m_Infolib->metaInfo(7);
    }
    // codec
    m_lastItem->setText(COL_TYPE,codec);
    // title
    m_lastItem->setText(COL_TITLE,m_Infolib->metaInfo(0));
    // artist
    m_lastItem->setText(COL_ARTIST,m_Infolib->metaInfo(2));
    // album
    m_lastItem->setText(COL_ALBUM,m_Infolib->metaInfo(4));
    int l = m_Infolib->length();
    int h = l/3600;
    l-=h*3600;
    int m = l/60;
    l-=m*60;
    codec = "";
    if (h>0) {
        codec+=QString("%1 h").arg(h);
    }
    if (m>0) {
        if (!codec.isEmpty()) codec+=" ";
        codec+=QString("%1 m").arg(m);
    }
    if (l>0) {
        if (!codec.isEmpty()) codec+=" ";
        codec+=QString("%1 s").arg(l);
    }
    // time
    m_lastItem->setText(COL_TIME,codec);
    m_lastItem->Video(m_Infolib->hasVideo());
    m_items.append(m_lastItem);
}

void PlaylistView::removeFromList(PlaylistItem*Item)
{
    if (!Item)return;
    t_itemlist::Iterator iter;
    iter = m_items.find(Item);
    if (iter!=m_items.end()) {
        m_items.remove(iter);
    }
    delete Item;
}

XINE::Lib*PlaylistView::getXine()
{
    checkLib();
    return m_Infolib;
}

void PlaylistView::setCurrentItem(PlaylistItem*aItem)
{
    setSelected(aItem,true);
}

PlaylistItem* PlaylistView::currentItem()const
{
    QListViewItem*it = selectedItem();
    if (!it) return 0;
    return (PlaylistItem*)it;
}

PlaylistItem* PlaylistView::nextItem(PlaylistItem*parent)const
{
    if (m_items.count()==0) return 0;
    if (!parent) return m_items[0];
    for (unsigned j=0; j<m_items.count()-1;++j) {
        if (m_items[j]==parent) {
            return m_items[j+1];
        }
    }
    return 0;
}

PlaylistItem* PlaylistView::prevItem(PlaylistItem*parent)const
{
    if (m_items.count()==0) return 0;
    if (!parent) return m_items[m_items.count()-1];
    for (unsigned j=m_items.count()-1; j>0;--j) {
        if (m_items[j]==parent) {
            return m_items[j-1];
        }
    }
    return 0;
}
