#include "opimxref.h"

OPimXRef::OPimXRef( const OPimXRefPartner& one, const OPimXRefPartner& two )
    : m_partners(2)
{
    m_partners[0] = one;
    m_partners[1] = two;
}
OPimXRef::OPimXRef()
    : m_partners(2)
{

}
OPimXRef::OPimXRef( const OPimXRef& ref) {
    *this = ref;
}
OPimXRef::~OPimXRef() {
}
OPimXRef &OPimXRef::operator=( const OPimXRef& ref) {
    m_partners = ref.m_partners;
    m_partners.detach();

    return* this;
}
bool OPimXRef::operator==( const OPimXRef& oper ) {
    if ( m_partners == oper.m_partners ) return true;

    return false;
}
OPimXRefPartner OPimXRef::partner( enum Partners par) const{
    return m_partners[par];
}
void OPimXRef::setPartner( enum Partners par,  const OPimXRefPartner& part) {
    m_partners[par] = part;
}
bool OPimXRef::containsString( const QString& string ) const{
    if ( m_partners[One].appName() == string ||
         m_partners[Two].appName() == string ) return true;

    return false;
}
bool OPimXRef::containsUid( int uid ) const{
    if ( m_partners[One].uid() == uid ||
         m_partners[Two].uid() == uid ) return true;

    return false;
}
