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
#ifndef OPIEFTP_H
#define OPIEFTP_H

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
class QListViewItem;
class QLabel;
class QProgressBar;
class QSpinBox;
class QTabWidget;
class QWidget;
class QPEToolBar;
class QPEMenuBar;
class QPopupMenu;
class QFile;
class QLineEdit;
class QPushButton;
class QToolButton;
class QStringList;
class QListBox;
class QTimer;

class OpieFtp : public QMainWindow
{
    Q_OBJECT

public:
    OpieFtp(  );
    ~OpieFtp();

    QTabWidget *TabWidget;
    QWidget *tab, *tab_2, *tab_3;
    QListView *Local_View, *Remote_View;
    QListBox *serverListView;

    QComboBox *UsernameComboBox, *ServerComboBox, *currentPathCombo;
    QLineEdit *PasswordEdit, *remotePath;
    QLabel *TextLabel2, *TextLabel1, *TextLabel3, *TextLabel4;;
    QSpinBox* PortSpinBox;
    QPopupMenu *connectionMenu, *localMenu, *remoteMenu, *tabMenu, *aboutMenu;
    QDir currentDir;
    QString currentRemoteDir;
    QString filterStr;
    QListViewItem * item;
    QPushButton *connectServerBtn, *newServerButton;//
    QToolButton  *cdUpButton, *homeButton, *docButton;
    bool b;
    int currentServerConfig;
protected slots:
    void timerOut();
    void upDir();
    void homeButtonPushed();
    void docButtonPushed();
    void doAbout(); 

    void serverComboEdited(const QString & );
    void UsernameComboBoxEdited(const QString & );
    void PasswordEditEdited(const QString & );

    void showLocalMenu( QListViewItem *);
    void showRemoteMenu( QListViewItem *);
    void doLocalCd();
    void doRemoteCd();
    void localUpload();
    void remoteDownload();
    void newConnection();
    void connector();
    void disConnector();
    void populateLocalView();
    bool populateRemoteView();
    void showHidden();
    void writeConfig();
    void readConfig();
    void localListClicked(QListViewItem *);
    void remoteListClicked(QListViewItem *);
    void ListPressed( int, QListViewItem *, const QPoint&, int);
    void RemoteListPressed( int, QListViewItem *, const QPoint&, int);
    void localMakDir();
    void localDelete();
    void remoteMakDir();
    void remoteDelete();

    bool remoteDirList(const QString &);
    bool remoteChDir(const QString &);

    void tabChanged(QWidget*);
    void cleanUp();

    void remoteRename();
    void localRename();

    void currentPathComboChanged();
    void currentPathComboActivated(const QString &);
    void switchToLocalTab();
    void switchToRemoteTab();
    void switchToConfigTab();

    void fillCombos();
    void clearCombos();
    void fillRemoteCombo(const QString&);
    void fillCombo(const QString &);
    void serverComboSelected(int);
    void deleteServer();
    void connectorBtnToggled(bool);
    void NewServer();
    void serverListClicked( const QString &);
    
protected:
    bool fuckeduphack;
  QStringList remoteDirPathStringList, localDirPathStringList;
  QString newServerName;
    void nullifyCallBack();
    QGridLayout* tabLayout;
    QGridLayout* tabLayout_2;
    QGridLayout* tabLayout_3;
    
};

#endif // OPIEFTP_H
