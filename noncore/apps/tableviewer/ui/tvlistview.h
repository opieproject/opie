/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef Tvlistview_H
#define Tvlistview_H

#include "../db/common.h"
#include <qlistview.h>
#include <qmap.h>

class QListViewItemIterator;

class TVListViewPrivate : public QListView
{
    Q_OBJECT

signals:
    void sortChanged(int i);

public:
    TVListViewPrivate( QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

    void setColumnWidth(int c, int w);
    void setSorting(int i, bool increasing=true);
};

class TVListView : public QWidget
{
	Q_OBJECT

signals:
	void loadFile();
    void browseView();
    void filterView();
    void editView();

protected slots:
    void setSorting(int);
    void setCurrent(QListViewItem *);

public:
	TVListView(TableState *t, QWidget* parent = 0, 
               const char* name = 0, WFlags fl = 0);
	~TVListView();                                                          
    
    /* to be used for setting up the list */
    void addItem(DataElem *);
    void removeItem(); // remove from list, not from program
    void clearItems();

    /* DBStore clone functions */
    void first();
    void last();
    void next();
    void previous();

    void rebuildKeys();
    void rebuildData();
    void reset(); 

    DataElem *getCurrentData();

    void findItem(int i, TVVariant v);

protected:
    QListViewItemIterator *it;
    TableState *ts;

    TVListViewPrivate *listViewDisplay;

    QMap<int, int> keyIds;

};

#endif
