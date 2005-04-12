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
#include "../opieplayer2/om3u.h"

#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <opie2/ofiledialog.h>

#include <qpe/resource.h>

#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qdir.h>

//#define DEFAULT_FILE_TYPES  "*.mp3;*.ogg;*.ogm;*.wma;*.wav;*.asf;*.au;*.avi;*.mpeg;*.mpg;*.mv1;*.mov;*.wmv;*.pls;*.m3u;*.mp4;*.m4a"
#define DEFAULT_FILE_TYPES  "*.mp3;*.ogg;*.ogm;*.wma;*.wav;*.asf;*.au;*.avi;*.mpeg;*.mpg;*.mv1;*.mov;*.wmv;*.mp4;*.m4a"

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
    m_lastDir = QDir::homeDirPath();
}

PlaylistView::~PlaylistView()
{
    if (m_Infolib) delete m_Infolib;
}

void PlaylistView::checkLib()
{
    if (!m_Infolib) {
        m_Infolib = new XINE::Lib(XINE::Lib::InitializeImmediately);
        connect(m_Infolib,SIGNAL(stopped()),this,SLOT(slotDummyStop()));
        m_Infolib->ensureInitialized();
    }
}

void PlaylistView::slotDummyStop()
{
    odebug << "void PlaylistView::slotDummyStop()" << oendl;
}

void PlaylistView::slotAddFile(const DocLnk&aLink)
{
    QFileInfo f(aLink.file());
    if (f.extension(FALSE).lower()=="m3u"||f.extension(FALSE).lower()=="pls") {
        readPlayList(aLink.file());
    } else {
        addFile(aLink.file(),aLink.name());
    }
    emit contentChanged(childCount());
}

void PlaylistView::addFile(const QString&aFile,const QString&aName)
{
    QFileInfo fileInfo(aFile);
    if (!fileInfo.exists()) return;
    checkLib();
    QString name = aName;
    if (name.isEmpty()) {
        name = fileInfo.fileName();
    }
    int i = m_Infolib->setfile(aFile.utf8().data());
    /* realy! otherwise we get an "stopped" signal when playing! - I don't know why */
    m_Infolib->stop();
    odebug << "File set: " << i << " ("<<aFile.utf8().data()<<")"<<oendl;
    if (i<1) {
        i = m_Infolib->error();
        odebug << "Errorcode: " << i << oendl;
        QString msg;
        msg = QString(tr("Error open file %1: ")).arg(name);
        switch (i) {
            case 1:
                msg += tr("No input plugin");
                break;
            case 2:
                msg += tr("No demuxer plugin");
                break;
            case 3:
                msg += tr("Demuxer failed");
                break;
            case 4:
                msg+=tr("Malformed mrl");
                break;
            default:
                msg += tr("Unknown error");
                break;
        }
        QMessageBox::warning(0,tr("Error reading fileheader"),msg);
        return;
    }
    m_lastItem = m_items.last();
    if (m_lastItem) {
        m_lastItem = new PlaylistItem(aFile,this,m_lastItem);
    } else {
        m_lastItem = new PlaylistItem(aFile,this);
    }
    m_lastItem->setExpandable(false);
    m_lastItem->setText(1,name);

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
        codec.sprintf("%2i:%2i:%2i h",h,m,l);
    } else {
        codec.sprintf("%02i:%02i m",m,l);
    }
    // time
    m_lastItem->setText(COL_TIME,codec);
    m_lastItem->Video(m_Infolib->hasVideo());
    m_items.append(m_lastItem);
    setSelected(m_lastItem,true);
}

void PlaylistView::slotAppendDir()
{
    QString _dir = Opie::Ui::OFileDialog::getDirectory(Opie::Ui::OFileSelector::Extended,m_lastDir,0,m_lastDir);
    if (_dir.isEmpty()) return;
    m_lastDir = _dir;
    QDir sDir(_dir);
    QStringList list = sDir.entryList(DEFAULT_FILE_TYPES,QDir::Files,QDir::Name | QDir::IgnoreCase);
    for (unsigned i = 0; i < list.count();++i) {
        addFile(_dir+QString("/")+list[i]);
    }
    emit contentChanged(childCount());
}

void PlaylistView::slotOpenM3u()
{
    QStringList types;
    QMap<QString, QStringList> mimeTypes;
    types << "audio/x-mpegurl";
    mimeTypes.insert("Playlists",types);
//    mimeTypes.insert("All",types);
    QString fileName= Opie::Ui::OFileDialog::getOpenFileName(Opie::Ui::OFileSelector::EXTENDED,
                                                 m_lastDir,"playlist.m3u", mimeTypes);
    if (fileName.isEmpty()) {
        return;
    }
    readPlayList(fileName);
    emit contentChanged(childCount());
}

void PlaylistView::readPlayList(const QString&fileName)
{
    QFileInfo f(fileName);

    Om3u _om3u(fileName, IO_ReadOnly);
    if (f.extension(FALSE).lower()=="m3u") {
        _om3u.readM3u();
    } else if (f.extension(FALSE).lower()=="pls") {
        _om3u.readPls();
    }
    for (unsigned int j=0; j<_om3u.count();++j) {
        addFile(_om3u[j]);
    }
}

void PlaylistView::slotSaveAsM3u()
{
    QStringList types;
    QMap<QString, QStringList> mimeTypes;
    types << "audio/x-mpegurl";
    mimeTypes.insert("Playlists",types);
    QString fileName= Opie::Ui::OFileDialog::getSaveFileName(Opie::Ui::OFileSelector::EXTENDED,
                                                 m_lastDir,"playlist.m3u", mimeTypes);
    odebug << "Save as " << fileName << oendl;
    if (fileName.isEmpty()) {
        return;
    }
    Om3u _om3u(fileName, IO_ReadWrite | IO_Truncate);
    for (unsigned j=0; j<m_items.count();++j) {
        _om3u.add(m_items[j]->Lnk().file());
    }
    _om3u.write();
}

void PlaylistView::removeFromList(PlaylistItem*Item)
{
    if (!Item)return;
    t_itemlist::Iterator iter,it2;
    iter = m_items.find(Item);
    it2 = m_items.end();
    if (iter!=m_items.end()) {
        it2 = iter;
        ++it2;
        m_items.remove(iter);
    }
    delete Item;
    if (it2!=m_items.end()) {
        setSelected((*it2),true);
    } else if (m_items.count()) {
        QListViewItem * it = m_items.last();
        setSelected(it,true);
    }
    emit contentChanged(childCount());
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
