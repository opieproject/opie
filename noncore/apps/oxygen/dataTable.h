
/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qtable.h>

class dataTable : public QTable
{
	Q_OBJECT
	
    public:
		dataTable();

    protected:
        virtual void paintCell(  QPainter *p, int row, int col, const QRect &cr, bool selected );
};
