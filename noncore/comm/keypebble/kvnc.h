// -*- c++ -*-

#ifndef KVNC_H
#define KVNC_H

#include <qmainwindow.h>
#include <qurl.h>
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
 * @version $Id: kvnc.h,v 1.2 2002-09-04 17:53:13 treke Exp $
 */
class KVNC : public QMainWindow
{
  Q_OBJECT
public: 
  KVNC( const char *name = 0 );
  ~KVNC(); 

public slots:
  void newConnection();
  void deleteBookmark();
  void openConnection(QListBoxItem *);
  void openConnection(QString);
  void openConnection(void);
  void toggleFullScreen();
  void closeConnection();

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

	KVNCBookmarkDlg * bookmarkSelector;
	QWidgetStack * stack;
	KRFBServer * curServer;
};

#endif // KVNC_H
