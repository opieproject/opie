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
bool Profile::operator==( const Profile& prof ) {
    if ( m_name == prof.m_name ) return true;

    return false;
}
Profile &Profile::operator=( const Profile& prof ) {
    m_name = prof.m_name;
    m_ioLayer = prof.m_ioLayer;
    m_back = prof.m_back;
    m_fore = prof.m_fore;
    m_terminal = prof.m_terminal;
    m_conf = prof.m_conf;

    return *this;
}
Profile::~Profile() {
}
QMap<QString, QString> Profile::conf()const {
    return m_conf;
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
/* config stuff */
void Profile::clearConf() {
    m_conf.clear();
}
void Profile::writeEntry( const QString& key,  const QString& value ) {
    m_conf.replace( key, value );
}
void Profile::writeEntry( const QString& key, int num ) {
    writeEntry( key,  QString::number( num ) );
}
void Profile::writeEntry( const QString& key, bool b ) {
    writeEntry( key, QString::number(b) );
}
void Profile::writeEntry( const QString& key, const QStringList& lis, const QChar& sep ) {
    writeEntry( key, lis.join(sep) );
}
QString Profile::readEntry( const QString& key,  const QString& deflt )const {
    QMap<QString, QString>::ConstIterator it;
    it = m_conf.find( key );

    if ( it != m_conf.end() )
        return it.data();

    return deflt;
}
int Profile::readNumEntry( const QString& key, int def )const {
    QMap<QString, QString>::ConstIterator it;
    it = m_conf.find( key );

    if ( it != m_conf.end() ) {
        bool ok;
        int val = it.data().toInt(&ok);

        if (ok)
            return val;
    }
    return def;
}
bool Profile::readBoolEntry( const QString& key,  bool def )const {
    return readNumEntry( key, def );
}
void Profile::setConf( const QMap<QString, QString>& conf ) {
    m_conf = conf;
};
