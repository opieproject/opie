// standard includes
#include <cmath>

// Qt includes
#include <qmap.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qvaluelist.h>

// Qt D-Bus includes
#include <dbus/qdbusdata.h>
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbusdatamap.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>

// local includes
#include "crosstest-server.h"
#include "interfaceimpls.h"

static const QString interfaceBase = "org.freedesktop.DBus.Binding.";

///////////////////////////////////////////////////////////////////////////////

SingleTestsImpl::SingleTestsImpl(TestNode* node) : m_node(node)
{
    m_node->registerTest(interfaceBase + "SingleTests.Sum");
}

///////////////////////////////////////////////////////////////////////////////

bool SingleTestsImpl::Sum(const QValueList<Q_UINT8>& input, Q_UINT32& output,
        QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "SingleTests.Sum");

    Q_UNUSED(error);

    output = 0;

    QValueList<Q_UINT8>::const_iterator it    = input.begin();
    QValueList<Q_UINT8>::const_iterator endIt = input.end();
    for (; it != endIt; ++it)
    {
        output += *it;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

void SingleTestsImpl::handleMethodReply(const QDBusMessage& reply)
{
    m_node->handleMethodReply(reply);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TestsImpl::TestsImpl(TestNode* node) : m_node(node)
{
    m_node->registerTest(interfaceBase + "Tests.Identity");
    m_node->registerTest(interfaceBase + "Tests.IdentityByte");
    m_node->registerTest(interfaceBase + "Tests.IdentityBool");
    m_node->registerTest(interfaceBase + "Tests.IdentityInt16");
    m_node->registerTest(interfaceBase + "Tests.IdentityUInt16");
    m_node->registerTest(interfaceBase + "Tests.IdentityInt32");
    m_node->registerTest(interfaceBase + "Tests.IdentityUInt32");
    m_node->registerTest(interfaceBase + "Tests.IdentityInt64");
    m_node->registerTest(interfaceBase + "Tests.IdentityUInt64");
    m_node->registerTest(interfaceBase + "Tests.IdentityDouble");
    m_node->registerTest(interfaceBase + "Tests.IdentityString");

    m_node->registerTest(interfaceBase + "Tests.IdentityArray");
    m_node->registerTest(interfaceBase + "Tests.IdentityByteArray");
    m_node->registerTest(interfaceBase + "Tests.IdentityBoolArray");
    m_node->registerTest(interfaceBase + "Tests.IdentityInt16Array");
    m_node->registerTest(interfaceBase + "Tests.IdentityUInt16Array");
    m_node->registerTest(interfaceBase + "Tests.IdentityInt32Array");
    m_node->registerTest(interfaceBase + "Tests.IdentityUInt32Array");
    m_node->registerTest(interfaceBase + "Tests.IdentityInt64Array");
    m_node->registerTest(interfaceBase + "Tests.IdentityUInt64Array");
    m_node->registerTest(interfaceBase + "Tests.IdentityDoubleArray");
    m_node->registerTest(interfaceBase + "Tests.IdentityStringArray");

    m_node->registerTest(interfaceBase + "Tests.Sum");
    m_node->registerTest(interfaceBase + "Tests.InvertMapping");
    m_node->registerTest(interfaceBase + "Tests.DeStruct");
    m_node->registerTest(interfaceBase + "Tests.Primitize");
    m_node->registerTest(interfaceBase + "Tests.Invert");
    m_node->registerTest(interfaceBase + "Tests.Trigger");
    m_node->registerTest(interfaceBase + "Tests.Exit");
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::Identity(const QDBusVariant& input, QDBusVariant& output,
        QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.Identity");

    Q_UNUSED(error);

    output = input;

    return true;
}


///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityByte(Q_UINT8 input, Q_UINT8& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityByte");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityBool(bool input, bool& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityBool");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityInt16(Q_INT16 input, Q_INT16& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityInt16");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityUInt16(Q_UINT16 input, Q_UINT16& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityUInt16");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityInt32(Q_INT32 input, Q_INT32& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityInt32");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityUInt32(Q_UINT32 input, Q_UINT32& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityUInt32");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityInt64(Q_INT64 input, Q_INT64& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityInt64");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityUInt64(Q_UINT64 input, Q_UINT64& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityUInt64");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityDouble(double input, double& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityDouble");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityString(const QString& input, QString& output,
        QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityString");

    Q_UNUSED(error);

    output = input;

    return true;
}


///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityArray(const QValueList<QDBusVariant>& input,
        QValueList<QDBusVariant>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityArray");

    Q_UNUSED(error);

    output = input;

    return true;
}


///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityByteArray(const QValueList<Q_UINT8>& input,
        QValueList<Q_UINT8>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityByteArray");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityBoolArray(const QValueList<bool>& input,
        QValueList<bool>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityBoolArray");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityInt16Array(const QValueList<Q_INT16>& input,
        QValueList<Q_INT16>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityInt16Array");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityUInt16Array(const QValueList<Q_UINT16>& input,
        QValueList<Q_UINT16>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityUInt16Array");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityInt32Array(const QValueList<Q_INT32>& input,
        QValueList<Q_INT32>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityInt32Array");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityUInt32Array(const QValueList<Q_UINT32>& input,
        QValueList<Q_UINT32>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityUInt32Array");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityInt64Array(const QValueList<Q_INT64>& input,
        QValueList<Q_INT64>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityInt64Array");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityUInt64Array(const QValueList<Q_UINT64>& input,
        QValueList<Q_UINT64>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityUInt64Array");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityDoubleArray(const QValueList<double>& input,
        QValueList<double>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityDoubleArray");

    Q_UNUSED(error);

    output = input;

    return true;
}


///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::IdentityStringArray(const QStringList& input, QStringList& output,
        QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.IdentityStringArray");

    Q_UNUSED(error);

    output = input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::Sum(const QValueList<Q_INT32>& input, Q_INT64& output,
        QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.Sum");

    Q_UNUSED(error);

    output = 0;

    QValueList<Q_INT32>::const_iterator it    = input.begin();
    QValueList<Q_INT32>::const_iterator endIt = input.end();
    for (; it != endIt; ++it)
    {
        output += *it;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::InvertMapping(const QMap<QString, QString>& input,
        QDBusDataMap<QString>& output, QDBusError& error)
{
    Q_UNUSED(error);

    m_node->acknowledgeTest(interfaceBase + "Tests.InvertMapping");

    // FIXME: should be in the generated code
    QDBusDataList listType(QDBusData::String);
    output = QDBusDataMap<QString>(QDBusData::fromList(listType));

    QMap<QString, QStringList> result;

    QMap<QString, QString>::const_iterator it    = input.begin();
    QMap<QString, QString>::const_iterator endIt = input.end();
    for (; it != endIt; ++it)
    {
        result[it.data()] << it.key();
    }

    QMap<QString, QStringList>::const_iterator resultIt    = result.begin();
    QMap<QString, QStringList>::const_iterator resultEndIt = result.end();
    for (; resultIt != resultEndIt; ++resultIt)
    {
        output.insert(resultIt.key(),
                      QDBusData::fromList(QDBusDataList(resultIt.data())));
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::DeStruct(const QDBusData& input, QString& outstring,
        Q_UINT32& outuint32, Q_INT16& outuint16, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.DeStruct");

    Q_UNUSED(error);

    bool ok  = false;
    bool ok1 = false;
    bool ok2 = false;
    bool ok3 = false;

    QValueList<QDBusData> inputStruct = input.toStruct(&ok);
    if (!ok || inputStruct.count() != 3)
    {
        error = QDBusError::stdInvalidArgs(
                           "DeStruct input not a structure or not three members");
        return false;
    }

    outstring = inputStruct[0].toString(&ok1);
    outuint32 = inputStruct[1].toUInt32(&ok1);
    outuint16 = inputStruct[2].toInt16(&ok1);

    if (ok1 && ok2 && ok3)
    {
        error = QDBusError::stdInvalidArgs(
                           QString("DeStruct input not valid: s(%1), u(%2), n(%3)").
                                   arg((ok1 ? "OK" : "WRONG")).
                                   arg((ok2 ? "OK" : "WRONG")).
                                   arg((ok3 ? "OK" : "WRONG")));
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

static bool primitize(const QDBusDataMap<Q_UINT8>&, QValueList<QDBusVariant>&);
static bool primitize(const QDBusDataMap<Q_INT16>&, QValueList<QDBusVariant>&);
static bool primitize(const QDBusDataMap<Q_UINT16>&, QValueList<QDBusVariant>&);
static bool primitize(const QDBusDataMap<Q_INT32>&, QValueList<QDBusVariant>&);
static bool primitize(const QDBusDataMap<Q_UINT32>&, QValueList<QDBusVariant>&);
static bool primitize(const QDBusDataMap<Q_INT64>&, QValueList<QDBusVariant>&);
static bool primitize(const QDBusDataMap<Q_UINT64>&, QValueList<QDBusVariant>&);
static bool primitize(const QDBusDataMap<QString>&, QValueList<QDBusVariant>&);
static bool primitize(const QDBusDataMap<QDBusObjectPath>&,
                      QValueList<QDBusVariant>&);

static bool primitize(const QDBusVariant&, QValueList<QDBusVariant>&);

static bool primitize(const QDBusData& data, QValueList<QDBusVariant>& list)
{
    switch (data.type())
    {
        case QDBusData::Variant:
            return primitize(data.toVariant().value, list);
        case QDBusData::List: {
            QValueList<QDBusVariant> subList = data.toList().toVariantList();

            QValueList<QDBusVariant>::const_iterator it    = subList.begin();
            QValueList<QDBusVariant>::const_iterator endIt = subList.end();
            for (; it != endIt; ++it)
            {
                if (!primitize(*it, list)) return false;
            }
            break;
        }
        case QDBusData::Struct: {
            QValueList<QDBusData> subList = data.toStruct();

            QValueList<QDBusData>::const_iterator it    = subList.begin();
            QValueList<QDBusData>::const_iterator endIt = subList.end();
            for (; it != endIt; ++it)
            {
                if (!primitize(*it, list)) return false;
            }
            break;
        }
        case QDBusData::Map:
            switch (data.keyType())
            {
                case QDBusData::Byte:
                    return primitize(data.toByteKeyMap(), list);
                case QDBusData::Int16:
                    return primitize(data.toInt16KeyMap(), list);
                case QDBusData::UInt16:
                    return primitize(data.toUInt16KeyMap(), list);
                case QDBusData::Int32:
                    return primitize(data.toInt32KeyMap(), list);
                case QDBusData::UInt32:
                    return primitize(data.toUInt32KeyMap(), list);
                case QDBusData::Int64:
                    return primitize(data.toInt64KeyMap(), list);
                case QDBusData::UInt64:
                    return primitize(data.toUInt64KeyMap(), list);
                case QDBusData::String:
                    return primitize(data.toStringKeyMap(), list);
                case QDBusData::ObjectPath:
                    return primitize(data.toObjectPathKeyMap(), list);
                default:
                    return false;
            }
            break;
        default:  // primitives
        {
            QDBusVariant variant;
            variant.value = data;

            switch (data.type())
            {
                case QDBusData::Invalid:
                    return false;
                case QDBusData::Bool:
                    variant.signature = "b";
                    break;
                case QDBusData::Byte:
                    variant.signature = "y";
                    break;
                case QDBusData::Int16:
                    variant.signature = "n";
                    break;
                case QDBusData::UInt16:
                    variant.signature = "q";
                    break;
                case QDBusData::Int32:
                    variant.signature = "i";
                    break;
                case QDBusData::UInt32:
                    variant.signature = "u";
                    break;
                case QDBusData::Int64:
                    variant.signature = "x";
                    break;
                case QDBusData::UInt64:
                    variant.signature = "t";
                    break;
                case QDBusData::Double:
                    variant.signature = "d";
                    break;
                case QDBusData::String:
                    variant.signature = "s";
                    break;
                case QDBusData::ObjectPath:
                    variant.signature = "o";
                    break;

                // handled in outer switch. use explicitly sowe can detect new
                // enum values
                case QDBusData::Variant: // fall through
                case QDBusData::List:    // fall through
                case QDBusData::Struct:  // fall through
                case QDBusData::Map:     // fall through
                    Q_ASSERT(false);
                    break;
            }

            list << variant;
            break;
        }
    }

    return true;
}

static bool primitize(const QDBusVariant& variant, QValueList<QDBusVariant>& list)
{
    if (variant.signature.isEmpty() || !variant.value.isValid()) return false;

    return primitize(variant.value, list);
}

static bool primitize(const QDBusDataMap<Q_UINT8>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<Q_UINT8>::const_iterator it    = map.begin();
    QDBusDataMap<Q_UINT8>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromByte(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

static bool primitize(const QDBusDataMap<Q_INT16>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<Q_INT16>::const_iterator it    = map.begin();
    QDBusDataMap<Q_INT16>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromInt16(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

static bool primitize(const QDBusDataMap<Q_UINT16>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<Q_UINT16>::const_iterator it    = map.begin();
    QDBusDataMap<Q_UINT16>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromUInt16(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

static bool primitize(const QDBusDataMap<Q_INT32>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<Q_INT32>::const_iterator it    = map.begin();
    QDBusDataMap<Q_INT32>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromInt32(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

static bool primitize(const QDBusDataMap<Q_UINT32>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<Q_UINT32>::const_iterator it    = map.begin();
    QDBusDataMap<Q_UINT32>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromUInt32(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

static bool primitize(const QDBusDataMap<Q_INT64>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<Q_INT64>::const_iterator it    = map.begin();
    QDBusDataMap<Q_INT64>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromInt64(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

static bool primitize(const QDBusDataMap<Q_UINT64>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<Q_UINT64>::const_iterator it    = map.begin();
    QDBusDataMap<Q_UINT64>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromUInt64(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

static bool primitize(const QDBusDataMap<QString>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<QString>::const_iterator it    = map.begin();
    QDBusDataMap<QString>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromString(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

static bool primitize(const QDBusDataMap<QDBusObjectPath>& map,
        QValueList<QDBusVariant>& list)
{
    QDBusDataMap<QDBusObjectPath>::const_iterator it    = map.begin();
    QDBusDataMap<QDBusObjectPath>::const_iterator endIt = map.end();
    for (; it != endIt; ++it)
    {
        primitize(QDBusData::fromObjectPath(it.key()), list);
        if (!primitize(it.data(), list)) return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::Primitize(const QDBusVariant& input,
        QValueList<QDBusVariant>& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.Primitize");

    bool result = primitize(input, output);
    if (!result)
    {
        error =
            QDBusError::stdFailed("Primitize encountered variant it can't handle");
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::Invert(bool input, bool& output, QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.Invert");

    Q_UNUSED(error);

    output = !input;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::Trigger(const QString& object, Q_UINT64 parameter,
        QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.Trigger");

    Q_UNUSED(error);

    emit triggered(object, parameter);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TestsImpl::Exit(QDBusError& error)
{
    m_node->acknowledgeTest(interfaceBase + "Tests.Exit");

    Q_UNUSED(error);

    QTimer::singleShot(10, this, SIGNAL(finished()));

    return true;
}

///////////////////////////////////////////////////////////////////////////////

void TestsImpl::handleMethodReply(const QDBusMessage& reply)
{
    m_node->handleMethodReply(reply);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TestSignalsImpl::TestSignalsImpl(TestNode* node)
    : m_node(node)
{
}

///////////////////////////////////////////////////////////////////////////////

void TestSignalsImpl::slotTriggered(const QString& object, Q_UINT64 parameter)
{
    m_objectPath = object;

    emitTriggered(parameter);

    m_objectPath = QString::null;
}

#if 0
///////////////////////////////////////////////////////////////////////////////

void TestSignalsImpl::slotTrigger()
{
    if (!m_objectPath.isEmpty() || m_responseState == Received) return;

    m_input1 = 0;
    m_input2 = -1e-10+1;

    m_responseState = SentTrigger;

    emitTrigger(m_input1, m_input2);
}
///////////////////////////////////////////////////////////////////////////////

void TestSignalsImpl::slotResponse(Q_UINT16 input1, double input2)
{
    m_responseState = Received;

    if (input1 == m_input1 && std::fabs(input2 - m_input2) < 1e-20)
    {
        m_node->acknowledgeTest(interfaceBase + "TestCallbacks.Response");
    }
}
#endif

///////////////////////////////////////////////////////////////////////////////

bool TestSignalsImpl::handleSignalSend(const QDBusMessage& reply)
{
    m_node->handleMethodReply(reply);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

QString TestSignalsImpl::objectPath() const
{
    return m_objectPath.isEmpty() ? QString::fromUtf8("/Test") : m_objectPath;
}

// End of File
