#include <qapplication.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include <qpe/qcopenvelope_qws.h>

#include <opie/odevice.h>

#include <stdio.h>

#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#ifdef USEPAM
extern "C" {
#include <security/pam_appl.h>
}
#else
#include <crypt.h>
#include <shadow.h>
#endif

#include "loginwindowimpl.h"
#include "inputmethods.h"

using namespace Opie;


LoginWindowImpl::LoginWindowImpl ( ) : LoginWindow ( 0, "LOGIN-WINDOW", WStyle_Customize | WStyle_NoBorder | WDestructiveClose )
{
	QPopupMenu *pop = new QPopupMenu ( this );
	pop-> insertItem ( tr( "Restart" ), this, SLOT( restart ( )));
	pop-> insertItem ( tr( "Quit" ), this, SLOT( quit ( )));
	m_menu-> setPopup ( pop );

	QHBoxLayout *lay = new QHBoxLayout ( m_taskbar, 4, 4 );
	m_input = new InputMethods ( m_taskbar );
	lay-> addWidget ( m_input );
	lay-> addStretch ( 10 );

	setActiveWindow ( );
	m_password-> setFocus ( );

	m_user-> insertStringList ( getAllUsers ( ));

	QTimer::singleShot ( 0, this, SLOT( showIM ( )));

	QString opiedir = ::getenv ( "OPIEDIR" );
	QPixmap bgpix ( opiedir + "/pics/launcher/opie-background.jpg" );

	if ( !bgpix. isNull ( ))
		setBackgroundPixmap ( bgpix );

	m_caption-> setText ( m_caption-> text ( ) + tr( "<center>%1 %2</center>" ). arg ( ODevice::inst ( )-> systemString ( )). arg ( ODevice::inst ( )-> systemVersionString ( )));
}

LoginWindowImpl::~LoginWindowImpl ( )
{
}

void LoginWindowImpl::keyPressEvent ( QKeyEvent *e )
{
	switch ( e-> key ( )) {
		case HardKey_Suspend:   suspend ( );
		                        break;
		case HardKey_Backlight: backlight ( );
		                        break;
		default:                e-> ignore ( );
		                        break;
	}
	LoginWindow::keyPressEvent ( e );
}


void LoginWindowImpl::toggleEchoMode ( bool t )
{
	m_password-> setEchoMode ( t ? QLineEdit::Normal : QLineEdit::Password );
}

QStringList LoginWindowImpl::getAllUsers ( )
{
	struct passwd *pwd;
	QStringList sl;

	while (( pwd = ::getpwent ( ))) {
		if (( pwd-> pw_uid == 0 ) || ( pwd-> pw_uid >= 500 && pwd-> pw_uid < 65534 ))
			sl << QString ( pwd-> pw_name );
	}

	::endpwent ( );

	return sl;
}

void LoginWindowImpl::showIM ( )
{
	m_input-> showInputMethod ( );
}

void LoginWindowImpl::restart ( )
{
	qApp-> quit ( );
}

void LoginWindowImpl::quit ( )
{
	qApp-> quit ( );
	::kill ( ::getppid ( ), SIGUSR1 );
}

void LoginWindowImpl::suspend ( )
{
	ODevice::inst ( )-> suspend ( );

	QCopEnvelope e("QPE/System", "setBacklight(int)");
	e << -3; // Force on
}

void LoginWindowImpl::backlight ( )
{
	QCopEnvelope e("QPE/System", "setBacklight(int)");
	e << -2; // toggle
}

#ifdef USEPAM

static const char *_PAM_SERVICE = "xdm";
static const char *PAM_password;

typedef const struct pam_message pam_message_type;

static int PAM_conv( int, pam_message_type **,  struct pam_response **,  void * );

static struct pam_conv PAM_conversation = {
  &PAM_conv,
  NULL
};

//----------------------------------------------------------------------------

static char *COPY_STRING( const char * s ) {
  return (s) ? strdup(s) : (char *)NULL;
}

#define GET_MEM if (reply) realloc(reply, size);\
                     else reply = (struct pam_response *)malloc(size); \
	             if (!reply) return PAM_CONV_ERR; \
	             size += sizeof(struct pam_response)


static int PAM_conv( int num_msg, pam_message_type **msg,
		     struct pam_response **resp,  void *)
{
  int count = 0, replies = 0;
  struct pam_response *reply = NULL;
  int size = sizeof(struct pam_response);

  for( count = 0; count < num_msg; count++ ) {
    switch (msg[count]->msg_style) {
    case PAM_PROMPT_ECHO_ON:
      /* user name given to PAM already */
      return PAM_CONV_ERR;

    case PAM_PROMPT_ECHO_OFF:
      /* wants password */
      GET_MEM;
      reply[replies].resp_retcode = PAM_SUCCESS;
      reply[replies].resp = COPY_STRING(PAM_password);
      replies++;
      /* PAM frees resp */
      break;
    case PAM_TEXT_INFO:
      break;
    default:
      /* unknown or PAM_ERROR_MSG */
      if (reply) free (reply);
      return PAM_CONV_ERR;
    }
  }
  if (reply) *resp = reply;
  return PAM_SUCCESS;
}


static bool pwcheck_PAM( const char *user, const char *password )
{
  bool pw_correct = false;
  int pam_error;
  int pam_return = 0;
  pam_handle_t *pamh = 0;
  PAM_password = password;

  pam_error = pam_start( _PAM_SERVICE, user, &PAM_conversation, &pamh );
  if( pam_error == PAM_SUCCESS ) {
    pam_error = pam_authenticate( pamh, 0 );
    if( pam_error == PAM_SUCCESS ) {
      //-- password correct
      pw_correct = true;
      pam_return = PAM_SUCCESS;
    } else {
      pam_return = pam_error;
    }
  } else {
    //  cerr << "PAM error: " << pam_strerror( pamh, pam_error ) << endl;
  }
  pam_end( pamh, pam_return );
  return pw_correct;
}

#else

//----------------------------------------------------------------------------

static bool pwcheck_Unix( const char *user, const char *pass )
{
	char *encrypted, *correct;
	struct passwd *pw;

	if ( !user || !pass )
		return false;

	pw = getpwnam ( user );

	if ( !pw )
		return false;

	if (( strcmp ( pw-> pw_passwd, "x" ) == 0 ) || ( strcmp ( pw-> pw_passwd, "*" ) == 0 )) {
		struct spwd *sp = getspnam ( pw-> pw_name );

		if ( !sp )
			return false;

		correct = sp-> sp_pwdp;
	}
	else
		correct = pw-> pw_passwd;

	if ( correct == 0 || correct[0] == '\0' )
		return true;

	encrypted = crypt ( pass, correct );
	return ( strcmp ( encrypted, correct ) == 0 );
}

#endif


bool LoginWindowImpl::changeIdentity ( const char *user )
{
	const char *DEFAULT_LOGIN_PATH      = "/bin:/usr/bin";
	const char *DEFAULT_ROOT_LOGIN_PATH = "/usr/sbin:/bin:/usr/bin:/sbin";

	bool fail = false;
	struct passwd *pw = getpwnam ( user );

	fail |= ( pw == 0 );
	fail |= ( initgroups ( pw-> pw_name, pw-> pw_gid ));
	endgrent ( );
	fail |= ( setgid ( pw-> pw_gid ));
	fail |= ( setuid ( pw-> pw_uid ));

	fail |= ( chdir ( pw-> pw_dir ) && chdir ( "/" ));

	fail |= ( setenv ( "HOME",    pw-> pw_dir, 1 ));
	fail |= ( setenv ( "SHELL",   pw-> pw_shell, 1 ));
	fail |= ( setenv ( "USER",    pw-> pw_name, 1 ));
	fail |= ( setenv ( "LOGNAME", pw-> pw_name, 1 ));
	fail |= ( setenv ( "PATH",    ( pw-> pw_uid ? DEFAULT_LOGIN_PATH : DEFAULT_ROOT_LOGIN_PATH ), 1 ));

	return !fail;
}

void LoginWindowImpl::login ( )
{
	const char *user = ::strdup ( m_user-> currentText ( ). local8Bit ( ));
	const char *pass = ::strdup ( m_password-> text ( ). local8Bit ( ));
	bool ok;

	if ( !user || !user [0] )
		return;
	if ( !pass )
		pass = "";

#if defined( USEPAM )
	ok = pwcheck_PAM ( user, pass );
#else
	ok = pwcheck_Unix ( user, pass );
#endif

	if ( ok ) {
		if ( changeIdentity ( user )) {
			char *opie = ::getenv ( "OPIEDIR" );
			char *arg = new char [::strlen ( opie ) + 8 + 1];

			::strcpy ( arg, opie );
			::strcat ( arg, "/bin/qpe" );

			// start qpe via a login shell
			::execl ( "/bin/sh", "-sh", "-c", arg, 0 );

			QMessageBox::critical ( this, tr( "Failure" ), tr( "Could not start OPIE\n(%1)." ). arg ( arg ));
			delete [] arg;

			restart ( );
		}
		else {
			QMessageBox::critical ( this, tr( "Failure" ), tr( "Could not switch to new user identity" ));
			restart ( );
		}
	}
	else {
		QMessageBox::warning ( this, tr( "Wrong password" ), tr( "The given password is incorrect." ));
		m_password-> clear ( );
	}
}
