/***************************************************************************
 *   Copyright (C) 2003 by Mattia Merzi                                    *
 *   ottobit@ferrara.linux.it                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OTDRIVERLIST_H
#define OTDRIVERLIST_H

#include <qvector.h>

namespace Opietooth2 {

class OTDriver;
class OTGateway;

class OTDriverList : public QVector<OTDriver> {

public:

    OTDriverList( OTGateway * _OT );
    ~OTDriverList();

    inline  OTGateway * gateway() const
      { return OT; }

    void update();

private:

    // backpointer
    OTGateway * OT;
};

}
#endif
