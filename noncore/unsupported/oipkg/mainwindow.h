#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qaction.h>
#include <qtimer.h>
#include <qpopupmenu.h>

#include "packagelist.h"
#include "pmipkg.h"
#include "pksettings.h"
#include "packagelistview.h"

class QComboBox;
class QPEToolBar;
class QLineEdit;
class PackageListItem;
class QCopChannel;
class QMessageBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
  MainWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~MainWindow();

	QCopChannel *channel;

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
  void sectionClose();
  void sectionShow(bool);
  void findClose();
  void findShow(bool);
  void filterList();
	void receive (const QCString &, const QByteArray &);
	void setDocument (const QString &);

private:
  void makeMenu();
  void setSections();
  void setSubSections();
	void installFile(const QString &);
  bool updateIcon;

  PmIpkg* ipkg;
  PackageManagerSettings *settings;
  PackageList packageList;
  QAction *runAction;
  QAction *detailsAction;
  QAction *updateAction;
  QAction *findAction;
  QAction *sectionAction;
  PackageListView *listViewPackages;
  QPEToolBar *findBar;
  QLineEdit *findEdit;
  QPEToolBar *sectionBar;
  QComboBox *section;
  QComboBox *subsection;
  QMessageBox *wait;
private slots:
  void rotateUpdateIcon();
};

#endif
