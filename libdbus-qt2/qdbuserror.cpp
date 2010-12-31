/* qdbuserror.cpp QDBusError object
 *
 * Copyright (C) 2005 Harald Fernengel <harry@kdevelop.org>
 * Copyright (C) 2005-2007 Kevin Krammer <kevin.krammer@gmx.at>
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

#include "dbus/qdbuserror.h"

#include <dbus/dbus.h>

#include <qmap.h>

typedef QMap<QString, QDBusError::ErrorType> ErrorNameMap;
static ErrorNameMap errorTypesByName;

static QString qDBusErrorNameForType(QDBusError::ErrorType type)
{
    switch (type)
    {
        case QDBusError::InvalidError:
            Q_ASSERT(false);
            return QString::null;

        case QDBusError::Failed:
            return QString::fromUtf8(DBUS_ERROR_FAILED);
        case QDBusError:: NoMemory:
            return QString::fromUtf8(DBUS_ERROR_NO_MEMORY);
        case QDBusError:: ServiceUnknown:
            return QString::fromUtf8(DBUS_ERROR_SERVICE_UNKNOWN);
        case QDBusError:: NameHasNoOwner:
            return QString::fromUtf8(DBUS_ERROR_NAME_HAS_NO_OWNER);
        case QDBusError:: NoReply:
            return QString::fromUtf8(DBUS_ERROR_NO_REPLY);
        case QDBusError:: IOError:
            return QString::fromUtf8(DBUS_ERROR_IO_ERROR);
        case QDBusError:: BadAddress:
            return QString::fromUtf8(DBUS_ERROR_BAD_ADDRESS);
        case QDBusError:: NotSupported:
            return QString::fromUtf8(DBUS_ERROR_NOT_SUPPORTED);
        case QDBusError:: LimitsExceeded:
            return QString::fromUtf8(DBUS_ERROR_LIMITS_EXCEEDED);
        case QDBusError:: AccessDenied:
            return QString::fromUtf8(DBUS_ERROR_ACCESS_DENIED);
        case QDBusError:: AuthFailed:
            return QString::fromUtf8(DBUS_ERROR_AUTH_FAILED);
        case QDBusError:: NoServer:
            return QString::fromUtf8(DBUS_ERROR_NO_SERVER);
        case QDBusError:: Timeout:
            return QString::fromUtf8(DBUS_ERROR_TIMEOUT);
        case QDBusError:: NoNetwork:
            return QString::fromUtf8(DBUS_ERROR_NO_NETWORK);
        case QDBusError:: Disconnected:
            return QString::fromUtf8(DBUS_ERROR_DISCONNECTED);
        case QDBusError:: InvalidArgs:
            return QString::fromUtf8(DBUS_ERROR_INVALID_ARGS);
        case QDBusError:: FileNotFound:
            return QString::fromUtf8(DBUS_ERROR_FILE_NOT_FOUND);
        case QDBusError:: FileExists:
            return QString::fromUtf8(DBUS_ERROR_FILE_EXISTS);
        case QDBusError:: UnknownMethod:
            return QString::fromUtf8(DBUS_ERROR_UNKNOWN_METHOD);
        case QDBusError:: TimedOut:
            return QString::fromUtf8(DBUS_ERROR_TIMED_OUT);
        case QDBusError:: InvalidSignature:
            return QString::fromUtf8(DBUS_ERROR_INVALID_SIGNATURE);

        case QDBusError::UserDefined:
            Q_ASSERT(false);
            return QString::null;
    }

    Q_ASSERT(false);
    return QString::null;
}

static void qDBusErrorSetupNameMapping()
{
    for (int i = QDBusError::InvalidError + 1; i < QDBusError::UserDefined; ++i)
    {
        QDBusError::ErrorType type = static_cast<QDBusError::ErrorType>(i);
        errorTypesByName[qDBusErrorNameForType(type)] = type;
    }
}

static QDBusError::ErrorType qDBusErrorTypeForName(const QString& name)
{
    if (name.isEmpty()) return QDBusError::InvalidError;

    if (errorTypesByName.isEmpty())
        qDBusErrorSetupNameMapping();

    ErrorNameMap::const_iterator it = errorTypesByName.find(name);
    if (it != errorTypesByName.end()) return it.data();

    return QDBusError::UserDefined;
}

QDBusError::QDBusError() : errorType(InvalidError)
{
}

QDBusError::QDBusError(const DBusError *error) : errorType(InvalidError)
{
    if (!error || !dbus_error_is_set(error))
        return;

    nm = QString::fromUtf8(error->name);
    msg = QString::fromUtf8(error->message);

    errorType = qDBusErrorTypeForName(nm);
}

QDBusError::QDBusError(const QString& error, const QString& message)
    : errorType(UserDefined), nm(error), msg(message)
{
    errorType = qDBusErrorTypeForName(nm);
}

bool QDBusError::isValid() const
{
    return errorType != InvalidError && !nm.isEmpty() && !msg.isEmpty();
}

QDBusError::QDBusError(ErrorType type, const QString& message)
    : errorType(type), msg(message)
{
    nm = qDBusErrorNameForType(type);
}

QDBusError QDBusError::stdFailed(const QString& message)
{
    return QDBusError(QDBusError::Failed, message);
}

QDBusError QDBusError::stdNoMemory(const QString& message)
{
    return QDBusError(QDBusError::NoMemory, message);
}

QDBusError QDBusError::stdNoReply(const QString& message)
{
    return QDBusError(QDBusError::NoReply, message);
}

QDBusError QDBusError::stdIOError(const QString& message)
{
    return QDBusError(QDBusError::IOError, message);
}

QDBusError QDBusError::stdNotSupported(const QString& message)
{
    return QDBusError(QDBusError::NotSupported, message);
}

QDBusError QDBusError::stdLimitsExceeded(const QString& message)
{
    return QDBusError(QDBusError::LimitsExceeded, message);
}

QDBusError QDBusError::stdAccessDenied(const QString& message)
{
    return QDBusError(QDBusError::AccessDenied, message);
}

QDBusError QDBusError::stdAuthFailed(const QString& message)
{
    return QDBusError(QDBusError::AuthFailed, message);
}

QDBusError QDBusError::stdTimeout(const QString& message)
{
    return QDBusError(QDBusError::Timeout, message);
}

QDBusError QDBusError::stdInvalidArgs(const QString& message)
{
    return QDBusError(QDBusError::InvalidArgs, message);
}

QDBusError QDBusError::stdFileNotFound(const QString& message)
{
    return QDBusError(QDBusError::FileNotFound, message);
}

QDBusError QDBusError::stdFileExists(const QString& message)
{
    return QDBusError(QDBusError::FileExists, message);
}

QDBusError QDBusError::stdUnknownMethod(const QString& message)
{
    return QDBusError(QDBusError::UnknownMethod, message);
}

QDBusError QDBusError::stdInvalidSignature(const QString& message)
{
    return QDBusError(QDBusError::InvalidSignature, message);
}
