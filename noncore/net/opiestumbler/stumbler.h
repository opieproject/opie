#ifndef STUMBLER_H
#define STUMBLER_H

#include <opie2/ostation.h>

#include <qobject.h>

class QString;
class QTimer;
namespace Opie {
    namespace Net {        
        class OWirelessNetworkInterface;
    }
}

class Stumbler: public QObject {
    Q_OBJECT

public:
    Stumbler(const QString &iface, QObject *parent = 0, const char *name = 0);
    void start();
    void stop();
    void setInterval(int msec);
    void setIface(const QString &iface);
    Opie::Net::OStationList * stations();
signals:
    void newdata();
protected slots:
    void slotRefresh();
protected:
    int m_interval;
    QString m_wifaceName;
    QTimer *m_timer;
    Opie::Net::OWirelessNetworkInterface *m_wiface;
    Opie::Net::OStationList *m_stationList;
};

#endif
