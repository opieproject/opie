
/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "datawidget.h"
#include "dataTable.h"

class QTable;
class QComboBox;

class dataWidgetUI : public QDialog
{
	Q_OBJECT
	
    public:
		dataWidgetUI();

    private:
        void createTableLayout();
        QTable *DataTable;
        QComboBox *dataCombo;

    private slots:
        void slotShowData(int);
};
