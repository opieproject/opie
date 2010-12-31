/* qdbusdatalist.h list of DBUS data transport type
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

#ifndef QDBUSDATALIST_H
#define QDBUSDATALIST_H

#include "dbus/qdbusdata.h"

template <typename T> class QValueList;
class QDBusObjectPath;
class QDBusVariant;
class QString;
class QStringList;

/**
 * @brief Class to transport lists of D-Bus data types
 *
 * \note while the D-Bus data type is actually called @c array this bindings
 *       use the term @c list since the behavior and characteristics of the
 *       implementation is more list like.
 *
 * There are basically two ways to create QDBusDataList objects:
 * - non-empty from content
 * - empty by specifying the desired element type
 *
 * Example for creating a filled list from content
 * @code
 * QValueList<Q_INT16> intList;
 * list << 2 << 3 << 5 << 7;
 *
 * QDBusDataList dbusList(intList);
 * QDBusData data = QDBusData::fromList(dbusList);
 *
 * // or even shorter, using implicit conversion
 * QDBusData other = QDBusData::fromList(intList);
 * @endcode
 *
 * Example for creating an empty list
 * @code
 * // empty list for a simple type
 * QDBusDataList list(QDBusData::Double);
 *
 * // empty list for a list of string lists
 * QDBusData elementType = QDBusData::fromList(QDBusDataList(QDBusData::String));
 * QDBusDataList outerList(elementType);
 * @endcode
 *
 * @see QDBusDataMap
 */
class QDBUS_EXPORT QDBusDataList
{
public:
    /**
     * @brief Creates an empty and invalid list
     *
     * @see QDBusData::Invalid
     */
    QDBusDataList();

    /**
     * @brief Creates an empty list with the given simple type for elements
     *
     * The given type has be one of the non-container types, i.e. any other than
     * QDBusData::Map, QDBusData::List or QDBusData::Struct
     *
     * For creating a list with elements which are containers themselves, use
     * QDBusDataList(const QDBusData&);
     *
     * @param simpleItemType the type of the elements in the new list
     */
    explicit QDBusDataList(QDBusData::Type simpleItemType);

    /**
     * @brief Creates an empty list with the given container type for elements
     *
     * For creating a list with simple elements you can also use
     * QDBusDataList(QDBusData::Type);
     *
     * @param containerItemType the type of the elements in the new list
     *
     * @see hasContainerItemType()
     */
    explicit QDBusDataList(const QDBusData& containerItemType);

    /**
     * @brief Creates a list from the given @p other list
     *
     * This behaves basically like copying a QValueList through its copy
     * constructor, i.e. no value are actually copied at this time.
     *
     * @param other the other list object to copy from
     */
    QDBusDataList(const QDBusDataList& other);

    /**
     * @brief Creates a list from the given QValueList of QDBusData objects
     *
     * If the @p other list is empty, this will behave like QDBusDataList(),
     * i.e. create an empty and invalid list object.
     *
     * Type information for the list object, i.e. element type and, if applicable,
     * container item type, will be derived from the @p other list's elements.
     *
     * \warning if the elements of the @p other list do not all have the same
     *          type, the list object will also be empty and invalid
     *
     * @param other the QValueList of QDBusData objects to copy from
     *
     * @see toQValueList()
     */
    QDBusDataList(const QValueList<QDBusData>& other);

    /**
     * @brief Creates a list from the given QValueList of boolean values
     *
     * Type information for the list object will be set to QDBusData::Bool
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::Bool
     *
     * @param other the QValueList of boolean values to copy from
     *
     * @see toBoolList()
     */
    QDBusDataList(const QValueList<bool>& other);

    /**
     * @brief Creates a list from the given QValueList of byte (unsigned char) values
     *
     * Type information for the list object will be set to QDBusData::Byte
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::Byte
     *
     * @param other the QValueList of byte (unsigned char) values to copy from
     *
     * @see toByteList()
     */
    QDBusDataList(const QValueList<Q_UINT8>& other);

    /**
     * @brief Creates a list from the given QValueList of signed 16-bit integer values
     *
     * Type information for the list object will be set to QDBusData::Int16
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::Int16
     *
     * @param other the QValueList of signed 16-bit integer values to copy from
     *
     * @see toInt16List()
     */
    QDBusDataList(const QValueList<Q_INT16>& other);

    /**
     * @brief Creates a list from the given QValueList of unsigned 16-bit integer values
     *
     * Type information for the list object will be set to QDBusData::UInt16
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::UInt16
     *
     * @param other the QValueList of unsigned 16-bit integer values to copy from
     *
     * @see toUInt16List()
     */
    QDBusDataList(const QValueList<Q_UINT16>& other);

    /**
     * @brief Creates a list from the given QValueList of signed 32-bit integer values
     *
     * Type information for the list object will be set to QDBusData::Int32
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::Int32
     *
     * @param other the QValueList of signed 32-bit integer values to copy from
     *
     * @see toInt32List()
     */
    QDBusDataList(const QValueList<Q_INT32>& other);

    /**
     * @brief Creates a list from the given QValueList of unsigned 32-bit integer values
     *
     * Type information for the list object will be set to QDBusData::UInt16
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::UInt32
     *
     * @param other the QValueList of unsigned 32-bit integer values to copy from
     *
     * @see toUInt32List()
     */
    QDBusDataList(const QValueList<Q_UINT32>& other);

    /**
     * @brief Creates a list from the given QValueList of signed 64-bit integer values
     *
     * Type information for the list object will be set to QDBusData::Int64
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::Int64
     *
     * @param other the QValueList of signed 64-bit integer values to copy from
     *
     * @see toInt64List()
     */
    QDBusDataList(const QValueList<Q_INT64>& other);

    /**
     * @brief Creates a list from the given QValueList of unsigned 64-bit integer values
     *
     * Type information for the list object will be set to QDBusData::UInt64
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::UInt64
     *
     * @param other the QValueList of unsigned 64-bit integer values to copy from
     *
     * @see toUInt64List()
     */
    QDBusDataList(const QValueList<Q_UINT64>& other);

    /**
     * @brief Creates a list from the given QValueList of double values
     *
     * Type information for the list object will be set to QDBusData::Double
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::Double
     *
     * @param other the QValueList of double values to copy from
     *
     * @see toDoubleList()
     */
    QDBusDataList(const QValueList<double>& other);

    /**
     * @brief Creates a list from the given QValueList of QDBusVariant values
     *
     * Type information for the list object will be set to QDBusData::Variant
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::Variant
     *
     * @param other the QValueList of variant values to copy from
     *
     * @see toVariantList()
     */
    QDBusDataList(const QValueList<QDBusVariant>& other);

    /**
     * @brief Creates a list from the given QStringList's values
     *
     * Type information for the list object will be set to QDBusData::String
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::String
     *
     * @param other the QStringList to copy from
     *
     * @see toQStringList()
     */
    QDBusDataList(const QStringList& other);

    /**
     * @brief Creates a list from the given QValueList of object path values
     *
     * Type information for the list object will be set to QDBusData::ObjectPath
     * also when the @p other list is empty, i.e. this allows to create an
     * empty but valid list object, comparable to using
     * QDBusDataList(QDBusData::Type) with QDBusData::ObjectPath
     *
     * @param other the QValueList of object path values to copy from
     *
     * @see toObjectPathList()
     */
    QDBusDataList(const QValueList<QDBusObjectPath>& other);

    /**
     * @brief Destroys the list object
     */
    ~QDBusDataList();

    /**
     * @brief Copies from the given @p other list
     *
     * This behaves basically like copying a QValueList through its assignment
     * operator, i.e. no value are actually copied at this time.
     *
     * @param other the other list object to copy from
     *
     * @return a reference to this list object
     */
    QDBusDataList& operator=(const QDBusDataList& other);

    /**
     * @brief Copies from the given @p other list
     *
     * This behaves basically like copying a QValueList through its assignment
     * operator, i.e. no value are actually copied at this time.
     *
     * \warning the elements of the given @p other list have to be of the same
     *          type. If they aren't this list's content will cleared and the
     *          type will be set to QDBusData::Invalid
     *
     * @param other the other list object to copy from
     *
     * @return a reference to this list object
     */
    QDBusDataList& operator=(const QValueList<QDBusData>& other);

    /**
     * @brief Copies from the given @p other list
     *
     * Convenience overload as QStringList is a very common data type in
     * Qt and D-Bus methods also use "arrays of strings" quite often.
     *
     * The list object's type will be set to QDBusData::String. If the object
     * previously had a container as its element type, this will be reset, i.e.
     * hasContainerItemType() will return @c false
     *
     * @param other the stringlist to copy from
     *
     * @return a reference to this list object
     */
    QDBusDataList& operator=(const QStringList& other);

    /**
     * @brief Returns the element type of the list object
     *
     * @return one of the values of the QDBusData#Type enum
     *
     * @see hasContainerItemType()
     * @see containerItemType()
     */
    QDBusData::Type type() const;

    /**
     * @brief Checks whether the element type is a data container itself
     *
     * If the elements of the list are containers as well, this will return
     * @c true
     * In this case containerItemType() will return a prototype for such a
     * container.
     *
     * @return @c true if the element type is either QDBusData::Map,
     *         QDBusData::List or QDBusData::Struct, otherwise @c false
     *
     * @see QDBusDataList(const QDBusData&)
     */
    bool hasContainerItemType() const;

    /**
     * @brief Returns a container prototype for the list's element type
     *
     * Lists which have containers as their elements, i.e. hasContainerItemType()
     * returns @c true this will actually specify the details for the use
     * container, i.e. the returned data object can be queried for type and
     * possible further subtypes.
     *
     * @return a data object detailing the element type or an invalid data object
     *         if the list does not have a container as its element type
     *
     * @see QDBusDataList(const QDBusData&);
     * @see type()
     * @see QDBusData::Invalid
     */
    QDBusData containerItemType() const;

    /**
     * @brief Checks whether this list object has a valid element type
     *
     * This is equal to checking type() for not being QDBusData::Invalid
     *
     * @return @c true if the list object is valid, otherwise @c false
     */
    inline bool isValid() const { return type() != QDBusData::Invalid; }

    /**
     * @brief Checks whether this list object has any elements
     *
     * @return @c true if there are no elements in this list, otherwise @c false
     *
     * @see count()
     */
    bool isEmpty() const;

    /**
     * @brief Returns the number of elements of this list object
     *
     * @return the number of elements
     *
     * @see isEmpty()
     */
    uint count() const;

    /**
     * @brief Checks whether the given @p other list is equal to this one
     *
     * Two lists are considered equal when they have the same type (and same
     * container item type if the have one) and the element lists are equal
     * as well.
     *
     * @param other the other list object to compare with
     *
     * @return @c true if the lists are equal, otherwise @c false
     *
     * @see QDBusData::operator==()
     */
    bool operator==(const QDBusDataList& other) const;

    /**
     * @brief Checks whether the given @p other list is different from this one
     *
     * Two lists are considered different when they have the different type (or
     * different container item type if the have one) or the element lists are
     * equal are different.
     *
     * @param other the other list object to compare with
     *
     * @return @c true if the lists are different, otherwise @c false
     *
     * @see QDBusData::operator!=()
     */
    bool operator!=(const QDBusDataList& other) const;

    /**
     * @brief Clears the list
     *
     * Type and, if applicable, container element type will stay untouched.
     */
    void clear();

    /**
     * @brief Appends a given value to the list
     *
     * Basically works like the respective QValueList operator, but checks if
     * type of the new value matches the type of the list.
     * Lists that are invalid will accept any new type and will then be
     * typed accordingly.
     *
     * If @p data is invalid itself, it will not be appended at any time.
     *
     * \note the more common use case is to work with a QValueList and then
     *       use the respective constructor to create the QDBusDataList object
     *
     * @param data the data item to append to the list
     *
     * @return a reference to this list object
     */
    QDBusDataList& operator<<(const QDBusData& data);

    /**
     * @brief Converts the list object into a QValueList with QDBusData elements
     *
     * @return the values of the list object as a QValueList
     */
    QValueList<QDBusData> toQValueList() const;

    /**
     * @brief Tries to get the list object's elements as a QStringList
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::String.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::String)
     *
     * @return a QStringList containing the list object's string elements or
     *         an empty list when converting fails
     *
     * @see toStringList()
     * @see QDBusData::toString()
     */
    QStringList toQStringList(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of bool
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::Bool.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::Bool)
     *
     * @return a QValueList of bool containing the list object's boolean
     *         elements or an empty list when converting fails
     *
     * @see QDBusData::toBool()
     */
    QValueList<bool> toBoolList(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of Q_UINT8
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::Byte.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::Byte)
     *
     * @return a QValueList of Q_UINT8 containing the list object's byte
     *         elements or an empty list when converting fails
     *
     * @see QDBusData::toByte()
     */
    QValueList<Q_UINT8> toByteList(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of Q_INT16
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::Int16.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::Int16)
     *
     * @return a QValueList of Q_INT16 containing the list object's
     *         signed 16-bit integer elements or an empty list when converting
     *         fails
     *
     * @see QDBusData::toInt16()
     */
    QValueList<Q_INT16> toInt16List(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of Q_UINT16
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::UInt16.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::UInt16)
     *
     * @return a QValueList of Q_UINT16 containing the list object's
     *         unsigned 16-bit integer elements or an empty list when converting
     *         fails
     *
     * @see QDBusData::toUInt16()
     */
    QValueList<Q_UINT16> toUInt16List(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of Q_INT32
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::Int32.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::Int32)
     *
     * @return a QValueList of Q_INT32 containing the list object's
     *         signed 32-bit integer elements or an empty list when converting
     *         fails
     *
     * @see QDBusData::toInt32()
     */
    QValueList<Q_INT32> toInt32List(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of Q_UINT32
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::UInt32.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::UInt32)
     *
     * @return a QValueList of Q_UINT32 containing the list object's
     *         unsigned 32-bit integer elements or an empty list when converting
     *         fails
     *
     * @see QDBusData::toUInt32()
     */
    QValueList<Q_UINT32> toUInt32List(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of Q_INT64
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::Int64.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::Int64)
     *
     * @return a QValueList of Q_INT64 containing the list object's
     *         signed 64-bit integer elements or an empty list when converting
     *         fails
     *
     * @see QDBusData::toInt64()
     */
    QValueList<Q_INT64> toInt64List(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of Q_UINT64
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::UInt64.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::UInt64)
     *
     * @return a QValueList of Q_UINT64 containing the list object's
     *         unsigned 64-bit integer elements or an empty list when converting
     *         fails
     *
     * @see QDBusData::toUInt64()
     */
    QValueList<Q_UINT64> toUInt64List(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of double
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::Double.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::Double)
     *
     * @return a QValueList of double containing the list object's double
     *         elements or an empty list when converting fails
     *
     * @see QDBusData::toDouble()
     */
    QValueList<double> toDoubleList(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of QString
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::String, see also toQStringList().
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::String)
     *
     * @return a QValueList of QString containing the list object's string
     *         elements or an empty list when converting fails
     *
     * @see QDBusData::toString()
     */
    QValueList<QString> toStringList(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of object paths
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::ObjectPath)
     *
     * @return a QValueList of object paths containing the list object's object path
     *         elements or an empty list when converting fails
     *
     * @see QDBusData::toObjectPath()
     */
    QValueList<QDBusObjectPath> toObjectPathList(bool* ok = 0) const;

    /**
     * @brief Tries to get the list object's elements as a QValueList of QDBusVariant
     *
     * This is a convenience overload for the case when the list is of
     * type QDBusData::Variant.
     *
     * @param ok optional pointer to a bool variable to store the
     *        success information in, i.e. will be set to @c true on success
     *        and to @c false if the conversion failed (not of type
     *        QDBusData::Variant)
     *
     * @return a QValueList of QDBusVariant containing the list object's
     *         QDBusVariant elements or an empty list when converting fails
     *
     * @see QDBusData::toVariant()
     */
    QValueList<QDBusVariant> toVariantList(bool* ok = 0) const;

private:
    class Private;
    Private* d;
};

#endif
