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

OxydataTable::OxydataTable(int numRows, int numCols, QWidget *parent,
        const char *name) : QTable(numRows, numRows, parent, name)
{
}

void OxydataTable::paintCell(  QPainter *p, int row, int col, const QRect &cr, bool selected)
{
    if ( cr.width() == 0 || cr.height() == 0 )
        return;
    selected = FALSE;

    int w = cr.width();
    int h = cr.height();

    QTableItem *itm = item( row, col );
    QColorGroup colgrp = colorGroup();
    if ( itm ) 
    {
        if ( row%2 )
            colgrp.setColor(  QColorGroup::Base, QColor( cyan ) );
        else
            colgrp.setColor(  QColorGroup::Base, QColor( white ) );
        p->save();
        itm->paint( p, colgrp, cr, selected );
        p->restore();
    }
}
