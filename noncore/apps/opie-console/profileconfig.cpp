
#include "profileconfig.h"

ProfileConfig::ProfileConfig( const QString& prof )
    : Config( prof )
{
}
ProfileConfig::~ProfileConfig() {

}
QStringList ProfileConfig::groups()const {
    QStringList list;
    QMap<QString, ConfigGroup>::ConstIterator it;
    it= Config::groups.begin();

    for (; it != Config::groups.end(); ++it )
        list << it.key();


    return list;

}
void ProfileConfig::clearAll() {
    QMap<QString, ConfigGroup>::ConstIterator it;
    it = Config::groups.begin();

    for ( ; it != Config::groups.end(); ++it )
        clearGroup( it.key() );
}
void ProfileConfig::clearGroup( const QString& str ) {
    QString cur =git.key();
    setGroup( str );
    Config::clearGroup();
    setGroup( cur );
}
QMap<QString, QString> ProfileConfig::items( const QString& group )const {
    QMap<QString, QString> map;
    QMap<QString, ConfigGroup>::ConstIterator it;
    it = Config::groups.find( group );

    if (it != Config::groups.end() )
        map = it.data();

    return map;
}
