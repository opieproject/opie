/* $Id: obexftpopup.cpp,v 1.2 2007-12-12 10:03:04 paule Exp $ */
/* OBEX file browser popup */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "obexdialog.h"
#include "obexftpopup.h"
#include "obexftpdialog.h"

/* OPIE */
#include <qpe/qpeapplication.h>
#include <opie2/odebug.h>
using namespace Opie::Core;
using namespace OpieTooth;

ObexFtpPopup::ObexFtpPopup(const OpieTooth::Services& service,  
    OpieTooth::BTDeviceItem* item)
    : QPopupMenu(), m_service(service)
{
    odebug << "ObexFtpPopup c'tor" << oendl; 

    m_item = item;
    /* connect action */
    m_push = new QAction( ); // so it's get deleted
    m_push->setText("Push file");
    m_push->addTo( this );
    connect(m_push, SIGNAL(activated()), SLOT(slotPush()));

    /* browse action */
    m_browse = new QAction(this);
    m_browse->setText("Browse device");
    m_browse->addTo(this);
    connect(m_browse, SIGNAL(activated()), SLOT(slotBrowse()));
}


ObexFtpPopup::~ObexFtpPopup()
{
    m_push->removeFrom(this);
    m_browse->removeFrom(this);
    delete m_push;
    delete m_browse;
}

void ObexFtpPopup::slotBrowse()
{
    odebug << "browse" <<oendl;
    ObexFtpDialog ftpDlg(m_item->mac(), 
        m_service.protocolDescriptorList().last().port());
    QPEApplication::execDialog(&ftpDlg);
}

void ObexFtpPopup::slotPush()
{
    QString device = m_item->mac();
    int port = m_service.protocolDescriptorList().last().port();
    device += "@";
    device += QString::number(port);
    owarn << "push something to " << device << oendl; 
    ObexDialog obexDialog(device);
    QPEApplication::execDialog( &obexDialog );
}

//eof
