/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
**********************************************************************/
#include "memorystatus.h"
#include "../../settings/sysinfo/memory.h"
#include "swapfile.h"

#include <qpainter.h>
#include <qpushbutton.h>
#include <qdrawutil.h>
#include <qlabel.h>

#include <opie/otabwidget.h>
#include <qlayout.h>

MemoryStatus::MemoryStatus(QWidget *parent, WFlags f )
    : QFrame(parent, 0, f), mi(0), sf(0)
{
    setCaption( tr("Memory Status") );
    //resize( 220, 180 );

    QVBoxLayout *lay = new QVBoxLayout( this );
    tab = new OTabWidget( this, "tabwidget", OTabWidget::Global );
    lay->addWidget( tab );
    tab->addTab( mi = new MemoryInfo( tab ), "memory/memorytabicon", tr("Memory") );
    tab->addTab( sf = new Swapfile( tab ), "memory/storagetabicon", tr("Swapfile") );

	QLabel* about = new QLabel(tr("<center><b>Memory Monitor Plugin</b><br>"
        "Copyright (C) 2003 Anton Maslovsky<br>"
        "&lt;<a href=\"mailto:my-zaurus@narod.ru\">my-zaurus@narod.ru</a>&gt;<br>"
        "<a href=\"http://my-zaurus.narod.ru\">http://my-zaurus.narod.ru</a><br>"
		"Based on source code from:<br> qswap (udoseidel@gmx.de) <br> Battery Applet (trolltech.com) <br> SysInfo (OPIE)<br><br>"
		"This program is licensed under GNU GPL.</center>"), tab);

    tab->addTab( about, "memory/info", tr("About") );

    tab->setCurrentTab( tr( "Memory" ) );
}

int MemoryStatus::percent()
{
	if (mi == 0)
		return 100;

	int total = mi->total;
	if (mi->swaptotal > 0)
		total += mi->swaptotal;

	int used = mi->realUsed;
	if (mi->swapused > 0)
		total += mi->swapused;

	return ((total - used) * 100)/total;
}

QSize MemoryStatus::sizeHint() const
{
	QSize s = tab->size();
	s.setWidth(200);
	s.setHeight((mi->swaptotal > 0) ? 220 : 200);
    return s;
}

MemoryStatus::~MemoryStatus()
{
}
