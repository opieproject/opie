/* $Id: rfcommassigndialogimpl.cpp,v 1.7 2006-03-25 18:10:13 korovkin Exp $ */
/* RFCOMM binding table edit dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "rfcommassigndialogimpl.h"
#include "rfcommassigndialogitem.h"
#include "rfcommconfhandler.h"

/* OPIE */
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qlayout.h>

using namespace OpieTooth;

// TODO: write only the file in bluebase?
// muss rfcommd dann neu gestartet werden
// sollte rfcomm bind all nicht eh default config sein ( polled das? - d.h. sobald nen gerät in der nähe ist bindet es?


RfcommAssignDialog::RfcommAssignDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
        : RfcommAssignDialogBase( parent, name, modal, fl )
{
    m_range = 5;

    m_scrollView = new QScrollView(this);
    m_scrollView->setResizePolicy( QScrollView::AutoOneFit );
    m_scrollView->setHScrollBarMode( QScrollView::AlwaysOff );

    RfcommAssignDialogBaseLayout->addWidget(m_scrollView);

    m_box = new QVBox(m_scrollView->viewport());
    m_scrollView->addChild(m_box);

    confHandler = new RfCommConfHandler( "/etc/bluetooth/rfcomm.conf" );

    loadConfig();
}

RfcommAssignDialog::~RfcommAssignDialog()
{
    if ( confHandler )
    {
        delete confHandler;
    }
}


void RfcommAssignDialog::newDevice(const QString & mac, int channel)
{

    for ( int i = 0 ; i < m_range; i++ )
    {
        QMap<QString, RfCommConfObject*>::Iterator it;
        it = confHandler->foundEntries().find(QString::number(i));
        // make sure that rfcommX is not assigned yet
        if ( it == confHandler->foundEntries().end() )
        {
            QDialog dialog( this,  "newdevice", true, WStyle_ContextHelp );
            dialog.setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7,
                (QSizePolicy::SizeType)7, sizePolicy().hasHeightForWidth()));
            QVBoxLayout layout(&dialog);
            layout.setSpacing( 0 );
            layout.setMargin( 0 );

            RfcommDialogItem newDev(&dialog);
            newDev.setIdent( i );
            newDev.setMac( mac );
            newDev.setChannel( channel );
            layout.addWidget(&newDev);

            if ( QPEApplication::execDialog( &dialog ) == QDialog::Accepted )
            {
                QMap<int, RfcommDialogItem*>::Iterator it;
                it = m_itemList.find( i );
                RfcommDialogItem *rfcomm = it.data();
                rfcomm->setIdent( i );
                rfcomm->setMac( mac );
                rfcomm->setChannel( newDev.channel() );
                rfcomm->setComment( newDev.comment() );
                rfcomm->setBind( newDev.isBind() );
                odebug << "New device set up" << oendl;
            }
            break;
        }
    }
}

/*
 * Load rfcomm bind configuration
 */
void RfcommAssignDialog::loadConfig()
{
    for ( int i = 0 ; i < m_range; i++ )
    {
        RfcommDialogItem *rfcomm = new RfcommDialogItem( m_box );
        m_itemList.insert(i, rfcomm);
        rfcomm->setIdent(i);
        QMap<QString, RfCommConfObject*>::Iterator it;
        it = confHandler->foundEntries().find(QString::number(i));
        if (it != confHandler->foundEntries().end())
        {
            rfcomm->setMac(it.data()->mac());
            rfcomm->setChannel(it.data()->channel());
            rfcomm->setComment(it.data()->comment());
            rfcomm->setBind(it.data()->isBind());
        }
    }
}


/*
 * Save rfcomm bind configuration
 */
void RfcommAssignDialog::saveConfig()
{
    QMap< int, RfcommDialogItem*>::Iterator it;
    QMap< QString, RfCommConfObject*> outMap;

    for( it = m_itemList.begin(); it != m_itemList.end(); ++it )
    {
        RfcommDialogItem *rfcomm = it.data();
        if (rfcomm->mac().isEmpty())
            continue;
        outMap.insert(QString::number(it.key()),
            new RfCommConfObject(it.key(),
                rfcomm->mac(), rfcomm->channel(), rfcomm->comment(),
                rfcomm->isBind()));
    }
    confHandler->save(outMap);
}

/*
 * Reaction on the OK button
 */
void RfcommAssignDialog::accept()
{
    odebug << "save configuration" << oendl;
    saveConfig();
    RfcommAssignDialogBase::accept();
}

//eof
