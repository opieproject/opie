

#include <qpe/taskbarappletinterface.h>

class SdMonitor;

class SdMonitorImpl : public TaskbarAppletInterface
{
public:
    SdMonitorImpl();
    virtual ~SdMonitorImpl();

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual QWidget *applet( QWidget *parent );
    virtual int position() const;

private:
    SdMonitor *sdMonitor;
    ulong ref;
};
