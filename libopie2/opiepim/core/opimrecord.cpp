#include <qpe/categories.h>
#include <qpe/categoryselect.h>

#include "opimrecord.h"

Qtopia::UidGen OPimRecord::m_uidGen( Qtopia::UidGen::Qtopia );


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
    m_xrefman = rec.m_xrefman;

    return  *this;
}
/*
 * category names
 */
QStringList OPimRecord::categoryNames()const {
    QStringList list;
    QArray<int> cats = categories();
    Categories catDB;
    catDB.load( categoryFileName() );

    for (uint i = 0; i < cats.count(); i++ ) {
        list << catDB.label("Todo List", cats[i] );
    }

    return list;
}
void OPimRecord::setCategoryNames( const QStringList& ) {

}
void OPimRecord::addCategoryName( const QString& ) {
    Categories catDB;
    catDB.load( categoryFileName() );


}
bool OPimRecord::isEmpty()const {
    return ( uid() == 0 );
}
/*QString OPimRecord::crossToString()const {
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
    }*/
/* if uid = 1 assign a new one */
void OPimRecord::setUid( int uid ) {
    if ( uid == 1)
        uid = uidGen().generate();

    Qtopia::Record::setUid( uid );
};
Qtopia::UidGen &OPimRecord::uidGen() {
    return m_uidGen;
}
OPimXRefManager &OPimRecord::xrefmanager() {
    return m_xrefman;
}
int OPimRecord::rtti(){
    return 0;
}
