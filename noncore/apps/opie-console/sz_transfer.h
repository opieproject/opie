#ifndef OPIE_FL_SZ_H
#define OPIE_FL_SZ_H

#include "file_layer.h"
#include <opie/oprocess.h>

class SzTransfer : public FileTransferLayer {

    Q_OBJECT

public:

    SzTransfer( IOLayer * );
    ~SzTransfer();

public slots:
    /**
     * send a file over the layer
     */
    void sendFile( const QString& file ) = 0;
    void sendFile( const QFile& ) = 0;

private slots:
    void SzRecievedStdout(OProcess *, char *, int);
    void SzRecievedStderr(OProcess *, char *, int);
    void recievedStdin(QByteArray &);

private:
    OProcess *proc;

};

#endif
