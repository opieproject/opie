/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2002-2003 Patrick S. Vogt <tille@handhelds.org>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qdialog.h>
#include <qaction.h>
#include <qlist.h>

#include <qmap.h>
#include <qtimer.h>
#include <qpopupmenu.h>

class QAction;
class QToolBar;
class QHBox;
class QVBoxLayout;
class QTextView;
class QFrame;
class OListView;
class OListViewItem;
class QListViewItem;
class QPopupMenu;
class QTimer;
class QLabel;

class QSignalMapper;
class QButton;

class SearchGroup;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
  MainWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~MainWindow();
  static QString appName() { return QString::fromLatin1("osearch"); }

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
  QToolButton* ClearSearchText;
  QLabel* LabelEnterText;

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

