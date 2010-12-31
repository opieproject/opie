// standard includes
#include <cmath>
#include <iostream>

// Qt includes
#include <qapplication.h>
#include <qtimer.h>

// Qt D-Bus includes
#include <dbus/qdbusdata.h>
#include <dbus/qdbusdatamap.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusvariant.h>

// local includes
#include "crosstest-client.h"
#include "testsproxy.h"
#include "testsignalsproxy.h"
#include "singletestsproxy.h"

using namespace org::freedesktop::DBus::Binding;

static const QString SERVICE_NAME  = "org.freedesktop.DBus.Binding.TestServer";
static const QString OBJECT_PATH   = "/Test";
static const QString interfaceBase = "org.freedesktop.DBus.Binding.";
static const QString reportError   = "DBusError: name='%1', message='%2'";
static const QString reportResult  = "unexpected result: expected %1, got %2";

///////////////////////////////////////////////////////////////////////////////

static QString formatVariant(const QDBusVariant& variant)
{
    return QString("Variant(") + variant.signature + ", " +
                   variant.value.typeName() + ")";
}

static QString formatBool(bool value)
{
    return value ? QString("true") : QString("false");
}

template <typename T>
QString formatList(const QValueList<T>& list)
{
    QString result = "[";

    typename QValueList<T>::const_iterator it    = list.begin();
    typename QValueList<T>::const_iterator endIt = list.end();
    for (; it != endIt; ++it)
    {
        result += QString(" %1,").arg(*it);
    }

    if (result.endsWith(",")) result = result.mid(0, result.length() - 1);

    return result + " ]";
}

template <>
QString formatList(const QValueList<QDBusVariant>& list)
{
    QString result = "[";

    QValueList<QDBusVariant>::const_iterator it    = list.begin();
    QValueList<QDBusVariant>::const_iterator endIt = list.end();
    for (; it != endIt; ++it)
    {
        result += QString(" %1,").arg(formatVariant(*it));
    }

    if (result.endsWith(",")) result = result.mid(0, result.length() - 1);

    return result + " ]";
}

template <>
QString formatList(const QValueList<bool>& list)
{
    QString result = "[";

    QValueList<bool>::const_iterator it    = list.begin();
    QValueList<bool>::const_iterator endIt = list.end();
    for (; it != endIt; ++it)
    {
        result += QString(" %1,").arg(formatBool(*it));
    }

    if (result.endsWith(",")) result = result.mid(0, result.length() - 1);

    return result + " ]";
}

///////////////////////////////////////////////////////////////////////////////

TestClientNode::TestClientNode(const QDBusConnection& connection)
    : m_connection(connection),
      m_testNumber(0),
      m_testInterface(0),
      m_mainProxy(0),
      m_singleProxy(0),
      m_genericProxy(0)
{
    m_mainProxy     = new Tests(SERVICE_NAME, OBJECT_PATH, this);
    m_singleProxy   = new SingleTests(SERVICE_NAME, OBJECT_PATH, this);

    m_testInterface = new TestClientImpl(this);
    m_connection.registerObject(m_testInterface->objectPath(), m_testInterface);

    m_mainProxy->setConnection(connection);
    m_singleProxy->setConnection(connection);

    m_genericProxy = new QDBusProxy(this);

    QObject::connect(m_genericProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                     this, SLOT(slotGenericSignal(const QDBusMessage&)));

    m_genericProxy->setInterface("org.freedesktop.DBus.Binding.TestSignals");
    m_genericProxy->setConnection(connection);

    registerTests();

    QTimer* testRunner = new QTimer(this);

    QObject::connect(testRunner, SIGNAL(timeout()), this, SLOT(slotNextTest()));

    testRunner->start(1000, false);
}

///////////////////////////////////////////////////////////////////////////////

TestClientNode::~TestClientNode()
{
    m_connection.unregisterObject(m_testInterface->objectPath());
    delete m_testInterface;
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::registerTest(const QString& function)
{
    m_tests[function].clear();
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::reportFailedTest(const QString& function, const QString& report)
{
    m_reports << report;

    m_tests[function] << m_reports.count();
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::writeSummary()
{
    m_testInterface->evaluateResponseTest();

    QMap<QString, QValueList<uint> >::const_iterator it    = m_tests.begin();
    QMap<QString, QValueList<uint> >::const_iterator endIt = m_tests.end();
    for (; it != endIt; ++it)
    {
        if (it.data().isEmpty())
            std::cout << it.key().local8Bit().data() << " pass" << std::endl;
    }

    it = m_tests.begin();
    for (; it != endIt; ++it)
    {
        if (it.data().isEmpty()) continue;

        QValueList<uint>::const_iterator numIt    = it.data().begin();
        QValueList<uint>::const_iterator numEndIt = it.data().end();
        for (; numIt != numEndIt; ++numIt)
        {
            std::cout << it.key().local8Bit().data() << " fail " << *numIt
                      << std::endl;
        }
    }

    QStringList::const_iterator reportIt    = m_reports.begin();
    QStringList::const_iterator reportEndIt = m_reports.end();
    for (uint index = 1; reportIt != reportEndIt; ++reportIt, ++index)
    {
        std::cout << "report " << index << ": " << (*reportIt).local8Bit().data()
                  << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::handleMethodReply(const QDBusMessage& message)
{
    m_connection.send(message);
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::slotGenericSignal(const QDBusMessage& message)
{
    if (message.member() == "Triggered")
    {
        if (message.count() == 1 && message[0].type() == QDBusData::UInt64)
        {
            QString objectPath = message.path();
            Q_UINT64 parameter = message[0].toUInt64();

            // TODO
            Q_UNUSED(parameter);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::slotNextTest()
{
    ++m_testNumber;

    switch (m_testNumber)
    {
        case 1:
            testSingleTestSum();
            break;

        case 2:
            testTriggerSignal();
            m_testInterface->sendTrigger();
            break;

        case 3:
            testIdentity();
            testIdentityBool();
            testIdentityByte();
            testIdentityInt16();
            testIdentityUInt16();
            testIdentityInt32();
            testIdentityUInt32();
            testIdentityInt64();
            testIdentityUInt64();
            testIdentityDouble();
            testIdentityString();
            break;

        case 4:
            testIdentityArray();
            testIdentityBoolArray();
            testIdentityByteArray();
            testIdentityInt16Array();
            testIdentityUInt16Array();
            testIdentityInt32Array();
            testIdentityUInt32Array();
            testIdentityInt64Array();
            testIdentityUInt64Array();
            testIdentityDoubleArray();
            testIdentityStringArray();
            break;

        case 5:
            testTestsSum();
            testInvert();
            testInvertMapping();
            testPrimitize();
            testDeStruct();
            break;

       default:
            finish();
            break;
    };
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testSingleTestSum()
{
    QDBusError error;

    QValueList<Q_UINT8> values;
    Q_UINT32 result = 1;

    if (!m_singleProxy->Sum(values, result, error))
    {
        reportFailedTest(interfaceBase + "SingleTests.Sum",
                         reportError.arg(error.name(), error.message()));
    }
    else if (result != 0)
    {
        reportFailedTest(interfaceBase + "SingleTests.Sum",
                         reportResult.arg(0).arg(result));
    }

    result = 0;

    values << 0 << 1 << 255 << 0;

    if (!m_singleProxy->Sum(values, result, error))
    {
        reportFailedTest(interfaceBase + "SingleTests.Sum",
                         reportError.arg(error.name(), error.message()));
    }
    else if (result != 256)
    {
        reportFailedTest(interfaceBase + "SingleTests.Sum",
                         reportResult.arg(256).arg(result));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testTriggerSignal()
{
    QDBusError error;

    QString path = "/Foo/Bar";
    Q_UINT64 parameter = 123456;

    if (!m_mainProxy->Trigger(path, parameter, error))
    {
        reportFailedTest(interfaceBase + "TestSignals.Trigger",
                         reportError.arg(error.name(), error.message()));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentity()
{
    QDBusVariant input;
    QDBusVariant output;

    input.signature = "s";
    input.value     = QDBusData::fromString("Qt3 D-Bus bindings");

    QDBusError error;
    if (!m_mainProxy->Identity(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.Identity",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.Identity",
                         reportResult.arg(formatVariant(input))
                                     .arg(formatVariant(output)));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityBool()
{
    bool input = false;
    bool output;

    QDBusError error;
    if (!m_mainProxy->IdentityBool(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityBool",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityBool",
                         reportResult.arg(formatBool(input))
                                     .arg(formatBool(output)));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityByte()
{
    Q_UINT8 input = 0;
    Q_UINT8 output;

    QDBusError error;
    if (!m_mainProxy->IdentityByte(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityByte",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityByte",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityInt16()
{
    Q_INT16 input = 0;
    Q_INT16 output;

    QDBusError error;
    if (!m_mainProxy->IdentityInt16(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt16",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt16",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityUInt16()
{
    Q_UINT16 input = 0;
    Q_UINT16 output;

    QDBusError error;
    if (!m_mainProxy->IdentityUInt16(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt16",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt16",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityInt32()
{
    Q_INT32 input = 0;
    Q_INT32 output;

    QDBusError error;
    if (!m_mainProxy->IdentityInt32(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt32",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt32",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityUInt32()
{
    Q_UINT32 input = 0;
    Q_UINT32 output;

    QDBusError error;
    if (!m_mainProxy->IdentityUInt32(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt32",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt32",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityInt64()
{
    Q_INT64 input = 0;
    Q_INT64 output;

    QDBusError error;
    if (!m_mainProxy->IdentityInt64(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt64",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt64",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityUInt64()
{
    Q_UINT64 input = 0;
    Q_UINT64 output;

    QDBusError error;
    if (!m_mainProxy->IdentityUInt64(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt64",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt64",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityDouble()
{
    double input = 0.0;
    double output;

    QDBusError error;
    if (!m_mainProxy->IdentityDouble(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityDouble",
                         reportError.arg(error.name(), error.message()));
    }
    else if (std::fabs(input - output) > 1e-10)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityDouble",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityString()
{
    QString input;
    QString output;

    input = "";

    QDBusError error;
    if (!m_mainProxy->IdentityString(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityString",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        reportFailedTest(interfaceBase + "Tests.IdentityString",
                         reportResult.arg(input).arg(output));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityArray()
{
    QValueList<QDBusVariant> input;
    QValueList<QDBusVariant> output;

    QDBusError error;
    if (!m_mainProxy->IdentityArray(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityArray",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<QDBusVariant>(input);
        QString outputMessage = formatList<QDBusVariant>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityArray",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityBoolArray()
{
    QValueList<bool> input;
    QValueList<bool> output;

    input << false;

    QDBusError error;
    if (!m_mainProxy->IdentityBoolArray(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityBoolArray",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<bool>(input);
        QString outputMessage = formatList<bool>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityBoolArray",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityByteArray()
{
    QValueList<Q_UINT8> input;
    QValueList<Q_UINT8> output;

    QDBusError error;
    if (!m_mainProxy->IdentityByteArray(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityByteArray",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<Q_UINT8>(input);
        QString outputMessage = formatList<Q_UINT8>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityByteArray",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityInt16Array()
{
    QValueList<Q_INT16> input;
    QValueList<Q_INT16> output;

    QDBusError error;
    if (!m_mainProxy->IdentityInt16Array(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt16Array",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<Q_INT16>(input);
        QString outputMessage = formatList<Q_INT16>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityInt64Array",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityUInt16Array()
{
    QValueList<Q_UINT16> input;
    QValueList<Q_UINT16> output;

    QDBusError error;
    if (!m_mainProxy->IdentityUInt16Array(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt16Array",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<Q_UINT16>(input);
        QString outputMessage = formatList<Q_UINT16>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityUInt16Array",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityInt32Array()
{
    QValueList<Q_INT32> input;
    QValueList<Q_INT32> output;

    QDBusError error;
    if (!m_mainProxy->IdentityInt32Array(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt32Array",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<Q_INT32>(input);
        QString outputMessage = formatList<Q_INT32>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityInt32Array",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityUInt32Array()
{
    QValueList<Q_UINT32> input;
    QValueList<Q_UINT32> output;

    QDBusError error;
    if (!m_mainProxy->IdentityUInt32Array(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt32Array",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<Q_UINT32>(input);
        QString outputMessage = formatList<Q_UINT32>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityUInt32Array",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityInt64Array()
{
    QValueList<Q_INT64> input;
    QValueList<Q_INT64> output;

    QDBusError error;
    if (!m_mainProxy->IdentityInt64Array(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityInt64Array",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<Q_INT64>(input);
        QString outputMessage = formatList<Q_INT64>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityInt64Array",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityUInt64Array()
{
    QValueList<Q_UINT64> input;
    QValueList<Q_UINT64> output;

    QDBusError error;
    if (!m_mainProxy->IdentityUInt64Array(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityUInt64Array",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<Q_UINT64>(input);
        QString outputMessage = formatList<Q_UINT64>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityUInt64Array",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityDoubleArray()
{
    QValueList<double> input;
    QValueList<double> output;

    QDBusError error;
    if (!m_mainProxy->IdentityDoubleArray(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityDoubleArray",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output) // FIXME equality for doubles not defined
    {
        QString inputMessage  = formatList<double>(input);
        QString outputMessage = formatList<double>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityDoubleArray",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testIdentityStringArray()
{
    QStringList input;
    QStringList output;

    QDBusError error;
    if (!m_mainProxy->IdentityStringArray(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.IdentityStringArray",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input != output)
    {
        QString inputMessage  = formatList<QString>(input);
        QString outputMessage = formatList<QString>(output);

        reportFailedTest(interfaceBase + "Tests.IdentityStringArray",
                         reportResult.arg(inputMessage).arg(outputMessage));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testTestsSum()
{
    uint num = 0;

    while (num <= 1024)
    {
        Q_INT64 expectedResult = 0;

        QValueList<Q_INT32> values;

        for (uint i = 0; i < num; ++i)
        {
            values << (i * num);

            expectedResult += (i * num);
        }

        QDBusError error;
        Q_INT64 result;

        if (!m_mainProxy->Sum(values, result, error))
        {
            reportFailedTest(interfaceBase + "Tests.Sum",
                             reportError.arg(error.name(), error.message()));
            break;
        }
        else if (expectedResult != result)
        {
            reportFailedTest(interfaceBase + "Tests.Sum",
                             reportResult.arg(expectedResult).arg(result));
        }

        if (num < 2)
            ++num;
        else
            num = num * num;
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testInvert()
{
    bool input = false;
    bool output;

    QDBusError error;
    if (!m_mainProxy->Invert(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.Invert",
                         reportError.arg(error.name(), error.message()));
    }
    else if (input == output)
    {
            reportFailedTest(interfaceBase + "Tests.Sum",
                             reportResult.arg(formatBool(!input))
                                         .arg(formatBool(output)));
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testInvertMapping()
{
    QMap<QString, QString> input;
    QDBusDataMap<QString> output;

    QDBusError error;

    if (!m_mainProxy->InvertMapping(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.InvertMapping",
                         reportError.arg(error.name(), error.message()));
    }
    else
    {
        QDBusData data = QDBusData::fromStringKeyMap(output);
        QCString signature = data.buildDBusSignature();

        if (signature != "a{sas}")
        {
            reportFailedTest(interfaceBase + "Tests.InvertMapping",
                QString("Output is not 'a{sas}' but '%1'").arg(signature));
        }
        else if (!output.isEmpty())
        {
            reportFailedTest(interfaceBase + "Tests.InvertMapping",
                QString("Output is not empty for empty input"));
        }
    }

    input.insert("1", "1");
    input.insert("2", "1");
    input.insert("1", "2");
    input.insert("2", "2");
    input.insert("1", "3");
    input.insert("3", "1");

    output.clear();
    error = QDBusError();

    if (!m_mainProxy->InvertMapping(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.InvertMapping",
                         reportError.arg(error.name(), error.message()));
    }
    else
    {
        QDBusData data = QDBusData::fromStringKeyMap(output);
        QCString signature = data.buildDBusSignature();

        if (signature != "a{sas}")
        {
            reportFailedTest(interfaceBase + "Tests.InvertMapping",
                QString("Output is not 'a{sas}' but '%1'").arg(signature));
        }
        else
        {
            // TODO check correctness
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testPrimitize()
{
    QDBusVariant input;
    QValueList<QDBusVariant> output;

    input.signature = "s";
    input.value     = QDBusData::fromString("Qt3 D-Bus bindings");

    QDBusError error;
    if (!m_mainProxy->Primitize(input, output, error))
    {
        reportFailedTest(interfaceBase + "Tests.Primitize",
                         reportError.arg(error.name(), error.message()));
    }
    else if (output.count() != 1 || input != output[0])
    {
        // TODO
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::testDeStruct()
{
    QValueList<QDBusData> memberList;

    memberList << QDBusData::fromString(QString::fromUtf8("öäüß"));
    memberList << QDBusData::fromUInt32(1029384756);
    memberList << QDBusData::fromInt16(-2007);

    QDBusData input = QDBusData::fromStruct(memberList);

    QString  out1;
    Q_UINT32 out2;
    Q_INT16  out3;

    QDBusError error;
    if (!m_mainProxy->DeStruct(input, out1, out2, out3, error))
    {
        reportFailedTest(interfaceBase + "Tests.DeStruct",
                         reportError.arg(error.name(), error.message()));
    }
    else
    {
        if (out1 != memberList[0].toString() || out2 != memberList[1].toUInt32() ||
            out3 != memberList[2].toInt16())
        {
            // TODO
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::finish()
{
    QDBusError error;
    if (!m_mainProxy->Exit(error))
    {
        reportFailedTest(interfaceBase + "Tests.Exit",
                         reportError.arg(error.name(), error.message()));
    }

    qApp->quit();
}

///////////////////////////////////////////////////////////////////////////////

void TestClientNode::registerTests()
{
    registerTest(interfaceBase + "SingleTests.Sum");

    registerTest(interfaceBase + "Tests.Identity");
    registerTest(interfaceBase + "Tests.IdentityBool");
    registerTest(interfaceBase + "Tests.IdentityByte");
    registerTest(interfaceBase + "Tests.IdentityInt16");
    registerTest(interfaceBase + "Tests.IdentityUInt16");
    registerTest(interfaceBase + "Tests.IdentityInt32");
    registerTest(interfaceBase + "Tests.IdentityUInt32");
    registerTest(interfaceBase + "Tests.IdentityInt64");
    registerTest(interfaceBase + "Tests.IdentityUInt64");
    registerTest(interfaceBase + "Tests.IdentityString");

    registerTest(interfaceBase + "Tests.IdentityArray");
    registerTest(interfaceBase + "Tests.IdentityBoolArray");
    registerTest(interfaceBase + "Tests.IdentityByteArray");
    registerTest(interfaceBase + "Tests.IdentityInt16Array");
    registerTest(interfaceBase + "Tests.IdentityUInt16Array");
    registerTest(interfaceBase + "Tests.IdentityInt32Array");
    registerTest(interfaceBase + "Tests.IdentityUInt32Array");
    registerTest(interfaceBase + "Tests.IdentityInt64Array");
    registerTest(interfaceBase + "Tests.IdentityUInt64Array");
    registerTest(interfaceBase + "Tests.IdentityStringArray");

    registerTest(interfaceBase + "Tests.Sum");
    registerTest(interfaceBase + "Tests.Invert");
    registerTest(interfaceBase + "Tests.InvertMapping");
    registerTest(interfaceBase + "Tests.DeStruct");
    registerTest(interfaceBase + "Tests.Primitize");

    registerTest(interfaceBase + "Tests.Exit");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TestClientImpl::TestClientImpl(TestClientNode* node)
    : m_node(node), m_responseCalled(false), m_input1(0), m_input2(0.0)
{
    m_node->registerTest(interfaceBase + "TestClient.Response");
}

///////////////////////////////////////////////////////////////////////////////

QString TestClientImpl::objectPath() const
{
    return "/Test";
}

///////////////////////////////////////////////////////////////////////////////

void TestClientImpl::sendTrigger()
{
    m_input1 = 51243;
    m_input2 = 1e-10;

    emitTrigger(m_input1, m_input2);
}

///////////////////////////////////////////////////////////////////////////////

void TestClientImpl::evaluateResponseTest()
{
    if (!m_responseCalled)
    {
        m_node->reportFailedTest(interfaceBase + "TestClient.Response",
                                 "Method never got called");
    }
    else
    {
        QStringList::const_iterator it    = m_responseErrors.begin();
        QStringList::const_iterator endIt = m_responseErrors.end();
        for (; it != endIt; ++it)
        {
            m_node->reportFailedTest(interfaceBase + "TestClient.Response", *it);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

bool TestClientImpl::Response(Q_UINT16 input1, double input2, QDBusError& error)
{
    Q_UNUSED(error);

    m_responseCalled = true;

    if (m_input1 != input1 || std::fabs(input2 - m_input2) > 1e-20)
    {
        QString message("Response called with (%1, %2) but expected (%3, %4)");
        m_responseErrors
            << message.arg(m_input1).arg(m_input2).arg(input1).arg(input2);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

void TestClientImpl::handleMethodReply(const QDBusMessage& reply)
{
    m_node->handleMethodReply(reply);
}

///////////////////////////////////////////////////////////////////////////////

bool TestClientImpl::handleSignalSend(const QDBusMessage& reply)
{
    m_node->handleMethodReply(reply);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    QApplication app(argc, argv, false);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected())
        qFatal("No connection to session bus");

    TestClientNode client(connection);

    int ret = app.exec();

    client.writeSummary();

    return ret;
}

// End of File
