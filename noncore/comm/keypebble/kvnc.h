// -*- c++ -*-

#ifndef KVNC_H
#define KVNC_H

#include <qmainwindow.h>
#include <qurl.h>

class QAction;
class KRFBCanvas;
class QPushButton;
class QToolBar;

/**
 * Top level window for Keystone.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id: kvnc.h,v 1.1 2002-01-25 22:14:52 kergoth Exp $
 */
class KVNC : public QMainWindow
{
  Q_OBJECT
public: 
  KVNC( const char *name = 0 );
  ~KVNC(); 

public slots:
  void toggleFullScreen();
  void openURL( const QUrl & );
  void closeConnection();
  void showOptions();

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
  QAction *connectAction;
};

#endif // KVNC_H
