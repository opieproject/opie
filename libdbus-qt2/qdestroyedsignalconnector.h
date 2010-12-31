#include <qobject.h>

class QDestroyedSignalConnector : public QObject
{
    Q_OBJECT

public:
    QDestroyedSignalConnector( QObject *obj ) { this->obj = obj; };

signals:
    void objectDestroyed(QObject *);

protected slots:
    void internalDestroyed() { emit objectDestroyed(obj); delete this; };

protected:
    QObject *obj;
};
