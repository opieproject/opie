
#include <qstringlist.h>

#include "parser.h"

using namespace OpieTooth;

Parser::Parser(const QString& output ) {
    parse( output );
}
void Parser::setText(const QString& output) {
    parse( output );
}
Services::ValueList Parser::services() const {
    return m_list;
}
void Parser::parse( const QString& string) {
    m_list.clear();
    m_complete = true;
    QStringList list = QStringList::split('\n', string );
    QStringList::Iterator it;
    for (it = list.begin(); it != list.end(); ++it ) {
        if ( (*it).startsWith("Browsing") ) continue;

        if ( (*it).isEmpty() ) { // line is empty because a new Service begins
        // now see if complete and add
            if (m_complete ) {
                m_list.append( m_item );
                Services serv;
                m_item = serv;
                continue;
            }
        }
        if (parseName(      (*it) ) ) ;//continue;
        if (parseRecHandle( (*it) ) ) ;//continue;
        if (parseClassId(   (*it) ) ) ;//continue;
        if (parseProtocol(  (*it) ) ) ;//continue;
        if (parseProfile(   (*it) ) ) ;//continue;
    }
}
bool Parser::parseName( const QString& str) {
    if (str.startsWith("Service Name:") ) {
        m_item.setServiceName( str.mid(13).stripWhiteSpace() );
        qWarning(m_item.serviceName() );
        return true;
    }
    return false;
}
bool Parser::parseRecHandle( const QString& str) {
    if (str.startsWith("Service RecHandle:" ) ) {
        QString out = str.mid(18 ).stripWhiteSpace();
        qWarning("out %s", out.latin1() );
        int value = out.toInt(&m_ok, 16 );
        if (m_ok && (value != -1) )
            m_complete = true;
        else
            m_complete = false;
        return true;
        m_item.setRecHandle( value );
    }
    return false;
}
bool Parser::parseClassId( const QString& str) {
    if (str.startsWith("Service Class ID List:") ) {
        m_classOver = true;
        return true;
    }else if ( m_classOver ) { // ok now are the informations in place

        m_classOver = false;
        QStringList list = QStringList::split('\n', str.stripWhiteSpace() );

        if ( list.count() == 2 ) {
            m_item.setClassIdList( list[0] );
            // now let's parse the number (0x1105)
            QString classId= list[1];
            int classIdInt;
            qWarning("%s",  list[1].latin1() );
            classId = classId.remove(0, 3 );
            classId = classId.remove( classId.length()-1, 1 );
            qWarning("%s", classId.latin1() );
            m_item.setClassIdList( classId.toInt(&m_ok, 16 ) );
        }
        return true;
    }else
        m_classOver = true;
    return false;
}
bool Parser::parseProtocol( const QString& str) {
    if (str.startsWith("Protocol Descriptor List:") ) {
        m_protocolOver = true;
        m_protocolAdded = false;
        return true;

    }else if (m_protocolOver && str.startsWith("  ") ) {
        qWarning("double protocol filter");
        if (!m_protocolAdded ) { // the protocol does neither supply a channel nor port so add it now
            Services::ProtocolDescriptor desc(  m_protName,  m_protId );
            m_item.insertProtocolDescriptor( desc );
        }
        m_protocolAdded = false;
        return true;
    }else if (m_protocolOver && str.startsWith("   ") ) {
        qWarning("tripple protocol filter");
        m_protocolAdded = true;
        return true;
    }else if (m_protocolOver ) {
        m_protocolOver = false;
    }
    return false;
}
bool Parser::parseProfile( const QString& ) {
    return false;
}
