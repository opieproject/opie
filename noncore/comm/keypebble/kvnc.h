// -*- c++ -*-

#ifndef KVNC_H
#define KVNC_H

#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qwidgetstack.h>
#include "kvncbookmarkdlg.h"

class QAction;
class KRFBCanvas;
class QPushButton;
class QToolBar;

/**
 * Top level window for Keystone.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id: kvnc.h,v 1.5 2003-10-27 19:06:54 mickeyl Exp $
 */
class KVNC : public QMainWindow
{
  Q_OBJECT
public:
  KVNC( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~KVNC();

public slots:
  void newConnection();
  void deleteBookmark();
  void openConnection(QListBoxItem *);
  void openConnection(QString);
  void openConnection(void);
  void toggleFullScreen();
  void closeConnection();
  static QString appName() { return QString::fromLatin1("keypebble"); }

protected:
  void setupActions();

protected slots:
  void showMenu();

  void connected();
  void loggedIn();
  void disconnected();
  void statusMessage( const QString & );
  void error( const QString & );

private:
  bool fullscreen;
  KRFBCanvas *canvas;
  QPopupMenu *cornerMenu;
  QPushButton *cornerButton;
  QAction *fullScreenAction;
  QAction *optionsAction;
  QAction *disconnectAction;
  QAction *ctlAltDelAction;;
  QAction *connectAction;
  QAction *rightClickAction;
  QAction *doubleClickAction;
	QToolBar * bar;

	KVNCBookmarkDlg * bookmarkSelector;
	QWidgetStack * stack;
	KRFBServer * curServer;
};

#endif // KVNC_H
