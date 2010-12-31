#if !defined(INTERFACEIMPLS_H_INCLUDED)
#define INTERFACEIMPLS_H_INCLUDED

// Qt includes
#include <qobject.h>

// local includes
#include "singletestsinterface.h"
#include "testsinterface.h"
#include "testsignalsinterface.h"

// forward declarations
class TestNode;

class SingleTestsImpl : public org::freedesktop::DBus::Binding::SingleTests
{
public:
    SingleTestsImpl(TestNode* node);

protected:
    virtual bool Sum(const QValueList<Q_UINT8>& input, Q_UINT32& output,
                     QDBusError& error);

protected: // implement sending replies
    virtual void handleMethodReply(const QDBusMessage& reply);

private:
    TestNode* m_node;
};

///////////////////////////////////////////////////////////////////////////////

class TestsImpl : public QObject, public org::freedesktop::DBus::Binding::Tests
{
    Q_OBJECT
public:
    TestsImpl(TestNode* node);

signals:
    void triggered(const QString& object, Q_UINT64 parameter);

    void finished();

protected:
    virtual bool Identity(const QDBusVariant& input, QDBusVariant& output, QDBusError& error);

    virtual bool IdentityByte(Q_UINT8 input, Q_UINT8& output, QDBusError& error);

    virtual bool IdentityBool(bool input, bool& output, QDBusError& error);

    virtual bool IdentityInt16(Q_INT16 input, Q_INT16& output, QDBusError& error);

    virtual bool IdentityUInt16(Q_UINT16 input, Q_UINT16& output, QDBusError& error);

    virtual bool IdentityInt32(Q_INT32 input, Q_INT32& output, QDBusError& error);

    virtual bool IdentityUInt32(Q_UINT32 input, Q_UINT32& output, QDBusError& error);

    virtual bool IdentityInt64(Q_INT64 input, Q_INT64& output, QDBusError& error);

    virtual bool IdentityUInt64(Q_UINT64 input, Q_UINT64& output, QDBusError& error);

    virtual bool IdentityDouble(double input, double& output, QDBusError& error);

    virtual bool IdentityString(const QString& input, QString& output, QDBusError& error);

    virtual bool IdentityArray(const QValueList<QDBusVariant>& input, QValueList<QDBusVariant>& output, QDBusError& error);

    virtual bool IdentityByteArray(const QValueList<Q_UINT8>& input, QValueList<Q_UINT8>& output, QDBusError& error);

    virtual bool IdentityBoolArray(const QValueList<bool>& input, QValueList<bool>& output, QDBusError& error);

    virtual bool IdentityInt16Array(const QValueList<Q_INT16>& input, QValueList<Q_INT16>& output, QDBusError& error);

    virtual bool IdentityUInt16Array(const QValueList<Q_UINT16>& input, QValueList<Q_UINT16>& output, QDBusError& error);

    virtual bool IdentityInt32Array(const QValueList<Q_INT32>& input, QValueList<Q_INT32>& output, QDBusError& error);

    virtual bool IdentityUInt32Array(const QValueList<Q_UINT32>& input, QValueList<Q_UINT32>& output, QDBusError& error);

    virtual bool IdentityInt64Array(const QValueList<Q_INT64>& input, QValueList<Q_INT64>& output, QDBusError& error);

    virtual bool IdentityUInt64Array(const QValueList<Q_UINT64>& input, QValueList<Q_UINT64>& output, QDBusError& error);

    virtual bool IdentityDoubleArray(const QValueList<double>& input, QValueList<double>& output, QDBusError& error);

    virtual bool IdentityStringArray(const QStringList& input, QStringList& output, QDBusError& error);

    virtual bool Sum(const QValueList<Q_INT32>& input, Q_INT64& output, QDBusError& error);

    virtual bool InvertMapping(const QMap<QString, QString>& input, QDBusDataMap<QString>& output, QDBusError& error);

    virtual bool DeStruct(const QDBusData& input, QString& outstring, Q_UINT32& outuint32, Q_INT16& outuint16, QDBusError& error);

    virtual bool Primitize(const QDBusVariant& input, QValueList<QDBusVariant>& output, QDBusError& error);

    virtual bool Invert(bool input, bool& output, QDBusError& error);

    virtual bool Trigger(const QString& object, Q_UINT64 parameter, QDBusError& error);

    virtual bool Exit(QDBusError& error);

protected: // implement sending replies
    virtual void handleMethodReply(const QDBusMessage& reply);

private:
    TestNode* m_node;
};

///////////////////////////////////////////////////////////////////////////////

class TestSignalsImpl : public QObject,
                        public org::freedesktop::DBus::Binding::TestSignals
{
    Q_OBJECT
public:
    TestSignalsImpl(TestNode* node);

public slots:
    void slotTriggered(const QString& object, Q_UINT64 parameter);

protected:
    virtual bool handleSignalSend(const QDBusMessage& reply);
    virtual QString objectPath() const;

private:
    TestNode* m_node;
    QString m_objectPath;
};

#endif

// End of File
