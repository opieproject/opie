/*
 * todopluginconfig.cpp
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

#include "todopluginconfig.h"

#include <qpe/config.h>

#include <qlayout.h>
#include <qtoolbutton.h>
#include <qlabel.h>


TodolistPluginConfig::TodolistPluginConfig( QWidget *parent,  const char* name)
    : ConfigWidget(parent,  name ) {

    QVBoxLayout * layout = new QVBoxLayout( this );

    QFrame* Frame9 = new QFrame( this, "Frame9" );
    Frame9->setGeometry( QRect( -5, 0, 230, 310 ) );
    Frame9->setFrameShape( QFrame::StyledPanel );
    Frame9->setFrameShadow( QFrame::Raised );

    QLabel* TextLabel6 = new QLabel( Frame9, "TextLabel6" );
    TextLabel6->setGeometry( QRect( 20, 10, 100, 60 ) );
    TextLabel6->setText( tr( "How many\n"
                             "tasks should \n"
                             "be shown?" ) );

    SpinBox2 = new QSpinBox( Frame9, "SpinBox2" );
    SpinBox2->setGeometry( QRect( 115, 20, 58, 25 ) );
    SpinBox2->setMaxValue( 20 );
    SpinBox2->setValue( 5 );

    layout->addWidget( Frame9 );

    readConfig();
}

void TodolistPluginConfig::readConfig() {
    Config cfg( "todaydatebookplugin" );
    cfg.setGroup( "config" );
    m_max_lines_task = cfg.readNumEntry( "maxlinestask", 5 );
    SpinBox2->setValue( m_max_lines_task );
}


void TodolistPluginConfig::writeConfig() {
    Config cfg( "todaydatebookplugin" );
    cfg.setGroup( "config" );
    cfg.writeEntry( "maxlinestask", m_max_lines_task );
    cfg.write();
}


TodolistPluginConfig::~TodolistPluginConfig() {
}
