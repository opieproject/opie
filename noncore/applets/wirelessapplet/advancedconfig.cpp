/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer
**               <mickey@tm.informatik.uni-frankfurt.de>
**               http://www.Vanille.de
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "advancedconfig.h"

#include <qpe/config.h>
#include <qwidget.h>
#include <qcheckbox.h>

AdvancedConfig::AdvancedConfig( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : AdvancedConfigBase( parent, name, modal, fl )
{
    Config cfg( "qpe" );
    cfg.setGroup( "Wireless" );
    
    bool rocESSID = cfg.readBoolEntry( "renew_dhcp_on_essid_change", false );
    bool rocFREQ = cfg.readBoolEntry( "renew_dhcp_on_freq_change", false );
    bool rocAP = cfg.readBoolEntry( "renew_dhcp_on_ap_change", false );
    bool rocMODE = cfg.readBoolEntry( "renew_dhcp_on_mode_change", false );
    
    cbESSID->setChecked( rocESSID );
    cbFrequency->setChecked( rocFREQ );
    cbAccessPoint->setChecked( rocAP );
    cbMODE->setChecked( rocMODE );
}

AdvancedConfig::~AdvancedConfig()
{
}    

void AdvancedConfig::accept()
{

    Config cfg( "qpe" );
    cfg.setGroup( "Wireless" );
    cfg.writeEntry( "renew_dhcp_on_essid_change", cbESSID->isChecked() );
    cfg.writeEntry( "renew_dhcp_on_freq_change", cbFrequency->isChecked() );
    cfg.writeEntry( "renew_dhcp_on_ap_change", cbAccessPoint->isChecked() );
    cfg.writeEntry( "renew_dhcp_on_mode_change", cbMODE->isChecked() );

    AdvancedConfigBase::accept();
}

