#ifndef OPIE_FILE_LAYER_H
#define OPIE_FILE_LAYER_H

#include "io_layer.h"

class QFile;
/**
 * this is the layer for sending files
 */
class FileTransferLayer : public QObject {
    Q_OBJECT
public:
    /**
     *the io layer to be used
     */
    FileTransferLayer( IOLayer* );
    virtual ~FileTransferLayer();

public slots:
    /**
     * send a file over the layer
     */
    virtual void sendFile( const QString& file ) = 0;

    /**
     * convience method
     */
    virtual void sendFile( const QFile& ) = 0;

signals:
    /**
     * sent the file
     */
    void sent();

    /**
     * an error occured
     */

    void error( int, const QString& );

    /*
     * 100 == done
     *
     */
    void progress( const QString& file, int progress );

protected:
    IOLayer* layer();

private:
    IOLayer* m_layer;

};

#endif
