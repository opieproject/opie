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
#include <qtimer.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include "graph.h"
#include "memory.h"

MemoryInfo::MemoryInfo( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    QVBoxLayout *vb = new QVBoxLayout( this, 5 );

    totalMem = new QLabel( this );
    vb->addWidget( totalMem );

    data = new GraphData();
    graph = new BarGraph( this );
    graph->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    vb->addWidget( graph, 1 );
    graph->setData( data );

    legend = new GraphLegend( this );
    vb->addWidget( legend );
    legend->setData( data );

    swapMem = new QLabel( this );
    vb->addWidget( swapMem );

    swapdata = new GraphData();
    swapgraph = new BarGraph( this );
    swapgraph->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    vb->addWidget( swapgraph, 1 );
    swapgraph->setData( swapdata );

    swaplegend = new GraphLegend( this );
    vb->addWidget( swaplegend );
    swaplegend->setData( swapdata );

    vb->addStretch( 1 );
    updateData();

    QTimer *t = new QTimer( this );
    connect( t, SIGNAL( timeout() ), this, SLOT( updateData() ) );
    t->start( 5000 );

    QWhatsThis::add( this, tr( "This page shows how memory (i.e. RAM) is being allocated on your device.\nMemory is categorized as follows:\n\n1. Used - memory used to by Opie and any running applications.\n2. Buffers - temporary storage used to improve performance\n3. Cached - information that has recently been used, but has not been freed yet.\n4. Free - memory not currently used by Opie or any running applications." ) );

}

MemoryInfo::~MemoryInfo()
{
    delete data;
}

void MemoryInfo::updateData()
{
    QFile file( "/proc/meminfo" );

    if ( file.open( IO_ReadOnly ) )
    {
        // local variables
        QString line;
        QString identifier;
        QString value;
        int position;
        QTextStream t( &file );

        while ( !t.atEnd() )
        {
            // read a line
            line = t.readLine();

            // extract identifier and value from line
            position = line.find( ":" );
            identifier = line.left( position );
            value = line.mid( position + 1, line.length() - position - 3 );
            value = value.stripWhiteSpace();

            // copy values in variables
            if ( identifier == "MemTotal" )
            {
                total = value.toULong();
            } else if ( identifier == "MemFree" )
            {
                memfree = value.toULong();
            } else if ( identifier == "Buffers" )
            {
                buffers = value.toULong();
            } else if ( identifier == "Cached" )
            {
                cached = value.toULong();
            } else if ( identifier == "SwapCached" )
            {
            } else if ( identifier == "SwapTotal" )
            {
                swaptotal = value.toULong();
            } else if ( identifier == "SwapFree" )
            {
                swapfree = value.toULong();
            }
        }

        file.close();

        // calculate values
        used = total - memfree;
        swapused = swaptotal - swapfree;
        realUsed = total - ( buffers + cached + memfree );

        // visualize values
        totalMem->setText( tr( "Total Memory: %1 kB" ).arg( total ) );
        data->clear();
        data->addItem( tr("Used (%1 kB)").arg(realUsed), realUsed );
        data->addItem( tr("Buffers (%1 kB)").arg(buffers), buffers );
        data->addItem( tr("Cached (%1 kB)").arg(cached), cached );
        data->addItem( tr("Free (%1 kB)").arg(memfree), memfree );

        graph->hide();
        graph->show();
        legend->update();

        if (swaptotal > 0)
        {
            swapMem->setText( tr( "Total Swap: %1 kB" ).arg( swaptotal ) );
            swapdata->clear();
            swapdata->addItem( tr("Used (%1 kB)").arg(swapused), swapused );
            swapdata->addItem( tr("Free (%1 kB)").arg(swapfree), swapfree );

            swapMem->show();
            swapgraph->show();
            swaplegend->show();

            swapgraph->repaint( FALSE );
            swaplegend->update();
        }
        else
        {
            swapMem->hide();
            swapgraph->hide();
            swaplegend->hide();
        }
    }
}
