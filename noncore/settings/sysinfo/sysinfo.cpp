/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************
**
**  Enhancements by: Dan Williams, <williamsdr@acm.org>
**
**********************************************************************/

#include "memory.h"
#include "load.h"
#include "storage.h"
#include "processinfo.h"
#include "modulesinfo.h"
#include "versioninfo.h"
#include "sysinfo.h"

#include <opie/otabwidget.h>

#include <qpe/config.h>
#include <qpe/resource.h>

#include <qlayout.h>

SystemInfo::SystemInfo( QWidget *parent, const char *name, WFlags )
    : QWidget( parent, name, WStyle_ContextHelp )
{
    setIcon( Resource::loadPixmap( "system_icon" ) );
    setCaption( tr("System Info") );

    resize( 220, 180 );

    Config config( "qpe" );
    config.setGroup( "Appearance" );
    bool advanced = config.readBoolEntry( "Advanced", TRUE );

    QVBoxLayout *lay = new QVBoxLayout( this );
    OTabWidget *tab = new OTabWidget( this, "tabwidget", OTabWidget::Global );
    lay->addWidget( tab );
    tab->addTab( new MemoryInfo( tab ), "sysinfo/memorytabicon.png", tr("Memory") );
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    tab->addTab( new StorageInfo( tab ), "sysinfo/storagetabicon.png", tr("Storage") );
#endif
    tab->addTab( new LoadInfo( tab ), "sysinfo/cputabicon.png", tr("CPU") );
    if ( advanced )
    {
        tab->addTab( new ProcessInfo( tab ), "sysinfo/processtabicon.png", tr("Process") );
        tab->addTab( new ModulesInfo( tab ), "sysinfo/moduletabicon.png", tr("Modules") );
    }
    tab->addTab( new VersionInfo( tab ), "sysinfo/versiontabicon.png", tr("Version") );

    tab->setCurrentTab( tr( "Memory" ) );
}


