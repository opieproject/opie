/* qdbusdataconverter.h QDBusDataConverter template
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

#ifndef QDBUSDATACONVERTER_H
#define QDBUSDATACONVERTER_H

#include "dbus/qdbusmacros.h"

class QDBusData;

/**
 * @brief Template based converter for getting complex data into or from QDBusData objects
 *
 * Any data to transport over D-Bus, i.e. method/signal paramaters or properties, need
 * to be converted into a QDBusData instance.
 * 
 * For complex types, e.g. structures or nested containers, this can be quite some code,
 * and will likely be needed for more than one call.
 * Therefore it is more convenient to implement the conversions once per complex type.
 *
 * Example: sending and recieving a QRect over D-Bus.
 * In D-Bus terminology a QRect is a struct of four 32-bit signed integers. The code to do
 * this manually looks like this:
 * @code
 * QRect rect(0, 0, 100, 100);
 * 
 * QValueList<QDBusData> structMembers;
 * structMembers << QDBusData::fromInt32(rect.x());
 * structMembers << QDBusData::fromInt32(rect.y());
 * structMembers << QDBusData::fromInt32(rect.wdth());
 * structMembers << QDBusData::fromInt32(rect.height());
 *
 * QDBusData rectStruct = QDBusData::fromStruct(structMembers);
 * @endcode
 * and reverse (without the error checking)
 * @code
 * QDBusData dbusData; // assume we got this from a D-Bus call
 * 
 * QValueList<QDBusData> structMembers = dbudData.toStruct();
 * 
 * int x = structMembers[0].toInt32();
 * int y = structMembers[1].toInt32();
 * int w = structMembers[2].toInt32();
 * int h = structMembers[3].toInt32();
 *
 * QRect rect(x, y, w, h);
 * @endcode
 *
 * Rather than implementing it in the method which performs the D-Bus call, basically the same
 * code can be used as a spezialisation of the QDBusDataConverter methods and then used like this:
 * @code
 * QRect rect(0, 0, 100, 100);
 * QDBusData rectStruct;
 *
 * QDBusDataConverter::convertToQDBusData<QRect>(rect, rectStruct);
 * @endcode
 * and
 * @code
 * QRect rect;
 * QDBusData dbusData; // assume we got this from a D-Bus call
 *
 * QDBusDataConverter::convertFromQDBusData<QRect>(dbusData, rect);
 * @endcode
 *
 * @note The bindings library contains the spezialisations for QRect, QPoint and QSize.
 */
class QDBUS_EXPORT QDBusDataConverter
{
public:
    /**
     * @brief Conversion result values
     */
    enum Result
    {
        /**
         * Conversion successfull
         */
        Success,
        
        /**
         * Conversion failed because the passed QDBusData instance does not contain data
         * of the needed signature, e.g. too few to too many members for a struct or wrong types.
         *
         * @see QDBusError::stdInvalidSignature()
         */
        InvalidSignature,
        
        /**
         * Conversion failed because the passed QDBusData contained values which are not allowed,
         * e.g. out of range for a numerical type used a an enum or flags.
         *
         * @see QDBusError::stdInvalidArgs()
         */
        InvalidArgument
    };

    /**
     * @brief Conversion from a filled QDBusData instance to a native type
     *
     * For example the implementation for QPoint looks like this:
     * @code
     * template <>
     * QDBusDataConverter::Result
     * QDBusDataConverter::convertFromQDBusData<QPoint>(const QDBusData& dbusData, QPoint& typeData)
     * {
     *     if (dbusData.type() != QDBusData::Struct) return InvalidSignature;
     *     
     *     QValueList<QDBusData> members = dbusData.toStruct();
     *     if (members.count() != 2) return InvalidSignature;
     *
     *     bool ok = false;
     *     int x = members[0].toInt32(&ok);
     *     if (!ok) return InvalidSignature;
     *
     *     int y = members[1].toInt32(&ok);
     *     if (!ok) return InvalidSignature;
     *
     *     typeData = QPoint(x, y);
     *
     *     return Success;
     * }
     * @endcode
     *
     * And then can be used like this:
     * @code
     * QDBusMessage reply; // assume we got this as a D-Bus call reply
     * QPoint point;
     *
     * if (QDBusDataConverter::convertFromQDBusData(reply[0], point) != QDBusDataConverter::Success)
     * {
     *     // error handling
     * }
     * @endcode
     *
     * @param dbusData the binding's data instance to get the content from
     * @param typeData the native type instance to put the content into
     *
     * @return the conversion result value
     */
    template <class T>
    static Result convertFromQDBusData(const QDBusData& dbusData, T& typeData);

    /**
     * @brief Conversion from a native type to a QDBusData instance
     *
     * For example the implementation for QPoint looks like this:
     * @code
     * template <>
     * QDBusDataConversion::Result
     * QDBusDataConversion::convertToQDBusData<QPoint>(const QPoint& typeData, QDBusData& dbusData)
     * {
     *     QValueList<QDBusData> members;
     *     members << QDBusData::fromInt32(typeData.x());
     *     members << QDBusData::fromInt32(typeData.y());
     *
     *     dbusData = QDBusData::fromStruct(members);
     *
     *     return Success;
     * }
     * @endcode
     *
     * And then can be used like this:
     * @code
     * QPoint point(-10, 100);
     * QDBusMessage methodCall; // assume created by QBusMessage::methodCall()
     *
     * QDBusData dbusData;
     * if (QDBusDataConverter::convertToQDBusData<QPoint>(point, dbusData) != QDBusDataConverter::Success)
     * {
     *     // error handling
     * }
     * else
     * {
     *     methodCall << dbusData;
     * }
     * @endcode
     *
     * @param typeData the native type instance to get the content from
     * @param dbusData the binding's data instance to put the content into
     *
     * @return the conversion result value
     */
    template <class T>
    static Result convertToQDBusData(const T& typeData, QDBusData& dbusData);
};

#endif
