/* $Id: hidpopup.cpp,v 1.1 2006-07-06 16:43:36 korovkin Exp $ */
/* PAN context menu */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qpe/qcopenvelope_qws.h>
#include <qmessagebox.h>
#include <opie2/odebug.h>
#include <opie2/oprocess.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;

#include <qtimer.h>
#include "hidpopup.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
HidPopup::HidPopup(const OpieTooth::Services&, OpieTooth::BTDeviceItem* item) :
    QPopupMenu()  {

    owarn << "HidPopup c'tor" << oendl;

    m_item = item;
    QAction* a;
    QAction* c;

    m_hidConnect = 0l;
    /* connect action */

    a = new QAction(); // so it's get deleted
    a->setText( tr("connect") );
    a->addTo( this );
    connect( a, SIGNAL( activated() ), this, SLOT( slotConnect() ) );

    c = new QAction();
    c->setText( tr( "disconnect" ) );
    c->addTo( this );
    connect( c, SIGNAL( activated() ), this, SLOT( slotDisconnect() ) );

};

HidPopup::~HidPopup() {
    delete m_hidConnect;
}

void HidPopup::slotConnect() {
    odebug << "connect" << oendl;
    m_hidConnect = new OProcess();
    *m_hidConnect << "hidd" << "--connect" << m_item->mac();
    connect(m_hidConnect,
        SIGNAL(receivedStdout(Opie::Core::OProcess*, char*, int)),
        this, SLOT(fillOutPut(Opie::Core::OProcess*, char*, int)));
    connect(m_hidConnect,
        SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int)),
        this,    SLOT(fillErr(Opie::Core::OProcess*, char*, int)));
    connect(m_hidConnect,
        SIGNAL(processExited(Opie::Core::OProcess*)),
        this, SLOT(slotProcessExited(Opie::Core::OProcess*)));
    if (!m_hidConnect->start(OProcess::Block, OProcess::AllOutput)) {
        QMessageBox::critical(this, tr("HID Connection"),
            tr("HID Connection\nto device ") + m_item->mac() + tr("\nfailed"));
        delete m_hidConnect;
        m_hidConnect = 0l;
    }
}

void HidPopup::slotDisconnect()  {
    OProcess hidKill;
    hidKill << "hidd" << "--kill" << m_item->mac();
    hidKill.start(OProcess::DontCare, OProcess::NoCommunication);
    sleep(1);
    QMessageBox::information(this, tr("HID Disconnect"), tr("HID Disconnected"));
}

void HidPopup::fillOutPut( OProcess*, char* cha, int len ) {
    QCString str(cha, len);
    odebug << str << oendl;
}

void HidPopup::fillErr(OProcess*, char* buf, int len)
{
    QCString str(buf, len);
    odebug << str << oendl;
}

void HidPopup::slotProcessExited(OProcess* proc) {
    if (m_hidConnect->normalExit())
        QMessageBox::information(this, tr("HID Connection"),
            tr("HID Connect\nstarted"));
    else
        QMessageBox::critical(this, tr("HID Connection"),
            tr("HID Connection\nto device ") + m_item->mac() + tr("\nfailed"));
    delete m_hidConnect;
    m_hidConnect = 0l;
}

//eof
