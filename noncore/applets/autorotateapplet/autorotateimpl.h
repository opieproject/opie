#include <qpe/taskbarappletinterface.h>

class AutoRotate;

class AutoRotateImpl : public TaskbarAppletInterface {
public:
    AutoRotateImpl();
    virtual ~AutoRotateImpl();

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual QWidget *applet( QWidget *parent );
    virtual int position() const;

private:
    AutoRotate *autoRotate;
};
