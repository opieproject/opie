
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

#include <qwidget.h>
#include <qtable.h>

class QLabel;
class OxydataTable;
class QTableItem;

/*
 * A OxydataWidget shows all known data of an element. It can 
 * be used for both the PSE and the pure Data-Dialog. It is 
 * a simple QHBox with 2 QStrings in it plus a OxydataTable
 * which contains the rest of the data.
 *
 * Author: Carsten Niehaus <cniehaus@handhelds.org>
 */

class OxydataWidget : public QWidget
{
    Q_OBJECT

    public:
        OxydataWidget(QWidget *parent=0);

        QLabel *left, *middle, *right;

    private:
        OxydataTable *DataTable;
        void setTable() const;
        
    public slots:
        void setElement( int );
};

/*
 * A OxydataTable is derived from QTable. I recoded the paintCell to have 
 * different colors in the backgound. Furthermore this widget never has a
 * grid, thus I removed that code in paintCell.
 *
 * Author: Carsten Niehaus <cniehaus@handhelds.org>
 */

class OxydataTable : public QTable
{
	Q_OBJECT
	
    public:
        OxydataTable( int numRows, int numCols,
                            QWidget *parent = 0, const char *name = 0 );

        virtual int alignment() const;
//        virtual QTableItem *item(  int row, int col ) const;

    protected:
        /*
         * This method is reimplemented form QTable. It implements the colourisation
         * of every second row.
         */
        virtual void paintCell( QPainter *p, int row, int col, const QRect &cr, bool selected );
};

//X class OxydataQTI : QTableItem
//X {
//X     Q_OBJECT
//X 
//X     public:
//X         OxydataQTI( 

#endif
