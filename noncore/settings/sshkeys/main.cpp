/* 
 *  ssh-agent key manipulation utility
 *
 *  (C) 2002 David Woodhouse <dwmw2@infradead.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

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
	   This is to ensure that SSH uses ssh-askpass */
	fd = open("/dev/tty", O_RDONLY);
        if (fd != -1) {
                ioctl(fd, TIOCNOTTY, NULL);
                close(fd);
        }

	a.showMainWidget(&app);

	return a.exec();
}

