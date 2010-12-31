#if !defined(CROSSTEST_SERVER_H_INCLUDED)
#define CROSSTEST_SERVER_H_INCLUDED

// Qt includes
#include <qdict.h>
#include <qmap.h>
#include <qobject.h>

// Qt D-Bus includes
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusobject.h>

// forward declarations
class QDBusProxy;
class TestSignalsImpl;

class TestNode : public QObject, public QDBusObjectBase
{
    Q_OBJECT
public:
    TestNode(const QDBusConnection& connection);

    virtual ~TestNode();

    void handleMethodReply(const QDBusMessage& reply);

    void registerTest(const QString& function);

    void acknowledgeTest(const QString& function);

    void writeSummary();

protected:
    virtual bool handleMethodCall(const QDBusMessage& message);

private:
    QDBusConnection m_connection;

    QDict<QDBusObjectBase> m_interfaces;
    TestSignalsImpl* m_signalInterface;

    QMap<QString, int> m_tests;

    QDBusProxy* m_clientProxy;

    QString m_introspectionData;

private:
    void handleIntrospect(const QDBusMessage& message);

private slots:
    void slotDBusSignal(const QDBusMessage& message);
};

#endif

// End of File
