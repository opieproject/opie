#include <qpe/taskbarappletinterface.h>

class CardMonitor;

class CardMonitorImpl : public TaskbarAppletInterface {
public:
    CardMonitorImpl();
    virtual ~CardMonitorImpl();

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual QWidget *applet( QWidget *parent );
    virtual int position() const;

private:
    CardMonitor *cardMonitor;
    ulong ref;
};
