/*
 * mailpluginwidget.cpp
 *
 * copyright   : (c) 2002,2003,2004 by Maximilian Reiﬂ
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "mailpluginwidget.h"

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>

using namespace Opie::Ui;
MailPluginWidget::MailPluginWidget( QWidget *parent,  const char* name)
    : QWidget(parent,  name ) {

    m_mailLabel = 0l;
    m_layout = 0l;

    if ( m_mailLabel )  {
        delete m_mailLabel;
    }
    m_mailLabel = new OClickableLabel( this );
    connect( m_mailLabel, SIGNAL( clicked() ), this, SLOT( startMail() ) );

    if ( m_layout )  {
        delete m_layout;
    }
    m_layout = new QHBoxLayout( this );
    m_layout->setAutoAdd( true );


#if defined(Q_WS_QWS)
#if !defined(QT_NO_COP)
    QCopChannel *qCopChannel = new QCopChannel( "QPE/Pim" , this );
    connect ( qCopChannel, SIGNAL( received(const QCString&,const QByteArray&) ),
              this, SLOT ( channelReceived(const QCString&,const QByteArray&) ) );
#endif
#endif

    readConfig();
    getInfo();
}


void MailPluginWidget::channelReceived( const QCString &msg, const QByteArray & data ) {
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "outgoingMails(int)" ) {
       stream >> m_outgoing;
    } else if ( msg == "newMails(int)" ) {
       stream >> m_newMails;
    }
    getInfo();
}
MailPluginWidget::~MailPluginWidget() {
    delete m_mailLabel;
    delete m_layout;
}


void MailPluginWidget::readConfig() {
    Config cfg( "todaymailplugin" );
    cfg.setGroup( "config" );

    Config cfg2( "mail" );
    cfg2.setGroup( "Status" );

    m_newMails = cfg2.readNumEntry( "newMails", 0 );
    m_outgoing = cfg2.readNumEntry( "outgoing", 0 );
}


void MailPluginWidget::refresh()  {
    getInfo();
}

void MailPluginWidget::getInfo() {



    m_mailLabel->setText(  QObject::tr( "<b>%1</b> new mail(s), <b>%2</b> outgoing" ).arg( m_newMails ).arg( m_outgoing ) );
}

/**
 * launches datebook
 */
void MailPluginWidget::startMail() {
    QCopEnvelope e("QPE/System", "execute(QString)");
    e << QString( "opiemail" );
}
