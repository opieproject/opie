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
#ifndef _PSEWIDGET_H
#define _PSEWIDGET_H

#include <qwidget.h>
#include <qlist.h>
#include "oxyframe.h"

class QGridLayout;
class QStringList;

class OxydataWidget;
class PSEframe;

class PSEWidget : public QWidget
{
	Q_OBJECT
	
    public:
		PSEWidget(const QStringList &list=0);
        QGridLayout *bottom_grid;
        OxydataWidget *oxyDW;

        QList<OxyFrame> PSEframe;

		int lastElement;

    private:
        QGridLayout *maingrid;
        
        void position( int, int&, int& );
        QColor PSEColor( QString );
		QStringList names;

    public slots:
        void slotShowElement(QString);
		void inverseColor( QString );

};

#endif
