/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qmainwindow.h>

class  QVBox;

class Oxygen : public QMainWindow
{
	Q_OBJECT
	
    public:
		Oxygen();
        QVBox *vbox;
	private slots:
        void slotCalculations() const;
        void slotData() const;
        void slotPSE() const;
		void close() const;
};
