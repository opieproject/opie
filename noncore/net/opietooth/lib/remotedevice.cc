
#include "remotedevice.h"

using namespace OpieTooth;

bool operator==(const RemoteDevices& rem1, const RemoteDevices& rem2){
  if( ( rem1.mac() == rem2.mac() ) && (rem1.name() == rem2.name() ) )
    return true;

  return false;
}

RemoteDevices::RemoteDevices(){

}
RemoteDevices::RemoteDevices(const RemoteDevices& ole ){
  (*this)  = ole;
}
RemoteDevices::RemoteDevices(const QString &mac, const QString& name ){
  m_mac = mac;
  m_name = name;
}
RemoteDevices::~RemoteDevices(){

}
bool RemoteDevices::isEmpty() const {
  if( m_name.isEmpty() && m_mac.isEmpty() )
    return true;
  return false;
};
RemoteDevices& RemoteDevices::operator=( const RemoteDevices& rem1){
  m_name = rem1.m_name;
  m_mac = rem1.m_mac;
  return *this;

}
QString  RemoteDevices::mac() const {
  return m_mac;
}
void RemoteDevices::setMac( const QString& mac ){
  m_mac = mac;
}
QString RemoteDevices::name() const{
  return m_name;
}
void RemoteDevices::setName( const QString& name ){
  m_name = name;
}
