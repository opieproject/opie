/***************************************************************************
   opieftp.h
                             -------------------
** Created: Sat Mar 9 23:33:09 2002
    copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ADVANCEDFM_H
#define ADVANCEDFM_H

#include <qvariant.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <qdir.h>
#include <qstring.h>
#include <qpoint.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QComboBox;
class QListView;
class QListviewItem;
class QLabel;
class QProgressBar;
class QSpinBox;
class QTabWidget;
class QWidget;
class QPEToolBar;
class QPEMenuBar;
class QPopupMenu;
class QFile;
class QListViewItem;
class QLineEdit;
class QPushButton;

class AdvancedFm : public QMainWindow
{
    Q_OBJECT
public:
    AdvancedFm();
    ~AdvancedFm();

    QTabWidget *TabWidget;
    QWidget *tab, *tab_2, *tab_3;
    QListView *Local_View, *Remote_View;

    QLineEdit *currentPathEdit;
    QPopupMenu *fileMenu, *localMenu, *remoteMenu, *viewMenu;
    QPushButton  *homeButton, *docButton, *cdUpButton, *sdButton, *cfButton;
    QDir currentDir, currentRemoteDir;
    QComboBox *currentPathCombo;
    QString filterStr;
    QListViewItem * item;
    bool b;
    int currentServerConfig;
protected slots:
    void showLocalMenu( QListViewItem *);
    void showRemoteMenu( QListViewItem *);
    void doLocalCd();
    void doRemoteCd();
//    void copy();
    void mkDir();
    void del();
    void rn();
    void populateLocalView();
    void populateRemoteView();
    void showHidden();
    void showRemoteHidden();
    void writeConfig();
    void readConfig();
    void localListClicked(QListViewItem *);
    void remoteListClicked(QListViewItem *);
    void localListPressed( int, QListViewItem *, const QPoint&, int);
    void remoteListPressed( int, QListViewItem *, const QPoint&, int);
    void localMakDir();
    void localDelete();
    void remoteMakDir();
    void remoteDelete();
/*     bool remoteDirList(const QString &); */
/*     bool remoteChDir(const QString &); */
    void tabChanged(QWidget*);
    void cleanUp();
    void remoteRename();
    void localRename();
    void runThis();
    void runText();
    void filePerms();
    void doProperties();
    void runCommand();
    void runCommandStd();
    QString getPath();
  void switchToLocalTab();
  void switchToRemoteTab();

protected:
    QGridLayout *tabLayout, *tabLayout_2, *tabLayout_3;
  QStringList remoteDirPathStringList, localDirPathStringList;
    
protected slots:
  void homeButtonPushed();
  void docButtonPushed();
  void SDButtonPushed();
  void CFButtonPushed();
  void upDir();
  void currentPathComboChanged();
  void copy();
  void copyAs();
  void currentPathComboActivated(const QString &);
  void fillCombo(const QString &);
  bool copyFile( const QString & , const QString & );
  void move();
  void fileStatus();
  void doAbout();
};

#endif // ADVANCEDFM_H
