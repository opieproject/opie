
#include "manager.h"


using namespace OpieTooth;

Manager::Manager( const QString& )
  : QObject()
{

}
Manager::Manager( Device* dev )
  : QObject()
{

}
Manager::Manager()
  : QObject()
{

}
Manager::~Manager(){

}
void Manager::setDevice( const QString& dev ){

}
void Manager::setDevice( Device* dev ){

}
void Manager::isConnected( const QString& device ){

}
void Manager::isConnected( Device* dev ){


}
void Manager::searchDevices( const QString& device ){

}

void Manager::searchDevices(Device* d ){


}
void Manager::addService(const QString& name ){

}
void Manager::addServices(const QStringList& ){

}
void Manager::removeService( const QString& name ){

}
void Manager::removeServices( const QStringList& ){

}
void Manager::searchServices( const QString& remDevice ){

}
void Manager::searchServices( const RemoteDevices& ){

}
QString Manager::toDevice( const QString& mac ){

}
QString Manager::toMac( const QString &device ){

}
