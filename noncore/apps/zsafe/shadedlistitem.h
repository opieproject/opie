/*
 * shadedlistitem.h
 *
 * (c) 2003 by Jeremy Bowman <jmbowman@alum.mit.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef SHADEDLISTITEM_H
#define SHADEDLISTITEM_H

#include <qlistview.h>

class ShadedListItem: public QListViewItem
{
public:
    ShadedListItem(int index, QListViewItem *parent);
    ShadedListItem(int index, QListView *parent);
    ShadedListItem(int index, QListView *parent, QListViewItem *after);
    ShadedListItem(int index, QListView *parent, QString label1, QString label2 = QString::null, QString label3 = QString::null);
    ShadedListItem(int index, QListView *parent, QListViewItem *after, QString label1, QString label2 = QString::null, QString label3 = QString::null);
    ~ShadedListItem();

    void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);

private:
    QListView *lv;
    bool oddRow;
};

#endif
