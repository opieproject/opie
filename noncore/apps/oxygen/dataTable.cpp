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
    if (  cr.width() == 0 || cr.height() == 0 )
        return;
    if (  selected &&
            row == currentRow() &&
            col == currentColumn() )
        selected = FALSE;

    int w = cr.width();
    int h = cr.height();

    QTableItem *itm = item(  row, col );
    if (  itm ) {
        p->save();
        itm->paint(  p, colorGroup(), cr, selected );
        p->restore();
    } else {
        if ( currentRow()%2 ) //every even row
            p->fillRect(  0, 0, w, h, selected ? colorGroup().brush(  QColorGroup::Highlight ) : colorGroup().brush(  QColorGroup::Base ) );
        else                  //every odd row
            p->fillRect(  0, 0, w, h, selected ? colorGroup().brush(  QColorGroup::Highlight ) : colorGroup().brush(  QColorGroup::Base ) );
    }
}




