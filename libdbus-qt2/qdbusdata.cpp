/* qdbusdata.cpp DBUS data transport type
 *
 * Copyright (C) 2007 Kevin Krammer <kevin.krammer@gmx.at>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 */

#include "dbus/dbus.h"

#include "dbus/qdbusdata.h"
#include "dbus/qdbusdatalist.h"
#include "dbus/qdbusdatamap.h"
#include "dbus/qdbusobjectpath.h"
#include "dbus/qdbusvariant.h"

#include <qshared.h>
#include <qstring.h>
#include <qvaluelist.h>

class QDBusData::Private : public QShared
{
public:
    Private() : QShared(), type(QDBusData::Invalid), keyType(QDBusData::Invalid) {}

    ~Private()
    {
        switch (type)
        {
            case QDBusData::String:
                delete (QString*)value.pointer;
                break;

            case QDBusData::ObjectPath:
                delete (QDBusObjectPath*)value.pointer;
                break;

            case QDBusData::List:
                delete (QDBusDataList*)value.pointer;
                break;

            case QDBusData::Struct:
                delete (QValueList<QDBusData>*)value.pointer;
                break;

            case QDBusData::Variant:
                delete (QDBusVariant*)value.pointer;
                break;

            case QDBusData::Map:
                switch (keyType)
                {
                    case QDBusData::Byte:
                        delete (QDBusDataMap<Q_UINT8>*)value.pointer;
                        break;

                    case QDBusData::Int16:
                        delete (QDBusDataMap<Q_INT16>*)value.pointer;
                        break;

                    case QDBusData::UInt16:
                        delete (QDBusDataMap<Q_UINT16>*)value.pointer;
                        break;

                    case QDBusData::Int32:
                        delete (QDBusDataMap<Q_INT32>*)value.pointer;
                        break;

                    case QDBusData::UInt32:
                        delete (QDBusDataMap<Q_UINT32>*)value.pointer;
                        break;

                    case QDBusData::Int64:
                        delete (QDBusDataMap<Q_INT64>*)value.pointer;
                        break;

                    case QDBusData::UInt64:
                        delete (QDBusDataMap<Q_UINT64>*)value.pointer;
                        break;

                    case QDBusData::String:
                        delete (QDBusDataMap<QString>*)value.pointer;
                        break;

                    case QDBusData::ObjectPath:
                        delete (QDBusDataMap<QDBusObjectPath>*)value.pointer;
                        break;

                    default:
                        qFatal("QDBusData::Private: unhandled map key type %d(%s)",
                               keyType, QDBusData::typeName(keyType));
                        break;
                }
                break;

            default:
                break;
        }
    }

public:
    Type type;
    Type keyType;

    union
    {
        bool boolValue;
        Q_UINT8 byteValue;
        Q_INT16 int16Value;
        Q_UINT16 uint16Value;
        Q_INT32 int32Value;
        Q_UINT32 uint32Value;
        Q_INT64 int64Value;
        Q_UINT64 uint64Value;
        double doubleValue;
        void* pointer;
    } value;
};

QDBusData::QDBusData() : d(new Private())
{
}

QDBusData::QDBusData(const QDBusData& other) : d(0)
{
    d = other.d;

    d->ref();
}

QDBusData::~QDBusData()
{
    if (d->deref()) delete d;
}

QDBusData& QDBusData::operator=(const QDBusData& other)
{
    if (&other == this) return *this;

    if (d->deref()) delete d;

    d = other.d;

    d->ref();

    return *this;
}

bool QDBusData::operator==(const QDBusData& other) const
{
    if (&other == this) return true;

    if (d == other.d) return true;

    if (d->type == other.d->type)
    {
        switch (d->type)
        {
            case QDBusData::Invalid:
                return true;

            case QDBusData::Bool:
                return d->value.boolValue == other.d->value.boolValue;

            case QDBusData::Byte:
                return d->value.byteValue == other.d->value.byteValue;

            case QDBusData::Int16:
                return d->value.int16Value == other.d->value.int16Value;

            case QDBusData::UInt16:
                return d->value.uint16Value == other.d->value.uint16Value;

            case QDBusData::Int32:
                return d->value.int32Value == other.d->value.int32Value;

            case QDBusData::UInt32:
                return d->value.uint32Value == other.d->value.uint64Value;

            case QDBusData::Int64:
                return d->value.int64Value == other.d->value.int64Value;

            case QDBusData::UInt64:
                return d->value.uint64Value == other.d->value.uint64Value;

            case QDBusData::Double:
                // FIXME: should not compare doubles for equality like this
                return d->value.doubleValue == other.d->value.doubleValue;

            case QDBusData::String:
                return toString() == other.toString();

            case QDBusData::ObjectPath:
                return toObjectPath() == other.toObjectPath();

            case QDBusData::List:
                return toList() == other.toList();

            case QDBusData::Struct:
                return toStruct() == other.toStruct();

            case QDBusData::Variant:
                return toVariant() == other.toVariant();

            case QDBusData::Map:
                if (d->keyType != other.d->keyType) return false;

                switch (d->keyType)
                {
                    case QDBusData::Byte:
                        toByteKeyMap() == other.toByteKeyMap();
                        break;

                    case QDBusData::Int16:
                        toInt16KeyMap() == other.toInt16KeyMap();
                        break;

                    case QDBusData::UInt16:
                        toUInt16KeyMap() == other.toUInt16KeyMap();
                        break;

                    case QDBusData::Int32:
                        toInt32KeyMap() == other.toInt32KeyMap();
                        break;

                    case QDBusData::UInt32:
                        toUInt32KeyMap() == other.toUInt32KeyMap();
                        break;
                    case QDBusData::Int64:
                        toInt64KeyMap() == other.toInt64KeyMap();
                        break;

                    case QDBusData::UInt64:
                        toUInt64KeyMap() == other.toUInt64KeyMap();
                        break;

                    case QDBusData::String:
                        toStringKeyMap() == other.toStringKeyMap();
                        break;

                    case QDBusData::ObjectPath:
                        toObjectPathKeyMap() == other.toObjectPathKeyMap();
                        break;

                    default:
                        qFatal("QDBusData operator== unhandled map key type %d(%s)",
                               d->keyType, QDBusData::typeName(d->keyType));
                        break;
                }

                break;
        }
    }

    return false;
}

bool QDBusData::operator!=(const QDBusData& other) const
{
    return !operator==(other);
}

QDBusData::Type QDBusData::type() const
{
    return d->type;
}

QDBusData::Type QDBusData::keyType() const
{
    if (d->type != QDBusData::Map) return QDBusData::Invalid;

    return d->keyType;
}

const char* QDBusData::typeName(Type type)
{
    switch (type)
    {
        case QDBusData::Invalid:    return "Invalid";
        case QDBusData::Bool:       return "Bool";
        case QDBusData::Byte:       return "Byte";
        case QDBusData::Int16:      return "Int16";
        case QDBusData::UInt16:     return "UInt16";
        case QDBusData::Int32:      return "Int32";
        case QDBusData::UInt32:     return "UInt32";
        case QDBusData::Int64:      return "Int64";
        case QDBusData::UInt64:     return "UInt64";
        case QDBusData::Double:     return "Double";
        case QDBusData::String:     return "String";
        case QDBusData::ObjectPath: return "ObjectPath";
        case QDBusData::List:       return "List";
        case QDBusData::Struct:     return "Struct";
        case QDBusData::Variant:    return "Variant";
        case QDBusData::Map:        return "Map";
    }

    return 0;
}

QDBusData QDBusData::fromBool(bool value)
{
    QDBusData data;

    data.d->type = QDBusData::Bool;
    data.d->value.boolValue = value;

    return data;
}

bool QDBusData::toBool(bool* ok) const
{
    if (d->type != QDBusData::Bool)
    {
        if (ok != 0) *ok = false;
        return false;
    }

    if (ok != 0) *ok = true;

    return d->value.boolValue;
}

QDBusData QDBusData::fromByte(Q_UINT8 value)
{
    QDBusData data;

    data.d->type = QDBusData::Byte;
    data.d->value.byteValue = value;

    return data;
}

Q_UINT8 QDBusData::toByte(bool* ok) const
{
    if (d->type != QDBusData::Byte)
    {
        if (ok != 0) *ok = false;
        return 0;
    }

    if (ok != 0) *ok = true;

    return d->value.byteValue;
}

QDBusData QDBusData::fromInt16(Q_INT16 value)
{
    QDBusData data;

    data.d->type = QDBusData::Int16;
    data.d->value.int16Value = value;

    return data;
}

Q_INT16 QDBusData::toInt16(bool* ok) const
{
    if (d->type != QDBusData::Int16)
    {
        if (ok != 0) *ok = false;
        return 0;
    }

    if (ok != 0) *ok = true;

    return d->value.int16Value;
}

QDBusData QDBusData::fromUInt16(Q_UINT16 value)
{
    QDBusData data;

    data.d->type = QDBusData::UInt16;
    data.d->value.uint16Value = value;

    return data;
}

Q_UINT16 QDBusData::toUInt16(bool* ok) const
{
    if (d->type != QDBusData::UInt16)
    {
        if (ok != 0) *ok = false;
        return 0;
    }

    if (ok != 0) *ok = true;

    return d->value.uint16Value;
}

QDBusData QDBusData::fromInt32(Q_INT32 value)
{
    QDBusData data;

    data.d->type = QDBusData::Int32;
    data.d->value.int32Value = value;

    return data;
}

Q_INT32 QDBusData::toInt32(bool* ok) const
{
    if (d->type != QDBusData::Int32)
    {
        if (ok != 0) *ok = false;
        return 0;
    }

    if (ok != 0) *ok = true;

    return d->value.int32Value;
}

QDBusData QDBusData::fromUInt32(Q_UINT32 value)
{
    QDBusData data;

    data.d->type = QDBusData::UInt32;
    data.d->value.uint32Value = value;

    return data;
}

Q_UINT32 QDBusData::toUInt32(bool* ok) const
{
    if (d->type != QDBusData::UInt32)
    {
        if (ok != 0) *ok = false;
        return 0;
    }

    if (ok != 0) *ok = true;

    return d->value.uint32Value;
}

QDBusData QDBusData::fromInt64(Q_INT64 value)
{
    QDBusData data;

    data.d->type = QDBusData::Int64;
    data.d->value.int64Value = value;

    return data;
}

Q_INT64 QDBusData::toInt64(bool* ok) const
{
    if (d->type != QDBusData::Int64)
    {
        if (ok != 0) *ok = false;
        return 0;
    }

    if (ok != 0) *ok = true;

    return d->value.int64Value;
}

QDBusData QDBusData::fromUInt64(Q_UINT64 value)
{
    QDBusData data;

    data.d->type = QDBusData::UInt64;
    data.d->value.uint64Value = value;

    return data;
}

Q_UINT64 QDBusData::toUInt64(bool* ok) const
{
    if (d->type != QDBusData::UInt64)
    {
        if (ok != 0) *ok = false;
        return 0;
    }

    if (ok != 0) *ok = true;

    return d->value.uint64Value;
}

QDBusData QDBusData::fromDouble(double value)
{
    QDBusData data;

    data.d->type = QDBusData::Double;
    data.d->value.doubleValue = value;

    return data;
}

double QDBusData::toDouble(bool* ok) const
{
    if (d->type != QDBusData::Double)
    {
        if (ok != 0) *ok = false;
        return 0.0;
    }

    if (ok != 0) *ok = true;

    return d->value.doubleValue;
}

QDBusData QDBusData::fromString(const QString& value)
{
    QDBusData data;

    data.d->type = QDBusData::String;
    data.d->value.pointer = new QString(value);

    return data;
}

QString QDBusData::toString(bool* ok) const
{
    if (d->type != QDBusData::String)
    {
        if (ok != 0) *ok = false;
        return QString::null;
    }

    if (ok != 0) *ok = true;

    return *((QString*)d->value.pointer);
}

QDBusData QDBusData::fromObjectPath(const QDBusObjectPath& value)
{
    QDBusData data;

    if (value.isValid())
    {
        data.d->type = QDBusData::ObjectPath;
        data.d->value.pointer = new QDBusObjectPath(value);
    }

    return data;
}

QDBusObjectPath QDBusData::toObjectPath(bool* ok) const
{
    if (d->type != QDBusData::ObjectPath)
    {
        if (ok != 0) *ok = false;
        return QDBusObjectPath();
    }

    if (ok != 0) *ok = true;

    return *((QDBusObjectPath*)d->value.pointer);
}

QDBusData QDBusData::fromList(const QDBusDataList& list)
{
    QDBusData data;

    if (list.type() == QDBusData::Invalid) return data;

    data.d->type = QDBusData::List;
    data.d->value.pointer = new QDBusDataList(list);

    return data;
}

QDBusDataList QDBusData::toList(bool* ok) const
{
    if (d->type != QDBusData::List)
    {
        if (ok != 0) *ok = false;
        return QDBusDataList();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataList*)d->value.pointer);
}

QDBusData QDBusData::fromQValueList(const QValueList<QDBusData>& list)
{
    return fromList(QDBusDataList(list));
}

QValueList<QDBusData> QDBusData::toQValueList(bool* ok) const
{
    bool internalOk = false;
    QDBusDataList list = toList(&internalOk);

    if (!internalOk)
    {
        if (ok != 0) *ok = false;
        return QValueList<QDBusData>();
    }

    return list.toQValueList();
}

QDBusData QDBusData::fromStruct(const QValueList<QDBusData>& memberList)
{
    QDBusData data;

    QValueList<QDBusData>::const_iterator it    = memberList.begin();
    QValueList<QDBusData>::const_iterator endIt = memberList.end();
    for (; it != endIt; ++it)
    {
        if ((*it).d->type == Invalid) return data;
    }

    data.d->type = QDBusData::Struct;
    data.d->value.pointer = new QValueList<QDBusData>(memberList);

    return data;
}

QValueList<QDBusData> QDBusData::toStruct(bool* ok) const
{
    if (d->type != QDBusData::Struct)
    {
        if (ok != 0) *ok = false;
        return QValueList<QDBusData>();
    }

    if (ok != 0) *ok = true;

    return *((QValueList<QDBusData>*)d->value.pointer);
}

QDBusData QDBusData::fromVariant(const QDBusVariant& value)
{
    QDBusData data;

    data.d->type = QDBusData::Variant;
    data.d->value.pointer = new QDBusVariant(value);

    return data;
}

QDBusVariant QDBusData::toVariant(bool* ok) const
{
    if (d->type != QDBusData::Variant)
    {
        if (ok != 0) *ok = false;
        return QDBusVariant();
    }

    if (ok != 0) *ok = true;

    return *((QDBusVariant*)d->value.pointer);
}

QDBusData QDBusData::fromByteKeyMap(const QDBusDataMap<Q_UINT8>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<Q_UINT8>(map);

    return data;
}

QDBusDataMap<Q_UINT8> QDBusData::toByteKeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map && d->keyType != QDBusDataMap<Q_UINT8>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<Q_UINT8>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<Q_UINT8>*)d->value.pointer);
}

QDBusData QDBusData::fromInt16KeyMap(const QDBusDataMap<Q_INT16>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<Q_INT16>(map);

    return data;
}

QDBusDataMap<Q_INT16> QDBusData::toInt16KeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map && d->keyType != QDBusDataMap<Q_INT16>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<Q_INT16>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<Q_INT16>*)d->value.pointer);
}

QDBusData QDBusData::fromUInt16KeyMap(const QDBusDataMap<Q_UINT16>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<Q_UINT16>(map);

    return data;
}

QDBusDataMap<Q_UINT16> QDBusData::toUInt16KeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map &&
        d->keyType != QDBusDataMap<Q_UINT16>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<Q_UINT16>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<Q_UINT16>*)d->value.pointer);
}

QDBusData QDBusData::fromInt32KeyMap(const QDBusDataMap<Q_INT32>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<Q_INT32>(map);

    return data;
}

QDBusDataMap<Q_INT32> QDBusData::toInt32KeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map && d->keyType != QDBusDataMap<Q_INT32>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<Q_INT32>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<Q_INT32>*)d->value.pointer);
}

QDBusData QDBusData::fromUInt32KeyMap(const QDBusDataMap<Q_UINT32>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<Q_UINT32>(map);

    return data;
}

QDBusDataMap<Q_UINT32> QDBusData::toUInt32KeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map &&
        d->keyType != QDBusDataMap<Q_UINT32>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<Q_UINT32>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<Q_UINT32>*)d->value.pointer);
}

QDBusData QDBusData::fromInt64KeyMap(const QDBusDataMap<Q_INT64>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<Q_INT64>(map);

    return data;
}

QDBusDataMap<Q_INT64> QDBusData::toInt64KeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map && d->keyType != QDBusDataMap<Q_INT64>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<Q_INT64>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<Q_INT64>*)d->value.pointer);
}

QDBusData QDBusData::fromUInt64KeyMap(const QDBusDataMap<Q_UINT64>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<Q_UINT64>(map);

    return data;
}

QDBusDataMap<Q_UINT64> QDBusData::toUInt64KeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map &&
        d->keyType != QDBusDataMap<Q_UINT64>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<Q_UINT64>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<Q_UINT64>*)d->value.pointer);
}

QDBusData QDBusData::fromStringKeyMap(const QDBusDataMap<QString>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<QString>(map);

    return data;
}

QDBusDataMap<QString> QDBusData::toStringKeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map && d->keyType != QDBusDataMap<QString>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<QString>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<QString>*)d->value.pointer);
}

QDBusData QDBusData::fromObjectPathKeyMap(const QDBusDataMap<QDBusObjectPath>& map)
{
    QDBusData data;

    data.d->type = QDBusData::Map;
    data.d->keyType = map.keyType();
    data.d->value.pointer = new QDBusDataMap<QDBusObjectPath>(map);

    return data;
}

QDBusDataMap<QDBusObjectPath> QDBusData::toObjectPathKeyMap(bool* ok) const
{
    if (d->type != QDBusData::Map &&
        d->keyType != QDBusDataMap<QDBusObjectPath>::m_keyType)
    {
        if (ok != 0) *ok = false;
        return QDBusDataMap<QDBusObjectPath>();
    }

    if (ok != 0) *ok = true;

    return *((QDBusDataMap<QDBusObjectPath>*)d->value.pointer);
}

static const char* qDBusTypeForQDBusType(QDBusData::Type type)
{
    switch (type)
    {
        case QDBusData::Invalid:
            return 0;
        case QDBusData::Bool:
            return DBUS_TYPE_BOOLEAN_AS_STRING;
        case QDBusData::Byte:
            return DBUS_TYPE_BYTE_AS_STRING;
        case QDBusData::Int16:
            return DBUS_TYPE_INT16_AS_STRING;
        case QDBusData::UInt16:
            return DBUS_TYPE_UINT16_AS_STRING;
        case QDBusData::Int32:
            return DBUS_TYPE_INT32_AS_STRING;
        case QDBusData::UInt32:
            return DBUS_TYPE_UINT32_AS_STRING;
        case QDBusData::Int64:
            return DBUS_TYPE_INT64_AS_STRING;
        case QDBusData::UInt64:
            return DBUS_TYPE_UINT64_AS_STRING;
        case QDBusData::Double:
            return DBUS_TYPE_DOUBLE_AS_STRING;
        case QDBusData::String:
            return DBUS_TYPE_STRING_AS_STRING;
        case QDBusData::ObjectPath:
            return DBUS_TYPE_OBJECT_PATH_AS_STRING;
        case QDBusData::Variant:
            return DBUS_TYPE_VARIANT_AS_STRING;
        default:
            break;
    }
    return 0;
}

template <typename T>
QCString qDBusSignatureForMapValue(const QDBusDataMap<T>& map)
{
    if (map.hasContainerValueType())
        return map.containerValueType().buildDBusSignature();
    else
        return qDBusTypeForQDBusType(map.valueType());
}

QCString QDBusData::buildDBusSignature() const
{
    QCString signature;

    switch (d->type)
    {
        case QDBusData::List:
        {
            QDBusDataList* list = (QDBusDataList*) d->value.pointer;
            signature = DBUS_TYPE_ARRAY_AS_STRING;
            if (list->hasContainerItemType())
                signature += list->containerItemType().buildDBusSignature();
            else
                signature += qDBusTypeForQDBusType(list->type());
            break;
        }

        case QDBusData::Struct:
        {
            signature += DBUS_STRUCT_BEGIN_CHAR;

            QValueList<QDBusData>* memberList =
                (QValueList<QDBusData>*) d->value.pointer;

            QValueList<QDBusData>::const_iterator it    = (*memberList).begin();
            QValueList<QDBusData>::const_iterator endIt = (*memberList).end();
            for (; it != endIt; ++it)
            {
                signature += (*it).buildDBusSignature();
            }
            signature += DBUS_STRUCT_END_CHAR;
            break;
        }

        case QDBusData::Map:
            signature += DBUS_TYPE_ARRAY_AS_STRING;
            signature += DBUS_DICT_ENTRY_BEGIN_CHAR;

            signature += qDBusTypeForQDBusType(keyType());

            switch (keyType())
            {
                case QDBusData::Byte:
                    signature += qDBusSignatureForMapValue<Q_UINT8>(
                        *((QDBusDataMap<Q_UINT8>*) d->value.pointer));
                    break;
                case QDBusData::Int16:
                    signature += qDBusSignatureForMapValue<Q_INT16>(
                        *((QDBusDataMap<Q_INT16>*) d->value.pointer));
                    break;
                case QDBusData::UInt16:
                    signature += qDBusSignatureForMapValue<Q_UINT16>(
                        *((QDBusDataMap<Q_UINT16>*) d->value.pointer));
                    break;
                case QDBusData::Int32:
                    signature += qDBusSignatureForMapValue<Q_INT32>(
                        *((QDBusDataMap<Q_INT32>*) d->value.pointer));
                    break;
                case QDBusData::UInt32:
                    signature += qDBusSignatureForMapValue<Q_UINT32>(
                        *((QDBusDataMap<Q_UINT32>*) d->value.pointer));
                    break;
                case QDBusData::Int64:
                    signature += qDBusSignatureForMapValue<Q_INT64>(
                        *((QDBusDataMap<Q_INT64>*) d->value.pointer));
                    break;
                case QDBusData::UInt64:
                    signature += qDBusSignatureForMapValue<Q_UINT64>(
                        *((QDBusDataMap<Q_UINT64>*) d->value.pointer));
                    break;
                case QDBusData::String:
                    signature += qDBusSignatureForMapValue<QString>(
                        *((QDBusDataMap<QString>*) d->value.pointer));
                    break;
                case QDBusData::ObjectPath:
                    signature += qDBusSignatureForMapValue<QDBusObjectPath>(
                        *((QDBusDataMap<QDBusObjectPath>*) d->value.pointer));
                    break;
                default:
                    break;
            }

            signature += DBUS_DICT_ENTRY_END_CHAR;
            break;

        default:
            signature = qDBusTypeForQDBusType(d->type);
            break;
    }

    return signature;
}


// key type definitions for QDBusDataMap
template <>
const QDBusData::Type QDBusDataMap<Q_UINT8>::m_keyType = QDBusData::Byte;

template <>
const QDBusData::Type QDBusDataMap<Q_INT16>::m_keyType = QDBusData::Int16;

template <>
const QDBusData::Type QDBusDataMap<Q_UINT16>::m_keyType = QDBusData::UInt16;

template <>
const QDBusData::Type QDBusDataMap<Q_INT32>::m_keyType = QDBusData::Int32;

template <>
const QDBusData::Type QDBusDataMap<Q_UINT32>::m_keyType = QDBusData::UInt32;

template <>
const QDBusData::Type QDBusDataMap<Q_INT64>::m_keyType = QDBusData::Int64;

template <>
const QDBusData::Type QDBusDataMap<Q_UINT64>::m_keyType = QDBusData::UInt64;

template <>
const QDBusData::Type QDBusDataMap<QString>::m_keyType = QDBusData::String;

template <>
const QDBusData::Type QDBusDataMap<QDBusObjectPath>::m_keyType =
        QDBusData::ObjectPath;
