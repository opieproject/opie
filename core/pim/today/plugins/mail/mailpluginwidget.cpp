/*
 * mailpluginwidget.cpp
 *
 * copyright   : (c) 2002 by Maximilian Reiﬂ
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


#include <qvaluelist.h>
#include <qtl.h>
#include <qstring.h>
#include <qobject.h>
#include <qlayout.h>

#include <qpe/config.h>
#include <qpe/timestring.h>
#include <qpe/qcopenvelope_qws.h>

#include "mailpluginwidget.h"

MailPluginWidget::MailPluginWidget( QWidget *parent,  const char* name)
    : QWidget(parent,  name ) {

    readConfig();
    getInfo();
}

MailPluginWidget::~MailPluginWidget() {
}


void MailPluginWidget::readConfig() {
    Config cfg( "todaymailplugin" );
    cfg.setGroup( "config" );
}


void MailPluginWidget::getInfo() {

    QHBoxLayout* layout = new QHBoxLayout( this );

    mailLabel = new OClickableLabel( this );
    mailLabel->setMaximumHeight( 15 );
    connect( mailLabel, SIGNAL( clicked() ), this, SLOT( startMail() ) );

    Config cfg( "opiemail" );
    cfg.setGroup( "today" );

    int NEW_MAILS = cfg.readNumEntry( "newmails", 0 );
    int OUTGOING = cfg.readNumEntry( "outgoing", 0 );

    QString output = QObject::tr( "<b>%1</b> new mail(s), <b>%2</b> outgoing" ).arg( NEW_MAILS ).arg( OUTGOING );

    mailLabel->setText( output );
    layout->addWidget( mailLabel );
}

/**
 * launches datebook
 */
void MailPluginWidget::startMail() {
    QCopEnvelope e("QPE/System", "execute(QString)");
    e << QString( "mail" );
}
