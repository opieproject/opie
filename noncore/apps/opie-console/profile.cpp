#include "profile.h"

Profile::Profile() {

}
Profile::Profile( const QString& name,
                  const QString& iolayerName,
                  int background,
                  int foreground,
                  int terminal )
    : m_name( name ), m_ioLayer( iolayerName ), m_back( background ),
      m_fore( foreground ), m_terminal( terminal )
{
}
Profile::Profile( const Profile& prof )
{
    (*this) = prof;
}
Profile &Profile::operator=( const Profile& prof ) {
    m_name = prof.m_name;
    m_ioLayer = prof.m_ioLayer;
    m_back = prof.m_back;
    m_fore = prof.m_fore;
    m_terminal = prof.m_terminal;

    return *this;
}
Profile::~Profile() {
}
QString Profile::name()const {
    return m_name;
}
QString Profile::ioLayerName()const {
    return m_ioLayer;
}
int Profile::foreground()const {
    return m_fore;
}
int Profile::background()const {
    return m_back;
}
int Profile::terminal()const {
    return m_terminal;
}
void Profile::setName( const QString& str ) {
    m_name = str;
}
void Profile::setIOLayer( const QString& name ) {
    m_ioLayer = name;
}
void Profile::setBackground( int back ) {
    m_back = back;
}
void Profile::setForeground( int fore ) {
    m_fore = fore;
}
void Profile::setTerminal( int term ) {
    m_terminal =  term;
}
