// -*- c++ -*-

#ifndef KRFBCONNECTION_H
#define KRFBCONNECTION_H

#include <qobject.h>
#include <qstring.h>
#include <qcstring.h>
#include <qurl.h>

class KRFBLogin;
class KRBUpdateHandler;
class KRFBOptions;
class QSocket;
class KRFBDecoder;
class KRFBBuffer;
class QTimer;

/**
 * Encapsulates the RFB socket.
 *
 */
class KRFBConnection : public QObject
{
  Q_OBJECT

public:
  friend class KRFBLogin;
  friend class KRFBDecoder;

  //* The state of the connection.
  enum State {
    Connecting,
    LoggingIn,
    Connected,
    Disconnecting,
    Disconnected,
    Error
  };

  KRFBConnection( QObject *parent = 0 );
  ~KRFBConnection();

  //* Get the state of a connection.
  State state() const;

  //* Get the options for this connection
  KRFBOptions *options() const { return options_; };

  KRFBBuffer *buffer() const { return buffer_; };

  KRFBDecoder *decoder() const { return decoder_; };

  //* Set the base from which the port for a given display will be calculated.
  void setPortBase( int base );

  //* Get the base from which the port for a given display is calculated.
  int portBase() const;

  //* Set the password which will be used to login
  void setPassword( const QCString &pass );

  //* Open a connection
  void connectTo( const QCString &host, int display );

  //* Close the connection
  void disconnect();

  //* Get the host
  const QCString host() const { return host_; };

  //* Get the display
  int display() const { return display_; };

  //* Get the current host/display as a URL
  const QUrl &url();

  //* Reload the display
  void refresh();

  //* Send text to the remote clipboard
  void sendCutText( const QString & );

protected slots:
  //* When the shit hits the fan
  void gotSocketError( int );

  //* When we have an open socket
  void gotSocketConnection();

  //* When we have logged in
  void gotRFBConnection();

  //* When some more data arrived
  void gotMoreData();

  void updateTimer();

  void disconnectDone();

signals:
  //* Emitted when the status of the connection changes.
  void statusChanged( const QString & );

  /**
   * Emitted when we *really* need a password. If the password
   * was specified before you tried to connect then you won't
   * see this.
   */
  void passwordRequired( KRFBConnection * );

  //* When we have a working RFB connection
  void connected();

  void loggedIn();

  void disconnected();

  //* What happened?
  void error( const QString &msg );

  //* Emitted in response to a waitForData() call.
  void gotEnoughData();

private:
  //
  // The following are called by our friends.
  //

  void waitForData( unsigned int );

  int read( void *buf, int sz );
  int write( void *buf, int sz );

private:
  QCString host_;
  int portBase_;
  int display_;
  QCString pass_;
  QSocket *sock;
  State currentState_;
  unsigned int minData_;
  QTimer *updater;
  KRFBLogin *login;
  KRFBDecoder *decoder_;
  KRFBOptions *options_;
  KRFBBuffer *buffer_;
  QUrl url_;
};

#endif // KRFBCONNECTION_H

