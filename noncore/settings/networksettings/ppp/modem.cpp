/*
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id: modem.cpp,v 1.7.2.1 2003-07-15 15:29:08 tille Exp $
 *
 *              Copyright (C) 1997 Bernd Johannes Wuebben
 *                      wuebben@math.cornell.edu
 *
 * This file was added by Harri Porten <porten@tu-harburg.de>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <regex.h>
#include <qregexp.h>
#include <assert.h>
#include <string.h>

#ifdef HAVE_RESOLV_H
#  include <arpa/nameser.h>
#  include <resolv.h>
#endif

#ifndef _PATH_RESCONF
#define _PATH_RESCONF "/etc/resolv.conf"
#endif

#define strlcpy strcpy
#include "auth.h"
#include "modem.h"
#include "pppdata.h"
//#include <klocale.h>
#define i18n QObject::tr
#define qError qDebug
//#include <kdebug.h>
//#include <config.h>

#define MY_ASSERT(x)  if (!(x)) { \
        qFatal( "ASSERT: \"%s\" in %s (%d)\n",#x,__FILE__,__LINE__); \
        exit(1); }


static sigjmp_buf jmp_buffer;

//Modem *Modem::modem = 0;


const char* pppdPath() {
  // wasting a few bytes
  static char buffer[sizeof(PPPDSEARCHPATH)+sizeof(PPPDNAME)];
  static char *pppdPath = 0L;
  char *p;

  if(pppdPath == 0L) {
    const char *c = PPPDSEARCHPATH;
    while(*c != '\0') {
      while(*c == ':')
        c++;
      p = buffer;
      while(*c != '\0' && *c != ':')
        *p++ = *c++;
      *p = '\0';
      strcat(p, "/");
      strcat(p, PPPDNAME);
      if(access(buffer, F_OK) == 0)
        return (pppdPath = buffer);
    }
  }

  return pppdPath;
}


Modem::Modem( PPPData* pd )
{
    _pppdata = pd;
    modemfd = -1;
    _pppdExitStatus = -1;
    pppdPid = -1;
    sn = 0L;
    data_mode = false;
    modem_is_locked = false;
    lockfile[0] = '\0';
    device = "/dev/modem";
}


Modem::~Modem()
{
}


speed_t Modem::modemspeed() {
  // convert the string modem speed int the gpppdata object to a t_speed type
  // to set the modem.  The constants here should all be ifdef'd because
  // other systems may not have them
    int i = _pppdata->speed().toInt()/100;

  switch(i) {
  case 24:
    return B2400;
    break;
  case 96:
    return B9600;
    break;
  case 192:
    return B19200;
    break;
  case 384:
    return B38400;
    break;
#ifdef B57600
  case 576:
    return B57600;
    break;
#endif

#ifdef B115200
  case 1152:
    return B115200;
    break;
#endif

#ifdef B230400
  case 2304:
    return B230400;
    break;
#endif

#ifdef B460800
  case 4608:
    return B460800;
    break;
#endif

  default:
    return B38400;
    break;
  }
}

bool Modem::opentty() {
  //  int flags;

//begin  if((modemfd = Requester::rq->openModem(gpppdata.modemDevice()))<0) {
    close(modemfd);
    device = _pppdata->modemDevice();
    if ((modemfd = open(device, O_RDWR|O_NDELAY|O_NOCTTY)) == -1) {
        qDebug("error opening modem device !");
        errmsg = i18n("Unable to open modem.");
        return false;
    }
//bend  if((modemfd = Requester::rq->openModem(gpppdata.modemDevice()))<0) {
//}

#if 0
  if(_pppdata->UseCDLine()) {
    if(ioctl(modemfd, TIOCMGET, &flags) == -1) {
      errmsg = i18n("Unable to detect state of CD line.");
      ::close(modemfd);
      modemfd = -1;
      return false;
    }
    if ((flags&TIOCM_CD) == 0) {
      errmsg = i18n("The modem is not ready.");
      ::close(modemfd);
      modemfd = -1;
      return false;
    }
  }
#endif

  tcdrain (modemfd);
  tcflush (modemfd, TCIOFLUSH);

  if(tcgetattr(modemfd, &tty) < 0){
    // this helps in some cases
    tcsendbreak(modemfd, 0);
    sleep(1);
    if(tcgetattr(modemfd, &tty) < 0){
      errmsg = i18n("The modem is busy.");
      ::close(modemfd);
      modemfd = -1;
      return false;
    }
  }

  memset(&initial_tty,'\0',sizeof(initial_tty));

  initial_tty = tty;

  tty.c_cc[VMIN] = 0; // nonblocking
  tty.c_cc[VTIME] = 0;
  tty.c_oflag = 0;
  tty.c_lflag = 0;

  tty.c_cflag &= ~(CSIZE | CSTOPB | PARENB);
  tty.c_cflag |= CS8 | CREAD;
  tty.c_cflag |= CLOCAL;                   // ignore modem status lines
  tty.c_iflag = IGNBRK | IGNPAR /* | ISTRIP */ ;
  tty.c_lflag &= ~ICANON;                  // non-canonical mode
  tty.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHOKE);


  if(_pppdata->flowcontrol() != "None") {
      if(_pppdata->flowcontrol() == "CRTSCTS") {
      tty.c_cflag |= CRTSCTS;
    }
    else {
      tty.c_iflag |= IXON | IXOFF;
      tty.c_cc[VSTOP]  = 0x13; /* DC3 = XOFF = ^S */
      tty.c_cc[VSTART] = 0x11; /* DC1 = XON  = ^Q */
    }
  }
  else {
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF);
  }

  cfsetospeed(&tty, modemspeed());
  cfsetispeed(&tty, modemspeed());

  tcdrain(modemfd);

  if(tcsetattr(modemfd, TCSANOW, &tty) < 0){
    errmsg = i18n("The modem is busy.");
    ::close(modemfd);
    modemfd=-1;
    return false;
  }

  errmsg = i18n("Modem Ready.");
  return true;
}


bool Modem::closetty() {
  if(modemfd >=0 ) {
    stop();
    /* discard data not read or transmitted */
    tcflush(modemfd, TCIOFLUSH);

    if(tcsetattr(modemfd, TCSANOW, &initial_tty) < 0){
      errmsg = i18n("Can't restore tty settings: tcsetattr()\n");
      ::close(modemfd);
      modemfd = -1;
      return false;
    }
    ::close(modemfd);
    modemfd = -1;
  }

  return true;
}


void Modem::readtty(int) {
  char buffer[200];
  unsigned char c;
  int len;

  // read data in chunks of up to 200 bytes
  if((len = ::read(modemfd, buffer, 200)) > 0) {
    // split buffer into single characters for further processing
    for(int i = 0; i < len; i++) {
      c = buffer[i] & 0x7F;
      emit charWaiting(c);
    }
  }
}


void Modem::notify(const QObject *receiver, const char *member) {
  connect(this, SIGNAL(charWaiting(unsigned char)), receiver, member);
  startNotifier();
}


void Modem::stop() {
  disconnect(SIGNAL(charWaiting(unsigned char)));
  stopNotifier();
}


void Modem::startNotifier() {
  if(modemfd >= 0) {
    if(sn == 0) {
      sn = new QSocketNotifier(modemfd, QSocketNotifier::Read, this);
      connect(sn, SIGNAL(activated(int)), SLOT(readtty(int)));
      qDebug("QSocketNotifier started!");
    } else {
        qDebug("QSocketNotifier re-enabled!");
        sn->setEnabled(true);
    }
  }
}


void Modem::stopNotifier() {
  if(sn != 0) {
    sn->setEnabled(false);
    disconnect(sn);
    delete sn;
    sn = 0;
    qDebug( "QSocketNotifier stopped!" );
  }
}


void Modem::flush() {
  char c;
  while(read(modemfd, &c, 1) == 1);
}


bool Modem::writeChar(unsigned char c) {
  int s;
  do {
    s = write(modemfd, &c, 1);
    if (s < 0) {
      qError( "write() in Modem::writeChar failed" );
      return false;
    }
  } while(s == 0);

  return true;
}


bool Modem::writeLine(const char *buf) {
  int len = strlen(buf);
  char *b = new char[len+2];
  memcpy(b, buf, len);
  // different modems seem to need different line terminations
  QString term = _pppdata->enter();
  if(term == "LF")
    b[len++]='\n';
  else if(term == "CR")
    b[len++]='\r';
  else if(term == "CR/LF") {
    b[len++]='\r';
    b[len++]='\n';
  }
  int l = len;
  while(l) {
    int wr = write(modemfd, &b[len-l], l);
    if(wr < 0) {
      // TODO do something meaningful with the error code (or ignore it
      qError( "write() in Modem::writeLine failed" );
      delete[] b;
      return false;
    }
    l -= wr;
  }
  delete[] b;
  return true;
}


bool Modem::hangup() {
  // this should really get the modem to hang up and go into command mode
  // If anyone sees a fault in the following please let me know, since
  // this is probably the most imporant snippet of code in the whole of
  // kppp. If people complain about kppp being stuck, this piece of code
  // is most likely the reason.
  struct termios temptty;

  if(modemfd >= 0) {

    // is this Escape & HangupStr stuff really necessary ? (Harri)

    if (data_mode) escape_to_command_mode();

    // Then hangup command
    writeLine(_pppdata->modemHangupStr().local8Bit());

    usleep(_pppdata->modemInitDelay() * 10000); // 0.01 - 3.0 sec

#ifndef DEBUG_WO_DIALING
    if (sigsetjmp(jmp_buffer, 1) == 0) {
      // set alarm in case tcsendbreak() hangs
      signal(SIGALRM, alarm_handler);
      alarm(2);

      tcsendbreak(modemfd, 0);

      alarm(0);
      signal(SIGALRM, SIG_IGN);
    } else {
      // we reach this point if the alarm handler got called
      closetty();
      close(modemfd);
      modemfd = -1;
      errmsg = i18n("The modem does not respond.");
      return false;
    }

#ifndef __svr4__ // drops DTR but doesn't set it afterwards again. not good for init.
    tcgetattr(modemfd, &temptty);
    cfsetospeed(&temptty, B0);
    cfsetispeed(&temptty, B0);
    tcsetattr(modemfd, TCSAFLUSH, &temptty);
#else
    int modemstat;
    ioctl(modemfd, TIOCMGET, &modemstat);
    modemstat &= ~TIOCM_DTR;
    ioctl(modemfd, TIOCMSET, &modemstat);
    ioctl(modemfd, TIOCMGET, &modemstat);
    modemstat |= TIOCM_DTR;
    ioctl(modemfd, TIOCMSET, &modemstat);
#endif

    usleep(_pppdata->modemInitDelay() * 10000); // 0.01 - 3.0 secs

    cfsetospeed(&temptty, modemspeed());
    cfsetispeed(&temptty, modemspeed());
    tcsetattr(modemfd, TCSAFLUSH, &temptty);
#endif
    return true;
  } else
    return false;
}


void Modem::escape_to_command_mode() {
  // Send Properly bracketed escape code to put the modem back into command state.
  // A modem will accept AT commands only when it is in command state.
  // When a modem sends the host the CONNECT string, that signals
  // that the modem is now in the connect state (no long accepts AT commands.)
  // Need to send properly timed escape sequence to put modem in command state.
  // Escape codes and guard times are controlled by S2 and S12 values.
  //
  tcflush(modemfd, TCIOFLUSH);

  // +3 because quiet time must be greater than guard time.
  usleep((_pppdata->modemEscapeGuardTime()+3)*20000);
  QCString tmp = _pppdata->modemEscapeStr().local8Bit();
  write(modemfd, tmp.data(), tmp.length());
  tcflush(modemfd, TCIOFLUSH);
  usleep((_pppdata->modemEscapeGuardTime()+3)*20000);

  data_mode = false;
}


const QString Modem::modemMessage() {
  return errmsg;
}


QString Modem::parseModemSpeed(const QString &s) {
  // this is a small (and bad) parser for modem speeds
  int rx = -1;
  int tx = -1;
  int i;
  QString result;

  qDebug( "Modem reported result string: %s", s.latin1());

  const int RXMAX = 7;
  const int TXMAX = 2;
  QRegExp rrx[RXMAX] = {
    QRegExp("[0-9]+[:/ ]RX", false),
    QRegExp("[0-9]+RX", false),
    QRegExp("[/: -][0-9]+[/: ]", false),
    QRegExp("[/: -][0-9]+$", false),
    QRegExp("CARRIER [^0-9]*[0-9]+", false),
    QRegExp("CONNECT [^0-9]*[0-9]+", false),
    QRegExp("[0-9]+") // panic mode
  };

  QRegExp trx[TXMAX] = {
    QRegExp("[0-9]+[:/ ]TX", false),
    QRegExp("[0-9]+TX", false)
  };

  for(i = 0; i < RXMAX; i++) {
    int len, idx, result;
    if((idx = rrx[i].match(s,0,&len)) > -1) {
//    if((idx = rrx[i].search(s)) > -1) {
        //     len = rrx[i].matchedLength();

      //
      // rrx[i] has been matched, idx contains the start of the match
      // and len contains how long the match is. Extract the match.
      //
      QString sub = s.mid(idx, len);

      //
      // Now extract the digits only from the match, which will
      // then be converted to an int.
      //
      if ((idx = rrx[RXMAX-1].match( sub,0,&len )) > -1) {
//      if ((idx = rrx[RXMAX-1].search( sub )) > -1) {
//        len = rrx[RXMAX-1].matchedLength();
        sub = sub.mid(idx, len);
        result = sub.toInt();
        if(result > 0) {
          rx = result;
          break;
        }
      }
    }
  }

  for(i = 0; i < TXMAX; i++) {
    int len, idx, result;
    if((idx = trx[i].match(s,0,&len)) > -1) {
//    if((idx = trx[i].search(s)) > -1) {
//      len = trx[i].matchedLength();

      //
      // trx[i] has been matched, idx contains the start of the match
      // and len contains how long the match is. Extract the match.
      //
      QString sub = s.mid(idx, len);

      //
      // Now extract the digits only from the match, which will then
      // be converted to an int.
      //
      if((idx = rrx[RXMAX-1].match(sub,0,&len)) > -1) {
//      if((idx = rrx[RXMAX-1].search(sub)) > -1) {
//        len = rrx[RXMAX-1].matchedLength();
        sub = sub.mid(idx, len);
        result = sub.toInt();
        if(result > 0) {
          tx = result;
          break;
        }
      }
    }
  }

  if(rx == -1 && tx == -1)
    result = i18n("Unknown speed");
  else if(tx == -1)
    result.setNum(rx);
  else if(rx == -1) // should not happen
    result.setNum(tx);
  else
    result.sprintf("%d/%d", rx, tx);

  qDebug( "The parsed result is: %s", result.latin1());

  return result;
}


// Lock modem device. Returns 0 on success 1 if the modem is locked and -1 if
// a lock file can't be created ( permission problem )
int Modem::lockdevice() {
  int fd;
  char newlock[80]=""; // safe

  if(!_pppdata->modemLockFile()) {
    qDebug("The user doesn't want a lockfile.");
    return 0;
  }

  if (modem_is_locked)
    return 1;

  QString lockfile = LOCK_DIR"/LCK..";
  lockfile += _pppdata->modemDevice().mid(5); // append everything after /dev/

  if(access(QFile::encodeName(lockfile), F_OK) == 0) {
//    if ((fd = Requester::rq->
if ((fd = openLockfile(QFile::encodeName(lockfile), O_RDONLY)) >= 0) {
      // Mario: it's not necessary to read more than lets say 32 bytes. If
      // file has more than 32 bytes, skip the rest
      char oldlock[33]; // safe
      int sz = read(fd, &oldlock, 32);
      close (fd);
      if (sz <= 0)
        return 1;
      oldlock[sz] = '\0';

      qDebug( "Device is locked by: %s", oldlock);

      int oldpid;
      int match = sscanf(oldlock, "%d", &oldpid);

      // found a pid in lockfile ?
      if (match < 1 || oldpid <= 0)
        return 1;

      // check if process exists
      if (kill((pid_t)oldpid, 0) == 0 || errno != ESRCH)
        return 1;

      qDebug( "lockfile is stale" );
    }
  }

  fd = openLockfile(_pppdata->modemDevice(),O_WRONLY|O_TRUNC|O_CREAT);
  if(fd >= 0) {
    sprintf(newlock,"%010d\n", getpid());
    qDebug("Locking Device: %s", newlock);

    write(fd, newlock, strlen(newlock));
    close(fd);
    modem_is_locked=true;

    return 0;
  }

  return -1;

}


// UnLock modem device
void Modem::unlockdevice() {
  if (modem_is_locked) {
    qDebug( "UnLocking Modem Device" );
    close(modemfd);
    modemfd = -1;
    unlink(lockfile);
    lockfile[0] = '\0';
    modem_is_locked=false;
  }
}

int Modem::openLockfile( QString lockfile, int flags)
{
    int fd;
    int mode;
    flags = O_RDONLY;
    if(flags == O_WRONLY|O_TRUNC|O_CREAT)
        mode = 0644;
    else
        mode = 0;

    lockfile = LOCK_DIR;
    lockfile += "/LCK..";
    lockfile += device.right( device.length() - device.findRev("/") -1 );
    qDebug("lockfile >%s<",lockfile.latin1());
    // TODO:
    //   struct stat st;
    //   if(stat(lockfile.data(), &st) == -1) {
    //     if(errno == EBADF)
    //       return -1;
    //   } else {
    //     // make sure that this is a regular file
    //     if(!S_ISREG(st.st_mode))
    //       return -1;
    //   }
    if ((fd = open(lockfile, flags, mode)) == -1) {
        qDebug("error opening lockfile!");
        lockfile = QString::null;
        fd = open(DEVNULL, O_RDONLY);
    } else
        fchown(fd, 0, 0);
    return fd;
}



void alarm_handler(int) {
  //  fprintf(stderr, "alarm_handler(): Received SIGALRM\n");

  // jump
  siglongjmp(jmp_buffer, 1);
}


const char* Modem::authFile(Auth method, int version) {
  switch(method|version) {
  case PAP|Original:
    return PAP_AUTH_FILE;
    break;
  case PAP|New:
    return PAP_AUTH_FILE".new";
    break;
  case PAP|Old:
    return PAP_AUTH_FILE".old";
    break;
  case CHAP|Original:
    return CHAP_AUTH_FILE;
    break;
  case CHAP|New:
    return CHAP_AUTH_FILE".new";
    break;
  case CHAP|Old:
    return CHAP_AUTH_FILE".old";
    break;
  default:
    return 0L;
  }
}


bool Modem::createAuthFile(Auth method, const char *username, const char *password) {
  const char *authfile, *oldName, *newName;
  char line[100];
  char regexp[2*MaxStrLen+30];
  regex_t preg;

  if(!(authfile = authFile(method)))
    return false;

  if(!(newName = authFile(method, New)))
    return false;

  // look for username, "username" or 'username'
  // if you modify this RE you have to adapt regexp's size above
  snprintf(regexp, sizeof(regexp), "^[ \t]*%s[ \t]\\|^[ \t]*[\"\']%s[\"\']",
          username,username);
  MY_ASSERT(regcomp(&preg, regexp, 0) == 0);

  // copy to new file pap- or chap-secrets
  int old_umask = umask(0077);
  FILE *fout = fopen(newName, "w");
  if(fout) {
    // copy old file
    FILE *fin = fopen(authfile, "r");
    if(fin) {
      while(fgets(line, sizeof(line), fin)) {
        if(regexec(&preg, line, 0, 0L, 0) == 0)
           continue;
        fputs(line, fout);
      }
      fclose(fin);
    }

    // append user/pass pair
    fprintf(fout, "\"%s\"\t*\t\"%s\"\n", username, password);
    fclose(fout);
  }

  // restore umask
  umask(old_umask);

  // free memory allocated by regcomp
  regfree(&preg);

  if(!(oldName = authFile(method, Old)))
    return false;

  // delete old file if any
  unlink(oldName);

  rename(authfile, oldName);
  rename(newName, authfile);

  return true;
}


bool Modem::removeAuthFile(Auth method) {
  const char *authfile, *oldName;

  if(!(authfile = authFile(method)))
    return false;
  if(!(oldName = authFile(method, Old)))
    return false;

  if(access(oldName, F_OK) == 0) {
    unlink(authfile);
    return (rename(oldName, authfile) == 0);
  } else
    return false;
}


bool Modem::setSecret(int method, const char* name, const char* password)
{

    Auth auth;
    if(method == AUTH_PAPCHAP)
        return setSecret(AUTH_PAP, name, password) &&
            setSecret(AUTH_CHAP, name, password);

    switch(method) {
    case AUTH_PAP:
        auth = Modem::PAP;
        break;
    case AUTH_CHAP:
        auth = Modem::CHAP;
        break;
    default:
        return false;
    }

    return createAuthFile(auth, name, password);

}

bool Modem::removeSecret(int method)
{
   Auth auth;

    switch(method) {
    case AUTH_PAP:
        auth = Modem::PAP;
        break;
    case AUTH_CHAP:
        auth = Modem::CHAP;
        break;
    default:
        return false;
    }
    return removeAuthFile( auth );
}

int checkForInterface()
{
// I don't know if Linux needs more initialization to get the ioctl to
// work, pppd seems to hint it does.  But BSD doesn't, and the following
// code should compile.
#if (defined(HAVE_NET_IF_PPP_H) || defined(HAVE_LINUX_IF_PPP_H)) && !defined(__svr4__)
    int s, ok;
    struct ifreq ifr;
    //    extern char *no_ppp_msg;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return 1;               /* can't tell */

    strlcpy(ifr.ifr_name, "ppp0", sizeof (ifr.ifr_name));
    ok = ioctl(s, SIOCGIFFLAGS, (caddr_t) &ifr) >= 0;
    close(s);

    if (ok == -1) {
// This is ifdef'd FreeBSD, because FreeBSD is the only BSD that supports
// KLDs, the old LKM interface couldn't handle loading devices
// dynamically, and thus can't load ppp support on the fly
#ifdef __FreeBSD__
        // If we failed to load ppp support and don't have it already.
        if (kldload("if_ppp") == -1) {
            return -1;
        }
        return 0;
#else
        return -1;
#endif
    }
    return 0;
#else
// We attempt to use the SunOS/SysVr4 method and stat /dev/ppp
   struct stat buf;

   memset(&buf, 0, sizeof(buf));
   return stat("/dev/ppp", &buf);
#endif
}

bool Modem::execpppd(const char *arguments) {
  char buf[MAX_CMDLEN];
  char *args[MaxArgs];
  pid_t pgrpid;

  if(modemfd<0)
    return false;

  _pppdExitStatus = -1;

  switch(pppdPid = fork())
    {
    case -1:
      fprintf(stderr,"In parent: fork() failed\n");
      return false;
      break;

    case 0:
      // let's parse the arguments the user supplied into UNIX suitable form
      // that is a list of pointers each pointing to exactly one word
      strlcpy(buf, arguments);
      parseargs(buf, args);
      // become a session leader and let /dev/ttySx
      // be the controlling terminal.
      pgrpid = setsid();
#ifdef TIOCSCTTY
      if(ioctl(modemfd, TIOCSCTTY, 0)<0)
        fprintf(stderr, "ioctl() failed.\n");
#elif defined (TIOCSPGRP)
       if(ioctl(modemfd, TIOCSPGRP, &pgrpid)<0)
       fprintf(stderr, "ioctl() failed.\n");
#endif
      if(tcsetpgrp(modemfd, pgrpid)<0)
        fprintf(stderr, "tcsetpgrp() failed.\n");

      dup2(modemfd, 0);
      dup2(modemfd, 1);

      switch (checkForInterface()) {
        case 1:
          fprintf(stderr, "Cannot determine if kernel supports ppp.\n");
          break;
        case -1:
          fprintf(stderr, "Kernel does not support ppp, oops.\n");
          break;
        case 0:
          fprintf(stderr, "Kernel supports ppp alright.\n");
          break;
      }

      execve(pppdPath(), args, 0L);
      _exit(0);
      break;

    default:
      qDebug("In parent: pppd pid %d\n",pppdPid);
      close(modemfd);
      modemfd = -1;
      return true;
      break;
    }
}


bool Modem::killpppd() {
  if(pppdPid > 0) {
    qDebug("In killpppd(): Sending SIGTERM to %d\n", pppdPid);
    if(kill(pppdPid, SIGTERM) < 0) {
      qDebug("Error terminating %d. Sending SIGKILL\n", pppdPid);
      if(kill(pppdPid, SIGKILL) < 0) {
        qDebug("Error killing %d\n", pppdPid);
        return false;
      }
    }
  }
  return true;
}


void Modem::parseargs(char* buf, char** args) {
  int nargs = 0;
  int quotes;

  while(nargs < MaxArgs-1 && *buf != '\0') {

    quotes = 0;

    // Strip whitespace. Use nulls, so that the previous argument is
    // terminated automatically.

    while ((*buf == ' ' ) || (*buf == '\t' ) || (*buf == '\n' ) )
      *buf++ = '\0';

    // detect begin of quoted argument
    if (*buf == '"' || *buf == '\'') {
      quotes = *buf;
      *buf++ = '\0';
    }

    // save the argument
    if(*buf != '\0') {
      *args++ = buf;
      nargs++;
    }

    if (!quotes)
      while ((*buf != '\0') && (*buf != '\n') &&
	     (*buf != '\t') && (*buf != ' '))
	buf++;
    else {
      while ((*buf != '\0') && (*buf != quotes))
	buf++;
      *buf++ = '\0';
    }
  }

  *args = 0L;
}

bool Modem::execPPPDaemon(const QString & arguments)
{
  if(execpppd(arguments)) {
    _pppdata->setpppdRunning(true);
    return true;
  } else
    return false;
}

void Modem::killPPPDaemon()
{
  _pppdata->setpppdRunning(false);
  killpppd();
}

int Modem::pppdExitStatus()
{
    return _pppdExitStatus;
}

int Modem::openResolv(int flags)
{
    int fd;
    if ((fd = open(_PATH_RESCONF, flags)) == -1) {
        qDebug("error opening resolv.conf!");
        fd = open(DEVNULL, O_RDONLY);
    }
    return fd;
}

bool Modem::setHostname(const QString & name)
{
    return sethostname(name, name.length()) == 0;
}

