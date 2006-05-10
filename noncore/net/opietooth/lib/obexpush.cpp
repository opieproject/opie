/* $Id: obexpush.cpp,v 1.1 2006-05-10 13:32:46 korovkin Exp $ */
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

#include <opie2/odebug.h>
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
    QString dev = mac;
    QString execName = "ussp-push";
    if (pushProc->isRunning())
        return 1;
    pushProc->clearArguments();
    dev += "@";
    dev += QString::number(port); 
    if (!dst.isEmpty())
        *pushProc << execName << "--timeo 30" << dev 
            << QFile::encodeName(src) << QFile::encodeName(dst);
    else
        *pushProc << execName << "--timeo 30" << dev 
            << QFile::encodeName(src) 
            << QFile::encodeName(QFileInfo(src).fileName());
    odebug << execName << " " << dev << " " << src << " " 
        << ((!dst.isEmpty())? dst: QFileInfo(src).fileName()) << oendl;
    pushProc->setUseShell(true);
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
    if (pushProc->normalExit())
        emit sendComplete(pushProc->exitStatus());
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
