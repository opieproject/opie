
#include "services.h"

using namespace OpieTooth;


Services::ProfileDescriptor::ProfileDescriptor(){

}
Services::ProfileDescriptor::ProfileDescriptor(const QString &id, int idInt, int version ){
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
void Services::ProfileDescriptor::setId(int id ){
  m_idInt = id;
}
int Services::ProfileDescriptor::idInt()const{
  return m_idInt;
}
int Services::ProfileDescriptor::version() const{
  return m_version;
}
void Services::ProfileDescriptor::setVersion(int version){
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
						 int number,
						 int channel){
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
int Services::ProtocolDescriptor::id()const {
  return m_number;
}
void Services::ProtocolDescriptor::setId( int id ){
  m_number = id;
}
int Services::ProtocolDescriptor::port()const {
  return m_channel;
}
void Services::ProtocolDescriptor::setPort( int port ){
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
    (*this) = service;
}
Services::~Services(){

}
Services &Services::operator=( const Services& ser){
    m_name =  ser.m_name;
    m_recHandle = ser.m_recHandle;
    m_classIds = ser.m_classIds;
    m_protocols = ser.m_protocols;
    m_profiles = ser.m_profiles;
  return *this;
}
bool operator==( const Services& one,
		 const Services& two){
    if ( ( one.recHandle() == two.recHandle() ) &&
         ( one.serviceName() == two.serviceName() ) &&
         ( one.protocolDescriptorList() == two.protocolDescriptorList() ) &&
         ( one.profileDescriptor() == two.profileDescriptor() )
         /* ( one.classIdList() == two.classIdList() ) */ )
        return true;
  return false;
}
QString Services::serviceName() const{
    return m_name;
}
void Services::setServiceName( const QString& service ){
    m_name = service;
}
int Services::recHandle() const{
    return m_recHandle;
}
void Services::setRecHandle( int handle){
    m_recHandle = handle;
}
QMap<int, QString> Services::classIdList()const {
    return m_classIds;
};
void Services::insertClassId( int id, const QString& str ) {
    m_classIds.insert( id, str );
}
void Services::removeClassId(int id) {
    m_classIds.remove( id );
}
void Services::clearClassId() {
    m_classIds.clear();
}
void Services::insertProtocolDescriptor( const ProtocolDescriptor& prot){
    m_protocols.append( prot );
}
void Services::clearProtocolDescriptorList(){
    m_protocols.clear();
}
void Services::removeProtocolDescriptor( const ProtocolDescriptor& prot){
    m_protocols.remove( prot );
}
Services::ProtocolDescriptor::ValueList Services::protocolDescriptorList()const{
    return m_protocols;
}


void Services::insertProfileDescriptor( const ProfileDescriptor& prof){
    m_profiles.append( prof );
}
void Services::clearProfileDescriptorList(){
    m_profiles.clear();
}
void Services::removeProfileDescriptor( const ProfileDescriptor& prof){
    m_profiles.remove(prof );
}
Services::ProfileDescriptor::ValueList Services::profileDescriptor() const{
    return m_profiles;
}
