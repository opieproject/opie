/* $Id: filelistitem.cpp,v 1.2 2006-03-20 21:43:19 korovkin Exp $ */
/* Directory tree entry */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "filelistitem.h"
#include <qpe/resource.h>

using namespace OpieTooth;

FileListItem::FileListItem(QListView* parent, stat_entry_t* ent, int size) :
    QListViewItem(parent)
{
    init(ent, size);
}

FileListItem::FileListItem(QListViewItem* parent, stat_entry_t* ent, int size) :
    QListViewItem(parent), m_name(ent->name)
{
    init(ent, size);
}

void FileListItem::init(stat_entry_t* ent, int size)
{
    if (ent == NULL) {
        type = IS_DIR;
        m_name = ".."; //Upper directory
        m_size = 0;
    }
    else {
        m_name = ent->name;
        if (ent->mode == 16877)
            type = IS_DIR;
        else
            type = IS_FILE;
    }
    if (type == IS_DIR) {
        setPixmap(0, Resource::loadPixmap("folder"));
        setText(0, m_name + QString("/"));
        m_size = 0;
    }
    else {
        setPixmap(0, Resource::loadPixmap("c_src"));
        setText(0, m_name);
        m_size = size;
        setText(1, QString::number(m_size));
    }
}

QString FileListItem::key(int, bool) const
{
    QString str; //resulting string
    if (type == IS_DIR)
        str = "0";
    else
        str = "1";
    str += m_name;
    return str;
}

enum dtype FileListItem::gettype()
{
    return type;
}

int FileListItem::getsize()
{
    return m_size;
}

//eof
