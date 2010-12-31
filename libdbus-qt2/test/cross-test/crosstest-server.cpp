// standard includes
#include <iostream>

// Qt includes
#include <qapplication.h>
#include <qdom.h>

// Qt D-Bus includes
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>

// local includes
#include "crosstest-server.h"
#include "interfaceimpls.h"

static const char* docTypeDeclaration =
"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\""
"\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n";

///////////////////////////////////////////////////////////////////////////////

TestNode::TestNode(const QDBusConnection& connection)
    : m_connection(connection), m_signalInterface(0), m_clientProxy(0)
{
    m_signalInterface = new TestSignalsImpl(this);

    TestsImpl* mainInterface = new TestsImpl(this);

    QObject::connect(mainInterface, SIGNAL(triggered(const QString&, Q_UINT64)),
                     m_signalInterface,
                     SLOT(slotTriggered(const QString&, Q_UINT64)));

    QObject::connect(mainInterface, SIGNAL(finished()), qApp, SLOT(quit()));

    m_interfaces.insert("org.freedesktop.DBus.Binding.SingleTests",
                        new SingleTestsImpl(this));
    m_interfaces.insert("org.freedesktop.DBus.Binding.Tests",
                        mainInterface);
    m_interfaces.insert("org.freedesktop.DBus.Binding.TestSignals",
                        m_signalInterface);

    m_clientProxy = new QDBusProxy(this, "TestClientProxy");

    m_clientProxy->setInterface("org.freedesktop.DBus.Binding.TestClient");

    QObject::connect(m_clientProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                     this, SLOT(slotDBusSignal(const QDBusMessage&)));

    m_clientProxy->setConnection(connection);

    m_connection.registerObject("/Test", this);
}

///////////////////////////////////////////////////////////////////////////////

TestNode::~TestNode()
{
    m_connection.unregisterObject("/Test");

    m_interfaces.setAutoDelete(true);
}

///////////////////////////////////////////////////////////////////////////////

void TestNode::handleMethodReply(const QDBusMessage& reply)
{
    m_connection.send(reply);
}

///////////////////////////////////////////////////////////////////////////////

void TestNode::registerTest(const QString& function)
{
    m_tests[function] = 0;
}

///////////////////////////////////////////////////////////////////////////////

void TestNode::acknowledgeTest(const QString& function)
{
    ++m_tests[function];
}

///////////////////////////////////////////////////////////////////////////////

void TestNode::writeSummary()
{
    QMap<QString, int>::const_iterator it    = m_tests.begin();
    QMap<QString, int>::const_iterator endIt = m_tests.end();
    for (; it != endIt; ++it)
    {
        if (it.data() > 0)
            std::cout << it.key().local8Bit().data() << " ok" << std::endl;
    }

    it    = m_tests.begin();
    for (; it != endIt; ++it)
    {
        if (it.data() == 0)
            std::cout << it.key().local8Bit().data() << " untested" << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////

bool TestNode::handleMethodCall(const QDBusMessage& message)
{
    if (message.interface() == "org.freedesktop.DBus.Introspectable" &&
        message.member() == "Introspect")
    {
        handleIntrospect(message);
        return true;
    }

    QDBusObjectBase* interface = m_interfaces[message.interface()];
    if (interface == 0) return false;

    return delegateMethodCall(message, interface);
}

///////////////////////////////////////////////////////////////////////////////

void TestNode::handleIntrospect(const QDBusMessage& message)
{
    if (m_introspectionData.isEmpty())
    {
        QDomDocument document;
        QDomElement nodeElement = document.createElement("node");
        nodeElement.setAttribute("name", "/Test");

        QDomElement interfaceElement = document.createElement("interface");
        interfaceElement.setAttribute("name",
                                      "org.freedesktop.DBus.Introspectable");
        QDomElement methodElement = document.createElement("method");
        methodElement.setAttribute("name", "Introspect");

        QDomElement argElement = document.createElement("arg");
        argElement.setAttribute("name",      "data");
        argElement.setAttribute("type",      "s");
        argElement.setAttribute("direction", "out");
        methodElement.appendChild(argElement);
        interfaceElement.appendChild(methodElement);
        nodeElement.appendChild(interfaceElement);

        interfaceElement = document.createElement("interface");
        SingleTestsImpl::buildIntrospectionData(interfaceElement);
        nodeElement.appendChild(interfaceElement);

        interfaceElement = document.createElement("interface");
        TestsImpl::buildIntrospectionData(interfaceElement);
        nodeElement.appendChild(interfaceElement);

        interfaceElement = document.createElement("interface");
        TestSignalsImpl::buildIntrospectionData(interfaceElement);
        nodeElement.appendChild(interfaceElement);

        document.appendChild(nodeElement);

        m_introspectionData = docTypeDeclaration;

        m_introspectionData += document.toString();
    }

    QDBusMessage reply = QDBusMessage::methodReply(message);
    reply << QDBusData::fromString(m_introspectionData);

    handleMethodReply(reply);
}

///////////////////////////////////////////////////////////////////////////////

void TestNode::slotDBusSignal(const QDBusMessage& message)
{
    if (message.member() == "Trigger")
    {
        m_clientProxy->setService(message.sender());
        m_clientProxy->setPath(message.path());

        QValueList<QDBusData> parameters = message;

        m_clientProxy->send("Response", parameters);

        m_clientProxy->setService(QString::null);
        m_clientProxy->setPath(QString::null);
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    QApplication app(argc, argv, false);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected())
        qFatal("No connection to session bus");

    if (!connection.requestName("org.freedesktop.DBus.Binding.TestServer"))
        qFatal("Cannot register name org.freedesktop.DBus.Binding.TestServer");

    TestNode node(connection);

    int ret = app.exec();

    node.writeSummary();

    return ret;
}

// End of File
