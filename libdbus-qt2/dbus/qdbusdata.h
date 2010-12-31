/* qdbusdata.h DBUS data transport type
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

#ifndef QDBUSDATA_H
#define QDBUSDATA_H

#include "dbus/qdbusmacros.h"
#include <qglobal.h>

class QCString;
class QDBusDataList;
class QDBusVariant;
class QDBusObjectPath;
class QString;

template<typename T> class QValueList;
template<typename T> class QDBusDataMap;

/**
 * @brief Class for accurately representing D-Bus data types
 *
 * The QDBusData class can be compared to Qt's QVariant class, but
 * specialized to contain data types used in D-Bus messages.
 *
 * Like QVariant objects of QDBusData use implicit sharing, i.e. copying
 * a QDBusData object is a cheap operation and does not require that the
 * content itself is copied.
 *
 * Depending on the #Type of the object, the content can be a recursive
 * construct of QDBusData objects, e.g. a #List can contain elements that are
 * containers themselves, e.g. #Map, #Struct, #Variant or even #List again.
 *
 * @see QDBusDataList
 * @see QDBusDataMap
 * @see QDBusDataConverter
 */
class QDBUS_EXPORT QDBusData
{
public:
    /**
     * @brief Enum for the data types used in D-Bus messages
     *
     * In order to provide correct mapping of C++ and Qt types and the data
     * types used in D-Bus messages, QDBusData uses explicit naming of types
     * where the name is usually the one used in D-Bus, with the exception of
     * #List and #Map since this is closer to the Qt container they are
     * implemented with (QValueList and QMap respectively)
     *
     * @see type(), keyType()
     * @see typeName()
     */
    enum Type
    {
        /**
         * Base type for QDBusData objects created by the default constructor.
         *
         * Also used as the type of returned objects when getter type methods
         * fail due to type incompatabilties, i.e. toInt32() called on a #List
         * object.
         *
         * @see isValid()
         */
        Invalid = 0,

        /**
         * Type when encapsulating a boolean value.
         *
         * @see fromBool(), toBool()
         */
        Bool,

        /**
         * Type when encapsulating a byte (unsigned char) value.
         *
         * @see fromByte(), toByte()
         */
        Byte,

        /**
         * Type when encapsulating a signed 16-bit integer value.
         *
         * @see fromInt16(), toInt16()
         */
        Int16,

        /**
         * Type when encapsulating an unsigned 16-bit integer value.
         *
         * @see fromUInt16(), toUInt16()
         */
        UInt16,

        /**
         * Type when encapsulating a signed 32-bit integer value.
         *
         * @see fromInt32(), toInt32()
         */
        Int32,

        /**
         * Type when encapsulating an unsigned 32-bit integer value.
         *
         * @see fromUInt32(), toUInt32()
         */
        UInt32,

        /**
         * Type when encapsulating a signed 64-bit integer value.
         *
         * @see fromInt64(), toInt64()
         */
        Int64,

        /**
         * Type when encapsulating an unsigned 64-bit integer value.
         *
         * @see fromUInt64(), toUInt64()
         */
        UInt64,

        /**
         * Type when encapsulating a double value.
         *
         * @see fromDouble(), toDouble()
         */
        Double,

        /**
         * Type when encapsulating a string value.
         *
         * All strings are converted to UTF-8 during transmission
         *
         * @see fromString(), toString()
         */
        String,

        /**
         * Type when encapsulating a D-Bus object path.
         *
         * D-Bus defines a special string variation for transporting the
         * paths used to address objects within D-Bus services, see
         * @ref dbusconventions-objectpath for formatting details.
         *
         * @note from the point of view of this bindings an object path is
         *       pretty much a normal string with externally checked restrictions.
         *       However, method calls or return values can require a signature
         *       to include an object path and any remote peer might then reject
         *       the normal string signature.
         *
         * @see fromObjectPath(), toObjectPath()
         */
        ObjectPath,

        /**
         * Type when encapsulating a list of values.
         *
         * The D-Bus type this maps to is called @c array but since the Qt
         * container class used to implement this type is QValueList (or rather
         * QDBusDataList), the QDBusData type is called @c List instead.
         *
         * A list can contain any of the supported types as elements, even
         * container types.
         * However it can only contain elements with the same type per list
         * object.
         *
         * @see fromList(), toList()
         */
        List,

        /**
         * Type when encapsulating a struct of values.
         *
         * A struct is basically a list of struct member variables, each
         * member can be any of the supported types, even containers types.
         *
         * The C++/Qt value type used in the converter methods is a QValueList
         * with type QDBusData.
         * For example a QRect could be mapped like this:
         * @code
         * QRect rect(0, 0, 640, 480);
         * QValueList<QDBusData> memberList;
         *
         * memberList << QDBusData::fromInt32(rect.x());
         * memberList << QDBusData::fromInt32(rect.y());
         * memberList << QDBusData::fromInt32(rect.width());
         * memberList << QDBusData::fromInt32(rect.height());
         *
         * QDBusData data = QDBusData:fromStruct(memberList);
         * @endcode
         *
         * And reconstructed like this:
         * @code
         * memberList = data.toStruct();
         *
         * int x = memberList[0].toInt32();
         * int y = memberList[1].toInt32();
         * int w = memberList[2].toInt32();
         * int h = memberList[3].toInt32();
         *
         * rect = QRect(x, y, w, h);
         * @endcode
         *
         * @note Empty structs, i.e. an empty member list, are not allowed
         *
         * @see fromStruct(), toStruct()
         * @see QDBusDataConverter
         */
        Struct,

        /**
         * Type when encapsulating a special variable container value.
         *
         * See QDBusVariant for details on variant usage.
         *
         * @see fromVariant(), toVariant()
         */
        Variant,

        /**
         * Type when encapsulating a map of keys to values.
         *
         * The D-Bus type this maps to is called @c dict but since the Qt
         * container class used to implement this type is QMap (or rather
         * QDBusDataMap), the QDBusData type is called @c Map instead.
         *
         * A map can contain any of the supported types as values, even
         * container types, but only the following basic types as keys:
         * - #Byte
         * - #Int16
         * - #UInt16
         * - #Int32
         * - #UInt32
         * - #Int64
         * - #UInt64
         * - #String
         * - #ObjectPath
         *
         * All values need to be of the same type.
         *
         * @see fromByteKeyMap(), toByteKeyMap()
         * @see fromInt16KeyMap(), toInt16KeyMap()
         * @see fromUInt16KeyMap(), toUInt16KeyMap()
         * @see fromInt32KeyMap(), toInt32KeyMap()
         * @see fromUInt32KeyMap(), toUInt32KeyMap()
         * @see fromInt64KeyMap(), toInt64KeyMap()
         * @see fromUInt64KeyMap(), toUInt64KeyMap()
         * @see fromStringKeyMap(), toStringKeyMap()
         * @see fromObjectPathKeyMap(), toObjectPathKeyMap()
         */
        Map
    };

    /**
     * @brief Creates an empty, #Invalid data object
     */
    QDBusData();

    /**
     * @brief Copies a given @p other data object
     *
     * Since QDBusData is implicitly shared, both objects will have the
     * same content and the last object to reference it will delete it.
     *
     * @param other the object to copy
     */
    QDBusData(const QDBusData& other);

    /**
     * @brief Destroys the data object
     *
     * If this is the last instance to a shared content, it will delete it
     * as well.
     */
    ~QDBusData();

    /**
     * @brief Copies a given @p other data object
     *
     * Since QDBusData is implicitly shared, both objects will have the
     * same content and the last object to reference it will delete it.
     *
     * @param other the object to copy
     *
     * @return a reference to this instance
     */
    QDBusData& operator=(const QDBusData& other);

    /**
     * @brief Checks if the given @p other data object is equal to this instance
     *
     * Two QDBusData object are considered equal if they reference the same
     * shared content or have the same type and the content's equality operator
     * says the contents are equal.
     *
     * @param other the object to compare with
     *
     * @return @c true if the two data objects are equal, otherwise @c false
     */
    bool operator==(const QDBusData& other) const;

    /**
     * @brief Checks if the given @p other data object is different from this instance
     *
     * @param other the object to compare with
     *
     * @return @c false if the two data objects are not equal, otherwise @c false
     *
     * @see operator==()
     */
    bool operator!=(const QDBusData& other) const;

    /**
     * @brief Checks whether the data object contains a valid content
     *
     * This is equal to checking type() for not being #Invalid
     *
     * @return @c true if the data object is valid, otherwise @c false
     */
    inline bool isValid() const { return type() != QDBusData::Invalid; }

    /**
     * @brief Returns the #Type of the data object
     *
     * @return one of the values of the #Type enum
     *
     * @see keyType()
     * @see typeName()
     */
    Type type() const;

    /**
     * @brief Returns the #Type of the key type for maps
     *
     * If the type of the data object is #Map, this method returns the type
     * of the map's key, #String for a QDBusDataMap<QString>
     *
     * If the type of the data object is not #Map, it will return #Invalid
     *
     * @return one of the values of the #Type enum, #Invalid if the object is
     *         not holding a #Map
     *
     * @see type()
     * @see typeName()
     */
    Type keyType() const;

    /**
     * @brief Returns the string representation of the object's #Type
     *
     * @return an ASCII C-string for the object's type
     *
     * @see type()
     * @see typeName(Type)
     */
    inline const char* typeName() const { return typeName(type()); }

    /**
     * @brief Returns the string representation for the given @p type
     *
     * @param type the #Type to get the string representation for
     *
     * @return an ASCII C-string for the given @p type
     *
     * @see type()
     * @see typeName()
     */
    static const char* typeName(Type type);

    /**
     * @brief Creates a data object for the given boolean @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #Bool containing the @p value
     *
     * @see toBool()
     */
    static QDBusData fromBool(bool value);

    /**
     * @brief Tries to get the encapsulated boolean value
     *
     * If the data object is not of type #Bool this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Bool)
     *
     * @return the encapsulated boolean value or @c false if it fails
     *
     * @see fromBool()
     */
    bool toBool(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given byte (unsigned char) @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #Byte containing the @p value
     *
     * @see toByte()
     */
    static QDBusData fromByte(Q_UINT8 value);

    /**
     * @brief Tries to get the encapsulated byte (unsigned char) value
     *
     * If the data object is not of type #Byte this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Byte)
     *
     * @return the encapsulated byte (unsigned char) value or @c 0 if it fails
     *
     * @see fromByte()
     */
    Q_UINT8 toByte(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given signed 16-bit integer @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #Int16 containing the @p value
     *
     * @see toInt16()
     */
    static QDBusData fromInt16(Q_INT16 value);

    /**
     * @brief Tries to get the encapsulated signed 16-bit integer value
     *
     * If the data object is not of type #Int16 this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Int16)
     *
     * @return the encapsulated signed 16-bit integer value or @c 0 if it fails
     *
     * @see fromInt16()
     */
    Q_INT16 toInt16(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given unsigned 16-bit integer @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #UInt16 containing the @p value
     *
     * @see toUInt16()
     */
    static QDBusData fromUInt16(Q_UINT16 value);

    /**
     * @brief Tries to get the encapsulated unsigned 16-bit integer value
     *
     * If the data object is not of type #UInt16 this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #UInt16)
     *
     * @return the encapsulated unsigned 16-bit integer value or @c 0 if it fails
     *
     * @see fromUInt16()
     */
    Q_UINT16 toUInt16(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given signed 32-bit integer @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #Int32 containing the @p value
     *
     * @see toInt32()
     */
    static QDBusData fromInt32(Q_INT32 value);

    /**
     * @brief Tries to get the encapsulated signed 32-bit integer value
     *
     * If the data object is not of type #Int32 this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Int32)
     *
     * @return the encapsulated signed 32-bit integer value or @c 0 if it fails
     *
     * @see fromInt32()
     */
    Q_INT32 toInt32(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given unsigned 32-bit integer @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #UInt32 containing the @p value
     *
     * @see toUInt32()
     */
    static QDBusData fromUInt32(Q_UINT32 value);

    /**
     * @brief Tries to get the encapsulated unsigned 32-bit integer value
     *
     * If the data object is not of type #UInt32 this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #UInt32)
     *
     * @return the encapsulated unsigned 32-bit integer value or @c 0 if it fails
     *
     * @see fromUInt32()
     */
    Q_UINT32 toUInt32(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given signed 64-bit integer @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #Int64 containing the @p value
     *
     * @see toInt64()
     */
    static QDBusData fromInt64(Q_INT64 value);

    /**
     * @brief Tries to get the encapsulated signed 64-bit integer value
     *
     * If the data object is not of type #Int64 this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Int64)
     *
     * @return the encapsulated signed 64-bit integer value or @c 0 if it fails
     *
     * @see fromInt64()
     */
    Q_INT64 toInt64(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given unsigned 64-bit integer @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #UInt64 containing the @p value
     *
     * @see toUInt64()
     */
    static QDBusData fromUInt64(Q_UINT64 value);

    /**
     * @brief Tries to get the encapsulated unsigned 64-bit integer value
     *
     * If the data object is not of type #UInt64 this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #UInt64)
     *
     * @return the encapsulated unsigned 64-bit integer value or @c 0 if it fails
     *
     * @see fromUInt64()
     */
    Q_UINT64 toUInt64(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given double @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #Double containing the @p value
     *
     * @see toDouble()
     */
    static QDBusData fromDouble(double value);

    /**
     * @brief Tries to get the encapsulated double value
     *
     * If the data object is not of type #Double this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Double)
     *
     * @return the encapsulated double value or @c 0.0 if it fails
     *
     * @see fromDouble()
     */
    double toDouble(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given string @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #String containing the @p value
     *
     * @see toString()
     */
    static QDBusData fromString(const QString& value);

    /**
     * @brief Tries to get the encapsulated string value
     *
     * If the data object is not of type #String this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #String)
     *
     * @return the encapsulated string value or @c QString::null if it fails
     *
     * @see fromString()
     */
    QString toString(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given object path @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #ObjectPath containing the @p value
     *
     * @see toObjectPath()
     */
    static QDBusData fromObjectPath(const QDBusObjectPath& value);

    /**
     * @brief Tries to get the encapsulated object path value
     *
     * If the data object is not of type #ObjectPath this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #ObjectPath)
     *
     * @return the encapsulated object path value or an empty and invalid object
     *         if it fails
     *
     * @see fromObjectPath()
     */
    QDBusObjectPath toObjectPath(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p list
     *
     * \note The list is allowed to be empty but is required to have a valid type
     *
     * Unless the list the is empty, the convenience method fromQValueList() will
     * usually be easier to use since it does not require to create a
     * QDBusDataList first. For empty lists this method has to be used to
     * make sure there is sufficient type information on the list's elements
     * available for the binding's marshalling code.
     *
     * @param list the list to encapsulate
     *
     * @return a data object of type #List containing the @p list or
     *         an #Invalid object if the list's type is #Invalid
     *
     * @see toList()
     */
    static QDBusData fromList(const QDBusDataList& list);

    /**
     * @brief Tries to get the encapsulated list
     *
     * If the data object is not of type #List this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #List)
     *
     * @return the encapsulated list or an empty and #Invalid list if it fails
     *
     * @see fromList()
     */
    QDBusDataList toList(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p list
     *
     * @warning All elements of the list have to be of the same #Type
     *
     * Convenience overload for fromList(), usually more straight forward to use
     * because it doesn't require to create a QDBusDataList object first,
     * however it can only handle lists which contain elements, for empty lists
     * fromList() is the only option.
     *
     * @param list the list to encapsulate
     *
     * @return a data object of type #List containing the @p list or
     *         an #Invalid object if the list is empty or if elements have
     *         different types.
     *
     * @see toQValueList()
     */
    static QDBusData fromQValueList(const QValueList<QDBusData>& list);

    /**
     * @brief Tries to get the encapsulated list
     *
     * Convenience overload for toList().
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #List)
     *
     * @return the encapsulated list or an empty and #Invalid list if it fails
     *
     * @see fromQValueList()
     */
    QValueList<QDBusData> toQValueList(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given struct's @p memberList
     *
     * See the documentation of #Struct for an example.
     *
     * @param memberList the list of already encapsulated struct members
     *
     * @return a data object of type #Struct containing the @p memberList
     *
     * @see toStruct()
     */
    static QDBusData fromStruct(const QValueList<QDBusData>& memberList);

    /**
     * @brief Tries to get the encapsulated struct memberList
     *
     * If the data object is not of type #Struct this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * See the documentation of #Struct for an example.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Struct)
     *
     * @return the encapsulated memberList or an empty list if it fails
     *
     * @see fromStruct()
     */
    QValueList<QDBusData> toStruct(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given variant @p value
     *
     * @param value the value to encapsulate
     *
     * @return a data object of type #Variant containing the @p value
     *
     * @see toVariant()
     */
    static QDBusData fromVariant(const QDBusVariant& value);

    /**
     * @brief Tries to get the encapsulated variant value
     *
     * If the data object is not of type #Variant this will fail, i.e.
     * the parameter @p ok will be set to @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Variant)
     *
     * @return the encapsulated variant value or an empty variant if it fails
     *
     * @see fromVariant()
     */
    QDBusVariant toVariant(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #Byte.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toByteKeyMap()
     */
    static QDBusData fromByteKeyMap(const QDBusDataMap<Q_UINT8>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not #Byte
     * this will fail, i.e. the parameter @p ok will be set to @c false if
     * present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #Byte)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromByteKeyMap()
     */
    QDBusDataMap<Q_UINT8> toByteKeyMap(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #Int16.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toInt16KeyMap()
     */
    static QDBusData fromInt16KeyMap(const QDBusDataMap<Q_INT16>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not #Int16
     * this will fail, i.e. the parameter @p ok will be set to @c false if
     * present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #Int16)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromInt16KeyMap()
     */
    QDBusDataMap<Q_INT16> toInt16KeyMap(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #UInt16.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toUInt16KeyMap()
     */
    static QDBusData fromUInt16KeyMap(const QDBusDataMap<Q_UINT16>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not #UInt16
     * this will fail, i.e. the parameter @p ok will be set to @c false if
     * present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #UInt16)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromUInt16KeyMap()
     */
    QDBusDataMap<Q_UINT16> toUInt16KeyMap(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #Int32.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toInt32KeyMap()
     */
    static QDBusData fromInt32KeyMap(const QDBusDataMap<Q_INT32>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not #Int32
     * this will fail, i.e. the parameter @p ok will be set to @c false if
     * present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #Int32)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromInt32KeyMap()
     */
    QDBusDataMap<Q_INT32> toInt32KeyMap(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #UInt32.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toUInt32KeyMap()
     */
    static QDBusData fromUInt32KeyMap(const QDBusDataMap<Q_UINT32>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not #UInt32
     * this will fail, i.e. the parameter @p ok will be set to @c false if
     * present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #UInt32)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromUInt32KeyMap()
     */
    QDBusDataMap<Q_UINT32> toUInt32KeyMap(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #Int64.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toInt64KeyMap()
     */
    static QDBusData fromInt64KeyMap(const QDBusDataMap<Q_INT64>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not #Int64
     * this will fail, i.e. the parameter @p ok will be set to @c false if
     * present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #Int64)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromInt64KeyMap()
     */
    QDBusDataMap<Q_INT64> toInt64KeyMap(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #UInt64.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toUInt64KeyMap()
     */
    static QDBusData fromUInt64KeyMap(const QDBusDataMap<Q_UINT64>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not #UInt64
     * this will fail, i.e. the parameter @p ok will be set to @c false if
     * present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #UInt64)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromUInt64KeyMap()
     */
    QDBusDataMap<Q_UINT64> toUInt64KeyMap(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #String.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toStringKeyMap()
     */
    static QDBusData fromStringKeyMap(const QDBusDataMap<QString>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not #String
     * this will fail, i.e. the parameter @p ok will be set to @c false if
     * present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #String)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromStringKeyMap()
     */
    QDBusDataMap<QString> toStringKeyMap(bool* ok = 0) const;

    /**
     * @brief Creates a data object for the given @p map
     *
     * \note The map is allowed to be empty but is required to have a valid
     *       value type
     *
     * The resulting data object will have the keyType() set to #ObjectPath.
     *
     * @param map the map to encapsulate
     *
     * @return a data object of type #Map containing the @p map or
     *         an #Invalid object if the map's value type is #Invalid
     *
     * @see toObjectPathKeyMap()
     */
    static QDBusData fromObjectPathKeyMap(const QDBusDataMap<QDBusObjectPath>& map);

    /**
     * @brief Tries to get the encapsulated map
     *
     * If the data object is not of type #Map or if its value type is not
     * #ObjectPath this will fail, i.e. the parameter @p ok will be set to
     * @c false if present.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type #Map or
     *        value type not #ObjectPath)
     *
     * @return the encapsulated map or an empty and #Invalid map if it fails
     *
     * @see fromObjectPathKeyMap()
     */
    QDBusDataMap<QDBusObjectPath> toObjectPathKeyMap(bool* ok = 0) const;

    /**
     * @brief Creates the data objects D-Bus signature
     *
     * Recursivly builds the D-Bus signature of the data object if it holds a
     * container type, i.e. if the object is of type #List, #Map or #Struct
     *
     * This can be used to create a signature for QDBusVariant when creating one
     * for sending over D-Bus.
     *
     * @return a string containing the content's signature, or a null string
     *         if the data object is #Invalid
     */
    QCString buildDBusSignature() const;

private:
    class Private;
    Private* d;
};

#endif
