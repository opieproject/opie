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

#include <assert.h>
#include <qregexp.h>
#include <opie2/odebug.h>

#include <OTUUID.h>
#include <OTSDPAttribute.h>

using namespace Opietooth2;

OTSDPAttribute::OTSDPAttribute() {
    type = INVALID;
    memset( &Value, 0, sizeof( Value )  );
}

OTSDPAttribute::~OTSDPAttribute() {
    if( type == INT ) {
      delete Value.intVal;
    } else if( type == UUID ) {
      delete Value.uuidVal;
    } else if( type == UINT ) {
      delete Value.uintVal;
    } else if( type == STRING ||
               type == URL
             ) {
      delete Value.stringVal;
    } else if( type == ALTERNATIVE ||
               type == SEQUENCE
             ) {
      delete Value.sequenceVal;
    }
}

OTSDPAttribute::OTSDPAttribute( sdp_data_t * attrib ) {
    setNil();
    switch( attrib->dtd ) {
      case SDP_DATA_NIL: // Nil type
        { setNil();
          break;
        }
      case SDP_UINT8: // Unsigned integer
        setUInt(attrib->val.uint8);
        break;
      case SDP_UINT16: // Unsigned integer
        setUInt(attrib->val.uint16);
        break;
      case SDP_UINT32: // Unsigned integer
        setUInt(attrib->val.uint32);
        break;
      case SDP_UINT64: // Unsigned integer
        setUInt(attrib->val.uint64);
        break;
      case SDP_UINT128: // Unsigned integer
        // setUInt(attrib->val.uint16);
        assert(false); // BUG/TODO: uint128 integers not supported
        break;
      case SDP_INT8: // Unsigned integer
        setInt(attrib->val.int8);
        break;
      case SDP_INT16: // Unsigned integer
        setInt(attrib->val.int16);
        break;
      case SDP_INT32: // Unsigned integer
        setInt(attrib->val.int32);
        break;
      case SDP_INT64: // Unsigned integer
        setInt(attrib->val.int64);
        break;
      case SDP_INT128: // Unsigned integer
        // newAttr.setInt(attrib->val.uint16);
        assert(false); // BUG/TODO: uint128 integers not supported
        break;
      case SDP_UUID16:
        { OTUUID id;
          ::uuid_t uuidVal = attrib->val.uuid;
          id.setUUID32(uuidVal.value.uuid16);
          setUUID(id );
        }
        break;
      case SDP_UUID32:
        { OTUUID id;
          ::uuid_t uuidVal = attrib->val.uuid;
          id.setUUID32(uuidVal.value.uuid32);
          setUUID(id );
        }
        break;
      case SDP_UUID128:
        { OTUUID id;
          ::uuid_t uuidVal = attrib->val.uuid;
          uint64_t* v128;
          v128 = reinterpret_cast<uint64_t*>(&(uuidVal.value.uuid128));
          id.setUUID128(v128[0], v128[1]);
          setUUID(id );
        }
        break;
      case SDP_TEXT_STR_UNSPEC :
      case SDP_TEXT_STR8 :
      case SDP_TEXT_STR16 :
      case SDP_TEXT_STR32 :
        setString( QString(attrib->val.str) );
        break;
      case SDP_URL_STR_UNSPEC :
      case SDP_URL_STR8 :
      case SDP_URL_STR16 :
      case SDP_URL_STR32 :
        setURL( QString(attrib->val.str) );
        break;
      case SDP_BOOL:
        setBool( attrib->val.int8 != 0);
        break;
      case SDP_SEQ_UNSPEC :
      case SDP_SEQ8 :
      case SDP_SEQ16 :
      case SDP_SEQ32 :
      case SDP_ALT_UNSPEC :
      case SDP_ALT8 :
      case SDP_ALT16 :
      case SDP_ALT32 :
        { AttributeVector subAttribs;
          OTSDPAttribute * Attr;
          sdp_data_t* subAttrib = attrib->val.dataseq;

          for (; subAttrib; subAttrib = subAttrib->next) {

            Attr = new OTSDPAttribute(subAttrib);
            subAttribs.resize( subAttribs.size() + 1 );
            subAttribs.insert( subAttribs.size() - 1, Attr );
          }

          if( attrib->dtd == SDP_ALT_UNSPEC  ||
              attrib->dtd == SDP_ALT8 ||
              attrib->dtd == SDP_ALT16 ||
              attrib->dtd == SDP_ALT32 ) {
            setAlternative(subAttribs);
          } else {
            setSequence(subAttribs);
          }
          break;
        }
    } // end case
}

QString OTSDPAttribute::toString( void ) {
    QString S;
    switch( type ) {
      case INVALID :
        S = "invalid";
        break;
      case NIL :
        S = "NIL";
        break;
      case UINT :
        S = Value.uintVal->toString();
        break;
      case INT :
        S = Value.intVal->toString();
        break;
      case UUID :
        S = Value.uuidVal->toString();
        break;
      case BOOLEAN :
        S = (Value.boolVal) ? "true" : "false";
        break;
      case STRING :
        S = *(Value.stringVal);
        break;
      case URL :
        S = *(Value.stringVal);
        break;
      case SEQUENCE :
        S.sprintf( "Sequence(%d)", Value.sequenceVal->count() );
        break;
      case ALTERNATIVE :
        S.sprintf( "Alternative(%d)", Value.sequenceVal->count() );
        break;
      case UNKNOWN :
        S = "unknown";
        break;
    }
    return S;
}

void OTSDPAttribute::setNil() {
    type = NIL;
}

void OTSDPAttribute::setInt(const OTSDPAttribute::int128_t & val) {
    type = INT;
    Value.intVal = new int128_t( val );
}

void OTSDPAttribute::setUInt(const uint128_t & val) {
    type = UINT;
    Value.uintVal = new uint128_t(val);
}

void OTSDPAttribute::setUUID(const OTUUID & val) {
    type = UUID;
    Value.uuidVal = new OTUUID( val );
}

void OTSDPAttribute::setBool(bool val) {
    type = BOOLEAN;
    Value.boolVal = val;
}

void OTSDPAttribute::setString( const QString & val) {
    type = STRING;
    Value.stringVal = new QString( val );
}

void OTSDPAttribute::setURL( const QString & val) {
    type = URL;
    Value.stringVal = new QString(val);
}

void OTSDPAttribute::setSequence(const AttributeVector& val) {
    type = SEQUENCE;
    Value.sequenceVal = new AttributeVector();
    Value.sequenceVal->setAutoDelete( TRUE );
    *Value.sequenceVal = val;
}

void OTSDPAttribute::setAlternative(const AttributeVector& val) {
    type = ALTERNATIVE;
    Value.sequenceVal = new AttributeVector();
    Value.sequenceVal->setAutoDelete( TRUE );
    *Value.sequenceVal = val;
}

QString OTSDPAttribute::getString() {
    assert(type == STRING);
    return *Value.stringVal;
}

QString OTSDPAttribute::getURL() {
    assert(type == URL);
    return *Value.stringVal;
}

const OTSDPAttribute::int128_t & OTSDPAttribute::getInt() {
    assert(type == INT);
    return *Value.intVal;
}


const OTSDPAttribute::uint128_t & OTSDPAttribute::getUInt() {
    assert(type == UINT);
    return *Value.uintVal;
}

const OTUUID & OTSDPAttribute::getUUID() {
    assert(type == UUID);
    return *Value.uuidVal;
}

bool OTSDPAttribute::getBool() {
    assert(type == BOOLEAN);
    return Value.boolVal;
}

AttributeVector * OTSDPAttribute::getSequence() {
    assert(type == SEQUENCE);
    return Value.sequenceVal;
}

AttributeVector * OTSDPAttribute::getAlternative() {
    assert(type == ALTERNATIVE);
    return Value.sequenceVal;
}

UUIDVector OTSDPAttribute::getAllUUIDs() {

    UUIDVector uuids;

    if (getType() == UUID) {
      uuids.resize( uuids.size()+1 );
      uuids[uuids.size()-1] = getUUID();
    } else {
      AttributeVector * subAttributes = 0 ;

      if (getType() == SEQUENCE) {
          subAttributes = getSequence();
      } else if (getType() == ALTERNATIVE) {
          subAttributes = getAlternative();
      }

      if (!subAttributes)
          return 0;

      int os;
      for( unsigned int i = 0; i < subAttributes->count(); i++ ) {
        UUIDVector subUUIDs = (*subAttributes)[i]->getAllUUIDs();

        os = uuids.size();
        uuids.resize( uuids.size()+subUUIDs.count() );

        for( unsigned int k = 0; k < subUUIDs.count(); k++ ) {
          uuids[os + k] = subUUIDs[k];
        }
      }
    }
    return uuids;
}

static char * Attr2String[] = {
        "Invalid",
        "Nil",
        "UInt",
        "int",
        "UUID",
        "Boolean",
        "String",
        "Sequence",
        "Alternative",
        "URL",
        "Unknown"
};

const char * OTSDPAttribute::getTypeString() {
    return Attr2String[type];
}
