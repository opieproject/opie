/* $Id: filelistitem.h,v 1.2 2006-03-20 21:43:20 korovkin Exp $ */
/* Directory tree entry */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef FILELISTITEM_H
#define FILELISTITEM_H

#include <qlistview.h>
#include <sys/stat.h>
#include <client.h>

enum dtype { IS_DIR = 0, IS_FILE = 1 };

namespace OpieTooth {

    class FileListItem : public QListViewItem {
    public:
        FileListItem(QListView * parent, stat_entry_t* ent, int size = 0);
        FileListItem(QListViewItem * parent, stat_entry_t* ent, int size = 0);
        virtual QString key ( int, bool ) const;
        virtual enum dtype gettype();
        virtual int getsize();
    protected:
        void init(stat_entry_t* ent, int size);
    protected:
        QString m_name; //name
        int m_size; //file (not directory) size
        enum dtype type; //type: file or directory
    };
};

#endif
