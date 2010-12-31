/* qdbusmarshall.cpp
 *
 * Copyright (C) 2005 Harald Fernengel <harry@kdevelop.org>
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

#include "qdbusmarshall.h"
#include "dbus/qdbusdata.h"
#include "dbus/qdbusdatalist.h"
#include "dbus/qdbusdatamap.h"
#include "dbus/qdbusobjectpath.h"
#include "dbus/qdbusvariant.h"

#include <qvariant.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qstringlist.h>
#include <qvaluevector.h>

#include <dbus/dbus.h>

template <typename T>
inline T qIterGet(DBusMessageIter *it)
{
    T t;
    dbus_message_iter_get_basic(it, &t);
    return t;
}

static QDBusData::Type qSingleTypeForDBusSignature(char signature)
{
    switch (signature)
    {
        case 'b': return QDBusData::Bool;
        case 'y': return QDBusData::Byte;
        case 'n': return QDBusData::Int16;
        case 'q': return QDBusData::UInt16;
        case 'i': return QDBusData::Int32;
        case 'u': return QDBusData::UInt32;
        case 'x': return QDBusData::Int64;
        case 't': return QDBusData::UInt64;
        case 'd': return QDBusData::Double;
        case 's': return QDBusData::String;
        case 'o': return QDBusData::ObjectPath;
        case 'g': return QDBusData::String;
        case 'v': return QDBusData::Variant;

        default:
            break;
    }

    return QDBusData::Invalid;
}

static QValueList<QDBusData> parseSignature(QCString& signature)
{
//    qDebug("parseSignature(%s)", signature.data());
    QValueList<QDBusData> result;

    while (!signature.isEmpty())
    {
        switch (signature[0])
        {
            case '(': {
                signature = signature.mid(1);
                QValueList<QDBusData> memberList = parseSignature(signature);
                result << QDBusData::fromStruct(memberList);
                Q_ASSERT(!signature.isEmpty() && signature[0] == ')');
                signature = signature.mid(1);
                break;
            }
            case ')': return result;
            case '{': {
                QDBusData::Type keyType =
                    qSingleTypeForDBusSignature(signature[1]);
                QDBusData::Type valueType =
                    qSingleTypeForDBusSignature(signature[2]);
                if (valueType != QDBusData::Invalid)
                {
                    switch (keyType)
                    {
                        case QDBusData::Byte:
                            result << QDBusData::fromByteKeyMap(
                                QDBusDataMap<Q_UINT8>(valueType));
                            break;
                        case QDBusData::Int16:
                            result << QDBusData::fromInt16KeyMap(
                                QDBusDataMap<Q_INT16>(valueType));
                            break;
                        case QDBusData::UInt16:
                            result << QDBusData::fromUInt16KeyMap(
                                QDBusDataMap<Q_UINT16>(valueType));
                            break;
                        case QDBusData::Int32:
                            result << QDBusData::fromInt32KeyMap(
                                QDBusDataMap<Q_INT32>(valueType));
                            break;
                        case QDBusData::UInt32:
                            result << QDBusData::fromUInt32KeyMap(
                                QDBusDataMap<Q_UINT32>(valueType));
                            break;
                        case QDBusData::Int64:
                            result << QDBusData::fromInt64KeyMap(
                                QDBusDataMap<Q_INT64>(valueType));
                            break;
                        case QDBusData::UInt64:
                            result << QDBusData::fromUInt64KeyMap(
                                QDBusDataMap<Q_UINT64>(valueType));
                            break;
                        case QDBusData::String:
                            result << QDBusData::fromStringKeyMap(
                                QDBusDataMap<QString>(valueType));
                            break;
                        case QDBusData::ObjectPath:
                            result << QDBusData::fromObjectPathKeyMap(
                                QDBusDataMap<QDBusObjectPath>(valueType));
                            break;
                        default:
                            qWarning("QDBusMarshall: unsupported map key type %s "
                                     "at de-marshalling",
                                     QDBusData::typeName(keyType));
                            break;
                    }
                    signature = signature.mid(3);
                }
                else
                {
                    signature = signature.mid(2);
                    QValueList<QDBusData> valueContainer =
                        parseSignature(signature);
                    Q_ASSERT(valueContainer.count() == 1);

                    switch (keyType)
                    {
                        case QDBusData::Byte:
                            result << QDBusData::fromByteKeyMap(
                                QDBusDataMap<Q_UINT8>(valueContainer[0]));
                            break;
                        case QDBusData::Int16:
                            result << QDBusData::fromInt16KeyMap(
                                QDBusDataMap<Q_INT16>(valueContainer[0]));
                            break;
                        case QDBusData::UInt16:
                            result << QDBusData::fromUInt16KeyMap(
                                QDBusDataMap<Q_UINT16>(valueContainer[0]));
                            break;
                        case QDBusData::Int32:
                            result << QDBusData::fromInt32KeyMap(
                                QDBusDataMap<Q_INT32>(valueContainer[0]));
                            break;
                        case QDBusData::UInt32:
                            result << QDBusData::fromUInt32KeyMap(
                                QDBusDataMap<Q_UINT32>(valueContainer[0]));
                            break;
                        case QDBusData::Int64:
                            result << QDBusData::fromInt64KeyMap(
                                QDBusDataMap<Q_INT64>(valueContainer[0]));
                            break;
                        case QDBusData::UInt64:
                            result << QDBusData::fromUInt64KeyMap(
                                QDBusDataMap<Q_UINT64>(valueContainer[0]));
                            break;
                        case QDBusData::String:
                            result << QDBusData::fromStringKeyMap(
                                QDBusDataMap<QString>(valueContainer[0]));
                            break;
                        case QDBusData::ObjectPath:
                            result << QDBusData::fromObjectPathKeyMap(
                                QDBusDataMap<QDBusObjectPath>(valueContainer[0]));
                            break;
                        default:
                            qWarning("QDBusMarshall: unsupported map key type %s "
                                     "at de-marshalling",
                                     QDBusData::typeName(keyType));
                            break;
                    }
                }
                Q_ASSERT(!signature.isEmpty() && signature[0] == '}');
                signature = signature.mid(1);
                break;
            }
            case '}': return result;
            case 'a': {
                QDBusData::Type elementType =
                    qSingleTypeForDBusSignature(signature[1]);
                if (elementType != QDBusData::Invalid)
                {
                    QDBusDataList list(elementType);
                    result << QDBusData::fromList(list);
                    signature = signature.mid(2);
                }
                else
                {
                    signature = signature.mid(1);
                    bool array = signature[0] != '{';

                    QValueList<QDBusData> elementContainer =
                        parseSignature(signature);
                    Q_ASSERT(elementContainer.count() == 1);

                    if (array)
                    {
                        QDBusDataList list(elementContainer[0]);
                        result << QDBusData::fromList(list);
                    }
                    else
                        result << elementContainer[0];
                }
                break;
            }
            default:
                result << QDBusData();
                signature = signature.mid(1);
                break;
        }
    }

    return result;
}

static QDBusData qFetchParameter(DBusMessageIter *it);

void qFetchByteKeyMapEntry(QDBusDataMap<Q_UINT8>& map, DBusMessageIter* it)
{
    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    Q_ASSERT(dbus_message_iter_has_next(&itemIter));

    Q_UINT8 key = qFetchParameter(&itemIter).toByte();

    dbus_message_iter_next(&itemIter);

    map.insert(key, qFetchParameter(&itemIter));
}

void qFetchInt16KeyMapEntry(QDBusDataMap<Q_INT16>& map, DBusMessageIter* it)
{
    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    Q_ASSERT(dbus_message_iter_has_next(&itemIter));

    Q_INT16 key = qFetchParameter(&itemIter).toInt16();

    dbus_message_iter_next(&itemIter);

    map.insert(key, qFetchParameter(&itemIter));
}

void qFetchUInt16KeyMapEntry(QDBusDataMap<Q_UINT16>& map, DBusMessageIter* it)
{
    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    Q_ASSERT(dbus_message_iter_has_next(&itemIter));

    Q_UINT16 key = qFetchParameter(&itemIter).toUInt16();

    dbus_message_iter_next(&itemIter);

    map.insert(key, qFetchParameter(&itemIter));
}

void qFetchInt32KeyMapEntry(QDBusDataMap<Q_INT32>& map, DBusMessageIter* it)
{
    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    Q_ASSERT(dbus_message_iter_has_next(&itemIter));

    Q_INT32 key = qFetchParameter(&itemIter).toInt32();

    dbus_message_iter_next(&itemIter);

    map.insert(key, qFetchParameter(&itemIter));
}

void qFetchUInt32KeyMapEntry(QDBusDataMap<Q_UINT32>& map, DBusMessageIter* it)
{
    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    Q_ASSERT(dbus_message_iter_has_next(&itemIter));

    Q_UINT32 key = qFetchParameter(&itemIter).toUInt32();

    dbus_message_iter_next(&itemIter);

    map.insert(key, qFetchParameter(&itemIter));
}

void qFetchInt64KeyMapEntry(QDBusDataMap<Q_INT64>& map, DBusMessageIter* it)
{
    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    Q_ASSERT(dbus_message_iter_has_next(&itemIter));

    Q_INT64 key = qFetchParameter(&itemIter).toInt64();

    dbus_message_iter_next(&itemIter);

    map.insert(key, qFetchParameter(&itemIter));
}

void qFetchUInt64KeyMapEntry(QDBusDataMap<Q_UINT64>& map, DBusMessageIter* it)
{
    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    Q_ASSERT(dbus_message_iter_has_next(&itemIter));

    Q_UINT64 key = qFetchParameter(&itemIter).toUInt64();

    dbus_message_iter_next(&itemIter);

    map.insert(key, qFetchParameter(&itemIter));
}

void qFetchStringKeyMapEntry(QDBusDataMap<QString>& map, DBusMessageIter* it)
{
    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    Q_ASSERT(dbus_message_iter_has_next(&itemIter));

    QString key = qFetchParameter(&itemIter).toString();

    dbus_message_iter_next(&itemIter);

    map.insert(key, qFetchParameter(&itemIter));
}

static QDBusData qFetchMap(DBusMessageIter *it, const QDBusData& prototype)
{
    if (dbus_message_iter_get_arg_type(it) == DBUS_TYPE_INVALID)
        return prototype;

    DBusMessageIter itemIter;
    dbus_message_iter_recurse(it, &itemIter);
    if (dbus_message_iter_get_arg_type(&itemIter) == DBUS_TYPE_INVALID)
        return prototype;

    switch (dbus_message_iter_get_arg_type(&itemIter)) {
        case DBUS_TYPE_BYTE: {
            QDBusDataMap<Q_UINT8> map = prototype.toByteKeyMap();
            do {
                qFetchByteKeyMapEntry(map, it);
            } while (dbus_message_iter_next(it));

            return QDBusData::fromByteKeyMap(map);
        }

        case DBUS_TYPE_INT16: {
            QDBusDataMap<Q_INT16> map = prototype.toInt16KeyMap();
            do {
                qFetchInt16KeyMapEntry(map, it);
            } while (dbus_message_iter_next(it));

            return QDBusData::fromInt16KeyMap(map);
        }

        case DBUS_TYPE_UINT16: {
            QDBusDataMap<Q_UINT16> map = prototype.toUInt16KeyMap();
            do {
                qFetchUInt16KeyMapEntry(map, it);
            } while (dbus_message_iter_next(it));

            return QDBusData::fromUInt16KeyMap(map);
        }

        case DBUS_TYPE_INT32: {
            QDBusDataMap<Q_INT32> map = prototype.toInt32KeyMap();
            do {
                qFetchInt32KeyMapEntry(map, it);
            } while (dbus_message_iter_next(it));

            return QDBusData::fromInt32KeyMap(map);
        }

        case DBUS_TYPE_UINT32: {
            QDBusDataMap<Q_UINT32> map = prototype.toUInt32KeyMap();
            do {
                qFetchUInt32KeyMapEntry(map, it);
            } while (dbus_message_iter_next(it));

            return QDBusData::fromUInt32KeyMap(map);
        }

        case DBUS_TYPE_INT64: {
            QDBusDataMap<Q_INT64> map = prototype.toInt64KeyMap();
            do {
                qFetchInt64KeyMapEntry(map, it);
            } while (dbus_message_iter_next(it));

            return QDBusData::fromInt64KeyMap(map);
        }

        case DBUS_TYPE_UINT64: {
            QDBusDataMap<Q_UINT64> map = prototype.toUInt64KeyMap();
            do {
                qFetchUInt64KeyMapEntry(map, it);
            } while (dbus_message_iter_next(it));

            return QDBusData::fromUInt64KeyMap(map);
        }

        case DBUS_TYPE_STRING:      // fall through
        case DBUS_TYPE_OBJECT_PATH: // fall through
        case DBUS_TYPE_SIGNATURE: {
            QDBusDataMap<QString> map = prototype.toStringKeyMap();
            do {
                qFetchStringKeyMapEntry(map, it);
            } while (dbus_message_iter_next(it));

            return QDBusData::fromStringKeyMap(map);
        }

        default:
            break;
    }

    return prototype;
}

static QDBusData qFetchParameter(DBusMessageIter *it)
{
    switch (dbus_message_iter_get_arg_type(it)) {
    case DBUS_TYPE_BOOLEAN:
        return QDBusData::fromBool(qIterGet<dbus_bool_t>(it));
    case DBUS_TYPE_BYTE:
        return QDBusData::fromByte(qIterGet<unsigned char>(it));
    case DBUS_TYPE_INT16:
       return QDBusData::fromInt16(qIterGet<dbus_int16_t>(it));
    case DBUS_TYPE_UINT16:
        return QDBusData::fromUInt16(qIterGet<dbus_uint16_t>(it));
    case DBUS_TYPE_INT32:
        return QDBusData::fromInt32(qIterGet<dbus_int32_t>(it));
    case DBUS_TYPE_UINT32:
        return QDBusData::fromUInt32(qIterGet<dbus_uint32_t>(it));
    case DBUS_TYPE_INT64:
        return QDBusData::fromInt64(qIterGet<dbus_int64_t>(it));
    case DBUS_TYPE_UINT64:
        return QDBusData::fromUInt64(qIterGet<dbus_uint64_t>(it));
    case DBUS_TYPE_DOUBLE:
        return QDBusData::fromDouble(qIterGet<double>(it));
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_SIGNATURE:
        return QDBusData::fromString(QString::fromUtf8(qIterGet<char *>(it)));
    case DBUS_TYPE_OBJECT_PATH:
        return QDBusData::fromObjectPath(QDBusObjectPath(qIterGet<char *>(it)));
    case DBUS_TYPE_ARRAY: {
        int arrayType = dbus_message_iter_get_element_type(it);

        char* sig = dbus_message_iter_get_signature(it);
        QCString signature = sig;
        dbus_free(sig);

        QValueList<QDBusData> prototypeList = parseSignature(signature);

        if (arrayType == DBUS_TYPE_DICT_ENTRY) {
            DBusMessageIter sub;
            dbus_message_iter_recurse(it, &sub);

            return qFetchMap(&sub, prototypeList[0]);

//        } else if (arrayType == DBUS_TYPE_BYTE) {
//            DBusMessageIter sub;
//            dbus_message_iter_recurse(it, &sub);
//            int len = dbus_message_iter_get_array_len(&sub);
//            char* data;
//            dbus_message_iter_get_fixed_array(&sub,&data,&len);
//            return QCString(data,len);
//         } else {

        } else {
            QDBusDataList list = prototypeList[0].toList();

            DBusMessageIter arrayIt;
            dbus_message_iter_recurse(it, &arrayIt);

            while (dbus_message_iter_get_arg_type(&arrayIt) != DBUS_TYPE_INVALID) {
                list << qFetchParameter(&arrayIt);

                dbus_message_iter_next(&arrayIt);
            }

            return QDBusData::fromList(list);
        }
    }
    case DBUS_TYPE_VARIANT: {
        QDBusVariant dvariant;
        DBusMessageIter sub;
        dbus_message_iter_recurse(it, &sub);

        char* signature = dbus_message_iter_get_signature(&sub);
        dvariant.signature = QString::fromUtf8(signature);
        dbus_free(signature);

        dvariant.value = qFetchParameter(&sub);

        return QDBusData::fromVariant(dvariant);
    }
    case DBUS_TYPE_STRUCT: {
        QValueList<QDBusData> memberList;

        DBusMessageIter subIt;
        dbus_message_iter_recurse(it, &subIt);

        uint index = 0;
        while (dbus_message_iter_get_arg_type(&subIt) != DBUS_TYPE_INVALID) {
            memberList << qFetchParameter(&subIt);

            dbus_message_iter_next(&subIt);
            ++index;
        }

        return QDBusData::fromStruct(memberList);
    }
#if 0
    case DBUS_TYPE_INVALID:
        // TODO: check if there is better way to detect empty arrays
        return QDBusData();
        break;
#endif
    default:
        qWarning("QDBusMarshall: Don't know how to de-marshall type %d '%c'",
                 dbus_message_iter_get_arg_type(it),
                 dbus_message_iter_get_arg_type(it));
        return QDBusData();
        break;
    }
}

void QDBusMarshall::messageToList(QValueList<QDBusData>& list, DBusMessage* message)
{
    Q_ASSERT(message);

    DBusMessageIter it;
    if (!dbus_message_iter_init(message, &it)) return;

    do
    {
        list << qFetchParameter(&it);
    }
    while (dbus_message_iter_next(&it));
}

static void qAppendToMessage(DBusMessageIter *it, const QString &str)
{
    QByteArray ba = str.utf8();
    const char *cdata = ba.data();
    dbus_message_iter_append_basic(it, DBUS_TYPE_STRING, &cdata);
}

static void qAppendToMessage(DBusMessageIter *it, const QDBusObjectPath &path)
{
    const char *cdata = path.data();
    dbus_message_iter_append_basic(it, DBUS_TYPE_OBJECT_PATH, &cdata);
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

        case QDBusData::List:
            return DBUS_TYPE_ARRAY_AS_STRING;

        case QDBusData::Struct:
            return DBUS_TYPE_STRUCT_AS_STRING;

        case QDBusData::Variant:
            return DBUS_TYPE_VARIANT_AS_STRING;

        case QDBusData::Map:
            return DBUS_TYPE_DICT_ENTRY_AS_STRING;
    }
    return 0;
}

static void qDBusDataToIterator(DBusMessageIter* it, const QDBusData& var);

static void qDBusByteKeyMapToIterator(DBusMessageIter* it, const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<Q_UINT8> map = var.toByteKeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<Q_UINT8>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        dbus_message_iter_append_basic(it, DBUS_TYPE_BYTE, &(mit.key()));
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusInt16KeyMapToIterator(DBusMessageIter* it, const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<Q_INT16> map = var.toInt16KeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<Q_INT16>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        dbus_message_iter_append_basic(it, DBUS_TYPE_INT16, &(mit.key()));
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusUInt16KeyMapToIterator(DBusMessageIter* it, const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<Q_UINT16> map = var.toUInt16KeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<Q_UINT16>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        dbus_message_iter_append_basic(it, DBUS_TYPE_UINT16, &(mit.key()));
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusInt32KeyMapToIterator(DBusMessageIter* it, const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<Q_INT32> map = var.toInt32KeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<Q_INT32>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &(mit.key()));
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusUInt32KeyMapToIterator(DBusMessageIter* it, const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<Q_UINT32> map = var.toUInt32KeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<Q_UINT32>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        dbus_message_iter_append_basic(it, DBUS_TYPE_UINT32, &(mit.key()));
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusInt64KeyMapToIterator(DBusMessageIter* it, const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<Q_INT64> map = var.toInt64KeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<Q_INT64>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        dbus_message_iter_append_basic(it, DBUS_TYPE_INT64, &(mit.key()));
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusUInt64KeyMapToIterator(DBusMessageIter* it, const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<Q_UINT64> map = var.toUInt64KeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<Q_UINT64>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        dbus_message_iter_append_basic(it, DBUS_TYPE_UINT64, &(mit.key()));
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusStringKeyMapToIterator(DBusMessageIter* it, const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<QString> map = var.toStringKeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<QString>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        qAppendToMessage(&itemIterator, mit.key());
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusObjectPathKeyMapToIterator(DBusMessageIter* it,
                                            const QDBusData& var)
{
    DBusMessageIter sub;
    QCString sig;

    QDBusDataMap<QDBusObjectPath> map = var.toObjectPathKeyMap();

    sig += DBUS_DICT_ENTRY_BEGIN_CHAR;
    sig += qDBusTypeForQDBusType(map.keyType());

    if (map.hasContainerValueType())
        sig += map.containerValueType().buildDBusSignature();
    else
        sig += qDBusTypeForQDBusType(map.valueType());
    sig += DBUS_DICT_ENTRY_END_CHAR;

    dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, sig.data(), &sub);

    QDBusDataMap<QDBusObjectPath>::const_iterator mit = map.begin();
    for (; mit != map.end(); ++mit)
    {
        DBusMessageIter itemIterator;
        dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY,
                                         0, &itemIterator);

        qAppendToMessage(&itemIterator, mit.key());
        qDBusDataToIterator(&itemIterator, mit.data());

        dbus_message_iter_close_container(&sub, &itemIterator);
    }

    dbus_message_iter_close_container(it, &sub);
}

static void qDBusDataToIterator(DBusMessageIter* it, const QDBusData& var)
{
    switch (var.type())
    {
        case QDBusData::Bool:
        {
            dbus_bool_t value = var.toBool();
            dbus_message_iter_append_basic(it, DBUS_TYPE_BOOLEAN, &value);
            break;
        }
        case QDBusData::Byte:
        {
            Q_UINT8 value = var.toByte();
            dbus_message_iter_append_basic(it, DBUS_TYPE_BYTE, &value);
            break;
        }
        case QDBusData::Int16: {
            Q_INT16 value = var.toInt16();
            dbus_message_iter_append_basic(it, DBUS_TYPE_INT16, &value);
            break;
        }
        case QDBusData::UInt16: {
            Q_UINT16 value = var.toUInt16();
            dbus_message_iter_append_basic(it, DBUS_TYPE_UINT16, &value);
            break;
        }
        case QDBusData::Int32: {
            Q_INT32 value = var.toInt32();
            dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &value);
            break;
        }
        case QDBusData::UInt32: {
            Q_UINT32 value = var.toUInt32();
            dbus_message_iter_append_basic(it, DBUS_TYPE_UINT32, &value);
            break;
        }
        case QDBusData::Int64: {
            Q_INT64 value = var.toInt64();
            dbus_message_iter_append_basic(it, DBUS_TYPE_INT64, &value);
            break;
        }
        case QDBusData::UInt64: {
            Q_UINT64 value = var.toUInt64();
            dbus_message_iter_append_basic(it, DBUS_TYPE_UINT64, &value);
            break;
        }
        case QDBusData::Double: {
            double value = var.toDouble();
            dbus_message_iter_append_basic(it, DBUS_TYPE_DOUBLE, &value);
            break;
        }
        case QDBusData::String:
            qAppendToMessage(it, var.toString());
            break;
        case QDBusData::ObjectPath:
            qAppendToMessage(it, var.toObjectPath());
            break;
        case QDBusData::List: {
            QDBusDataList list = var.toList();

            QCString signature = 0;
            if (list.hasContainerItemType())
                signature = list.containerItemType().buildDBusSignature();
            else
                signature = qDBusTypeForQDBusType(list.type());

            DBusMessageIter sub;
            dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY,
                                             signature.data(), &sub);

            const QValueList<QDBusData> valueList = var.toQValueList();
            QValueList<QDBusData>::const_iterator listIt    = valueList.begin();
            QValueList<QDBusData>::const_iterator listEndIt = valueList.end();
            for (; listIt != listEndIt; ++listIt)
            {
                qDBusDataToIterator(&sub, *listIt);
            }
            dbus_message_iter_close_container(it, &sub);
            break;
        }
        case QDBusData::Map: {
            switch (var.keyType()) {
                case QDBusData::Byte:
                    qDBusByteKeyMapToIterator(it, var);
                    break;
                case QDBusData::Int16:
                    qDBusInt16KeyMapToIterator(it, var);
                    break;
                case QDBusData::UInt16:
                    qDBusUInt16KeyMapToIterator(it, var);
                    break;
                case QDBusData::Int32:
                    qDBusInt32KeyMapToIterator(it, var);
                    break;
                case QDBusData::UInt32:
                    qDBusUInt32KeyMapToIterator(it, var);
                    break;
                case QDBusData::Int64:
                    qDBusInt64KeyMapToIterator(it, var);
                    break;
                case QDBusData::UInt64:
                    qDBusUInt64KeyMapToIterator(it, var);
                    break;
                case QDBusData::String:
                    qDBusStringKeyMapToIterator(it, var);
                    break;
                case QDBusData::ObjectPath:
                    qDBusObjectPathKeyMapToIterator(it, var);
                    break;
                default:
                    qWarning("QDBusMarshall: unhandled map key type %s "
                             "at marshalling",
                             QDBusData::typeName(var.keyType()));
                    break;
            }
            break;
        }
        case QDBusData::Variant: {
            QDBusVariant variant = var.toVariant();
            if (variant.signature.isEmpty() || !variant.value.isValid()) break;

            DBusMessageIter sub;
            dbus_message_iter_open_container(it, DBUS_TYPE_VARIANT,
                                             variant.signature.utf8(), &sub);

            qDBusDataToIterator(&sub, variant.value);

            dbus_message_iter_close_container(it, &sub);
            break;
        }
        case QDBusData::Struct: {
            QValueList<QDBusData> memberList = var.toStruct();
            if (memberList.isEmpty()) break;

            DBusMessageIter sub;
            dbus_message_iter_open_container(it, DBUS_TYPE_STRUCT, NULL, &sub);

            QValueList<QDBusData>::const_iterator memberIt    = memberList.begin();
            QValueList<QDBusData>::const_iterator memberEndIt = memberList.end();
            for (; memberIt != memberEndIt; ++memberIt)
            {
                qDBusDataToIterator(&sub, *memberIt);
            }

            dbus_message_iter_close_container(it, &sub);
        }
#if 0
        case QVariant::ByteArray: {
            const QByteArray array = var.toByteArray();
            const char* cdata = array.data();
                DBusMessageIter sub;
                dbus_message_iter_open_container(it, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &sub);
            dbus_message_iter_append_fixed_array(&sub, DBUS_TYPE_BYTE, &cdata, array.size());
                dbus_message_iter_close_container(it, &sub);
        break;
        }
#endif
        default:
            //qWarning("Don't know how to handle type %s", var.typeName());
            break;
    }
}

void qListToIterator(DBusMessageIter* it, const QValueList<QDBusData>& list)
{
    if (list.isEmpty()) return;

    QValueList<QDBusData>::const_iterator listIt    = list.begin();
    QValueList<QDBusData>::const_iterator listEndIt = list.end();
    for (; listIt != listEndIt; ++listIt)
    {
        qDBusDataToIterator(it, *listIt);
    }
}

void QDBusMarshall::listToMessage(const QValueList<QDBusData> &list, DBusMessage *msg)
{
    Q_ASSERT(msg);
    DBusMessageIter it;
    dbus_message_iter_init_append(msg, &it);
    qListToIterator(&it, list);
}
