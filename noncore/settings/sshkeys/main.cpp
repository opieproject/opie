
#include <qpe/qpeapplication.h>
#include "sshkeys.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	QPEApplication a(argc, argv);
	SSHKeysApp app;
        int fd;

	/* If we had a controlling TTY, detach from it. 
	   This is to ensure the SSH uses ssh-askpass */
	fd = open("/dev/tty", O_RDONLY);
        if (fd != -1) {
                ioctl(fd, TIOCNOTTY, NULL);
                close(fd);
        }

	a.showMainWidget(&app);

	return a.exec();
}

