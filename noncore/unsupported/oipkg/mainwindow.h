#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qaction.h>
#include <qpopupmenu.h>

#include "packagelist.h"
#include "pmipkg.h"
#include "pksettings.h"
#include "pkdesc.h"

class QComboBox;
class QPEToolBar;
class QLineEdit;
class QListView;


class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
  MainWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~MainWindow();

protected slots:
  void runIpkg();
  void getList();
  void updateList();
  void displayList();
  void subSectionChanged();
  void sectionChanged();
  void showSettings();
  void showSettingsSrv();
  void showSettingsDst();

public slots:
  void showDetails();
  void toggleActivePackage();
  void setCurrent( QListViewItem* );
  void sectionClose();
  void sectionShow(bool);
  void findClose();
  void findShow(bool);
  void filterList();

private:
  void makeMenu();
  void setSections();
  void setSubSections();

  PmIpkg* ipkg;
  PackageManagerSettings *settings;
  PackageDetails* details;
  PackageList packageList;
  Package *activePackage;
  QAction *runAction;
  QAction *detailsAction;
  QAction *updateAction;
  QAction *findAction;
  QAction *sectionAction;
  QListView *listViewPackages;
  QPopupMenu *contextMenu;
  QPEToolBar *findBar;
  QLineEdit *findEdit;
  QPEToolBar *sectionBar;
  QComboBox *section;
  QComboBox *subsection;
  QPopupMenu *popupMenu;
};

#endif
