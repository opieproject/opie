/****************************************************************************
** Created: Sat Jul 20 08:10:31 2002
**      by:  L.J. Potter <ljp@llornkcor.com>
**     copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/
#ifndef BARTENDER_H
#define BARTENDER_H
#include "newdrinks.h"

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qfile.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QListView;
class QListViewItem;
class QToolBar;

class Bartender : public QMainWindow
{ 
    Q_OBJECT

public:
    Bartender( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Bartender();

    QToolBar* ToolBar1;
    QListView* DrinkView;
    QFile dbFile;
public slots:
    void doEdit();
    
protected:

protected slots:
    void fileMenuActivated(int);
    void editMenuActivated(int); 
    void cleanUp();
    void fileNew();
    void showDrink( QListViewItem *);
    void showDrink(int, QListViewItem *, const QPoint&, int);
    void initDrinkDb();
    void doSearchByName();
    void doSearchByDrink();
    void showSearchResult(QStringList &);
    void doBac();
    void openCurrentDrink();
    void askSearch();
private:
    void clearList();
    void fillList();
 
};

#endif // BARTENDER_H
