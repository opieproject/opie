#ifndef OPIE_IO_SERIAL
#define OPIE_IO_SERIAL

#include <qsocketnotifier.h>
#include "io_layer.h"

/* Default values to be used if the profile information is incomplete */
#define SERIAL_DEFAULT_DEVICE       "/dev/ttyS0"
#define SERIAL_DEFAULT_BAUD         9600
#define SERIAL_DEFAULT_PARITY       0
#define SERIAL_DEFAULT_DBITS        8
#define SERIAL_DEFAULT_SBITS        1
#define SERIAL_DEFAULT_FLOW         0

/* IOSerial implements a RS232 IO Layer */

class IOSerial : public IOLayer {
    Q_OBJECT
public:
    enum Parity {
        ParityNone = 0,
        ParityEven,
        ParityOdd,
        ParitySpace,
        ParityMark
    };

    enum Flow {
        FlowHW = 0x01,
        FlowSW = 0x02
    };

    IOSerial(const Profile &);
    ~IOSerial();

    QString identifier() const;
    QString name() const;
    int rawIO()const;
    void closeRawIO(int fd );
    QBitArray supports()const;
/*signals:
    void received(const QByteArray &);
    void error(int, const QString &);
*/
public slots:
    void send(const QByteArray &);
    bool open();
    void close();
    void reload(const Profile &);
protected:
    int baud(int baud) const;
protected slots:
    void dataArrived();
    void errorOccured();
protected:
    QSocketNotifier *m_read;
    QSocketNotifier *m_error;
    QString          m_device;
    int              m_baud;
    int              m_parity;
    int              m_dbits;
    int              m_sbits;
    int              m_flow;
    int              m_fd;
};

#endif /* OPIE_IO_SERIAL */
