
#include "sz_transfer.h"
#include <qfile.h>
#include <opie/oprocess.h>
#include <stdio.h>
#include <sys/termios.h>

/* following function ripped out of minicom's sysdep2.c */


/*
 * Set cbreak mode.
 * Mode 0 = normal.
 * Mode 1 = cbreak, no echo
 * Mode 2 = raw, no echo.
 * Mode 3 = only return erasechar (for wkeys.c)
 *
 * Returns: the current erase character.
 */  

static struct termios savetty;
int setcbreak(int mode)
{
#if 1
  struct termios tty;
  static int init = 0;
  static int erasechar;


  if (init == 0) {
	tcgetattr(0, &savetty);
	erasechar = savetty.c_cc[VERASE];
	init++;
  }

  if (mode == 3) return(erasechar);



  /* Always return to default settings first */
  tcsetattr(0, TCSADRAIN, &savetty);

  if (mode == 0) {
  	return(erasechar);
  }

  tcgetattr(0, &tty);
  if (mode == 1) {
	tty.c_oflag &= ~OPOST;
	tty.c_lflag &= ~(XCASE|ECHONL|NOFLSH);
  	tty.c_lflag &= ~(ICANON | ISIG | ECHO);
	tty.c_iflag &= ~(ICRNL|INLCR);
  	tty.c_cflag |= CREAD;
  	tty.c_cc[VTIME] = 5;
  	tty.c_cc[VMIN] = 1;
  }
  if (mode == 2) { /* raw */
  	tty.c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC | 
  		IXANY | IXON | IXOFF | INPCK | ISTRIP);
  	tty.c_iflag |= (BRKINT | IGNPAR);
	tty.c_oflag &= ~OPOST;
	tty.c_lflag &= ~(XCASE|ECHONL|NOFLSH);
  	tty.c_lflag &= ~(ICANON | ISIG | ECHO);
  	tty.c_cflag |= CREAD;
  	tty.c_cc[VTIME] = 5;
  	tty.c_cc[VMIN] = 1;
  }	
  tcsetattr(0, TCSADRAIN, &tty);
  return(erasechar);
#else
  struct sgttyb args;
  static int init = 0;
  static int erasechar;
  
  if (init == 0) {
	(void) ioctl(0, TIOCGETP, &savetty);
	(void) ioctl(0, TIOCGETC, &savetty2);
	erasechar = savetty.sg_erase;
	init++;
  }

  if (mode == 3) return(erasechar);

  if (mode == 0) {
  	(void) ioctl(0, TIOCSETP, &savetty);
	(void) ioctl(0, TIOCSETC, &savetty2);
  	return(erasechar);
  }

  (void) ioctl(0, TIOCGETP, &args);
  if (mode == 1) {
	args.sg_flags |= CBREAK;
	args.sg_flags &= ~(ECHO|RAW);
  }
  if (mode == 2) {
  	args.sg_flags |= RAW;
  	args.sg_flags &= ~(ECHO|CBREAK);
  }
  (void) ioctl(0, TIOCSETP, &args);
  return(erasechar);
#endif

}


SzTransfer::SzTransfer(Type t, IOLayer *layer) : FileTransferLayer(layer), m_t(t)
{
}

SzTransfer::~SzTransfer() {
}

void SzTransfer::sendFile(const QFile& file) {

    sendFile(file.name());
}

void SzTransfer::sendFile(const QString& file) {

    //setcbreak(2); /* raw no echo */

    proc = new OProcess;
    *proc << "sz";
    *proc << "-v" << "-v" << "-v" << "-b" << file;
    connect(proc, SIGNAL(processExited(OProcess *)), 
            this, SLOT(sent()));
    connect(proc, SIGNAL(receivedStdout(OProcess *, char *, int)), 
            this, SLOT(SzReceivedStdout(OProcess *, char *, int)));
    connect(proc, SIGNAL(receivedStderr(OProcess *, char *, int)),
            this, SLOT(SzReceivedStderr(OProcess *, char *, int)));
    connect(layer(), SIGNAL(received(const QByteArray &)),
            this, SLOT(receivedStdin(const QByteArray &)));
    proc->start(OProcess::NotifyOnExit, OProcess::All);

}

void SzTransfer::SzReceivedStdout(OProcess *, char *buffer, int buflen) {

    //qWarning("recieved from sz %d bytes", buflen);

    QByteArray data(buflen);
    data.fill(*buffer, buflen);

    // send out through the io layer
    (layer())->send(data);
}

void SzTransfer::SzReceivedStderr(OProcess *, char *buffer, int length) {

    // parse and show data in a progress dialog/widget
    printf("\n");
    for (int i = 0; i < length; i++)
        printf("%c", buffer[i]);
}

void SzTransfer::receivedStdin(const QByteArray &data) {

    //qWarning("recieved from io_serial %d bytes", data.size());
    
    // recieved data from the io layer goes to sz
    proc->writeStdin(data.data(), data.size());

}

void SzTransfer::sent() {

    qWarning("sent file");

    //setcbreak(0); /* default */


    delete proc;
    disconnect(layer(), SIGNAL(received(const QByteArray &)),
            this, SLOT(receivedStdin(const QByteArray &)));

}
