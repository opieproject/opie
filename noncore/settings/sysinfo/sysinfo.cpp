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
#include "devicesinfo.h"
#include "storage.h"
#include "processinfo.h"
#include "modulesinfo.h"
#include "benchmarkinfo.h"
#include "sysloginfo.h"
#include "versioninfo.h"
#include "sysinfo.h"

/* OPIE */
#include <opie2/oresource.h>
#include <opie2/otabwidget.h>
using namespace Opie::Ui;
#include <qpe/config.h>

/* QT */
#include <qlayout.h>

SystemInfo::SystemInfo( QWidget *parent, const char *name, WFlags )
    : QWidget( parent, name, WStyle_ContextHelp )
{
    setIcon( Opie::Core::OResource::loadPixmap( "system_icon", Opie::Core::OResource::SmallIcon ) );
    setCaption( tr("System Info") );

    QVBoxLayout *lay = new QVBoxLayout( this );
    OTabWidget *tab = new OTabWidget( this, "tabwidget", OTabWidget::Global );
    lay->addWidget( tab );

    tab->addTab( new MemoryInfo( tab ), "sysinfo/memorytabicon", tr("Memory") );
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    tab->addTab( new FileSysInfo( tab ), "sysinfo/storagetabicon", tr("Storage") );
#endif
    tab->addTab( new DevicesInfo( tab ), "sysinfo/cputabicon", tr("Devices") );
    tab->addTab( new ProcessInfo( tab ), "sysinfo/processtabicon", tr( "Process" ) );
    tab->addTab( new ModulesInfo( tab ), "sysinfo/moduletabicon", tr( "Modules" ) );
    tab->addTab( new SyslogInfo( tab ), "sysinfo/syslogtabicon", tr( "Syslog" ) );
    tab->addTab( new BenchmarkInfo( tab ), "sysinfo/benchmarktabicon", tr( "Benchmark" ) );
    tab->addTab( new VersionInfo( tab ), "sysinfo/versiontabicon", tr("Version") );

    tab->setCurrentTab( tr( "Memory" ) );
}
