
#include "device.h"

using namespace OpieTooth;

Device::Device(const QString &device, const QString &mode )
  : QObject(0, "device" ){

}
Device::~Device(){

}
void Device::attach(){

}
void Device::detach(){

}
bool Device::isLoaded()const{
  return false;
}
QString Device::devName()const {
  return QString::fromLatin1("hci0");
};
