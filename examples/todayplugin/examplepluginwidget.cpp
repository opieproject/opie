/*
 * examplepluginwidget.cpp
 *
 * copyright   : (c) 2004 by Maximilian Reiﬂ
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

#include "examplepluginwidget.h"

ExamplePluginWidget::ExamplePluginWidget( QWidget *parent,  const char* name)
    : QWidget(parent,  name ) {

    m_exampleLabel = 0l;
    m_layout = 0l;

    if ( m_exampleLabel )  {
        delete m_exampleLabel;
    }

   // since here a OClickableLabel is used, the plugin part will be clickable, and the actions
   // that should be triggered when clicked are defined in  slotClicked()
   // of course also normal widgets can be used.
    m_exampleLabel = new OClickableLabel( this );
    connect( m_exampleLabel, SIGNAL( clicked() ), this, SLOT( slotClicked() ) );

    if ( m_layout )  {
        delete m_layout;
    }
    m_layout = new QHBoxLayout( this );
    m_layout->setAutoAdd( true );

    readConfig();
    getInfo();
}


ExamplePluginWidget::~ExamplePluginWidget() {
    delete m_exampleLabel;
    delete m_layout;
}


void ExamplePluginWidget::readConfig() {
// we dont have any config entries in this plugin
// normally this method is used after today config was used
}


void ExamplePluginWidget::refresh()  {

}

void ExamplePluginWidget::getInfo() {
    m_exampleLabel->setText(  "Example text"  );
}

void ExamplePluginWidget::slotClicked() {
   getInfo();
}
