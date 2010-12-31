/* qdbusconnection.h QDBusConnection object
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

#ifndef QDBUSCONNECTION_H
#define QDBUSCONNECTION_H

/**
 * @mainpage Qt3 Bindings for D-Bus
 *
 * D-Bus is an IPC (inter process communication) technology designed to allow
 * applications to interoperate without requiring tight coupling.
 *
 * For more information about D-Bus itself see its website:
 * http://www.freedesktop.org/wiki/Software_2fdbus
 *
 * The Qt3 D-Bus bindings described here are a Qt3 style API around the base
 * implementation to enable Qt3 developers to use D-Bus in their applications
 * without requiring them to know about the details of the C based D-Bus API.
 *
 * The two main use cases are:
 * - using the API to access service implemented in other applications.
 *   See section @ref dbusclient for an introduction on this
 *
 * - using the API to provide access to services implemented in your application
 *   See section @ref dbusservice for an introduction on this
 *
 * Of course an application can do both at the same time.
 */

/**
 * @page dbusconventions Naming and syntax conventions in D-Bus
 *
 * @section dbusconventions-servicename Service names
 *
 * The service name is D-Bus application identifier, i.e. either
 * the unique name handed out to the peer application by the bus on connect
 * (see QDBusConnection::uniqueName()) or, more likely, a well known name the
 * peer application has requested, see QDBusConnection::requestName()
 *
 * Such well known names have the form of word separated by dots, like
 * Internet domain names but in reverse order.
 *
 * For example the name for the bus itself (the D-Bus daemon) would be
 * @code
 *   "org.freedesktop.DBus"
 * @endcode
 *
 * @section dbusconventions-objectpath Object paths
 *
 * The object path is like an address within the peer host application.
 * The path format looks like a Unix file path, i.e. words separated by
 * slash @c '/' characters.
 *
 * For example the path for the bus itself (the D-Bus daemon's main object)
 * would be
 * @code
 *   "/org/freedesktop/DBus"
 * @endcode
 *
 * @section dbusconventions-interfacename Interface names
 *
 * The interface name specifies which group of methods and signals
 * implemented by the peer service object is used in proxy operations.
 *
 * Interface names have the form of word separated by dots, like Internet
 * domain names but in reverse order or like a fully qualified Java class name.
 *
 * For example the interface for the bus itself (the D-Bus daemon's main
 * interface) would be
 * @code
 *   "org.freedesktop.DBus"
 * @endcode
 *
 * @section dbusconventions-errorname Error names
 *
 * A D-Bus error name is a sequence of words separated by dots, similar
 * to D-Bus service names or interface names, or like a fully qualified
 * Java class name.
 *
 * For example if a D-Bus service does not handle a method invocation sent
 * to it because it doesn't know about the method it will return a D-Bus
 * error named
 * @code
 *   "org.freedesktop.DBus.Error.UnknownMethod"
 * @endcode
 * QDBusError can create some of the more common errors based on a type value
 * and decode their names into the type respectively. See QDBusError#ErrorType
 *
 * @section dbusconventions-membername Method and signal names
 *
 * There is no mandatory convention for member names, neither for methods nor
 * for signals.
 *
 * However, using the standard interfaces of D-Bus as a hint, it is recommended
 * to use "camel case" names starting with an uppercase letter, for example
 * @code
 *   "GetConnectionUnixUser"
 * @endcode
 */

#include "dbus/qdbusmacros.h"
#include <qstring.h>

class QDBusConnectionPrivate;
class QDBusError;
class QDBusMessage;
class QDBusObjectBase;
class QObject;

/**
 * @brief Provides access to a specific D-Bus bus
 *
 * In order to access a D-Bus message bus an application has to connect to it.
 * This is very similar to connecting to an FTP server using QFtp, where any
 * number of commands can be sent in sequence over the same connection.
 *
 * Additionally to the asynchronous command execution provided by QFtp a
 * QDBusConnection can also execute synchronous (blocking) calls so the
 * code around those calls stays closer to in-process method incovations.
 *
 * However it is recommended to only perform blocking calls on D-Bus service
 * methods that are likely to be processed fast.
 *
 * QDBusConnection implements a shared resource, i.e. if you create a
 * connection with a specific name in one part of your code and then
 * create one with the same name somewhere else, the second creation will
 * actually return the same shared connection object the first caller created.
 *
 * The application can be connected to more than one message bus simultaniously
 * using one or more connections per bus, however the most common case is to
 * have one connection per bus.
 *
 * The two main bus types are:
 * - System bus: a bus connecting applications on one system across user
 *               or session boundaries, for example allowing to communicate
 *               with system services like printer spoolers, etc
 *
 * - Session bus: a bus connecting applications within one user session, for
 *                example started at login or by a session manager. Use cases
 *                or this kind of bus would be accessing user specific
 *                resources like addressbooks, retrieving user settings or
 *                controlling session services (e.g. disabling screensaver
 *                in a video player application during playback)
 *
 * While QDBusConnection provides the basic API to access D-Bus services
 * it is more convenient to use QDBusProxy on top of the connection.
 *
 * See sections @ref dbusclient and @ref dbusservice for examples
 */
class QDBUS_EXPORT QDBusConnection
{
public:
    /**
     * DBus bus types
     */
    enum BusType
    {
        /**
         * The session bus is a user and user session specific message
         * channel. It will usually be started by a login script or a
         * session manager.
         */
        SessionBus,

        /**
         * The system bus is a message channel bridging user level and
         * system level process boundaries, e.g. it can allow a user process
         * with normal user access restrictions to perform a limited subset
         * of operations on a process running with elevated rights.
         *
         * @warning if an applications exposed services on the system bus, i.e.
         *          registers objects using registerObject(), it should be
         *          carefully examined on potential security issues
         */
        SystemBus,

        // TODO find out about ActivationBus purpose
        ActivationBus
    };

    /**
     * @brief Creates an empty/disconnected connection handle
     *
     * This is mainly used for initializing variables of this type, i.e. like
     * the default QString constructor.
     *
     * A variable set to such an empty connection can be assigned a working
     * connection at any time.
     */
    QDBusConnection();

    /**
     * @brief Creates a connection handle to a named connection
     *
     * This will result in an disconnected connection handle if no
     * connection with that name has been created by addConnection before.
     *
     * Therefore it is recommended to use addConnection() instead to get a
     * connection handle.
     *
     * @param name identifier of the shared connection object
     */
    QDBusConnection(const QString &name);

    /**
     * @brief Creates a shallow copy of the given connection
     *
     * Allows to pass connection handles around by value, similar to QString
     * thus avoiding problems like dangling pointers in application code
     *
     * @param other the connection to copy from
     */
    QDBusConnection(const QDBusConnection &other);

    /**
     * @brief Destroys the connection handle
     *
     * If this handle is the last one referencing the shared connection object
     * it will delete it, disconnecting it from any objects it was
     * collaborating with
     */
    ~QDBusConnection();

    /**
     * @brief Creates a shallow copy of the given connection
     *
     * Allows to pass connection handles around by value, similar to QString
     * thus avoiding problems like dangling pointers in application code
     *
     * @param other the connection to copy from
     *
     * @return a reference to this instance as required by assigment operator
     *         semantics
     */
    QDBusConnection &operator=(const QDBusConnection &other);

    /**
     * @brief Returns whether the connection is connected to a bus
     *
     * @return @c true if the connection can be used, @c false if the handle
     *         does not have access to a shared connection object or if the
     *         connection to the bus could not be established or broke
     */
    bool isConnected() const;

    /**
     * @brief Returns the last error seen by the connection
     *
     * This can be a connection error, e.g. attempt to connect failed, or a
     * transmission error or an error reported by a method call
     *
     * @return the last error seen by the connection
     */
    QDBusError lastError() const;

    /**
     * @brief Flags for controlling the behavior name collision handling
     *
     * @see requestName()
     */
    enum NameRequestMode
    {
        /**
         * Do not allow others to take over a name requested by this
         * application
         */
        NoReplace = 0,

        /**
         * Allow other applications that request the same name to get it,
         * i.e. allow the bus to transfer the name from this application
         * to the one requesting it
         */
        AllowReplace = 1,

        /**
         * Try to get the name transferred from the current owner to this
         * application. This will only work if the other application as
         * requested the name using the AllowReplace flag
         */
        ReplaceExisting = 2
    };

    /**
     * @brief Requests to be addressable on the bus by a given name
     *
     * Each connection to a bus gets a unique name once the connection is
     * established. This is similar to getting an IP address when connecting
     * to the Internet.
     *
     * If an application's purpose is to provide services to other applications
     * the other applications require to know how to address the service
     * provider. Similar to a domain name on the Internet D-Bus allows to
     * register names on the bus and be addressed through those names instead
     * of the connection identifier.
     *
     * @note this is not required if the application only needs to acccess
     *       services or only implements generic service APIs
     *
     * If more than one application request the same name, D-Bus will try
     * to resolve this conflict as good as possible.
     * The #NameRequestMode flags allow to control how an application prefers
     * to be treated in such a conflict.
     *
     * @param name the name the connection should be addressable with. See
     *             section @ref dbusconventions-servicename
     * @param modeFlags an OR'ed combination of #NameRequestMode flags
     *
     * @return @c true if the name request was successfull, @c false if
     *         the connection is not connected to a bus or the name is already
     *         taken and cannot be tranferred
     *
     * @see uniqueName()
     */
    bool requestName(const QString &name, int modeFlags = NoReplace);

    /**
     * @brief Returns the connection identifier assigned at connect
     *
     * The unique name is the connection address or identifier the bus assigned
     * to this connection when it got established.
     *
     * @return the connection's unique bus identifier
     *
     * @see requestName()
     */
    QString uniqueName() const;

    /**
     * @brief Sends a message over the bus
     *
     * Sends a message composed through the QDBusMessage API to the bus.
     * This is the main method for service objects (see QDBusObjectBase) to
     * send replies and errors for method calls they accepted or for sending
     * D-Bus signals.
     *
     * @note for doing method calls it is more convenient to use QDBusProxy,
     *       see QDBusProxy::send()
     *
     * @param message the message to send
     *
     * @return @c true if sending succeeded, @c false if the connection is not
     *         connected, if the message lacks information about the recepient
     *         or if sending fails a at a lower level in the communication
     *         stack
     *
     * @see lastError()
     */
    bool send(const QDBusMessage &message) const;

    /**
     * @brief Sends a message over the bus and waits for the reply
     *
     * Sends a message composed through the QDBusMessage API to the bus.
     * It then blocks and waits until the associated reply is received.
     * Any message received in between is stored and can be processed
     * by calling dispatch() or scheduleDispatch()
     *
     * @note for doing method calls it is more convenient to use QDBusProxy,
     *       see QDBusProxy::sendWithReply()
     *
     * @param message the message to send
     * @param error an optional parameter to directly get any error that might
     *              occur during processing of the call
     *
     * @return a message containing either the call's reply or an invalid
     *         message in case the call failed
     *
     * @see lastError()
     */
    QDBusMessage sendWithReply(const QDBusMessage &message, QDBusError *error = 0) const;

    /**
     * @brief Sends a message over the bus, specifying a receiver object for
     *        replies
     *
     * Sends a message composed through the QDBusMessage API to the bus and
     * returns an identifier number to associate with the reply once it is
     * received by the given receiver.
     * See QDBusMessage::replySerialNumber()
     *
     * The required slot signature is
     * @code
     *   void slotname(const QDBusMessage&);
     * @endcode
     *
     * @note for doing method calls it is more convenient to use QDBusProxy,
     *       see QDBusProxy::sendWithAsyncReply()
     *
     * @param message the message to send
     * @param receiver the QObject to relay the reply to
     * @param slot the slot to invoke for the reply
     *
     * @return a numeric identifier for association with the reply or @c 0 if
     *         sending failed
     *
     * @see lastError()
     */
    int sendWithAsyncReply(const QDBusMessage &message, QObject *receiver,
                           const char *slot) const;

    /**
     * @brief Flushes buffered outgoing message
     *
     * Attempts to send all enqueued outgoing messages before returning.
     */
    void flush() const;

    /**
     * @brief Processes buffered inbound messages
     *
     * Attempts to process all enqueued inbound messages, e.g. replies to
     * method calls or received signals.
     *
     * @warning dispatching message can result in Qt signals being emitted
     *          before this method returns. In case you just want to make sure
     *          no inbound message is forgotten, call scheduleDispatch() which
     *          will execute the dispatch delayed through the event loop.
     */
    void dispatch() const;

    /**
     * @brief Request a delayed check for inbound buffer processing
     *
     * Similar to dispatch() but delayed by a single shot timer to ensure
     * the method has returned before the processing is started.
     *
     * If a asynchronous method call is followed by a synchronous call without
     * returning to the event loop in between, a call to scheduleDispatch()
     * ensures that a pending reply to the asynchronous call is processed
     * as soon as possible
     */
    void scheduleDispatch() const;

    /**
     * @brief Connects an object to receive D-Bus signals
     *
     * This provides a basic access to all D-Bus signals received on this
     * connection.
     * For every D-Bus signal processed by the connection object a Qt signal
     * is emitted and thus delivered to all receiver objects connected
     * through this method.
     *
     * The required slot signature is
     * @code
     *   void slotname(const QDBusMessage&);
     * @endcode
     *
     * so a suitable receiver could look like this
     * @code
     *   class DBusSignalReceiver : public QObject
     *   {
     *       Q_OBJECT
     *   public slots:
     *       void dbusSignal(const QDBusMessage&);
     *   };
     * @endcode
     *
     * and would be connected like this
     * @code
     *    // assuming the following variables
     *    QDBusConnection connection;
     *    DBusSignalReceiver* receiver;
     *
     *    connection.connect(receiver, SLOT(dbusSignal(const QDBusMessage&)));
     * @endcode
     *
     * See QDBusProxy::dbusSignal() for a more obvious way of connecting slots.
     *
     * @param object the receiver object
     * @param slot the receiver slot (or signal for signal->signal connections)
     *
     * @return @c true if the connection was successfull, otherwise @c false
     *
     * @see disconnect()
     */
    bool connect(QObject* object, const char* slot);

    /**
     * @brief Disconnects a given receiver from the D-Bus signal handling
     *
     * @param object the receiver object to disconnect from
     * @param slot the receiver slot (or signal for signal->signal connections)
     *
     * @return @c true if the disconnect was successfull, otherwise @c false
     *
     * @see connect()
     */
    bool disconnect(QObject* object, const char* slot);

    /**
     * @brief Registers a service object for a given path
     *
     * In order to receive method calls over the D-Bus connection the service
     * objects path within its host application has to be registered with the
     * connection. See section @ref dbusconventions-objectpath for details.
     *
     * Only one objects can be registered for a single object path, i.e.
     * the path -> object mapping is unambiguous, similar to mapping of
     * filesystem paths to files.
     *
     * If a service object offers more than one interface it is up to the
     * service implementation if all are implemented in the object path to this
     * method or if the passed object is just another demultiplexer which
     * relays the message to the interface implementor.
     *
     * @param path the object path to register the object for
     * @param object the service implementation object for that path
     *
     * @return @c true if the given object is now registered for the given path
     *         or @c false if path is empty, object is null or another object
     *         is already registered for this path
     *
     * @see unregisterObject()
     */
    bool registerObject(const QString& path, QDBusObjectBase* object);

    /**
     * @brief Unregister a service object on a given path
     *
     * Removes any mapping of object path to service object previously
     * registered by registerObject().
     * See section @ref dbusconventions-objectpath for details.
     *
     * @warning always(!) unregister a service object before deleting it
     *
     * @param path the object path of the object to unregister
     *
     * @see registerObject()
     */
    void unregisterObject(const QString &path);

    /**
     * @brief Gets a connection to the session bus
     *
     * Convenience overload for creating the default shared connection to the
     * D-Bus session bus.
     *
     * Equivalent to calling addConnection(SessionBus);
     *
     * @return a connection handle. Check isConnected() to find out if the
     *         connection attempt has been successfull
     *
     * @see addConnection(BusType,const QString&);
     */
    static QDBusConnection sessionBus();

    /**
     * @brief Gets a connection to the system bus
     *
     * Convenience overload for creating the default shared connection to the
     * D-Bus system bus.
     *
     * Equivalent to calling addConnection(SystemBus);
     *
     * @return a connection handle. Check isConnected() to find out if the
     *         connection attempt has been successfull
     *
     * @see addConnection(BusType,const QString&);
     */
    static QDBusConnection systemBus();

    /**
     * @brief Add a connection to a bus with a specific bus type
     *
     * This is a factory method as it will create a connection for the given
     * name if its not available yet, but return a previously created
     * connection for that name if available.
     *
     * Depending on the #BusType the D-Bus library will connect to the address
     * configured for that type, so this is the recommended way to create
     * connection to D-Bus.
     *
     * @code
     *   // Associate the default connection name with a connection to the user's
     *   // session bus
     *   QDBusConnection con = QDBusConnection::addConnection(QDBusConnection::SessionBus);
     *
     *   // check if we are connected and which uniqueName we got
     *   if (con.isConnected())
     *   {
     *       qDebug("Connected to session bus. We got uniqueName %s",
     *              con.uniqueName().local8Bit().data());
     *   }
     * @endcode
     * For the common use cases see also sessionBus() and systemBus()
     *
     * @param type the #BusType of the bus to connect to
     * @param name the name to use for QDBusConnection's connection sharing
     *
     * @return a connection handle. Check isConnected() to find out if the
     *         connection attempt has been successfull
     *
     * @see closeConnection()
     */
    static QDBusConnection addConnection(BusType type,
                                         const QString &name = default_connection_name);

    /**
     * @brief Add a connection to a bus at a specific address
     *
     * This is a factory method as it will create a connection for the given
     * name if its not available yet, but return a previously created
     * connection for that name if available.
     *
     * @note this requires to know the address of a D-Bus daemon to connect to
     *
     * @param address the address of the D-Bus daemon. Usually a Unix domain
     *                socket address
     * @param name the name to use for QDBusConnection's connection sharing
     *
     * @return a connection handle. Check isConnected() to find out if the
     *         connection attempt has been successfull
     *
     * @see closeConnection()
     */
    static QDBusConnection addConnection(const QString &address,
                                         const QString &name = default_connection_name);

    // TODO check why this doesn't close the D-Bus connection
    /**
     * @brief Closes a connection with a given name
     *
     * Removes the name from the pool of shared connections, i.e. a call to
     * addConnection() with the same name afterwards will create a new
     * connection.
     *
     * @param name the connection name as used in addConnection()
     */
    static void closeConnection(const QString &name = default_connection_name);

    /**
     * String used as the default parameter for connection names
     */
    QT_STATIC_CONST char *default_connection_name;

private:
    QDBusConnectionPrivate *d;
};

#endif
