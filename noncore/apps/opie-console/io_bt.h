#ifndef OPIE_IO_BT
#define OPIE_IO_BT

#include <opie/oprocess.h>
#include "io_serial.h"

/* Default values to be used if the profile information is incomplete */
#define BT_DEFAULT_DEVICE       "/dev/ttyU0"
#define BT_DEFAULT_BAUD         9600
#define BT_DEFAULT_PARITY       0
#define BT_DEFAULT_DBITS        8
#define BT_DEFAULT_SBITS        1
#define BT_DEFAULT_FLOW         0
#define BT_DEFAULT_MAC          0


/* IOSerial implements a RS232 IO Layer */

class IOBt : public IOSerial {

    Q_OBJECT

public:

    IOBt(const Profile &);
    ~IOBt();

    QString identifier() const;
    QString name() const;

signals:
    void received(const QByteArray &);
    void error(int, const QString &);

public slots:
    bool open();
    void close();
    void reload(const Profile &);

private:
    OProcess *m_attach;
    QString m_mac;
private slots:
    void slotExited(OProcess* proc);

};

#endif /* OPIE_IO_IRDA */
