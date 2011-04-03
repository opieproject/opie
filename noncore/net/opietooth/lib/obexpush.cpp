/* $Id: obexpush.cpp,v 1.2 2008-08-26 21:27:00 paule Exp $ */
/* OBEX push functions implementation */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "obexpush.h"

#include <qfileinfo.h>
#include <qfile.h>
#include <qstringlist.h>

#include <opie2/odebug.h>

using namespace Opie::Core;
using namespace OpieTooth;

ObexPush::ObexPush()
{
    pushProc = new OProcess();
    connect(pushProc, SIGNAL(processExited(Opie::Core::OProcess*)),
        this, SLOT(slotPushExited(Opie::Core::OProcess*)));
    connect(pushProc, SIGNAL(receivedStdout(Opie::Core::OProcess*, char*, int)),
        this, SLOT(slotPushOut(Opie::Core::OProcess*, char*, int)));
    connect(pushProc, SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int)),
        this, SLOT(slotPushErr(Opie::Core::OProcess*, char*, int)));

}

ObexPush::~ObexPush()
{
    if (pushProc->isRunning())
        pushProc->kill();
    delete pushProc;
    pushProc = NULL;
}

/**
 * Function that sends a file
 * @param mac destination device MAC address
 * @param port service port number
 * @param src source file name
 * @param dst destination file name
 * @return 0 on success, -1 on error, 1 if sending process is running
 */
int ObexPush::send(QString& mac, int port, QString& src, QString& dst)
{
    if (pushProc->isRunning())
        return 1;
    pushProc->clearArguments();
    *pushProc << "obexftp" << "-b" << mac << "-B" << QString::number(port) 
            << "-H" << "-S" << "-U" << "none" << "-p" << QFile::encodeName(src);

    const QValueList<QCString> &args = pushProc->args();
    QString argStr;
    for ( QValueList<QCString>::ConstIterator it = args.begin(); it != args.end(); ++it ) {
        argStr += (*it) + " ";
    }
    odebug << argStr << oendl;

    if (!pushProc->start(OProcess::NotifyOnExit, OProcess::All))
        return -1;
    else
        return 0;
}

void ObexPush::slotPushExited(Opie::Core::OProcess* proc)
{
    if (pushProc != proc)
        return;
    odebug << "Process exited" << oendl;
    if (pushProc->normalExit()) {
        int ret = pushProc->exitStatus();
        if( ret == 255 )  // apparently this means success for obexftp (!)
            ret = 0;
        emit sendComplete(ret);
    }
    else
        emit sendError(-1);
}

/**
 * Function makes a notification from slotPushOut and slotPushErr
 */
void ObexPush::notifyInfo(Opie::Core::OProcess* proc, char* buf, int len)
{
    if (proc != pushProc)
        return;
    QCString str(buf, len);
    odebug << str << oendl;
    emit status(str);
}

void ObexPush::slotPushOut(Opie::Core::OProcess* proc, char* buf, int len)
{
    notifyInfo(proc, buf, len);
}

void ObexPush::slotPushErr(Opie::Core::OProcess* proc, char* buf, int len)
{
    notifyInfo(proc, buf, len);
}

//eof
