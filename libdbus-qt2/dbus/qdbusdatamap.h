/* qdbusdatamap.h DBUS data mapping transport type
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

#ifndef QDBUSDATAMAP_H
#define QDBUSDATAMAP_H

#include "dbus/qdbusmacros.h"
#include <qmap.h>

class QDBusData;
class QDBusObjectPath;
class QDBusVariant;

/**
 * @brief Class to transport maps of D-Bus data types
 *
 * \note while the D-Bus data type is actually called @c dict this bindings
 *       use the term @c map since QDBusDataMap is essentially a QMap
 *
 * There are basically two ways to create QDBusDataMap objects:
 * - non-empty from content
 * - empty by specifying the desired element type
 *
 * Example for creating a filled map from content
 * @code
 * QMap<Q_INT16, QString> intToStringMap;
 * map.insert(2, "two");
 * map.insert(3, "three");
 * map.insert(5, "five");
 * map.insert(7, "seven");
 *
 * QDBusDataMap<Q_INT16> dbusMap(intToStringMap);
 * QDBusData data = QDBusData::fromInt16KeyMap(dbusMap);
 *
 * // or even shorter, using implicit conversion
 * QDBusData other = QDBusData::fromInt16KeyMap(intList);
 * @endcode
 *
 * Example for creating an empty map
 * @code
 * // empty map for a simple type, mapping from QString to double
 * QDBusDataMap<QString> list(QDBusData::Double);
 *
 * // empty map for value type string lists
 * QDBusData valueType = QDBusData::fromList(QDBusDataList(QDBusData::String));
 * QDBusDataMap<QString> map(valueType);
 * @endcode
 *
 * @see QDBusDataList
 */
template <typename T>
class QDBUS_EXPORT QDBusDataMap : private QMap<T, QDBusData>
{
    friend class QDBusData;

public:
    /**
     * Constant iterator. A QMapConstIterator with value type specified
     * as QDBusData
     */
    typedef QMapConstIterator<T, QDBusData> const_iterator;

    /**
     * @brief Creates an empty and invalid map
     *
     * @see QDBusData::Invalid
     */
    QDBusDataMap<T>()
        : QMap<T, QDBusData>(), m_valueType(QDBusData::Invalid) {}

    /**
     * @brief Creates an empty map with the given simple type for values
     *
     * The given type has be one of the non-container types, i.e. any other than
     * QDBusData::Map, QDBusData::List or QDBusData::Struct
     *
     * For creating a map with elements which are containers themselves, use
     * QDBusDataMap<T>(const QDBusData&);
     *
     * @param simpleValueType the type of the values in the new map
     */
    explicit QDBusDataMap<T>(QDBusData::Type simpleValueType)
        : QMap<T, QDBusData>(), m_valueType(simpleValueType) {}

    /**
     * @brief Creates an empty map with the given container type for values
     *
     * For creating a map with simple values you can also use
     * QDBusDataMap<T>(QDBusData::Type);
     *
     * @param containerValueType the type of the values in the new map
     *
     * @see hasContainerValueType()
     */
    explicit QDBusDataMap<T>(const QDBusData& containerValueType)
        : QMap<T, QDBusData>(), m_valueType(containerValueType.type())
    {
        if (hasContainerValueType()) m_containerValueType = containerValueType;
    }

    /**
     * @brief Creates a map from the given @p other map
     *
     * This behaves basically like copying a QMap through its copy
     * constructor, i.e. no value are actually copied at this time.
     *
     * @param other the other map object to copy from
     */
    QDBusDataMap<T>(const QDBusDataMap<T>& other)
        : QMap<T, QDBusData>(other), m_valueType(other.m_valueType),
          m_containerValueType(other.m_containerValueType) {}

    /**
     * @brief Creates a map from the given QMap of QDBusData objects
     *
     * If the @p other map is empty, this will behave like QDBusDataMap<T>(),
     * i.e. create an empty and invalid map object.
     *
     * Type information for the map object, i.e. value type and, if applicable,
     * container value type, will be derived from the @p other map's elements.
     *
     * \warning if the values of the @p other map do not all have the same
     *          type, the map object will also be empty and invalid
     *
     * @param other the QMap of QDBusData objects to copy from
     *
     * @see toQMap()
     */
    QDBusDataMap<T>(const QMap<T, QDBusData>& other)
        : QMap<T, QDBusData>(other), m_valueType(QDBusData::Invalid)
    {
        const_iterator it = begin();
        if (it == end()) return;

        m_valueType = (*it).type();

        QCString containerSignature;
        if (hasContainerValueType())
        {
            m_containerValueType = it.data();
            containerSignature = m_containerValueType.buildDBusSignature();
        }

        for (++it; it != end(); ++it)
        {
            if ((*it).type() != m_valueType)
            {
                m_valueType = QDBusData::Invalid;
                m_containerValueType = QDBusData();

                clear();
                return;
            }
            else if (hasContainerValueType())
            {
                if (it.data().buildDBusSignature() != containerSignature)
                {
                    m_valueType = QDBusData::Invalid;
                    m_containerValueType = QDBusData();

                    clear();
                    return;
                }
            }
        }
    }

    /**
     * @brief Creates a list from the given QMap of boolean values
     *
     * Type information for the map object will be set to QDBusData::Bool
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::Bool
     *
     * @param other the QMap of boolean values to copy from
     *
     * @see toBoolMap()
     */
    QDBusDataMap<T>(const QMap<T, bool>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::Bool)
    {
        typename QMap<T, bool>::const_iterator it    = other.begin();
        typename QMap<T, bool>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromBool(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of byte (unsigned char) values
     *
     * Type information for the map object will be set to QDBusData::Byte
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::Byte
     *
     * @param other the QMap of byte (unsigned char) values to copy from
     *
     * @see toByteMap()
     */
    QDBusDataMap<T>(const QMap<T, Q_UINT8>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::Byte)
    {
        typename QMap<T, Q_UINT8>::const_iterator it    = other.begin();
        typename QMap<T, Q_UINT8>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromByte(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of signed 16-bit integer values
     *
     * Type information for the map object will be set to QDBusData::Int16
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::Int16
     *
     * @param other the QMap of signed 16-bit integer values to copy from
     *
     * @see toInt16Map()
     */
    QDBusDataMap<T>(const QMap<T, Q_INT16>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::Int16)
    {
        typename QMap<T, Q_INT16>::const_iterator it    = other.begin();
        typename QMap<T, Q_INT16>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromInt16(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of unsigned 16-bit integer values
     *
     * Type information for the map object will be set to QDBusData::UInt16
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::UInt16
     *
     * @param other the QMap of unsigned 16-bit integer values to copy from
     *
     * @see toUInt16Map()
     */
    QDBusDataMap<T>(const QMap<T, Q_UINT16>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::UInt16)
    {
        typename QMap<T, Q_UINT16>::const_iterator it    = other.begin();
        typename QMap<T, Q_UINT16>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromUInt16(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of signed 32-bit integer values
     *
     * Type information for the map object will be set to QDBusData::Int32
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::Int32
     *
     * @param other the QMap of signed 32-bit integer values to copy from
     *
     * @see toInt32Map()
     */
    QDBusDataMap<T>(const QMap<T, Q_INT32>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::Int32)
    {
        typename QMap<T, Q_INT32>::const_iterator it    = other.begin();
        typename QMap<T, Q_INT32>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromInt32(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of unsigned 32-bit integer values
     *
     * Type information for the map object will be set to QDBusData::UInt16
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::UInt32
     *
     * @param other the QMap of unsigned 32-bit integer values to copy from
     *
     * @see toUInt32Map()
     */
    QDBusDataMap<T>(const QMap<T, Q_UINT32>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::UInt32)
    {
        typename QMap<T, Q_UINT32>::const_iterator it    = other.begin();
        typename QMap<T, Q_UINT32>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromUInt32(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of signed 64-bit integer values
     *
     * Type information for the map object will be set to QDBusData::Int64
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::Int64
     *
     * @param other the QMap of signed 64-bit integer values to copy from
     *
     * @see toInt64Map()
     */
    QDBusDataMap<T>(const QMap<T, Q_INT64>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::Int64)
    {
        typename QMap<T, Q_INT64>::const_iterator it    = other.begin();
        typename QMap<T, Q_INT64>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromInt64(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of unsigned 64-bit integer values
     *
     * Type information for the map object will be set to QDBusData::UInt64
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::UInt64
     *
     * @param other the QMap of unsigned 64-bit integer values to copy from
     *
     * @see toUInt64Map()
     */
    QDBusDataMap<T>(const QMap<T, Q_UINT64>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::UInt64)
    {
        typename QMap<T, Q_UINT64>::const_iterator it    = other.begin();
        typename QMap<T, Q_UINT64>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromUInt64(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of double values
     *
     * Type information for the map object will be set to QDBusData::Double
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::Double
     *
     * @param other the QMap of double values to copy from
     *
     * @see toDoubleMap()
     */
    QDBusDataMap<T>(const QMap<T, double>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::Double)
    {
        typename QMap<T, double>::const_iterator it    = other.begin();
        typename QMap<T, double>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromDouble(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of QString values
     *
     * Type information for the map object will be set to QDBusData::String
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::String
     *
     * @param other the QMap of QString values to copy from
     *
     * @see toStringMap()
     */
    QDBusDataMap<T>(const QMap<T, QString>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::String)
    {
        typename QMap<T, QString>::const_iterator it    = other.begin();
        typename QMap<T, QString>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromString(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of object path values
     *
     * Type information for the map object will be set to QDBusData::ObjectPath
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::ObjectPath
     *
     * @param other the QMap of object path values to copy from
     *
     * @see toObjectPathMap()
     */
    QDBusDataMap<T>(const QMap<T, QDBusObjectPath>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::ObjectPath)
    {
        typename QMap<T, QDBusObjectPath>::const_iterator it    = other.begin();
        typename QMap<T, QDBusObjectPath>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromObjectPath(it.data()));
        }
    }

    /**
     * @brief Creates a map from the given QMap of QDBusVariant values
     *
     * Type information for the map object will be set to QDBusData::Variant
     * also when the @p other map is empty, i.e. this allows to create an
     * empty but valid map object, comparable to using
     * QDBusDataMap<T>(QDBusData::Type) with QDBusData::Variant
     *
     * @param other the QMap of variant values to copy from
     *
     * @see toVariantMap()
     */
    QDBusDataMap<T>(const QMap<T, QDBusVariant>& other)
        : QMap<T, QDBusData>(), m_valueType(QDBusData::Variant)
    {
        typename QMap<T, QDBusVariant>::const_iterator it    = other.begin();
        typename QMap<T, QDBusVariant>::const_iterator endIt = other.end();
        for (; it != endIt; ++it)
        {
            insert(it.key(), QDBusData::fromVariant(it.data()));
        }
    }

    /**
     * @brief Copies from the given @p other map
     *
     * This behaves basically like copying a QMap through its assignment
     * operator, i.e. no value are actually copied at this time.
     *
     * @param other the other map object to copy from
     *
     * @return a reference to this map object
     */
    QDBusDataMap<T>& operator=(const QDBusDataMap<T>& other)
    {
        QMap<T, QDBusData>::operator=(other);

        m_valueType = other.m_valueType;
        m_containerValueType = other.m_containerValueType;

        return *this;
    }

    /**
     * @brief Copies from the given @p other map
     *
     * This behaves basically like copying a QMap through its assignment
     * operator, i.e. no value are actually copied at this time.
     *
     * \warning the value of the given @p other map have to be of the same
     *          type. If they aren't this maps's content will cleared and the
     *          value type will be set to QDBusData::Invalid
     *
     * @param other the other map object to copy from
     *
     * @return a reference to this map object
     */
    QDBusDataMap<T>& operator=(const QMap<T, QDBusData>& other)
    {
        QMap<T, QDBusData>::operator=(other);

        m_valueType = QDBusData::Invalid;
        m_containerValueType = QDBusData();

        const_iterator it = begin();
        if (it == end()) return *this;

        m_valueType = (*it).type();

        QCString containerSignature;
        if (hasContainerValueType())
        {
            m_containerValueType = it.data();
            containerSignature = m_containerValueType.buildDBusSignature();
        }

        for (++it; it != end(); ++it)
        {
            if ((*it).type() != m_valueType)
            {
                m_valueType = QDBusData::Invalid;
                m_containerValueType = QDBusData();

                clear();
                return *this;
            }
            else if (hasContainerValueType())
            {
                if (it.data()->buildSignature() != containerSignature)
                {
                    m_valueType = QDBusData::Invalid;
                    m_containerValueType = QDBusData();

                    clear();
                    return *this;
                }
            }
        }

        return *this;
    }

    /**
     * @brief Returns the key type of the map object
     *
     * @return one of the values of the QDBusData#Type enum suitable for
     *         map keys. See QDBusData::Map for details
     *
     * @see valueType()
     */
    QDBusData::Type keyType() const { return m_keyType; }

    /**
     * @brief Returns the value type of the map object
     *
     * @return one of the values of the QDBusData#Type enum
     *
     * @see hasContainerValueType()
     * @see containerValueType()
     * @see keyType()
     */
    QDBusData::Type valueType() const { return m_valueType; }

    /**
     * @brief Checks whether the value type is a data container itself
     *
     * If the value of the map are containers as well, this will return
     * @c true
     * In this case containerValueType() will return a prototype for such a
     * container.
     *
     * @return @c true if the value type is either QDBusData::Map,
     *         QDBusData::List or QDBusData::Struct, otherwise @c false
     *
     * @see QDBusDataMap<T>(const QDBusData&)
     */
    bool hasContainerValueType() const
    {
        return m_valueType == QDBusData::List || m_valueType == QDBusData::Struct
                                              || m_valueType == QDBusData::Map;
    }

    /**
     * @brief Returns a container prototype for the map's value type
     *
     * Lists which have containers as their elements, i.e. hasContainerValueType()
     * returns @c true this will actually specify the details for the use
     * container, i.e. the returned data object can be queried for type and
     * possible further subtypes.
     *
     * @return a data object detailing the value type or an invalid data object
     *         if the map does not have a container as its element type
     *
     * @see QDBusDataMap<T>(const QDBusData&);
     * @see valueType()
     * @see QDBusData::Invalid
     */
    QDBusData containerValueType() const { return m_containerValueType; }

    /**
     * @brief Checks whether this map object has a valid value type
     *
     * This is equal to checking valueType() for not being QDBusData::Invalid
     *
     * @return @c true if the map object is valid, otherwise @c false
     */
    inline bool isValid() const { return valueType() != QDBusData::Invalid; }

    /**
     * @brief Checks whether this map object has any key/value pairs
     *
     * @return @c true if there are no key/values in this map, otherwise @c false
     *
     * @see count()
     */
    bool isEmpty() const { return QMap<T, QDBusData>::empty(); }

    /**
     * @brief Returns the number of key/value pairs of this map object
     *
     * @return the number of key/value pairs
     *
     * @see isEmpty()
     */
    uint count() const { return QMap<T, QDBusData>::count(); }

    /**
     * @brief Checks whether the given @p other map is equal to this one
     *
     * Two maps are considered equal when they have the same value type (and same
     * container value type if the have one) and the key/value pairs are equal
     * as well.
     *
     * @param other the other map object to compare with
     *
     * @return @c true if the maps are equal, otherwise @c false
     *
     * @see QDBusData::operator==()
     */
    bool operator==(const QDBusDataMap<T>& other) const
    {
        if (m_valueType != other.m_valueType) return false;

        if (count() != other.count()) return false;

        if (hasContainerValueType() != other.hasContainerValueType()) return false;

        if (hasContainerValueType())
        {
            if (m_containerValueType.buildDBusSignature() !=
                other.m_containerValueType.buildDBusSignature()) return false;
        }

        const_iterator it = begin();
        const_iterator otherIt = other.begin();
        for (; it != end() && otherIt != other.end(); ++it, ++otherIt)
        {
            if (it.key() != otherIt.key()) return false;

            if (!(it.data() == otherIt.data())) return false;
        }

        return true;
    }

    /**
     * @brief Clears the map
     *
     * Value type and, if applicable, container value type will stay untouched.
     */
    void clear() { QMap<T, QDBusData>::clear(); }

    /**
     * @brief Returns an iterator to the first item according to the key sort order
     *
     * @see QMap::begin()
     */
    const_iterator begin() const
    {
        return QMap<T, QDBusData>::begin();
    }

    /**
     * @brief Returns an iterator to an invalid position
     *
     * @see QMap::end()
     */
    const_iterator end() const
    {
        return QMap<T, QDBusData>::end();
    }

    /**
     * @brief Inserts a given value for a given key
     *
     * Basically works like the respective QMap method, but checks if
     * type of the new value matches the value type of the list.
     * Maps that are invalid will accept any new type and will then be
     * typed accordingly.
     *
     * If @p data is invalid itself, it will not be inserted at any time.
     *
     * \note the more common use case is to work with a QMap and then
     *       use the respective constructor to create the QDBusDataMap object
     *
     * @param key the key were to insert into the map
     * @param data the data item to insert into the map
     *
     * @return @c true on successfull insert, otherwise @c false
     */
    bool insert(const T& key, const QDBusData& data)
    {
        if (data.type() == QDBusData::Invalid) return false;

        if (m_valueType == QDBusData::Invalid)
        {
            m_valueType = data.type();

            // TODO: create empty copy of container
            if (hasContainerValueType()) m_containerValueType = data;

            QMap<T, QDBusData>::insert(key, data);
        }
        else if (data.type() != m_valueType)
        {
            qWarning("QDBusDataMap: trying to add data of type %s to map of type %s",
                     data.typeName(), QDBusData::typeName(m_valueType));
        }
        else if (hasContainerValueType())
        {
            QCString ourSignature  = m_containerValueType.buildDBusSignature();
            QCString dataSignature = data.buildDBusSignature();

            if (ourSignature != dataSignature)
            {
                qWarning("QDBusDataMap: trying to add data with signature %s "
                        "to map with value signature %s",
                        dataSignature.data(), ourSignature.data());
            }
            else
                QMap<T, QDBusData>::insert(key, data);
        }
        else
            QMap<T, QDBusData>::insert(key, data);

        return true;
    }

    /**
     * @brief Converts the map object into a QMap with QDBusData elements
     *
     * @return the key/value pairs of the map object as a QMap
     */
    QMap<T, QDBusData> toQMap() const { return *this; }

    /**
     * @brief Tries to get the map object's pairs as a QMap of bool
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::Bool.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::Bool)
     *
     * @return a QMap of bool containing the list object's boolean
     *         values or an empty map when converting fails
     *
     * @see QDBusData::toBool()
     */
    QMap<T, bool> toBoolMap(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::Bool)
        {
            if (ok != 0) *ok = false;
            return QMap<T, bool>();
        }

        QMap<T, bool> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toBool());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of Q_UINT8
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::Byte.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::Byte)
     *
     * @return a QMap of Q_UINT8 containing the list object's byte
     *         values or an empty map when converting fails
     *
     * @see QDBusData::toBool()
     */
    QMap<T, Q_UINT8> toByteMap(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::Byte)
        {
            if (ok != 0) *ok = false;
            return QMap<T, Q_UINT8>();
        }

        QMap<T, Q_UINT8> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toByte());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of Q_INT16
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::Int16.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::Int16)
     *
     * @return a QMap of Q_INT16 containing the map object's
     *         signed 16-bit integer values or an empty map when converting
     *         fails
     *
     * @see QDBusData::toInt16()
     */
    QMap<T, Q_INT16> toInt16Map(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::Int16)
        {
            if (ok != 0) *ok = false;
            return QMap<T, Q_INT16>();
        }

        QMap<T, Q_INT16> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toInt16());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of Q_UINT16
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::UInt16.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::UInt16)
     *
     * @return a QMap of Q_UINT16 containing the map object's
     *         unsigned 16-bit integer values or an empty map when converting
     *         fails
     *
     * @see QDBusData::toUInt16()
     */
    QMap<T, Q_UINT16> toUInt16Map(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::UInt16)
        {
            if (ok != 0) *ok = false;
            return QMap<T, Q_UINT16>();
        }

        QMap<T, Q_UINT16> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toUInt16());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of Q_INT32
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::Int32.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::Int32)
     *
     * @return a QMap of Q_INT32 containing the map object's
     *         signed 32-bit integer values or an empty map when converting
     *         fails
     *
     * @see QDBusData::toInt32()
     */
    QMap<T, Q_INT32> toInt32Map(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::Int32)
        {
            if (ok != 0) *ok = false;
            return QMap<T, Q_INT32>();
        }

        QMap<T, Q_INT32> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toInt32());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of Q_UINT32
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::UInt32.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::UInt32)
     *
     * @return a QMap of Q_UINT32 containing the map object's
     *         unsigned 32-bit integer values or an empty map when converting
     *         fails
     *
     * @see QDBusData::toUInt32()
     */
    QMap<T, Q_UINT32> toUInt32Map(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::UInt32)
        {
            if (ok != 0) *ok = false;
            return QMap<T, Q_UINT32>();
        }

        QMap<T, Q_UINT32> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toUInt32());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of Q_INT64
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::Int64.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::Int64)
     *
     * @return a QMap of Q_INT64 containing the map object's
     *         signed 64-bit integer values or an empty map when converting
     *         fails
     *
     * @see QDBusData::toInt64()
     */
    QMap<T, Q_INT64> toInt64Map(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::Int64)
        {
            if (ok != 0) *ok = false;
            return QMap<T, Q_INT64>();
        }

        QMap<T, Q_INT64> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toInt64());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of Q_UINT64
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::UInt64.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::UInt64)
     *
     * @return a QMap of Q_UINT64 containing the map object's
     *         unsigned 64-bit integer values or an empty map when converting
     *         fails
     *
     * @see QDBusData::toUInt64()
     */
    QMap<T, Q_UINT64> toUInt64Map(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::UInt64)
        {
            if (ok != 0) *ok = false;
            return QMap<T, Q_UINT64>();
        }

        QMap<T, Q_UINT64> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toUInt64());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of double
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::Double.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::Double)
     *
     * @return a QMap of double containing the map object's double
     *         values or an empty map when converting fails
     *
     * @see QDBusData::toDouble()
     */
    QMap<T, double> toDoubleMap(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::Double)
        {
            if (ok != 0) *ok = false;
            return QMap<T, double>();
        }

        QMap<T, double> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toDouble());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of QString
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::String, see also toQStringList().
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::String)
     *
     * @return a QMap of QString containing the map object's string
     *         values or an empty map when converting fails
     *
     * @see QDBusData::toString()
     */
    QMap<T, QString> toStringMap(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::String)
        {
            if (ok != 0) *ok = false;
            return QMap<T, QString>();
        }

        QMap<T, QString> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toString());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of object paths
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::ObjectPath)
     *
     * @return a QMap of object paths containing the map object's object path
     *         values or an empty map when converting fails
     *
     * @see QDBusData::toObjectPath()
     */
    QMap<T, QDBusObjectPath> toObjectPathMap(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::ObjectPath)
        {
            if (ok != 0) *ok = false;
            return QMap<T, QDBusObjectPath>();
        }

        QMap<T, QDBusObjectPath> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toObjectPath());
        }

        if (ok != 0) *ok = true;

        return result;
    }

    /**
     * @brief Tries to get the map object's pairs as a QMap of QDBusVariant
     *
     * This is a convenience overload for the case when the map is of
     * value type QDBusData::Variant.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of value type
     *        QDBusData::Variant)
     *
     * @return a QMap of QDBusVariant containing the map object's
     *         QDBusVariant values or an empty map when converting fails
     *
     * @see QDBusData::toVariant()
     */
    QMap<T, QDBusVariant> toVariantMap(bool* ok = 0) const
    {
        if (m_valueType != QDBusData::Variant)
        {
            if (ok != 0) *ok = false;
            return QMap<T, QDBusVariant>();
        }

        QMap<T, QDBusVariant> result;

        const_iterator it    = begin();
        const_iterator endIt = end();
        for (; it != endIt; ++it)
        {
            result.insert(it.key(), (*it).toVariant());
        }

        if (ok != 0) *ok = true;

        return result;
    }

private:
    QDBusData::Type m_valueType;
    QDBusData m_containerValueType;

    static const QDBusData::Type m_keyType;
};

#endif
