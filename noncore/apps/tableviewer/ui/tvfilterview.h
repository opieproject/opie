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
#ifndef Tvfilterview_H
#define Tvfilterview_H

#include "filterkeyentry.h"
#include "../db/common.h"
#include <qlist.h>
#include <qmap.h>
#include <qdialog.h>

class QListViewItem;
class QPushButton;
class QListView;
class TVFilterKeyEntry;

class TVFilterView : public QDialog
{
	Q_OBJECT

signals:
    void editView();
    void listView();
    void browseView();
    void loadFile();

protected slots:
    void newTerm();
    void deleteTerm();
    void clearTerms();
    void updateTerm();
    void setTerm(QListViewItem *);

public:
	TVFilterView(TableState *t, QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
	~TVFilterView();

    /* Access Methods */
    void rebuildKeys();
    void rebuildData();
    void reset();

    bool passesFilter(DataElem *d);
    bool filterActive() const; /* return true if and only if filtering is on */

    QListView* display;
    QPushButton* newFilterButton;
    QPushButton* deleteFilterButton;
    QPushButton* clearFilterButton;
    QComboBox* keyNameCombo;

    TVFilterKeyEntry* keyEntry;
private:

    typedef struct _FilterTerm {
        int keyIndex;
        CmpType ct;
        TVVariant value;
        QListViewItem *view;
    } FilterTerm;

    QList<FilterTerm> terms;
    FilterTerm *current;
    bool do_filter;

    TableState *ts;

    QMap<int, int> keyIds;
};

#endif
