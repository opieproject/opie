#ifndef OPIE_FL_SZ_H
#define OPIE_FL_SZ_H

#include "file_layer.h"
#include <opie/oprocess.h>

class SzTransfer : public FileTransferLayer {

    Q_OBJECT

public:
    enum Type {
      SZ=0,
      SX,
      SY
    };

    SzTransfer( Type t, IOLayer * );
    ~SzTransfer();

public slots:
    /**
     * send a file over the layer
     */
    void sendFile( const QString& file ) ;
    void sendFile( const QFile& );

private slots:
    void SzRecievedStdout(OProcess *, char *, int);
    void SzRecievedStderr(OProcess *, char *, int);
    void recievedStdin(QByteArray &);

private:
    OProcess *proc;
    Type m_t;

};

#endif
