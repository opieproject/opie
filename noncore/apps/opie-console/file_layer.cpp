#include <qfile.h>
#include <opie/oprocess.h>

#include "file_layer.h"

FileTransferLayer::FileTransferLayer(IOLayer *layer)
    : QObject(), m_layer( layer )
{
}

FileTransferLayer::~FileTransferLayer() {
}

void FileTransferLayer::sendFile(const QFile& file) {

}

void FileTransferLayer::sendFile(const QString& file) {
}

IOLayer* FileTransferLayer::layer() {
    return m_layer;
}
