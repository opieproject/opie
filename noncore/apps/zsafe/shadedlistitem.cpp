/*
 * shadedlistitem.cpp
 *
 * (c) 2003 by Jeremy Bowman <jmbowman@alum.mit.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "zsafe.h"
#include "shadedlistitem.h"


ShadedListItem::ShadedListItem(int index, QListViewItem *parent)
    : QListViewItem(parent)
{
    oddRow = (index % 2 != 0);
}

ShadedListItem::ShadedListItem(int index, QListView *parent)
    : QListViewItem(parent), lv(parent)
{
    oddRow = (index % 2 != 0);
}

ShadedListItem::ShadedListItem(int index, QListView *parent, QListViewItem *after)
    : QListViewItem(parent, after), lv(parent)
{
    oddRow = (index % 2 != 0);
}

ShadedListItem::ShadedListItem(int index, QListView *parent, QString label1, QString label2, QString label3)
    : QListViewItem(parent, label1, label2, label3), lv(parent)
{
    oddRow = (index % 2 != 0);
}

ShadedListItem::ShadedListItem(int index, QListView *parent, QListViewItem *after, QString label1, QString label2, QString label3)
    : QListViewItem(parent, after, label1, label2, label3), lv(parent)
{
    oddRow = (index % 2 != 0);
}

ShadedListItem::~ShadedListItem()
{

}


void ShadedListItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    QColorGroup colors(cg);
#if QT_VERSION >=300
    const QColorGroup::ColorRole crole = QColorGroup::Base;
    // const QWidget::BackgroundMode bgmode = lv->viewport()->backgroundMode();
    // const QColorGroup::ColorRole crole = QPalette::backgroundRoleFromMode(bgmode);
#else
    const QColorGroup::ColorRole crole = QColorGroup::Base;
#endif
    if (oddRow) {
        colors.setColor(crole, *ZSafe::oddRowColor);
    }
    else {
        colors.setColor(crole, *ZSafe::evenRowColor);
    }
    QListViewItem::paintCell(p, colors, column, width, alignment);
}
