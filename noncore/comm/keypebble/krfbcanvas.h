// -*- c++ -*-

#ifndef KRFBCANVAS_H
#define KRFBCANVAS_H

#include <qscrollview.h>
#include <qurl.h>

class KRFBConnection;
class KRFBServer;

/**
 * Displays data from an KRFBDecoder, and sends events to the
 * KRFBConnection.
 */
class KRFBCanvas : public QScrollView
{
  Q_OBJECT
public:
  KRFBCanvas( QWidget *parent, const char *name=0 );
  ~KRFBCanvas();

  void setConnection( KRFBConnection * );
  KRFBConnection *connection() { return connection_; };

public slots:

  void openConnection (KRFBServer);
  void openURL( const QUrl & );
  void closeConnection();

  void refresh();
  void bell();
  void sendCtlAltDel(void);

protected:
  virtual void keyPressEvent( QKeyEvent * );
  virtual void keyReleaseEvent( QKeyEvent * );
  virtual void contentsMousePressEvent( QMouseEvent * );
  virtual void contentsMouseReleaseEvent( QMouseEvent * );
  virtual void contentsMouseMoveEvent( QMouseEvent * );

  virtual void viewportPaintEvent( QPaintEvent *e );

protected slots:
  void loggedIn();
  void viewportUpdate( int x, int y, int w, int h );
  void clipboardChanged();

private:
  KRFBConnection *connection_;
  QString password;
  bool loggedIn_;
};

#endif // KRFBCANVAS_H
