// -*- c++ -*-

#ifndef KRFBLOGIN_H
#define KRFBLOGIN_H

#include <qobject.h>

class KRFBConnection;

/**
 * Handles logging into the RFB server.
 *
 * @version $Id: krfblogin.h,v 1.1 2002-01-25 22:14:52 kergoth Exp $
 * @author Richard Moore, rich@kde.org
 */
class KRFBLogin : QObject
{
  Q_OBJECT

public:
  enum State {
    AwaitingServerVersion,
    AwaitingAuthScheme,
    AwaitingChallenge,
    WaitingForResponse,
    Authenticated,
    Ready,
    Failed,
    Error
  };

  KRFBLogin( KRFBConnection *con );
  ~KRFBLogin();

  KRFBLogin::State state() const;

protected:
  void sendClientVersion();

protected slots:
  void getPassword();
  void gotServerVersion();
  void gotAuthScheme();
  void gotChallenge();
  void gotAuthResult();
  void gotFailureReasonSize();

signals:
  void passwordRequired( KRFBConnection * );
  void error( const QString & );
  void status( const QString & );

private:
  KRFBConnection *con;
  State currentState;
  QCString versionString;
  int serverMajor;
  int serverMinor;
};

#endif // KRFBLOGIN_H

