#include "file_layer.h"

FileTransferLayer::FileTransferLayer(IOLayer *layer)
    : QObject(), m_layer( layer )
{
}

FileTransferLayer::~FileTransferLayer() {
}
IOLayer* FileTransferLayer::layer() {
    return m_layer;
}

