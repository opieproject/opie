
#include "services.h"

using namespace OpieTooth;


Services::ProfileDescriptor::ProfileDescriptor(){

}
Services::ProfileDescriptor::ProfileDescriptor(const QString &id, uint idInt, uint version ){
  m_id = id;
  m_idInt = idInt;
  m_version = version;
}
Services::ProfileDescriptor::ProfileDescriptor( const ProfileDescriptor& rem){
  (*this) = rem;
}
QString Services::ProfileDescriptor::id() const {
  return m_id;
}
void Services::ProfileDescriptor::setId( const QString& id ){
  m_id = id;
}
void Services::ProfileDescriptor::setId(uint id ){
  m_idInt = id;
}
uint Services::ProfileDescriptor::idInt()const{
  return m_idInt;
}
uint Services::ProfileDescriptor::version() const{
  return m_version;
}
void Services::ProfileDescriptor::setVersion(uint version){
  m_version = version;
}
Services::ProfileDescriptor& Services::ProfileDescriptor::operator=( const Services::ProfileDescriptor& prof){
  m_id = prof.m_id;
  m_idInt = prof.m_idInt;
  m_version = prof.m_version;
  return *this;
}
bool operator==(const Services::ProfileDescriptor& first, 
		const Services::ProfileDescriptor& second ){

  if( (first.id() == second.id() ) && 
      (first.version() == second.version() ) &&
      (first.idInt() == second.idInt() ) )
    return true;
  return false;
}

Services::ProtocolDescriptor::ProtocolDescriptor(){
  m_number = 0;
  m_channel = 0;
}
Services::ProtocolDescriptor::ProtocolDescriptor(const QString& name,
						 uint number,
						 uint channel){
  m_name = name;
  m_number = number;
  m_channel = channel;
}
Services::ProtocolDescriptor::ProtocolDescriptor( const ProtocolDescriptor& ole ){
  (*this) = ole;
}
Services::ProtocolDescriptor::~ProtocolDescriptor(){

}
QString Services::ProtocolDescriptor::name() const{
  return m_name;
}
void Services::ProtocolDescriptor::setName(const QString& name ){
  m_name = name;
}
uint Services::ProtocolDescriptor::id()const {
  return m_number;
}
void Services::ProtocolDescriptor::setId( uint id ){
  m_number = id;
}
uint Services::ProtocolDescriptor::port()const {
  return m_channel;
}
void Services::ProtocolDescriptor::setPort( uint port ){
  m_channel = port;
}
Services::ProtocolDescriptor &Services::ProtocolDescriptor::operator=( const Services::ProtocolDescriptor& desc ){
  m_name = desc.m_name;
  m_channel = desc.m_channel;
  m_number = desc.m_number;
  return *this;
}
bool operator==( const Services::ProtocolDescriptor &first,
		 const Services::ProtocolDescriptor &second ){
  if( ( first.name() == second.name() ) &&
      ( first.id() == second.id() ) && 
      ( first.port() == second.port() ) )
    return true;

    return false;

}

Services::Services(){

}
Services::Services(const Services& service ){

}
Services::~Services(){

}
Services &Services::operator=( const Services& ){
  return *this;
}
bool operator==( const Services&,
		 const Services& ){
  return false;
}
QString Services::serviceName() const{

}
void Services::setServiceName( const QString& service ){

}
int Services::recHandle() const{

}
void Services::setRecHandle( int ){

}
QString Services::classIdList() const{

}
void Services::setClassIdList( const QString& ){

}
int Services::classIdListInt() const{

}
void Services::setClassIdList(int ){

}
void Services::insertProtocolDescriptor( const ProtocolDescriptor& ){

}
void Services::clearProtocolDescriptorList(){

}
void Services::removeProtocolDescriptor( const ProtocolDescriptor& ){

}
Services::ProtocolDescriptor::ValueList Services::protocolDescriptorList()const{

}

void Services::insertProfileDescriptor( const ProfileDescriptor& ){

}
void Services::clearProfileDescriptorList(){

}
void Services::removeProfileDescriptor( const ProfileDescriptor& ){

}
Services::ProfileDescriptor::ValueList Services::profileDescriptor() const{

}
