/* qdbuserror.h QDBusError object
 *
 * Copyright (C) 2005 Harald Fernengel <harry@kdevelop.org>
 * Copyright (C) 2005 Kevin Krammer <kevin.krammer@gmx.at>
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

#ifndef QDBUSERROR_H
#define QDBUSERROR_H

#include "dbus/qdbusmacros.h"
#include <qstring.h>

struct DBusError;

/**
 * @brief Class for transporting D-Bus errors
 *
 * A D-Bus error has two parts: an error name (see section
 * @ref dbusconventions-errorname) and a message string detailing the error in
 * human presentable form.
 */
class QDBUS_EXPORT QDBusError
{
public:
    /**
     * @brief Enum of standard D-Bus error types
     *
     * D-Bus defines a list of common error types and their names.
     * The values of this enum map to those an application is likely to encounter
     * and likely to create itself.
     *
     * Standard errors can either be created by specifying the D-Bus error name
     * or, as a convenience, by using factory methods of this class for the
     * most common ones.
     *
     * All D-Bus standard error names are within the @c "org.freedesktop.DBus.Error"
     * namespace.
     *
     * @see name()
     */
    enum ErrorType
    {
        /**
         * @brief QDBusError specific value, to represent invalid error objects.
         *
         * @see isValid()
         */
        InvalidError,

        /**
         * @brief Generic failure cause
         *
         * Can be used whenever the other predefined errors do no match. Basically
         * just meaning "something when wrong, see message() for details"
         *
         * @see stdFailed()
         */
        Failed,

        /**
         * @brief An operation could not allocate enough memory
         *
         * @see stdNoMemory()
         */
        NoMemory,

        /**
         * @brief An addressed service is neither connected nor can it be activated
         */
        ServiceUnknown,

        /**
         * @brief A non-unique name used in a message is not known
         *
         * If a message addresses a D-Bus connection through a non-unique
         * (requested) name and the D-Bus does not have a mapping to any of the
         * unique names.
         */
        NameHasNoOwner,

        /**
         * @brief An call failed to send a reply but one was expected
         *
         * @see stdNoReply()
         */
        NoReply,

        /**
         * @brief An IO error occured during an operation
         *
         * Generic indicator that some kind of IO operation failed, e.g.
         * reading from a socket.
         *
         * @see stdIOError()
         */
        IOError,

        /**
         * @brief Caused by trying to connect to a malformed address
         *
         * Returned by QDBusConnection's addConnection if the specified address
         * isn't a valid D-Bus bus address.
         *
         * @see QDBusConnection::addConnection(const QString&,const QString&);
         */
        BadAddress,

        /**
         * @brief An otherwise valid operation request could not be handled
         *
         * Primarily useful when a service implements a specific interface but
         * does not (yet) handle all possible situations.
         *
         * @see stdNotSupported()
         */
        NotSupported,

        /**
         * @brief Use of a limited resource reached its limit
         *
         * @see stdLimitsExceeded()
         */
        LimitsExceeded,

        /**
         * @brief Caused by security restrictions denying an operation
         *
         * Primarily useful when a client tries to manipulate resources a service
         * has associated with a different client and which should not be changable
         * by anyone else.
         *
         * @see stdAccessDenied()
         */
        AccessDenied,

        /**
         * @brief An authentification mechanism failed
         *
         * @see stdAuthFailed()
         */
        AuthFailed,

        /**
         * @brief Connection to a D-Bus server failed
         */
        NoServer,

        /**
         * @brief An timeout occured during an operation
         *
         * @warning D-Bus defined to quite similar errors but does not detail
         *          when either one can occur. See #TimedOut
         *
         * @see stdTimeout()
         */
        Timeout,

        /**
         * @brief The network intended as a transport channel is not available
         */
        NoNetwork,

        /**
         * @brief Caused by trying to use an unconnected D-Bus connection
         *
         * @see QDBusConnection::isConnected()
         */
        Disconnected,

        /**
         * @brief Caused by invalid arguments passed to a method call
         *
         * Primarily usefull for service implementations when the incoming
         * call does not transport the expected parameters, e.g. wrong types
         * or wrong values.
         *
         * @see stdInvalidArgs()
         */
        InvalidArgs,

        /**
         * @brief A file necessary for an operation is not avaiable
         *
         * @see stdFileNotFound()
         */
        FileNotFound,

        /**
         * @brief Target file exists but operation does not allow overwriting
         *
         * @see stdFileExists()
         */
        FileExists,

        /**
         * @brief A method call addresses and unknown method
         *
         * @see stdUnknownMethod()
         */
        UnknownMethod,

        /**
         * @brief An operation timed out
         *
         * @warning D-Bus defined to quite similar errors but does not detail
         *          when either one can occur. See #Timeout
         */
        TimedOut,

        /**
         * @brief An type signature is not valid
         *
         * A possible cause is a QDBusVariant with an invalid signature, i.e.
         * the transported signature is empty, contains unknown type characters
         * or has mismatched container enclosings.
         *
         * @note in case a service implementation wants to indicate that a method
         *       call did not transport the correct parameter types, use
         *       #InvalidArgs instead
         *
         * @see stdInvalidSignature()
         */
        InvalidSignature,

        /**
         * @brief Generic type for all errors not matching on of the other predefined
         *
         * @see QDBusError(const QString&,const QString&);
         */
        UserDefined
    };

    /**
     * @brief Creates an empty and invalid error object
     */
    QDBusError();

    /**
     * @brief Creates an error object from an C API D-Bus error object
     *
     * @param error a pointer to the C API D-Bus error
     */
    QDBusError(const DBusError *error);

    /**
     * @brief Creates an error object for its two given components
     *
     * @param error a D-Bus error name
     * @param message the potentially i18n'ed error description message
     *
     * @see name()
     */
    QDBusError(const QString& error, const QString& message);

    /**
     * @brief Returns the D-Bus error name
     *
     * See section @ref dbusconventions-errorname for details.
     *
     * @return the D-Bus error name
     *
     * @see message()
     */
    inline QString name() const { return nm; }

    /**
     * @brief Returns a string describing the error
     *
     * The message is meant to further detail or describe the error.
     * It is usually a translated error message meant for direct
     * presentation to the user.
     *
     * @return the error's message
     *
     * @see name()
     */
    inline QString message() const { return msg; }

    /**
     * @brief Returns a type for checking of standard errors
     *
     * D-Bus specifies a couple of standard error names, which are mapped to
     * QDBusError types in order to make creating and checking for them easier.
     *
     * @return the error's type
     *
     * @see name()
     */
    inline ErrorType type() const { return errorType; }

    /**
     * @brief Returns whether the error object is valid
     *
     * A QDBusError is considered valid if both name and message are set.
     *
     * @return @c true if neither name nor message is @c QString::null and the
     *         error type is a valid type
     */
    bool isValid() const;

    /**
     * @brief Creates a D-Bus standard error for generic failure
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #Failed with the given @p message
     */
    static QDBusError stdFailed(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for out of memory situations
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #NoMemory with the given @p message
     */
    static QDBusError stdNoMemory(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for expected reply missing
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #NoReply with the given @p message
     */
    static QDBusError stdNoReply(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for generic IO errors
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #IOError with the given @p message
     */
    static QDBusError stdIOError(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for unsupported operations
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #NotSupported with the given @p message
     */
    static QDBusError stdNotSupported(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for exceeding a limited resource
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #LimitsExceeded with the given @p message
     */
    static QDBusError stdLimitsExceeded(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for access to a resource being denied
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #AccessDenied with the given @p message
     */
    static QDBusError stdAccessDenied(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for failed authentification
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #AuthFailed with the given @p message
     */
    static QDBusError stdAuthFailed(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for timeouts during operations
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #Timeout with the given @p message
     */
    static QDBusError stdTimeout(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for call arguments being invalid
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #InvalidArgs with the given @p message
     */
    static QDBusError stdInvalidArgs(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for a file not being available
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #FileNotFound with the given @p message
     */
    static QDBusError stdFileNotFound(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for a file being in the way
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #FileExists with the given @p message
     */
    static QDBusError stdFileExists(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for an unknown methods being called
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #UnknownMethod with the given @p message
     */
    static QDBusError stdUnknownMethod(const QString& message);

    /**
     * @brief Creates a D-Bus standard error for D-Bus type signature not valid
     *
     * @param message the message detailing the encountered problem
     *
     * @return an error object of type #InvalidSignature with the given @p message
     */
    static QDBusError stdInvalidSignature(const QString& message);

private:
    ErrorType errorType;

    QString nm, msg;

    /**
     * @brief Creates an error object for one of the standard D-Bus errors
     *
     * @param type one of the standard error causes
     * @param message the potentially i18n'ed error description message
     *
     * @see ErrorType
     */
    QDBusError(ErrorType type, const QString& message);
};

#endif
