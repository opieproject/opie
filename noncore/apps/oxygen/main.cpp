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


#include <qpe/qpeapplication.h>
#include "oxygen.h"

int main(int argc, char **argv)
{
    QPEApplication app(argc, argv);
    Oxygen *oxi = new Oxygen();
    app.setMainWidget(oxi);
    oxi->showMaximized();
    return app.exec();
}
