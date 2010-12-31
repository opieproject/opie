/* qdbusdatalist.cpp list of DBUS data transport type
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

#include "dbus/qdbusdatalist.h"
#include "dbus/qdbusobjectpath.h"
#include "dbus/qdbusvariant.h"

#include <qstringlist.h>

class QDBusDataList::Private
{
public:
    Private() : type(QDBusData::Invalid) {}

public:
    QDBusData::Type type;
    QDBusData containerItem;
    QValueList<QDBusData> list;
};

QDBusDataList::QDBusDataList() : d(new Private())
{
}

QDBusDataList::QDBusDataList(QDBusData::Type simpleItemType) : d(new Private())
{
    d->type = simpleItemType;
}

QDBusDataList::QDBusDataList(const QDBusData& containerItemType) : d(new Private())
{
    d->type = containerItemType.type();

    switch(d->type)
    {
        case QDBusData::List:   // fall through
        case QDBusData::Struct: // fall through
        case QDBusData::Map:
            d->containerItem = containerItemType;
            break;

        default:   // not a container
            break;
    }
}

QDBusDataList::QDBusDataList(const QDBusDataList& other) : d(new Private())
{
    d->type = other.d->type;
    d->list = other.d->list;
    d->containerItem = other.d->containerItem;
}

QDBusDataList::QDBusDataList(const QValueList<QDBusData>& other) : d(new Private())
{
    if (other.isEmpty()) return;

    QValueList<QDBusData>::const_iterator it    = other.begin();
    QValueList<QDBusData>::const_iterator endIt = other.end();

    d->type = (*it).type();

    QCString elementSignature;
    if (hasContainerItemType())
    {
        d->containerItem = other[0]; // would be nice to get an empty one
        elementSignature = d->containerItem.buildDBusSignature();
    }

    for (++it; it != endIt; ++it)
    {
        if (d->type != (*it).type())
        {
            d->type = QDBusData::Invalid;
            d->containerItem = QDBusData();

            return;
        }
        else if (hasContainerItemType())
        {
            if ((*it).buildDBusSignature() != elementSignature)
            {
                d->type = QDBusData::Invalid;
                d->containerItem = QDBusData();

                return;
            }
        }
    }

    d->list = other;
}

QDBusDataList::QDBusDataList(const QValueList<bool>& other) : d(new Private())
{
    d->type = QDBusData::Bool;

    if (other.isEmpty()) return;

    QValueList<bool>::const_iterator it    = other.begin();
    QValueList<bool>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromBool(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<Q_UINT8>& other) : d(new Private())
{
    d->type = QDBusData::Byte;

    if (other.isEmpty()) return;

    QValueList<Q_UINT8>::const_iterator it    = other.begin();
    QValueList<Q_UINT8>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromByte(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<Q_INT16>& other) : d(new Private())
{
    d->type = QDBusData::Int16;

    if (other.isEmpty()) return;

    QValueList<Q_INT16>::const_iterator it    = other.begin();
    QValueList<Q_INT16>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromInt16(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<Q_UINT16>& other) : d(new Private())
{
    d->type = QDBusData::UInt16;

    if (other.isEmpty()) return;

    QValueList<Q_UINT16>::const_iterator it    = other.begin();
    QValueList<Q_UINT16>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromUInt16(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<Q_INT32>& other) : d(new Private())
{
    d->type = QDBusData::Int32;

    if (other.isEmpty()) return;

    QValueList<Q_INT32>::const_iterator it    = other.begin();
    QValueList<Q_INT32>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromInt32(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<Q_UINT32>& other) : d(new Private())
{
    d->type = QDBusData::UInt32;

    if (other.isEmpty()) return;

    QValueList<Q_UINT32>::const_iterator it    = other.begin();
    QValueList<Q_UINT32>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromUInt32(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<Q_INT64>& other) : d(new Private())
{
    d->type = QDBusData::Int64;

    if (other.isEmpty()) return;

    QValueList<Q_INT64>::const_iterator it    = other.begin();
    QValueList<Q_INT64>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromInt64(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<Q_UINT64>& other) : d(new Private())
{
    d->type = QDBusData::UInt64;

    if (other.isEmpty()) return;

    QValueList<Q_UINT64>::const_iterator it    = other.begin();
    QValueList<Q_UINT64>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromUInt64(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<double>& other) : d(new Private())
{
    d->type = QDBusData::Double;

    if (other.isEmpty()) return;

    QValueList<double>::const_iterator it    = other.begin();
    QValueList<double>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromDouble(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<QDBusVariant>& other)
    : d(new Private())
{
    d->type = QDBusData::Variant;

    if (other.isEmpty()) return;

    QValueList<QDBusVariant>::const_iterator it    = other.begin();
    QValueList<QDBusVariant>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromVariant(*it);
    }
}

QDBusDataList::QDBusDataList(const QStringList& other) : d(new Private())
{
    d->type = QDBusData::String;

    if (other.isEmpty()) return;

    QStringList::const_iterator it    = other.begin();
    QStringList::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromString(*it);
    }
}

QDBusDataList::QDBusDataList(const QValueList<QDBusObjectPath>& other)
    : d(new Private())
{
    d->type = QDBusData::ObjectPath;

    if (other.isEmpty()) return;

    QValueList<QDBusObjectPath>::const_iterator it    = other.begin();
    QValueList<QDBusObjectPath>::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromObjectPath(*it);
    }
}

QDBusDataList::~QDBusDataList()
{
    delete d;
}

QDBusDataList& QDBusDataList::operator=(const QDBusDataList& other)
{
    if (&other == this) return *this;

    d->type = other.d->type;
    d->list = other.d->list;
    d->containerItem = other.d->containerItem;

    return *this;
}

QDBusDataList& QDBusDataList::operator=(const QValueList<QDBusData>& other)
{
    d->list.clear();
    d->type = QDBusData::Invalid;
    d->containerItem = QDBusData();

    if (other.isEmpty()) return *this;

    QValueList<QDBusData>::const_iterator it    = other.begin();
    QValueList<QDBusData>::const_iterator endIt = other.end();

    d->type = (*it).type();

    QCString elementSignature;
    if (hasContainerItemType())
    {
        d->containerItem = other[0]; // would be nice to get an empty one

        elementSignature = d->containerItem.buildDBusSignature();
    }

    for (++it; it != endIt; ++it)
    {
        if (d->type != (*it).type())
        {
            d->type = QDBusData::Invalid;
            d->containerItem = QDBusData();

            return *this;
        }
        else if (hasContainerItemType())
        {
            if ((*it).buildDBusSignature() != elementSignature)
            {
                d->type = QDBusData::Invalid;
                d->containerItem = QDBusData();

                return *this;
            }
        }
    }

    d->list = other;

    return *this;
}

QDBusDataList& QDBusDataList::operator=(const QStringList& other)
{
    d->list.clear();
    d->type = QDBusData::String;
    d->containerItem = QDBusData();

    QStringList::const_iterator it    = other.begin();
    QStringList::const_iterator endIt = other.end();
    for (; it != endIt; ++it)
    {
        d->list << QDBusData::fromString(*it);
    }

    return *this;
}

QDBusData::Type QDBusDataList::type() const
{
    return d->type;
}

bool QDBusDataList::hasContainerItemType() const
{
    return d->type == QDBusData::List || d->type == QDBusData::Map
                                      || d->type == QDBusData::Struct;
}

QDBusData QDBusDataList::containerItemType() const
{
    return d->containerItem;
}

bool QDBusDataList::isEmpty() const
{
    return d->list.isEmpty();
}

uint QDBusDataList::count() const
{
    return d->list.count();
}

bool QDBusDataList::operator==(const QDBusDataList& other) const
{
    if (&other == this) return true;
    if (d == other.d) return true;

    bool containerEqual = true;
    if (hasContainerItemType())
    {
        if (other.hasContainerItemType())
        {
            containerEqual = d->containerItem.buildDBusSignature() ==
                             other.d->containerItem.buildDBusSignature();
        }
        else
            containerEqual = false;
    }
    else if (other.hasContainerItemType())
        containerEqual = false;

    return d->type == other.d->type && containerEqual && d->list == other.d->list;
}

bool QDBusDataList::operator!=(const QDBusDataList& other) const
{
    if (&other == this) return false;
    if (d == other.d) return false;

    bool containerEqual = true;
    if (hasContainerItemType())
    {
        if (other.hasContainerItemType())
        {
            containerEqual = d->containerItem.buildDBusSignature() ==
                             other.d->containerItem.buildDBusSignature();
        }
        else
            containerEqual = false;
    }
    else if (other.hasContainerItemType())
        containerEqual = false;

    return d->type != other.d->type || !containerEqual || d->list != other.d->list;
}

void QDBusDataList::clear()
{
    d->list.clear();
}

QDBusDataList& QDBusDataList::operator<<(const QDBusData& data)
{
    if (data.type() == QDBusData::Invalid) return *this;

    if (d->type == QDBusData::Invalid)
    {
        d->type = data.type();

        // check if we are now have container items
        if (hasContainerItemType()) d->containerItem = data;

        d->list << data;
    }
    else if (d->type != data.type())
    {
        qWarning("QDBusDataList: trying to add data of type %s to list of type %s",
                 data.typeName(), QDBusData::typeName(d->type));
    }
    else if (hasContainerItemType())
    {
        QCString ourSignature  = d->containerItem.buildDBusSignature();
        QCString dataSignature = data.buildDBusSignature();

        if (ourSignature != dataSignature)
        {
            qWarning("QDBusDataList: trying to add data with signature %s "
                     "to list with item signature %s",
                     dataSignature.data(), ourSignature.data());
        }
        else
            d->list << data;
    }
    else
        d->list << data;

    return *this;
}

QValueList<QDBusData> QDBusDataList::toQValueList() const
{
    return d->list;
}

QStringList QDBusDataList::toQStringList(bool* ok) const
{
    if (d->type != QDBusData::String)
    {
        if (ok != 0) *ok = false;
        return QStringList();
    }

    QStringList result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toString();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<bool> QDBusDataList::toBoolList(bool* ok) const
{
    if (d->type != QDBusData::Bool)
    {
        if (ok != 0) *ok = false;
        return QValueList<bool>();
    }

    QValueList<bool> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toBool();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<Q_UINT8> QDBusDataList::toByteList(bool* ok) const
{
    if (d->type != QDBusData::Byte)
    {
        if (ok != 0) *ok = false;
        return QValueList<Q_UINT8>();
    }

    QValueList<Q_UINT8> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toByte();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<Q_INT16> QDBusDataList::toInt16List(bool* ok) const
{
    if (d->type != QDBusData::Int16)
    {
        if (ok != 0) *ok = false;
        return QValueList<Q_INT16>();
    }

    QValueList<Q_INT16> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toInt16();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<Q_UINT16> QDBusDataList::toUInt16List(bool* ok) const
{
    if (d->type != QDBusData::UInt16)
    {
        if (ok != 0) *ok = false;
        return QValueList<Q_UINT16>();
    }

    QValueList<Q_UINT16> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toUInt16();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<Q_INT32> QDBusDataList::toInt32List(bool* ok) const
{
    if (d->type != QDBusData::Int32)
    {
        if (ok != 0) *ok = false;
        return QValueList<Q_INT32>();
    }

    QValueList<Q_INT32> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toInt32();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<Q_UINT32> QDBusDataList::toUInt32List(bool* ok) const
{
    if (d->type != QDBusData::UInt32)
    {
        if (ok != 0) *ok = false;
        return QValueList<Q_UINT32>();
    }

    QValueList<Q_UINT32> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toUInt32();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<Q_INT64> QDBusDataList::toInt64List(bool* ok) const
{
    if (d->type != QDBusData::Int64)
    {
        if (ok != 0) *ok = false;
        return QValueList<Q_INT64>();
    }

    QValueList<Q_INT64> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toInt64();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<Q_UINT64> QDBusDataList::toUInt64List(bool* ok) const
{
    if (d->type != QDBusData::UInt64)
    {
        if (ok != 0) *ok = false;
        return QValueList<Q_UINT64>();
    }

    QValueList<Q_UINT64> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toUInt64();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<double> QDBusDataList::toDoubleList(bool* ok) const
{
    if (d->type != QDBusData::Double)
    {
        if (ok != 0) *ok = false;
        return QValueList<double>();
    }

    QValueList<double> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toDouble();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<QString> QDBusDataList::toStringList(bool* ok) const
{
    return toQStringList(ok);
}

QValueList<QDBusObjectPath> QDBusDataList::toObjectPathList(bool* ok) const
{
    if (d->type != QDBusData::ObjectPath)
    {
        if (ok != 0) *ok = false;
        return QValueList<QDBusObjectPath>();
    }

    QValueList<QDBusObjectPath> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toObjectPath();
    }

    if (ok != 0) *ok = true;

    return result;
}

QValueList<QDBusVariant> QDBusDataList::toVariantList(bool* ok) const
{
    if (d->type != QDBusData::Variant)
    {
        if (ok != 0) *ok = false;
        return QValueList<QDBusVariant>();
    }

    QValueList<QDBusVariant> result;

    QValueList<QDBusData>::const_iterator it    = d->list.begin();
    QValueList<QDBusData>::const_iterator endIt = d->list.end();
    for (; it != endIt; ++it)
    {
        result << (*it).toVariant();
    }

    if (ok != 0) *ok = true;

    return result;
}
