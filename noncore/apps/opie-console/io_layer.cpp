#include "io_layer.h"

IOLayer::IOLayer()
    : QObject()
{
}

IOLayer::IOLayer(const Profile &)
    : QObject()
{
}

IOLayer::~IOLayer() {
}
int IOLayer::rawIO()const{
    return -1;
}
void IOLayer::closeRawIO(int) {

}
void IOLayer::setSize(int, int ) {
}

