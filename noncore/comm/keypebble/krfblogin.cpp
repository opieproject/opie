#include <assert.h>


extern "C" {
#include "vncauth.h"
}

#include "krfblogin.h"
#include "krfbconnection.h"
#include <qtimer.h>

// The length of the various messages (used to decide how many bytes to
// wait for).
const int ServerVersionLength = 12;
const int ClientVersionLength = 12;
const int AuthSchemeLength = 4;
const int FailureReasonSizeLength = 4;
const int ChallengeLength = 16;
const int AuthResultLength = 4;

// Authentication results
enum AuthResult {
  AuthOk,
  AuthFailed,
  AuthTooMany
};

typedef unsigned char CARD8;
typedef unsigned short CARD16;
typedef unsigned long CARD32;

const int endianTest = 1;

// Endian stuff
#define Swap16IfLE(s) \
    (*(char *)&endianTest ? ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)) : (s))

#define Swap32IfLE(l) \
    (*(char *)&endianTest ? ((((l) & 0xff000000) >> 24) | \
			     (((l) & 0x00ff0000) >> 8)  | \
			     (((l) & 0x0000ff00) << 8)  | \
			     (((l) & 0x000000ff) << 24))  : (l))

KRFBLogin::KRFBLogin( KRFBConnection *con )
  : QObject( con, "RFB login manager" )
{
  assert( con );
  this->con = con;
  currentState = AwaitingServerVersion;

  connect( this, SIGNAL( error( const QString & ) ),
	   con, SIGNAL( error( const QString & ) ) );

  connect( this, SIGNAL( passwordRequired( KRFBConnection * ) ),
	   con, SIGNAL( passwordRequired( KRFBConnection * ) ) );

  qWarning( "Waiting for server version..." );

  static QString statusMsg = tr( "Waiting for server version..." );
  emit status( statusMsg );

  // Kick off the state machine
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotServerVersion() ) );
  con->waitForData( ServerVersionLength );
}

KRFBLogin::~KRFBLogin()
{

}

KRFBLogin::State KRFBLogin::state() const
{
  return currentState;
}

void KRFBLogin::gotServerVersion()
{
  qWarning( "Got server version" );

  disconnect( con, SIGNAL( gotEnoughData() ), 
	      this, SLOT( gotServerVersion() ) );

  // Read the server's version message
  char serverVersion[ ServerVersionLength + 1 ];
  con->read( serverVersion, ServerVersionLength );
  serverVersion[ ServerVersionLength ] = '\0';

  QCString rfbString( serverVersion, ServerVersionLength + 1 );
  versionString = rfbString;

  QRegExp regexp( "RFB [0-9][0-9][0-9]\\.[0-9][0-9][0-9]\n" );

  if ( rfbString.find( regexp ) == -1 ) {
    static QString msg = tr( "Error: Invalid server version, %1" ).arg( rfbString );

    qWarning( msg );
    emit error( msg );
    currentState = Error;
    return;
  }

  // Calculate the actual version number
  serverMajor = (serverVersion[4] - '0') * 100
    + (serverVersion[5] - '0') * 10
    + (serverVersion[6] - '0');
  serverMinor = (serverVersion[8] - '0') * 100
    + (serverVersion[9] - '0') * 10
    + (serverVersion[10] - '0');

  qWarning("Server Version: %03d.%03d", serverMajor, serverMinor );

  if ( serverMajor != 3 ) {
    QString msg = tr( "Error: Unsupported server version, %1" )
      .arg( rfbString );

    qWarning( msg );
    emit error( msg );
    currentState = Error;
    return;    
  }
  
  if ( serverMinor != 3 ) {
    qWarning( "Minor version mismatch: %d", serverMinor );
  }

  // Setup for the next state
  sendClientVersion();

  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotAuthScheme() ) );
  con->waitForData( AuthSchemeLength );
}

void KRFBLogin::gotAuthScheme()
{
  disconnect( con, SIGNAL( gotEnoughData() ), 
	      this, SLOT( gotAuthScheme() ) );

  // Got data
  CARD32 scheme;
  con->read( &scheme, AuthSchemeLength );
  scheme = Swap32IfLE( scheme );

  static QString statusMsgOk = tr( "Logged in" );

  switch ( scheme ) {
  case 0:
    qWarning( "Failed" );
    // Handle failure
    connect( con, SIGNAL( gotEnoughData() ), SLOT( gotFailureReasonSize() ) );
    con->waitForData( FailureReasonSizeLength );
    break;
  case 1:
    // Handle no auth
    emit status( statusMsgOk );
    con->gotRFBConnection();
    break;
  case 2:
    // Handle VNC auth
    connect( con, SIGNAL( gotEnoughData() ), SLOT( gotChallenge() ) );
    con->waitForData( ChallengeLength );
    break;
  default:
    qWarning( "Unknown authentication scheme, 0x%08lx", scheme );
    currentState = Error;
    break;
  };
}

void KRFBLogin::gotChallenge()
{
  disconnect( con, SIGNAL( gotEnoughData() ),
	      this, SLOT( gotChallenge() ) );

  QTimer::singleShot( 0, this, SLOT(getPassword()) );
}

void KRFBLogin::getPassword()
{
  // Got data
  CARD8 challenge[ ChallengeLength ];
  con->read( challenge, ChallengeLength );

  // Last chance to enter a password
  if ( con->pass_.isNull() ) {
    qWarning( "krfblogin needs a password" );
    emit passwordRequired( con );
  }

  if ( con->pass_.isNull() ) {
    QString msg = tr( "Error: This server requires a password, but none "
			"has been specified.\n" );
    
    emit error( msg );
    return;
  }

  vncEncryptBytes( (unsigned char *) challenge, con->pass_.data() );
  con->write( challenge, ChallengeLength );

  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotAuthResult() ) );
  con->waitForData( AuthResultLength );
}

void KRFBLogin::gotFailureReasonSize()
{
  disconnect( con, SIGNAL( gotEnoughData() ), this,
	      SLOT( gotFailureReasonSize() ) );
}

void KRFBLogin::gotAuthResult()
{
  // Got data
  disconnect( con, SIGNAL( gotEnoughData() ), this,
	      SLOT( gotAuthResult() ) );

  long result;
  con->read( &result, AuthResultLength );
  result = Swap32IfLE( result );

  qWarning( "Authentication Result is 0x%08lx", result );

  static QString failed = tr( "Error: The password you specified was incorrect." );
  static QString tooMany = tr( "Error: Too many invalid login attempts have been made\n"
				 "to this account, please try later." );

  static QString statusMsgOk = tr( "Logged in" );
  static QString statusMsgFailed = tr( "Login Failed" );
  static QString statusMsgTooMany = tr( "Too many failures" );

  switch( result ) {
  case AuthOk:
    emit status( statusMsgOk );
    con->gotRFBConnection();
    break;
  case AuthFailed:
    qWarning( "Dammit" );
    emit status( statusMsgFailed );
    emit error( failed );
    break;
  case AuthTooMany:
    emit status( statusMsgTooMany );
    emit error( tooMany );    
    break;
  default:
    qWarning( "Invalid authentication result, %lx", result );
    break;
  }
}

void KRFBLogin::sendClientVersion()
{
  qWarning( "Sending client version" );
  con->write( (void*)"RFB 003.003\n", ClientVersionLength );
}
