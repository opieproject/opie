/* qdbusobject.h DBUS service object interface
 *
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

#ifndef QDBUSOBJECT_H
#define QDBUSOBJECT_H

/**
 * @page dbusservice Providing services over D-Bus
 *
 * Contents:
 * - @ref dbusservice-introduction
 * - @ref dbusservice-example
 * - @ref dbusservice-requestname
 * - @ref dbusservice-registerobjects
 * - @ref dbusservice-interfaces
 *
 * @section dbusservice-introduction Introduction
 *
 * The Qt3 bindings do not support autogeneration of service objects yet. In
 * order to provide interfaces over D-Bus, an application has to implement the
 * QDBusObjectBase interface and register an instance of the resulting class
 * with the QDBusConnection.
 *
 * @section dbusservice-example A simple D-Bus client example
 *
 * @code
 *   #include <dbus/qdbusconnection.h>;
 *   #include <dbus/qdbusobject.h>;
 *
 *   class QStringList;
 *
 *   class TestService : public QDBusObjectBase
 *   {
 *   public:
 *       TestService(const QDBusConnection& connection);
 *       virtual ~TestService();
 *
 *   protected:
 *       virtual bool handleMethodCall(const QDBusMessage& message);
 *
 *   private:
 *       QDBusConnection m_connection;
 *
 *   private:
 *       QStringList sortStrings(const QStringList& list);
 *   };
 * @endcode
 * @code
 *
 *   #include <qstringlist.h>;
 *
 *   #include <dbus/qdbuserror.h>;
 *   #include <dbus/qdbusmessage.h>;
 *
 *   TestService::TestService(const QDBusConnection& connection) : m_connection(connection)
 *   {
 *       m_connection.registerObject("/ListSorter", this);
 *   }
 *
 *   TestService::~TestService()
 *   {
 *       m_connection.unregisterObject("/ListSorter");
 *   }
 *
 *   // return false to let D-Bus send a standard error message that the method is unknown
 *
 *   bool TestService::handleMethod(const QDBusMessage& message)
 *   {
 *       if (message.interface() != "org.example.Sort") return false;
 *
 *       if (message.member() == "Strings")
 *       {
 *           // check parameters
 *
 *           if (message.count() != 1 || message[0].type() != QDBusData::List)
 *           {
 *               // method signature not what we expected
 *
 *               QDBusError error = QDBusError::stdInvalidArgs(
 *                                "Expected one argument of type array of string");
 *
 *               QDBusMessage reply = QDBusMessage::methodError(message, error);
 *
 *               // send error
 *
 *               m_connection.send(reply);
 *
 *               // tell D-Bus we did handle the call
 *
 *               return true;
 *           }
 *
 *           // call implementation
 *
 *           QStringList result = sortStrings(message[0].toQStringList());
 *
 *           // prepare reply
 *
 *           QDBusMessage reply = QDBusMessage::methodReply(message);
 *
 *           reply << QDBusData::fromList(result);
 *
 *           // send reply
 *
 *           m_connection.send(reply);
 *
 *           // tell D-Bus we did handle the call
 *
 *           return true;
 *       }
 *
 *       return false;
 *   }
 *
 *   QStringList TestService::sortStrings(const QStringList& list)
 *   {
 *       QStringList result = list;
 *
 *       result.sort();
 *
 *       return result;
 *   }
 * @endcode
 * @code
 *   int main(int argc, char** argv)
 *   {
 *       QApplication app(argc, argv, false);
 *
 *       QDBusConnection connection = QDBusConnection::sessionBus();
 *       if (!connection.isConnected())
 *           qFatal("Cannot connect to session bus");
 *
 *       // try to get a specific service name
 *       if (!connection.requestName("org.example.SortService"))
 *       {
 *           qWarning("Requesting name 'org.example.SortService' failed. "
 *                    "Will only be addressable through unique name '%s'",
 *                    connection.uniqueName().local8Bit().data());
 *       }
 *       else
 *       {
 *           qDebug("Requesting name 'org.example.SortService' successfull");
 *       }
 *
 *       TestService service(connection);
 *
 *       return app.exec();
 *    }
 * @endcode
 *
 * @section dbusservice-requestname Requesting service name
 *
 * When an application connects to D-Bus it gets a unique name generated by
 * the bus daemon.
 *
 * However, an application providing service will often want to be reachable
 * under a fixed name, like a webserver being reachable through a domain name
 * independent from its actual IP address.
 * See section @ref dbusconventions-servicename for details on service names.
 *
 * In order to get such a specific name an application has to request it
 * using QDBusConnection::requestName()
 *
 * The example above request @c "org.example.SortService" but continues with
 * the default unique name in the case some other application is currently
 * owning that name.
 *
 * @section dbusservice-registerobjects Registering objects
 *
 * To make service objects available to other applications on the same
 * bus the application has to register the objects instances with the
 * connection to the bus using QDBusConnection::registerObject()
 *
 * Registering means to specify an object path where the object will be
 * located, i.e. how it can be unambiguously be addressed in method calls.
 * See section @ref dbusconventions-objectpath for details on object paths.
 *
 * If the applications has introspectable objects it is recommended to
 * register an introspectable root object, i.e. using @c "/" as the path, so
 * other applications have a common place to start asking for introspection
 * data.
 *
 * In the example above a service object providing sorting services on lists is
 * registered on the path @c "/ListSorter"
 *
 * @section dbusservice-interfaces Service interfaces
 *
 * D-Bus methods and signals of a service object a grouped into interfaces.
 *
 * See section @ref dbusconventions-interfacename for details on interface
 * naming.
 *
 * An object can implement any number of interfaces, for example the interface
 * for the functionality it wants to provide and a D-Bus standard interface like
 * @c "org.freedesktop.DBus.Introspectable" for providing an XML description of
 * all its interfaces.
 *
 *
 * The service object of the example above implements just one interface
 * @c "org.example.Sort" and its handleMethodCall() explicitly checks all
 * received messages and rejects any messsage not sent to this particular
 * interface by returning @c false and thus telling the D-Bus layer to
 * generate a standard error response.
 *
 * Multiple interfaces can of course be directly implemented in one C++ class,
 * however it might sometimes be wise to delegate calls for different
 * interfaces to different implementations:
 * @code
 *   class Interface1 : public QDBusObjectBase
 *   {
 *   public:
 *       Interface1(const QDBusConnection&);
 *
 *   protected:
 *       virtual bool handleMethodCall(const QDBusMessage&);
 *   };
 *
 *   class Interface2 : public QDBusObjectBase
 *   {
 *   public:
 *       Interface2(const QDBusConnection&);
 *
 *   protected:
 *       virtual bool handleMethodCall(const QDBusMessage&);
 *   };
 *
 *   class MultiInterfaceService : public QDBusObjectBase
 *   {
 *   public:
 *       MultiInterfaceService(const QDBusConnection&);
 *
 *   protected:
 *       virtual bool handleMethodCall(const QDBusMessage&);
 *
 *   private:
 *       QMap<QString, QDBusObjectBase*> m_interfaces;
 *   };
 *
 *   MultiInterfaceService::MultiInterfaceService(const QDBusConnection& connection)
 *   {
 *       m_interfaces.insert("org.example.Interface1", new Interface1(connection));
 *       m_interfaces.insert("org.example.Interface2", new Interface2(connection));
 *   }
 *
 *   bool MultiInterfaceService::handleMethodCall(const QDBusMessage& message)
 *   {
 *       // delegate call to its interface handler
 *       QDBusObjectBase* handler = m_interfaces[message.interface()];
 *       if (handler != 0)
 *           return delegateMethodCall->(message, handler);
 *       else
 *           return false; // no such interface
 *   }
 * @endcode
 */

/**
 * @include example-service.h
 * @example example-service.cpp
 */

#include "dbus/qdbusmacros.h"

class QDBusMessage;

/**
 * @brief Base interface for D-Bus service objects
 *
 * In order to register a service object with the QDBusConnection it needs to
 * implement the interface specified by this class.
 *
 * The connection will forward all method calls that have a path equivalent
 * to the path the service object was registered with to the object's
 * handleMethodCall() method. See QDBusConnection::registerObject()
 *
 * If for some reason, e.g. the call is not meant for this interface, or the
 * method is unknown, the implementation can just return @c false and the
 * connection will handle the rest.
 *
 * See section @ref dbusservice for documentation on how to use QDBusObjectBase
 */
class QDBUS_EXPORT QDBusObjectBase
{
    friend class QDBusConnectionPrivate;
public:
    /**
     * @brief Destroys the object
     */
    virtual ~QDBusObjectBase() {}

protected:
    /**
     * @brief Method call entry point
     *
     * This method has to be implemented to handle method calls sent to the
     * service object.
     * An object implementation can handle all its interfaces in one class or
     * again forward the method call to interface implementators.
     *
     * If for some reason, e.g. the call is not meant for this interface, or
     * the method is unknown, the implementation can just return @c false and
     * the connection will handle the rest.
     *
     * If an error occurs during the method call, e.g. the number of parameters
     * or their types are not what would be expected, the service object
     * should reply with a QDBusMessage of type QDBusMessage::ErrorMessage
     * which in turn should include the D-Bus error describing the problem.
     * See QDBusConnection::send() for sending reply messages.
     *
     * See QDBusMessage::methodError() and QDBusMessage::methodReply() on
     * how to create suitable reply messages for the given method call.
     *
     * @param message the method call to handle
     *
     * @return @c true if the message can be handled independent if handling
     *         resulted in an error. In this case implementations should an
     *         error reply. Returns @c false only if interface or method are
     *         unknown
     */
    virtual bool handleMethodCall(const QDBusMessage& message) = 0;

    /**
     * @brief Delegate a method call to another object
     *
     * When a service object is built as a collection of separated interface
     * class instances, i.e. each interface of the object is implemented in
     * its own QDBusObjectBase subclass and the main object just wanst to pass
     * on the method calls to the respective interface implementations, it
     * can do so by calling this base class method.
     *
     * Since it is a method of the base class, it can call the otherwise
     * protected handleMethodCall() of the interface implementor.
     *
     * See @ref dbusservice-interfaces for an example.
     *
     * @param message the method call to delegate
     * @param delegate the object which should handle the call instead
     *
     * @return @c true if the message can be handled independent if handling
     *         resulted in an error. In this case implementations should an
     *         error reply. Returns @c false only if interface or method are
     *         unknown
     *
     */
    bool delegateMethodCall(const QDBusMessage& message, QDBusObjectBase* delegate)
    {
        return delegate->handleMethodCall(message);
    }
};

#endif

