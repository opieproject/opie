
/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#ifndef _DATATABLE_H
#define _DATATABLE_H


#include <qtable.h>

class OxydataTable : public QTable
{
	Q_OBJECT
	
    public:
		OxydataTable();
        OxydataTable( int numRows, int numCols,
                            QWidget *parent = 0, const char *name = 0 );

    protected:
        /*
         * This method is reimplemented form QTable. It implements the colourisation
         * of every second row.
         */
        virtual void paintCell(  QPainter *p, int row, int col, const QRect &cr, bool selected );
};

#endif
