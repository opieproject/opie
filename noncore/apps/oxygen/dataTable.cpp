/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "oxygen.h"

#include "dataTable.h"
#include <qtable.h>

dataTable::dataTable() : QTable()
{
}

void dataTable::paintCell(  QPainter *p, int, int, const QRect &cr, bool )
{
    if ( currentRow()%2) qDebug("foo" );
}



