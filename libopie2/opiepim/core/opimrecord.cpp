#include "opimrecord.h"

OPimRecord::OPimRecord( int uid )
    : Qtopia::Record() {

    setUid( uid );
}
OPimRecord::~OPimRecord() {
}
OPimRecord::OPimRecord( const OPimRecord& rec )
    : Qtopia::Record( rec )
{
    (*this) = rec;
}

OPimRecord &OPimRecord::operator=( const OPimRecord& rec) {
    Qtopia::Record::operator=( rec );
    m_relations = rec.m_relations;

    return  *this;
}
QStringList OPimRecord::categoryNames()const {
    QStringList list;

    return list;
}
void OPimRecord::setCategoryNames( const QStringList& ) {

}
void OPimRecord::addCategoryName( const QString& ) {

}
bool OPimRecord::isEmpty()const {
    return ( uid() == 0 );
}
QStringList OPimRecord::relatedApps()const{
    QStringList list;
    QMap<QString, QArray<int> >::ConstIterator it;
    for ( it = m_relations.begin(); it != m_relations.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QArray<int> OPimRecord::relations(const QString& app )const {
    QArray<int> tmp;
    QMap<QString, QArray<int> >::ConstIterator it;
    it = m_relations.find( app);
    if ( it != m_relations.end() )
        tmp = it.data();
    return tmp;
}
void OPimRecord::clearRelation( const QString& app ) {
    m_relations.remove( app );
}
void OPimRecord::addRelation( const QString& app,  int id ) {

    QMap<QString, QArray<int> >::Iterator  it;
    QArray<int> tmp;

    it = m_relations.find( app );
    if ( it == m_relations.end() ) {
        tmp.resize(1 );
        tmp[0] = id;
    }else{
        tmp = it.data();
        tmp.resize( tmp.size() + 1 );
        tmp[tmp.size() - 1] = id;
    }
    m_relations.replace( app,  tmp );
}
void OPimRecord::setRelations( const QString& app,  QArray<int> ids ) {

    QMap<QString, QArray<int> >::Iterator it;
    QArray<int> tmp;

    it = m_relations.find( app);
    if ( it == m_relations.end() ) {
        tmp = ids;
    }else{
        tmp = it.data();
        int offset = tmp.size()-1;
        tmp.resize( tmp.size() + ids.size() );
        for (uint i = 0; i < ids.size(); i++ ) {
            tmp[offset+i] = ids[i];
        }

    }
    m_relations.replace( app,  tmp );
}
QString OPimRecord::crossToString()const {
    QString str;
    QMap<QString, QArray<int> >::ConstIterator it;
    for (it = m_relations.begin(); it != m_relations.end(); ++it ) {
        QArray<int> id = it.data();
        for ( uint i = 0; i < id.size(); ++i ) {
            str += it.key() + "," + QString::number( i ) + ";";
        }
    }
    str = str.remove( str.length()-1, 1); // strip the ;
    //qWarning("IDS " + str );

    return str;
}
/* if uid = 1 assign a new one */
void OPimRecord::setUid( int uid ) {

    if ( uid == 1)
        uid = uidGen().generate();

    Qtopia::Record::setUid( uid );
};
