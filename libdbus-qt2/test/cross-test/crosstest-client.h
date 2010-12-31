#if !defined(CROSSTEST_CLIENT_H_INCLUDED)
#define CROSSTEST_CLIENT_H_INCLUDED

// Qt includes
#include <qmap.h>
#include <qobject.h>
#include <qstringlist.h>

// Qt D-Bus includes
#include <dbus/qdbusconnection.h>

// local includes
#include "testclientinterface.h"

// forward declarations
class QDBusMessage;
class QDBusProxy;
class TestClientImpl;

namespace org
{
    namespace freedesktop
    {
        namespace DBus
        {
            namespace Binding
            {
                class Tests;
                class SingleTests;
            };
        };
    };
};

class TestClientNode : public QObject
{
    Q_OBJECT
public:
    TestClientNode(const QDBusConnection& connection);

    virtual ~TestClientNode();

    void registerTest(const QString& function);

    void reportFailedTest(const QString& function, const QString& report);

    void writeSummary();

    void handleMethodReply(const QDBusMessage& message);

private slots:
    void slotGenericSignal(const QDBusMessage& message);

    void slotNextTest();

private:
    void testSingleTestSum();

    void testTriggerSignal();

    void testIdentity();
    void testIdentityBool();
    void testIdentityByte();
    void testIdentityInt16();
    void testIdentityUInt16();
    void testIdentityInt32();
    void testIdentityUInt32();
    void testIdentityInt64();
    void testIdentityUInt64();
    void testIdentityDouble();
    void testIdentityString();

    void testIdentityArray();
    void testIdentityBoolArray();
    void testIdentityByteArray();
    void testIdentityInt16Array();
    void testIdentityUInt16Array();
    void testIdentityInt32Array();
    void testIdentityUInt32Array();
    void testIdentityInt64Array();
    void testIdentityUInt64Array();
    void testIdentityDoubleArray();
    void testIdentityStringArray();

    void testTestsSum();

    void testInvert();

    void testInvertMapping();

    void testPrimitize();

    void testDeStruct();

    void finish();

    void registerTests();

private:
    QDBusConnection m_connection;

    uint m_testNumber;

    QMap<QString, QValueList<uint> > m_tests;
    QStringList m_reports;

    TestClientImpl* m_testInterface;

    org::freedesktop::DBus::Binding::Tests* m_mainProxy;
    org::freedesktop::DBus::Binding::SingleTests* m_singleProxy;

    QDBusProxy* m_genericProxy;
};

///////////////////////////////////////////////////////////////////////////////

class TestClientImpl : public org::freedesktop::DBus::Binding::TestClient
{
public:
    TestClientImpl(TestClientNode* node);

    virtual QString objectPath() const;

    void sendTrigger();

    void evaluateResponseTest();

protected:
    virtual bool Response(Q_UINT16 input1, double input2, QDBusError& error);

protected: // implement sending replies
    virtual void handleMethodReply(const QDBusMessage& reply);
    virtual bool handleSignalSend(const QDBusMessage& reply);

private:
    TestClientNode* m_node;

    bool m_responseCalled;

    QStringList m_responseErrors;

    Q_UINT16 m_input1;
    double   m_input2;
};

#endif

// End of File
