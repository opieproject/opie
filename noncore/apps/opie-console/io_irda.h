#ifndef OPIE_IO_IRDA
#define OPIE_IO_IRDA

#include <opie2/oprocess.h>
#include "io_serial.h"

/* Default values to be used if the profile information is incomplete */
#define IRDA_DEFAULT_DEVICE       "/dev/ircomm0"
#define IRDA_DEFAULT_BAUD         9600
#define IRDA_DEFAULT_PARITY       0
#define IRDA_DEFAULT_DBITS        8
#define IRDA_DEFAULT_SBITS        1
#define IRDA_DEFAULT_FLOW         0

/* IOSerial implements a RS232 IO Layer */

class IOIrda : public IOSerial {

    Q_OBJECT

public:

    IOIrda(const Profile &);
    ~IOIrda();

    virtual QString identifier() const;
    virtual QString name() const;
    virtual QBitArray supports() const;
    virtual bool isConnected();

signals:
    void received(const QByteArray &);
    void error(int, const QString &);

public slots:
    virtual void send( const QByteArray& );
    virtual bool open();
    virtual void close();
    virtual void reload(const Profile &);

private:
    OProcess *m_attach;

private slots:
    void slotExited(OProcess* proc);

};

#endif /* OPIE_IO_IRDA */
