/* -------------------------------------------------------------------------- */
/*                                                                            */
/* [MyPty.C]               Pseudo Terminal Device                             */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole - an X terminal for KDE                       */
/* -------------------------------------------------------------------------- */
/*                        */
/* Ported Konsole to Qt/Embedded                                              */
/*                        */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*                        */
/* -------------------------------------------------------------------------- */

/* If you're compiling konsole on non-Linux platforms and find
   problems that you can track down to this file, please have
   a look into ../README.ports, too.
*/

/*! \file
*/

/*! \class TEPty

    \brief Ptys provide a pseudo terminal connection to a program.

    Although closely related to pipes, these pseudo terminal connections have
    some ability, that makes it nessesary to uses them. Most importent, they
    know about changing screen sizes and UNIX job control.

    Within the terminal emulation framework, this class represents the
    host side of the terminal together with the connecting serial line.

    One can create many instances of this class within a program.
    As a side effect of using this class, a signal(2) handler is
    installed on SIGCHLD.

    \par FIXME

    [NOTE: much of the technical stuff below will be replaced by forkpty.]

    publish the SIGCHLD signal if not related to an instance.

    clearify TEPty::done vs. TEPty::~TEPty semantics.
    check if pty is restartable via run after done.

    \par Pseudo terminals

    Pseudo terminals are a unique feature of UNIX, and always come in form of
    pairs of devices (/dev/ptyXX and /dev/ttyXX), which are connected to each
    other by the operating system. One may think of them as two serial devices
    linked by a null-modem cable. Being based on devices the number of
    simultanous instances of this class is (globally) limited by the number of
    those device pairs, which is 256.

    Another technic are UNIX 98 PTY's. These are supported also, and prefered
    over the (obsolete) predecessor.

    There's a sinister ioctl(2), signal(2) and job control stuff
    nessesary to make everything work as it should.
*/


#include <qapplication.h>
#include <qsocketnotifier.h>
#include <qstring.h>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#ifdef HAVE_OPENPTY
#include <pty.h>
#endif

#include "MyPty.h"


#undef VERBOSE_DEBUG


/* -------------------------------------------------------------------------- */

/*!
   Informs the client program about the
   actual size of the window.
*/

void MyPty::setSize(int lines, int columns)
{
  struct winsize wsize;
  wsize.ws_row = (unsigned short)lines;
  wsize.ws_col = (unsigned short)columns;
  if(fd < 0) return;
  ioctl(fd,TIOCSWINSZ,(char *)&wsize);
}


void MyPty::donePty()
{
    // This is code from the Qt DumbTerminal example
    int status = 0;

    ::close(fd);

    if (cpid) {
  kill(cpid, SIGHUP);
  waitpid(cpid, &status, 0);
    }

    emit done(status);
}


const char* MyPty::deviceName()
{
    return ttynam;
}


void MyPty::error()
{
    // This is code from the Qt DumbTerminal example
    donePty();
}


/*!
    start the client program.
*/
int MyPty::run(const char* cmd, QStrList &, const char*, int)
{
      // This is code from the Qt DumbTerminal example
    cpid = fork();

    if ( !cpid ) {
  // child - exec shell on tty
  for (int sig = 1; sig < NSIG; sig++) signal(sig,SIG_DFL);
  int ttyfd = open(ttynam, O_RDWR);
  dup2(ttyfd, STDIN_FILENO);
  dup2(ttyfd, STDOUT_FILENO);
  dup2(ttyfd, STDERR_FILENO);
  // should be done with tty, so close it
  close(ttyfd);
  static struct termios ttmode;
  if ( setsid() < 0 )
      perror( "failed to set process group" );
#if defined (TIOCSCTTY)
  // grabbed from APUE by Stevens
  ioctl(STDIN_FILENO, TIOCSCTTY, 0);
#endif
  tcgetattr( STDIN_FILENO, &ttmode );
  ttmode.c_cc[VINTR] = 3;
  ttmode.c_cc[VERASE] = 8;
  tcsetattr( STDIN_FILENO, TCSANOW, &ttmode );
  setenv("TERM","vt100",1);
  setenv("COLORTERM","0",1);

  if (getuid() == 0) {
      char msg[] = "WARNING: You are running this shell as root!\n";
      write(ttyfd, msg, sizeof(msg));
  }
; //creates a login shell
  execl(cmd, cmd, "--login", 0);

  donePty();
  exit(-1);
    }

    // parent - continue as a widget
    QSocketNotifier* sn_r = new QSocketNotifier(fd,QSocketNotifier::Read,this);
    QSocketNotifier* sn_e = new QSocketNotifier(fd,QSocketNotifier::Exception,this);
    connect(sn_r,SIGNAL(activated(int)),this,SLOT(readPty()));
    connect(sn_e,SIGNAL(activated(int)),this,SLOT(error()));

    return 0;
}

int MyPty::openPty()
{
    // This is code from the Qt DumbTerminal example
    int ptyfd = -1;

#ifdef HAVE_OPENPTY
    int ttyfd;
    if ( openpty(&ptyfd,&ttyfd,ttynam,0,0) )
  ptyfd = -1;
    else
  close(ttyfd); // we open the ttynam ourselves.
#else
    for (const char* c0 = "pqrstuvwxyzabcde"; ptyfd < 0 && *c0 != 0; c0++) {
  for (const char* c1 = "0123456789abcdef"; ptyfd < 0 && *c1 != 0; c1++) {
      sprintf(ptynam,"/dev/pty%c%c",*c0,*c1);
      sprintf(ttynam,"/dev/tty%c%c",*c0,*c1);
      if ((ptyfd = ::open(ptynam,O_RDWR)) >= 0) {
    if (geteuid() != 0 && !access(ttynam,R_OK|W_OK) == 0) {
        ::close(ptyfd);
        ptyfd = -1;
    }
      }
  }
    }
#endif

    if ( ptyfd < 0 ) {
  qApp->exit(1);
  return -1;
    }

    return ptyfd;
}

/*!
    Create an instance.
*/
MyPty::MyPty() : cpid(0)
{
  fd = openPty();
}

/*!
    Destructor.
    Note that the related client program is not killed
    (yet) when a instance is deleted.
*/
MyPty::~MyPty()
{
    donePty();
}


/*! sends len bytes through the line */
void MyPty::send_bytes(const char* s, int len)
{

#ifdef VERBOSE_DEBUG
  // verbose debug
  printf("sending bytes:\n");
  for (int i = 0; i < len; i++)
    printf("%c", s[i]);
  printf("\n");
#endif

  ::write(fd, s, len);  
}

/*! indicates that a block of data is received */
void MyPty::readPty()
{ 
  char buf[4096];

  int len = ::read( fd, buf, 4096 );

  if (len == -1)
     donePty();

  if (len < 0)
     return;

  emit block_in(buf,len);
  
#ifdef VERBOSE_DEBUG
  // verbose debug
  printf("read bytes:\n");
  for (int i = 0; i < len; i++)
    printf("%c", buf[i]);
  printf("\n");
#endif

}

