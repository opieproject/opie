/*
 * mailpluginwidget.cpp
 *
 * copyright   : (c) 2002,2003 by Maximilian Reiﬂ
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

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>

#include "mailpluginwidget.h"

MailPluginWidget::MailPluginWidget( QWidget *parent,  const char* name)
    : QWidget(parent,  name ) {

    m_mailLabel = 0l;
    m_layout = 0l;

    if ( m_mailLabel )  {
        delete m_mailLabel;
    }
    m_mailLabel = new OClickableLabel( this );
    m_mailLabel->setMaximumHeight( 15 );
    connect( m_mailLabel, SIGNAL( clicked() ), this, SLOT( startMail() ) );

    if ( m_layout )  {
        delete m_layout;
    }
    m_layout = new QHBoxLayout( this );
    m_layout->setAutoAdd( true );

    readConfig();
    getInfo();
}

MailPluginWidget::~MailPluginWidget() {
    delete m_mailLabel;
    delete m_layout;
}


void MailPluginWidget::readConfig() {
    Config cfg( "todaymailplugin" );
    cfg.setGroup( "config" );
}


void MailPluginWidget::refresh()  {
    getInfo();
}

void MailPluginWidget::getInfo() {

    Config cfg( "opiemail" );
    cfg.setGroup( "today" );

    int NEW_MAILS = cfg.readNumEntry( "newmails", 0 );
    int OUTGOING = cfg.readNumEntry( "outgoing", 0 );

    //QString output = QObject::tr( "<b>%1</b> new mail(s), <b>%2</b> outgoing" ).arg( NEW_MAILS ).arg( OUTGOING );

    m_mailLabel->setText(  QObject::tr( "<b>%1</b> new mail(s), <b>%2</b> outgoing" ).arg( NEW_MAILS ).arg( OUTGOING ) );
}

/**
 * launches datebook
 */
void MailPluginWidget::startMail() {
    QCopEnvelope e("QPE/System", "execute(QString)");
    e << QString( "mail" );
}
