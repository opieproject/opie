/***************************************************************************
   advancedfm.h
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
#define QTOPIA_INTERNAL_FSLP // to get access to fileproperties
#define QT_QWS_OPIE

//#include <opie/otabwidget.h>
#include <opie/oprocess.h>

#include <qpe/ir.h>
#include <qpe/qcopenvelope_qws.h>

#include <qvariant.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qstring.h>
#include <qpoint.h>
#include <qtimer.h>
#include <qpixmap.h>

class OSplitter;
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
class QPopupMenu;
class QFile;
class QListViewItem;
class QLineEdit;
class MenuButton;

class QToolButton;
class Ir;

class AdvancedFm : public QMainWindow
{
    Q_OBJECT
public:
   AdvancedFm();
   ~AdvancedFm();
protected slots:
   void slotSwitchMenu(int);
   void selectAll();
   void addToDocs();
   void doDirChange();
   void mkDir();
   void del();
   void rn();
   void populateView();
   void rePopulate();
   void showHidden();
   void showMenuHidden();
   void writeConfig();
   void readConfig();
   void ListClicked(QListViewItem *);
   void ListPressed( int, QListViewItem *, const QPoint&, int);
   void makeDir();
   void doDelete();
   void tabChanged(QWidget*);
   void cleanUp();
   void renameIt();
   void runThis();
   void runText();
   void filePerms();
   void doProperties();
   void runCommand();
   void runCommandStd();
   QStringList getPath();
   void mkSym();
   void switchToLocalTab();
   void switchToRemoteTab();

protected:

   OSplitter *TabWidget;
   QCopChannel * channel;
   QPixmap unknownXpm;
   int whichTab;
//    QTabWidget *TabWidget;
   QWidget *tab, *tab_2, *tab_3;
   QListView *Local_View, *Remote_View;

   QLineEdit *currentPathEdit;
   QPopupMenu *fileMenu, *localMenu, *remoteMenu, *viewMenu /*, *customDirMenu*/;
   QToolButton  *homeButton, *docButton, *cdUpButton, *sdButton, *cfButton, *qpeDirButton;
//    QPushButton  *homeButton, *docButton, *cdUpButton, *sdButton, *cfButton, *qpeDirButton;
   QDir currentDir, currentRemoteDir;
   QComboBox *currentPathCombo;
   QString filterStr, s_addBookmark, s_removeBookmark;
   QListViewItem * item;
   bool b;
   QStringList fileSystemTypeList, fsList;
   int currentServerConfig;
   bool zaurusDevice;
   QGridLayout *tabLayout, *tabLayout_2, *tabLayout_3;
   QStringList remoteDirPathStringList, localDirPathStringList;
   QLineEdit *renameBox;

   void init();
   void initConnections();
   void keyReleaseEvent( QKeyEvent *);
   void keyPressEvent( QKeyEvent *);
   QString getFileSystemType(const QString &);
   QString getDiskSpace(const QString &);
   void parsetab(const QString &fileName);
   QString checkDiskSpace(const QString &);
   QString dealWithSymName(const QString &);
   QDir *CurrentDir();
   QDir *OtherDir();
   QListView *CurrentView();
   QListView *OtherView();
   void setOtherTabCurrent();
   void dealWithSchmooSchmaa(QWidget *);

protected slots:
   void openSearch();
   void dirMenuSelected(int);
   void showFileMenu();
   void cancelMenuTimer();
   void homeButtonPushed();
   void docButtonPushed();
   void SDButtonPushed();
   void CFButtonPushed();
   void QPEButtonPushed();
   void upDir();
   void currentPathComboChanged();
   void copy();
   void copyAs();
   void copySameDir();
   void currentPathComboActivated(const QString &);
   void fillCombo(const QString &);
   bool copyFile( const QString & , const QString & );
   void move();
   void fileStatus();
   void doAbout();
   void doBeam();
   void fileBeamFinished( Ir *);

//   void slotSwitchtoLocal(int);

private:
   MenuButton *menuButton;
   QString oldName;
   QTimer menuTimer;
   void startProcess(const QString &);
   bool eventFilter( QObject * , QEvent * );
   void cancelRename();
   void doRename(QListView *);
   void okRename();
   void customDirsToMenu();
   void addCustomDir();
   void removeCustomDir();
   void gotoDirectory(const QString &);
   void navigateToSelected();
   void findFile(const QString &);

private slots:
   void processEnded(OProcess *);
   void oprocessStderr(OProcess *, char *, int);
   void gotoCustomDir(const QString &);
   void qcopReceive(const QCString&, const QByteArray&);
   void setDocument(const QString &);
   
};

#endif // ADVANCEDFM_H
