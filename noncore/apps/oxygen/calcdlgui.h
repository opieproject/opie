
/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include <qstring.h>
#include "calcdlg.h"

class  QVBox;
class KMolCalc;

class calcDlgUI : public CalcDlg
{
	Q_OBJECT
	
    public:
		calcDlgUI();
        KMolCalc *kmolcalc;

    public slots:
        void calc();
        void clear();
};
