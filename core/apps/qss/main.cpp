/**********************************************************************
** Copyright (C) 2000-2005 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
**
** A copy of the GNU GPL license version 2 is included in this package as
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qsoundqss_qws.h>
#include <qtopia/qpeapplication.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    (void)new QWSSoundServer(0);

    //if (1) {
    // later, check if root first
	setpriority(PRIO_PROCESS, 0, -15);
    //}
    // hook up some quit mech.
    return a.exec();
}
