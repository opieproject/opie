/*
 * todopluginconfig.cpp
 *
 * copyright   : (c) 2002, 2003 by Maximilian Reiﬂ
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
#include <qwhatsthis.h>


TodolistPluginConfig::TodolistPluginConfig( QWidget *parent,  const char* name)
    : TodayConfigWidget(parent,  name ) {

    QVBoxLayout * layout = new QVBoxLayout( this );

    m_gui = new TodoPluginConfigBase( this );

    layout->addWidget( m_gui );

    readConfig();
}

void TodolistPluginConfig::readConfig() {
    Config cfg( "todaytodoplugin" );
    cfg.setGroup( "config" );
    m_max_lines_task = cfg.readNumEntry( "maxlinestask", 5 );
    m_gui->SpinBox2->setValue( m_max_lines_task );
    m_maxCharClip =  cfg.readNumEntry( "maxcharclip", 38 );
    m_gui->SpinBoxClip->setValue( m_maxCharClip );
}


void TodolistPluginConfig::writeConfig() {
    Config cfg( "todaytodoplugin" );
    cfg.setGroup( "config" );
    m_max_lines_task = m_gui->SpinBox2->value();
    cfg.writeEntry( "maxlinestask", m_max_lines_task );
    m_maxCharClip = m_gui->SpinBoxClip->value();
    cfg.writeEntry( "maxcharclip",  m_maxCharClip );
    cfg.write();
}


TodolistPluginConfig::~TodolistPluginConfig() {
}
