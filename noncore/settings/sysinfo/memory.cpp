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
**********************************************************************/

#include <qlabel.h>
#include <qfile.h>
#include <qlayout.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include "graph.h"
#include "memory.h"

MemoryInfo::MemoryInfo( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, WStyle_ContextHelp )
{
    QVBoxLayout *vb = new QVBoxLayout( this, 5 );

    totalMem = new QLabel( this );
    vb->addWidget( totalMem );

    data = new GraphData();
//    graph = new PieGraph( this );
    graph = new BarGraph( this );
    graph->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    vb->addWidget( graph, 1 );
    graph->setData( data );

    legend = new GraphLegend( this );
    vb->addWidget( legend );
    legend->setData( data );

    vb->addStretch( 1 );
    updateData();

    QTimer *t = new QTimer( this );
    connect( t, SIGNAL( timeout() ), this, SLOT( updateData() ) );
    t->start( 5000 );
    
    QWhatsThis::add( this, tr( "This page shows how memory (i.e. RAM) is being allocated on your handheld device.\nMemory is categorized as follows:\n\n1. Used - memory used to by Opie and any running applications.\n2. Buffers - temporary storage used to improve performance\n3. Cached - information that has recently been used, but has not been freed yet.\n4. Free - memory not currently used by Opie or any running applications." ) );
}

MemoryInfo::~MemoryInfo()
{
    delete data;
}

void MemoryInfo::updateData()
{
    QFile file( "/proc/meminfo" );

    if ( file.open( IO_ReadOnly ) ) {
	QTextStream t( &file );
	QString dummy = t.readLine();	// title
	t >> dummy;
	int total, used, memfree, shared, buffers, cached;
	t >> total;
	total /= 1000;
	t >> used;
	used /= 1000;
	t >> memfree;
	memfree /= 1000;
	t >> shared;
	shared /= 1000;
	t >> buffers;
	buffers /= 1000;
	t >> cached;
	cached /= 1000;
	int realUsed = total - ( buffers + cached + memfree );
	data->clear();
	data->addItem( tr("Used (%1 kB)").arg(realUsed), realUsed );
	data->addItem( tr("Buffers (%1 kB)").arg(buffers), buffers );
	data->addItem( tr("Cached (%1 kB)").arg(cached), cached );
	data->addItem( tr("Free (%1 kB)").arg(memfree), memfree );
	totalMem->setText( tr( "Total Memory: %1 kB" ).arg( total ) );
	graph->repaint( FALSE );
	legend->update();
    }
}


