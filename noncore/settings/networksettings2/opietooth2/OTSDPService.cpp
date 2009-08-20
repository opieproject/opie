/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <opie2/odebug.h>

#include <OTSDPAttribute.h>
#include <OTSDPService.h>

using namespace Opietooth2;

OTSDPService::OTSDPService() : attributeList() {
}

OTSDPService::~OTSDPService() {
    for (unsigned int n=0; n < attributeList.count(); ++n) {
      delete attributeList[n].attr;
    }
}

void OTSDPService::addAttribute(int id, OTSDPAttribute * attr) {
    attributeList.resize( attributeList.size() + 1 );

    attributeList[attributeList.size() - 1].id = id;
    attributeList[attributeList.size() - 1].attr = attr;
}

const OTSDPAttribute & OTSDPService::attribute(int index) {
    return *(attributeList[index].attr);
}

int OTSDPService::attributeID(int index) {
    return attributeList[index].id;

}

OTSDPAttribute * OTSDPService::attributeByID(int id) {

    for (unsigned int n=0; n < attributeList.count(); ++n) {
      if (attributeList[n].id == id) {
        return attributeList[n].attr;
      }
    }
    return 0;
}

bool OTSDPService::recordHandle(uint32_t *handle) {
    OTSDPAttribute * attrib;
    attrib = attributeByID(0x00);
    if( attrib && attrib->getType() == OTSDPAttribute::UINT) {
      *handle = (uint32_t)attrib->getUInt().lo;
      return true;
    }
    return false;
}

QString OTSDPService::name(void) {
    QString S;
    OTSDPAttribute * attrib;
    attrib = attributeByID(0x100);
    if( attrib && attrib->getType() == OTSDPAttribute::STRING) {
      S = attrib->getString();
    }
    return S;
}

QString OTSDPService::description() {
    QString S;
    OTSDPAttribute * attrib;
    attrib = attributeByID(0x101);
    if ( attrib && attrib->getType() == OTSDPAttribute::STRING) {
      S = attrib->getString();
    }
    return S;
}

UUIDVector OTSDPService::allUUIDs() {
    UUIDVector uuidList;

    for ( unsigned int i = 0;
          i < attributeList.count();
          i ++ ) {
      int os;
      UUIDVector subList = attributeList[i].attr->getAllUUIDs();
      os = uuidList.size();
      uuidList.resize( uuidList.size()+subList.count() );

      for( unsigned int k = 0; k < subList.count(); k++ ) {
        uuidList[os + k] = subList[k];
      }
    }
    return uuidList;
}

bool OTSDPService::rfcommChannel(unsigned int &n) {
    // Get the rfcomm channel
    OTSDPAttribute * protoDescAttr;
    // Get the the protocol descriptor list attribute (0x04)
    protoDescAttr = attributeByID(0x04);
    if( ! protoDescAttr ) {
        return false;
    }

    AttributeVector & protoDescList = *(protoDescAttr->getSequence());

    for( unsigned int i = 0;
         i < protoDescList.count();
         i ++ ) {
      AttributeVector & attrList = *(protoDescList[i]->getSequence());

      //The List must have at least 2 Attributes
      //Example:
      //  UUID16 : 0x0003 - RFCOMM
      //  Channel/Port (Integer) : 0x6
      if(attrList.size() >= 2) {
        // The first Attribute of the list must be an UUID
        if( attrList[0]->getType() != OTSDPAttribute::UUID)
          continue;
        // The UUID must have the value of "0x0003" that's the RFCOMM UUID
        OTUUID rfcommUUID( "0x0003" );
        if( attrList[0]->getUUID() != rfcommUUID) //RFCOMM UUID
          continue;
        //If the UUID is ok we get the rfcomm channel number
        if( attrList[1]->getType() != OTSDPAttribute::UINT)
          continue;

        n = attrList[1]->getUInt().lo;

        return true;
      }
    }
	// If we're here, we haven't found a correct Rfcomm channel, so we return false
    return false;
}

bool OTSDPService::hasClassID(const OTUUID & uuid) {
    OTSDPAttribute * ClassIDAttr;

    // Get the the ClassID descriptor list attribute (0x01)
    ClassIDAttr = attributeByID( 0x01);
    if( ! ClassIDAttr ) {
      return false;
    }

    AttributeVector & ClassIDList = *(ClassIDAttr->getSequence());
    for( unsigned int i = 0 ;
         i < ClassIDList.count() ;
         i ++ ) {
      if( ClassIDList[i]->getType() != OTSDPAttribute::UUID)
          continue;
      if( ClassIDList[i]->getUUID() == uuid)
        return true;
    }
    return false;
}

/** Get a vector of UUID of the services class Id List */
UUIDVector OTSDPService::classIDList( void ) {

    UUIDVector uuidList;
    OTSDPAttribute * ClassIDAttr;

    // Get the the ClassID descriptor list attribute (0x01)
    ClassIDAttr = attributeByID( 0x01);
    if( ! ClassIDAttr ) {
      return uuidList;
    }

    AttributeVector & ClassIDList = *(ClassIDAttr->getSequence());

    for( unsigned int i = 0 ;
         i < ClassIDList.count() ;
         i ++ ) {
        if( ClassIDList[i]->getType() != OTSDPAttribute::UUID)
            continue;

        uuidList.resize( uuidList.size() + 1 );
        uuidList[ uuidList.size() - 1 ] = ClassIDList[i]->getUUID();
    }

    return uuidList;
}

