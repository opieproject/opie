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
#include <qlist.h>
#include <qmap.h>

class QAction;
class QPEToolBar;
class QHBox;
class QVBoxLayout;
class QTextView;
class QFrame;
class OListView;
class OListViewItem;
class QListViewItem;
class QPopupMenu;
class QSignalMapper;
class QTimer;
class SearchGroup;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
  MainWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~MainWindow();


public slots:
  void setCurrent(QListViewItem*);
  void setSearch( const QString& );
  void searchAll();

protected slots:
  void slotAction(int);
  void showPopup();
  void stopTimer( QListViewItem* );
  void searchStringChanged();
  void optionChanged(int);

private:
  QHBox *buttonBox;
  QMap<int, QButton*> buttonMap;
  QSignalMapper* signalMapper;
  OListView *resultsList;
  QTextView *richEdit;
  OListViewItem *_currentItem;
  QVBoxLayout *mainLayout;
  QFrame *detailsFrame;
  QTimer *popupTimer;
  QTimer *searchTimer;

  QString _searchString;
  QList<SearchGroup> searches;
  QAction *SearchAllAction;
  QAction *actionCaseSensitiv;
  QAction *actionWildcards;
  //QAction *actionWholeWordsOnly;
  uint _buttonCount;
  void makeMenu();
};

#endif

