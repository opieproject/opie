#ifndef DATAWIDGETUI_H
#define DATAWIDGETUI_H


/***************************************************************************
   application:             : Oxygen

   begin                    : September 2002
   copyright                : ( C ) 2002 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/


class OxydataWidget;
class OxydataTable;

class QComboBox;
class QStringList;

#include <qwidget.h>

class dataWidgetUI : public QWidget
{
	Q_OBJECT
	
    public:
		dataWidgetUI();
		QStringList names;

    private:
        OxydataTable *DataTable;
        QComboBox *dataCombo;
        OxydataWidget *oxyDW;

		void loadNames();
};
#endif
