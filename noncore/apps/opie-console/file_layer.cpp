#include "file_layer.h"

FileTransferLayer::FileTransferLayer(IOLayer *layer)
    : QObject(), m_layer( layer )
{
}

FileTransferLayer::~FileTransferLayer() {
}

void FileTransferLayer::sendFile(const QFile&) {

}

void FileTransferLayer::sendFile(const QString&) {
}

IOLayer* FileTransferLayer::layer() {
    return m_layer;
}
