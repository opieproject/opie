/**********************************************************************
** Copyright (C) 2001 LISA Systems
**
** This file is an additional part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** For further information contact info@lisa.de 
**
**********************************************************************/

/*
 * AUTHOR: Christian Rahn
 * EMAIL: cdr@lisa.de
 *
 * $Id: qdmdialogimpl.cpp,v 1.1 2002-01-25 22:14:57 kergoth Exp $
 */

#include "qdm_config.h"

#ifdef QT_QWS_LOGIN

#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream.h>
#include <assert.h>

#include <qlabel.h>
#include <qregexp.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qtranslator.h>
#include <qpeapplication.h>

#include <qwsdisplay_qws.h>

#include <string.h>                
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#include <global.h>

#if defined(QT_QWS_LOGIN_USEPAM)
extern "C" {
#include <security/pam_appl.h>
}
#else
#define _XOPEN_SOURCE      
#include <unistd.h>
#include <crypt.h>
#endif


#include "qdmdialogimpl.h"
#include "../taskbar/inputmethods.h"


//----------------------------------------------------------------------------

//-- taken from semctl man page
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
//-- union semun is defined by including <sys/sem.h>
#else
//-- according to X/OPEN we have to define it ourselves
union semun {
  int val;                    // value for SETVAL
  struct semid_ds *buf;       // buffer for IPC_STAT, IPC_SET
  unsigned short int *array;  // array for GETALL, SETALL
  struct seminfo *__buf;      // buffer for IPC_INFO
};
#endif

//----------------------------------------------------------------------------

static const int ShowClockFreq = 1;

QDM_SHOWNUSERS;

#ifdef QT_QWS_LOGIN_USEPAM

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

#endif


//----------------------------------------------------------------------------

QDMDialogImpl::QDMDialogImpl( QWidget* parent, const char* name, bool modal, WFlags f )
   : QDMDialog( parent, name, modal, f )
{
  showTime();
  clockTimer = startTimer( ShowClockFreq * 1000 );	//-- call timer evry min.
  setActiveWindow();
  setFocus();
  
  input = new InputMethods( this );
  input->resize( input->sizeHint() );
  input->move( 0, height() - input->height() );

  for( int i=0; Shown_Users[i]; ++i ) {
    input_user->insertItem( Shown_Users[i] );
  }
  input_user->clearEdit();

  label_welcome->setText( QDM_WELCOME_STRING );

};



QDMDialogImpl::~QDMDialogImpl()
{
  input->lower();
  input->close( false );
  input->hide();
  delete input;
  input = 0;
  if( parent() ) {
    ((QWidget*)parent())->repaint(true);
  }
};


void QDMDialogImpl::accept () { };
void QDMDialogImpl::reject () { };


void QDMDialogImpl::showTime( void )
{
  label_date->setText( QDate::currentDate().toString() );
  label_time->setText( QTime::currentTime().toString() );
}


void QDMDialogImpl::timerEvent( QTimerEvent * e )
{
  if( e->timerId() == clockTimer )
    showTime();
}


//----------------------------------------------------------------------------

void QDMDialogImpl::slot_sleepmode()
{
  const int button = QMessageBox::warning( this, "Shutdown", tr( "Do you really want to go\nto sleep mode now?" ),
					   QString::null, tr( "Cancel" ), QString::null,0,1 );
  switch( button ) {
  case 0:
    done( Rejected );
    //    Global::execute( cmd_shutdown  );
    if( vfork() == 0 ) {
      execl( QDM_CMD_SLEEP, 0 );
      cerr << "Sleepmode:  " << strerror( errno ) << endl;
    }
    break;

  default:
    break;
  }
}


//----------------------------------------------------------------------------

void QDMDialogImpl::slot_shutdown()
{
  const int button = QMessageBox::warning( this, "Shutdown", tr("Do you really want to shut\nthe system down now?"),
					   QString::null, tr("Cancel"), QString::null,0,1 );
  switch( button ) {
  case 0:
    done( Rejected );
    //    Global::execute( cmd_shutdown );
    if( vfork() == 0 ) {
      execl( QDM_CMD_SHUTDOWN, 0 );
      cerr << "Shutdown:  " << strerror( errno ) << endl;
    }
    break;

  default:
    break;
  }
}




//----------------------------------------------------------------------------

void QDMDialogImpl::informBadPassword()
{
  QMessageBox::warning( this, tr("Password wrong"),
			tr("The given password is incorrect") );
}


//----------------------------------------------------------------------------

#if defined(QT_QWS_LOGIN_USEPAM)

static bool pwcheck_PAM( const char *user, const char *password )
{
  bool pw_correct = false;
  int pam_error;
  int pam_return = 0;
  pam_handle_t *pamh = 0;
  PAM_password = password.latin1();
  
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

static bool pwcheck_Unix( const char *user, const char *password )
{
  struct passwd * pword = getpwnam( user );
  if( pword  ) {
    if( strcmp( crypt(password, password), pword->pw_passwd) == 0 ) {
      return true;
    }
  }
  return false;
}

#endif



//----------------------------------------------------------------------------

void QDMDialogImpl::slot_login()
{
  bool pw_correct = false;
  const char *username = input_user->currentText().latin1();
  const char *password = input_password->text().latin1();
  
  assert( username );

#if defined(QT_QWS_LOGIN_USEPAM)
  pw_correct = pwcheck_PAM( username, password );
#else
  pw_correct = pwcheck_Unix( username, password );
#endif

  if( pw_correct ) {
    if( changePersona( username ) ) {
      //      cerr << "Password correct" << endl;
      done( Accepted );
      return;
    }
  } else {
    //    cerr << "Password incorrect" << endl;
  }
  informBadPassword();
}


//----------------------------------------------------------------------------

bool QDMDialogImpl::changePersona( const char *username )
{
  int err;

  //-- get some info on user <username>
  struct passwd * pword;
  pword = getpwnam( username );

  if( pword == 0 )
    return false;

  gid_t gid = pword->pw_gid;
  uid_t uid = pword->pw_uid;

  //-- some  very dirty hacks following
  //  extern int qws_display_id;
  extern QString qws_qtePipeFilename();
  extern QString qws_dataDir();


  const QString QTEdataDir = qws_dataDir();
  QString QTEdataDirNew = QTEdataDir;
  QTEdataDirNew.replace( QRegExp("root"), username );

  const char *qws_display_str = getenv("QWS_DISPLAY");

  //-- get name of semaphore and lock
  QString pipe = qws_qtePipeFilename();

  //-- change owner of semaphore
  key_t semkey = ftok( pipe.latin1(), 'd' );
  int semid = semget( semkey, 0, 0 );
  if( semid < 0 )
    cerr << "error: semget, " << strerror( errno ) << endl;

  struct shmid_ds shminfo;
  semun arg;
  semid_ds  semidds;
  arg.buf = & semidds;

  if( semctl( semid, 0, IPC_STAT, arg ) < 0 )
    cerr << "error: semctl stat, " << strerror( errno ) << endl;
  
  arg.buf->sem_perm.uid = uid;
  arg.buf->sem_perm.gid = gid;
  
  if( semctl( semid, 0, IPC_SET, arg ) < 0 )
    cerr << "error: semctl set, " << strerror( errno ) << endl;

  //-- change owner of shared memory
  key_t memkey =  ftok( pipe.latin1(), 'm' );
  int ramid = shmget( memkey, 0, 0 );
  if( ramid < 0 ) cerr << "error: shmget, " << strerror( errno ) << endl;

  if( shmctl( ramid, IPC_STAT, &shminfo ) < 0 )
    cerr << "error: shmctl stat, " << strerror( errno ) << endl;

  shminfo.shm_perm.uid = uid;
  shminfo.shm_perm.gid = gid;

  if( shmctl( ramid, IPC_SET, &shminfo ) < 0 )
    cerr << "error: shmctl set, " << strerror( errno ) << endl;

  //-- change owner of region manager
  memkey = ftok( pipe.latin1(), 'r' );
  int regionid = shmget( memkey, 0, 0 );
  if( regionid < 0 )
    cerr << "error: shmget, " << strerror( errno ) << endl;

  if( shmctl( regionid, IPC_STAT, &shminfo ) < 0 )
    cerr << "error: shmctl stat, " << strerror( errno ) << endl;

  shminfo.shm_perm.uid = uid;
  shminfo.shm_perm.gid = gid;

  if( shmctl( regionid, IPC_SET, &shminfo ) < 0 )
    cerr << "error: shmctl set, " << strerror( errno ) << endl;
  
  //  cerr << "ungrabbing qws display: " << qws_display_id << " on lock " << pipe << endl;
    //  QWSDisplay::ungrab();

  //-- presenting socket-file to new user
  chown( pipe.latin1(), uid, gid );
  chown( QTEdataDir.latin1(), uid, gid );

  
  //-- another dirty hack - force framebuffer to be writeable...
  struct stat devstat;
  if( ! stat( "/dev/fb0", &devstat ) ) {
    if( chmod( "/dev/fb0", devstat.st_mode |S_IWOTH |S_IWUSR |S_IWGRP ) ) {
      cerr << "chmod error: " << strerror( errno ) << endl;
    }
  }
  
  err = rename( QTEdataDir, QTEdataDirNew ) ;
  if( err < 0 )  cerr << "error: rename " << strerror(errno)
		      << " , " << QTEdataDir << " -> " << QTEdataDirNew << endl;

  // 
  //-- actually change uid and  gid
  //
  //  cerr << "changing persona, uid: " << uid  << "  gid: " << gid << endl;
  
  err = setgid( gid );
  if( err != 0 )  cerr << "error: gid changePersona " << err << endl;
  
  err = setuid( uid );
  if( err != 0 )  cerr << "error: uid changePersona " << err << endl;

  
  //-- set some environment
  setenv( "QWS_DISPLAY", qws_display_str,  true );
  setenv( "LOGNAME", username, true );
  setenv( "USER", username, true );
  setenv( "HOME", pword->pw_dir, true );

  //  cout << "QTE data dir: " << qws_dataDir() << endl;

  //-- I am reborn

  return true;
}



//----------------------------------------------------------------------------

bool QDMDialogImpl::login( QWidget *parent )
{
  //-- only when called as 'root' do login-box
  if( getuid() != 0 )
    return true;

  
  //-- are we coming from a 'restart' ?
  if( getenv("QDM_STARTED") )
    return true;
  else
    unsetenv("QDM_STARTED");
  

#ifndef QT_NO_TRANSLATION
  QString lang = getenv( "LANG" );

  QTranslator * trans = new QTranslator(qApp);
  QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/login.qm";
  if ( trans->load( tfn ))
    qApp->installTranslator( trans );
  else {
    delete trans;
    trans = 0;
  }
#endif  

  if( parent ) parent->erase();

  QDMDialog *dialog = new QDMDialogImpl( parent, "Login", true //);
					 ,WStyle_NoBorder | WStyle_Customize   );

#if QT_VERSION >= 300
  Q_CHECK_PTR( dialog );
#else
  CHECK_PTR( dialog );
#endif
  int result = dialog->exec();
  delete dialog;

  if( parent ) parent->erase();

#ifndef QT_NO_TRANSLATION
  if( trans ) {
    qApp->removeTranslator( trans );
    delete trans;
    trans = 0;
  }
#endif

  setenv( "QDM_STARTED", "", true );

  //  if( parent ) parent->erase();

  //-- get all configs going
  Global::restart();

  return result;
}


#endif //-- QT_QWS_LOGIN
