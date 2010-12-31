/* qdbusdataconverter.cpp QDBusDataConverter template
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

#include "dbus/qdbusdataconverter.h"
#include "dbus/qdbusdata.h"

#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>
#include <qvaluelist.h>

template <>
QDBusDataConverter::Result QDBusDataConverter::convertFromQDBusData<QRect>(const QDBusData& dbusData, QRect& typeData)
{
    if (dbusData.type() != QDBusData::Struct) return InvalidSignature;
    
    QValueList<QDBusData> members = dbusData.toStruct();
    if (members.count() != 4) return InvalidSignature;
    
    Q_INT32 values[4];
    
    QValueList<QDBusData>::const_iterator it    = members.begin();
    QValueList<QDBusData>::const_iterator endIt = members.end();
    for (uint i = 0; it != endIt; ++it, ++i)
    {
        bool ok = false;
        values[i] = (*it).toInt32(&ok);
        if (!ok) return InvalidSignature;
    }
    
    typeData = QRect(values[0], values[1], values[2], values[3]);
    
    return Success;
}

template <>
QDBusDataConverter::Result QDBusDataConverter::convertToQDBusData<QRect>(const QRect& typeData, QDBusData& dbusData)
{
    QValueList<QDBusData> members;
    
    members << QDBusData::fromInt32(typeData.x());
    members << QDBusData::fromInt32(typeData.y());
    members << QDBusData::fromInt32(typeData.width());
    members << QDBusData::fromInt32(typeData.height());
    
    dbusData = QDBusData::fromStruct(members);
    
    return Success;
}

template <>
QDBusDataConverter::Result QDBusDataConverter::convertFromQDBusData<QPoint>(const QDBusData& dbusData, QPoint& typeData)
{
    if (dbusData.type() != QDBusData::Struct) return InvalidSignature;
    
    QValueList<QDBusData> members = dbusData.toStruct();
    if (members.count() != 2) return InvalidSignature;
    
    Q_INT32 values[2];
    
    QValueList<QDBusData>::const_iterator it    = members.begin();
    QValueList<QDBusData>::const_iterator endIt = members.end();
    for (uint i = 0; it != endIt; ++it, ++i)
    {
        bool ok = false;
        values[i] = (*it).toInt32(&ok);
        if (!ok) return InvalidSignature;
    }
    
    typeData = QPoint(values[0], values[1]);
    
    return Success;
}

template <>
QDBusDataConverter::Result QDBusDataConverter::convertToQDBusData<QPoint>(const QPoint& typeData, QDBusData& dbusData)
{
    QValueList<QDBusData> members;
    
    members << QDBusData::fromInt32(typeData.x());
    members << QDBusData::fromInt32(typeData.y());
    
    dbusData = QDBusData::fromStruct(members);
    
    return Success;
}

template <>
QDBusDataConverter::Result QDBusDataConverter::convertFromQDBusData<QSize>(const QDBusData& dbusData, QSize& typeData)
{
    if (dbusData.type() != QDBusData::Struct) return InvalidSignature;
    
    QValueList<QDBusData> members = dbusData.toStruct();
    if (members.count() != 2) return InvalidSignature;
    
    Q_INT32 values[2];
    
    QValueList<QDBusData>::const_iterator it    = members.begin();
    QValueList<QDBusData>::const_iterator endIt = members.end();
    for (uint i = 0; it != endIt; ++it, ++i)
    {
        bool ok = false;
        values[i] = (*it).toInt32(&ok);
        if (!ok) return InvalidSignature;
    }
    
    typeData = QSize(values[0], values[1]);
    
    return Success;
}

template <>
QDBusDataConverter::Result QDBusDataConverter::convertToQDBusData<QSize>(const QSize& typeData, QDBusData& dbusData)
{
    QValueList<QDBusData> members;
    
    members << QDBusData::fromInt32(typeData.width());
    members << QDBusData::fromInt32(typeData.height());
    
    dbusData = QDBusData::fromStruct(members);
    
    return Success;
}
