/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#include "tvlistview.h"
#include "../db/common.h"
#include <qtoolbutton.h>
#include <qlistview.h>
#include <qlayout.h>

void TVListViewPrivate::setColumnWidth(int column, int width)
{
    if(width > 70) width = 70;
    QListView::setColumnWidth(column, width);
}

void TVListViewPrivate::setSorting(int column, bool increasing)
{
    emit sortChanged(column);
    QListView::setSorting(column, increasing);
}

TVListViewPrivate::TVListViewPrivate(QWidget *parent, const char* name,
        WFlags fl) : QListView(parent, name, fl) {
        ;
}

class TVListViewItem : public QListViewItem 
{
public:

    TVListViewItem(QListView *parent, DataElem *d);
    ~TVListViewItem();

    QString text(int i) const
    {
        return data_reference->toQString(i);
    }

    /* Do nothing... all data for this item should be generated */
    void setText(int i, const QString &) 
    {
        ;
    }
    QString key(int i, bool a) const 
    {
        return data_reference->toSortableQString(i);
    }

    void setDataElem(DataElem *d) 
    {
        data_reference = d;
    }

    DataElem *getDataElem() {
        return data_reference;
    }
private: 
    DataElem *data_reference;
};

TVListViewItem::TVListViewItem(QListView *parent, DataElem *d) 
        : QListViewItem(parent)
{
    data_reference =  d;
}

TVListViewItem::~TVListViewItem()
{
    data_reference = 0;
}

TVListView::TVListView(TableState *t, QWidget* parent, 
	const char *name, WFlags fl ) : QWidget(parent, name, fl)
{
    if (!name) 
    	setName("TVListView");

    // the next two lines need to be rationalized.
    resize(318,457);
    setSizePolicy(QSizePolicy((QSizePolicy::SizeType)7, 
    		  (QSizePolicy::SizeType)7, sizePolicy().hasHeightForWidth()));
    setCaption(tr("List View"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    listViewDisplay = new TVListViewPrivate(this, "listViewDisplay");
    layout->addWidget(listViewDisplay);

    connect(listViewDisplay, SIGNAL(currentChanged(QListViewItem *)), this,
    		SLOT(setCurrent(QListViewItem *)));
    connect(listViewDisplay, SIGNAL(sortChanged(int)), this, 
    		SLOT(setSorting(int)));

    listViewDisplay->setShowSortIndicator(true);

    it = new QListViewItemIterator(listViewDisplay);
    ts = t;
}

TVListView::~TVListView()
{
}

void TVListView::addItem(DataElem *d) 
{
    TVListViewItem *i = new TVListViewItem(listViewDisplay, d);

    delete it;
    it = new QListViewItemIterator(i);
}

/* remove current (it) item */
void TVListView::removeItem() 
{
    QListViewItemIterator other(*it);

    QListViewItemIterator tmp = *it;
    (*it)++;
    if (!it->current()) {
        *it = tmp;
		(*it)--;
		if (!it->current()) {
		   delete it;
		   it = 0;
	   }
    }

   delete other.current();
}

void TVListView::clearItems() 
{
    /* This is ok since the destructor for TVListItem does not know about
    the data_reference pointer.. and hence will leave it alone */
    listViewDisplay->clear();
    delete it;
    it = new QListViewItemIterator(listViewDisplay);
}

void TVListView::first()
{
    delete it;
    it = new QListViewItemIterator(listViewDisplay);
}

void TVListView::last()
{
    qWarning("TVListView::last not yet implemented");
}

void TVListView::next()
{
    QListViewItemIterator tmp = *it;
    (*it)++;
    if (!it->current()) {
        *it = tmp;
    }
}

void TVListView::previous()
{
    QListViewItemIterator tmp = *it; 
    (*it)--;
    if (!it->current()) {
        *it = tmp;
    }
}

DataElem *TVListView::getCurrentData() {
    if (it->current()) {
        return ((TVListViewItem *)it->current())->getDataElem();
    }
    return NULL;
}

/*! Now to implement the closest match function */
void TVListView::findItem(int keyId, TVVariant value) 
{
    QListViewItem *i;
    TVListViewItem *best_so_far = NULL;
    /* start at the beginning... go through till find the closest elem */
    i = listViewDisplay->firstChild();
    while (i) {
        /* search stuff */
        if(best_so_far) {
            if (DataElem::closer(
                    ((TVListViewItem *)i)->getDataElem(), 
                    best_so_far->getDataElem(), value, keyId)) 
                best_so_far = (TVListViewItem *)i;
        } else {
            if (DataElem::closer(
                    ((TVListViewItem *)i)->getDataElem(), 
                    NULL, value, keyId)) 
                best_so_far = (TVListViewItem *)i;
        }
            
        i = i->itemBelow();
    }
    if (best_so_far) {
        /* set best_so_far to current element */
        delete it;
        it = new QListViewItemIterator(best_so_far);
    }
}

void TVListView::rebuildKeys()
{
    int i;
    if(!ts) return;
    if(!ts->kRep) return;

    i = listViewDisplay->columns();

    while(i > 0) 
        listViewDisplay->removeColumn(--i);

    KeyListIterator kit(*ts->kRep);
    i = 0;
    while(kit.current()) {
        if(!kit.current()->delFlag()) {
            listViewDisplay->addColumn(kit.current()->name());
            keyIds.insert(i, kit.currentKey());
            ++i;
        }
        ++kit;
    }
}


void TVListView::setSorting(int column) 
{
    /* Without table state can't do anything */
    if (ts == 0) 
        return;
    if (keyIds[column] != ts->current_column) {
        ts->current_column = keyIds[column];
    }
}

void TVListView::rebuildData() {
    int i;
    QMap<int, int>::Iterator kit;
    /* Need to set sort order */
    if(!ts)
        return;

    /* revers lookup the column */
    i = -1;
    for(kit = keyIds.begin(); kit != keyIds.end(); ++kit) {
        if (kit.data() == ts->current_column) {
            i = kit.key();
            break;
        }
    }
    if (i == -1)
        return;

    listViewDisplay->setSorting(i);
    listViewDisplay->sort();

    /* reset current element */
    listViewDisplay->setCurrentItem(it->current());
    listViewDisplay->setSelected(it->current(), true);
    listViewDisplay->ensureItemVisible(it->current());
}

void TVListView::reset() 
{
    int i;
    listViewDisplay->clear();

    i = listViewDisplay->columns();
    while (i > 0)
        listViewDisplay->removeColumn(--i);

    keyIds.clear();
}

void TVListView::setCurrent(QListViewItem *i) 
{
    /* cast */
    TVListViewItem *t = (TVListViewItem *)i;

    if(!t) {
        /* set current to null */
        ts->current_elem = 0;
        return;
    }

    ts->current_elem = t->getDataElem();
    /* now also set up the iterator */

    delete it;
    it = new QListViewItemIterator(i);

    //emit browseView();
}
