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
#ifndef Tableviewer_H
#define Tableviewer_H

#include <qmainwindow.h>
#include "db/common.h"
#include <qpe/fileselector.h>

/* Forward class declarations */
class QWidgetStack;
class QDialog;
class QMenuBar;

class TVListView;
class TVBrowseView;
class TVFilterView;
class TVEditView;

class DBStore;

class TableViewerWindow: public QMainWindow
{
    Q_OBJECT
public:
    TableViewerWindow( QWidget *parent = 0, 
                        const char *name = 0, WFlags f = 0 );
    ~TableViewerWindow();

public slots:
    void selectDocument();

    void newDocument();
    void saveDocument();
    void openDocument(const DocLnk &);

    void nextItem();
    void previousItem();

    void listViewSlot();
    void browseViewSlot();
    void filterViewSlot();

    void editItemSlot();
    void newItemSlot();
    void deleteItemSlot();

    void editKeysSlot();

    /* reveiw the sig. for this function TODO */
    void searchOnKey(int, TVVariant);
    void setPrimaryKey(int);

/* TODO add new event */
protected:

/* TODO add new slots */
private slots:

/* TODO add other widgets used here */
private:
    bool dirty;

    QMenuBar *menu;
    QToolBar *navigation;

    TVListView *listView;
    TVBrowseView *browseView;
    TVFilterView *filterView;
    TVEditView *editView;
    FileSelector *fileSelector;

    DocLnk doc;

    QWidgetStack *cw;
    DBStore *ds;
    TableState ts; /* not a pointer.. this class keeps the state */

    enum UserState {
        BrowseState = 0,
        ListState,
        FilterState,
        EditState,
  FileState,
    };

    UserState current_view;

    void applyFilter();
};

#endif
