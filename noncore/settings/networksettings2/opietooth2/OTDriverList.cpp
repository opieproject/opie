/***************************************************************************
 *   Copyright (C) 2003 by Mattia Merzi                                    *
 *   ottobit@ferrara.linux.it                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <malloc.h>
#include <bluezlib.h>

#include <opie2/odebug.h>

#include <OTDriverList.h>
#include <OTGateway.h>
#include <OTDriver.h>

using namespace Opietooth2;

OTDriverList::OTDriverList( OTGateway * _OT ) : QVector<OTDriver>() {

    OT = _OT;
    setAutoDelete( true );
}

OTDriverList::~OTDriverList() {
}

void OTDriverList::update() {

    struct hci_dev_list_req *dl;
    struct hci_dev_req *dr;
    struct hci_dev_info di;
    int cur;

    dl = 0;
    cur = 0;
    do {
      cur += 5;

      dl = (struct hci_dev_list_req*)
          ::realloc( dl, sizeof( struct hci_dev_list_req ) +
                       ( cur * sizeof(struct hci_dev_req) )
                 );

      if( dl == 0 ) {
        // memory problem
        exit(1);
      }

      dl->dev_num = cur;

      if( ioctl( OT->getSocket(), HCIGETDEVLIST, (void*)dl) ) {
        odebug << "WARNING : cannot read device list. "
              << errno 
              << strerror( errno ) << oendl;
        return;
      }

      // if num == cur perhaps we did not get all devices yet
    } while( dl->dev_num == cur );

    if( dl->dev_num != count() ) {
      // new or missing devices
      clear();

      dr = dl->dev_req;
      resize( dl->dev_num );

      for( cur=0; cur < dl->dev_num; cur ++) {
          memset( &di, 0, sizeof( di ) );
          di.dev_id = (dr+cur)->dev_id;

          // get device info 
          if( ioctl( OT->getSocket(), HCIGETDEVINFO, (void*)&di) != 0 )
              continue;  // uh ?
          insert( cur, new OTDriver( OT, &di ) );
      }

      odebug << "Found " << count() << " devices" << oendl;

      ::free( dl );
    }
}
