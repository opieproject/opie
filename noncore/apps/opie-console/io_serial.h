#ifndef OPIE_IO_SERIAL
#define OPIE_IO_SERIAL

#include "io_layer.h"

class IOSerial : public IOLayer {
    Q_OBJECT
public:
    IOSerial(const Config &);
public slots:
    void received(const QByteArray &);
    void error(int, const QString &);
};


#endif /* OPIE_IO_SERIAL */
