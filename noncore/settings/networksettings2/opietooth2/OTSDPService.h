//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SDPSERVICE_H
#define SDPSERVICE_H

#include <qvector.h>
#include <qarray.h>
#include <OTSDPAttribute.h>

namespace Opietooth2 {

/**
@author Fred Schaettgen
*/
class OTSDPService {

public:

    OTSDPService();
    virtual ~OTSDPService();

    void addAttribute(int id, OTSDPAttribute * attr);

    const OTSDPAttribute & attribute( int index );
    int attributeID( int index );

    AttributeVector subAttributes( OTSDPAttribute * attr);
    OTSDPAttribute * attributeByID( int id );

    // See Bluetooth Core Spec Sec. 5.1
    bool recordHandle(uint32_t *handle); // 0x00

    //bool getServiceClassIDList(vector<uuid_t> *classIDList); // 0x01
    //bool getServiceRecordState(uint32_t *state); // 0x02
    //bool getServiceID(uuid_t *id); // 0x03;
    //bool getProtocolDescriptorList(SDP::Attribute *attribute); // 0x04
    //bool getBrowseGroupList(vector<uuid_t> *browseGroupList); // 0x05
    QString name(void); // langBase + 0x00
    QString description(void); // langBase + 0x01

    UUIDVector allUUIDs();

    // return list of classes this service belongs to
    UUIDVector classIDList();

    /** Retrieves the Rfcomm channel.
    This function is provided just for conveniance.
    @param n The Rfcomm channel.
    @ret true if an rfcomm channel was found, false otherwise. */
    bool rfcommChannel(unsigned int &n);

    bool hasClassID(const OTUUID & uuid);

private:

    struct AttributeEntry {
        int id;
        OTSDPAttribute * attr;
    };

    QArray<AttributeEntry> attributeList;
};

}

#endif
