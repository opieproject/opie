/* qdbusobjectpath.h DBUS object path data type
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

#ifndef QDBUSOBJECTPATH_H
#define QDBUSOBJECTPATH_H

#include <qcstring.h>

/**
 * @brief Class for representing D-Bus object paths
 *
 * This data type is necessary to correctly represent object paths in the
 * context of D-Bus messages, since normal strings have a different D-Bus
 * signature than object paths.
 *
 * @see @ref dbusconventions-objectpath
 */
class QDBusObjectPath : public QCString
{
public:
    /**
     * @brief Creates an empty and invalid object path
     */
    QDBusObjectPath();

    /**
     * @brief Creates copy of the given @p other object path
     *
     * @param other the object path to copy
     */
    QDBusObjectPath(const QDBusObjectPath& other);

    /**
     * @brief Creates copy of the given @p other object path
     *
     * @param other the object path to copy
     */
    QDBusObjectPath(const QCString& other);

    /**
     * @brief Creates an object path for an object as a child of the parent node
     *
     * This is basically like specifying a directory and a file name to create
     * the file's full path.
     *
     * Example:
     * @code
     * QDBusObjectPath rootNode("/"); // => "/"
     *
     * QDBusObjectPath childNod(rootNode, "child"); // => "/child"
     *
     * QDBusObjectPath grandChildNode(childNode, "grandchild"); // => "/child/grandchild"
     * @endcode
     *
     * @param parentNode the object path to create the child on
     * @param nodeName the name of the child node
     */
    QDBusObjectPath(const QDBusObjectPath& parentNode, const QCString& nodeName);

    /**
     * @brief Returns whether the current content is considered a valid object path
     *
     * @note Calls validate() to perform a check on the current content
     *
     * @return \c true if the object's content describe a valid object path,
     *         otherwise @c false
     *
     * @see @ref dbusconventions-objectpath
     */
    bool isValid() const;

    /**
     * @brief Returns the object path of this path's parent node
     *
     * This is basically like getting the directory of an file path
     *
     * @return the parent node's object path or an empty and invalid object
     *         if this is already the root node
     */
    QDBusObjectPath parentNode() const;

    /**
     * @brief Checks the given string for validity as a D-Bus object path
     *
     * See section @ref dbusconventions-objectpath for information on object
     * path formatting.
     *
     * @param path the string to check
     *
     * @return @c -1 if the object path is valid, otherwise the index of the
     *         first violating character
     *
     * @see isValid()
     */
    static int validate(const QCString& path);
};

#endif
