/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2002 Patrick S. Vogt <tille@handhelds.org>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qdialog.h>
#include <qaction.h>
#include <qtimer.h>
#include <qpopupmenu.h>

class QPEToolBar;
class QVBoxLayout;
class QTextView;
class QFrame;
class QListViewItem;
class OListView;
class OListViewItem;
class AdressSearch;
class TodoSearch;
class DatebookSearch;
class AppLnkSearch;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
  MainWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~MainWindow();


public slots:
  void setCurrent(QListViewItem*);
  void showPopup();
  void stopTimer( QListViewItem* );
  void setSearch( const QString& );

protected slots:
  void showItem();
  void editItem();

private:
  OListView *resultsList;
  QTextView *richEdit;
  OListViewItem *_currentItem;
  QVBoxLayout *mainLayout;
  QFrame *detailsFrame;
  OListViewItem *_item;
  QTimer *popupTimer;

  AdressSearch *adrSearch;
  TodoSearch *todoSearch;
  DatebookSearch *datebookSearch;
  AppLnkSearch *applnkSearch;

  void makeMenu();
};

#endif

