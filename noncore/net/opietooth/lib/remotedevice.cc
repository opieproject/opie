
#include "remotedevice.h"

using namespace OpieTooth;

bool operator==(const RemoteDevice& rem1, const RemoteDevice& rem2){
  if( ( rem1.mac() == rem2.mac() ) && (rem1.name() == rem2.name() ) )
    return true;

  return false;
}

RemoteDevice::RemoteDevice(){

}
RemoteDevice::RemoteDevice(const RemoteDevice& ole ){
  (*this)  = ole;
}
RemoteDevice::RemoteDevice(const QString &mac, const QString& name ){
  m_mac = mac;
  m_name = name;
}
RemoteDevice::~RemoteDevice(){

}
bool RemoteDevice::isEmpty() const {
  if( m_name.isEmpty() && m_mac.isEmpty() )
    return true;
  return false;
};
RemoteDevice& RemoteDevice::operator=( const RemoteDevice& rem1){
  m_name = rem1.m_name;
  m_mac = rem1.m_mac;
  return *this;

}
QString  RemoteDevice::mac() const {
  return m_mac;
}
void RemoteDevice::setMac( const QString& mac ){
  m_mac = mac;
}
QString RemoteDevice::name() const{
  return m_name;
}
void RemoteDevice::setName( const QString& name ){
  m_name = name;
}
bool RemoteDevice::equals( const RemoteDevice& dev ) const {
    if ( m_mac == dev.m_mac && m_name == dev.m_name )
        return true;
    else
        return false;
};
