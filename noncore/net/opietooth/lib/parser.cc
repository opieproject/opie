
#include <qstringlist.h>

#include "parser.h"

using namespace OpieTooth;

namespace {


    // "Test Foo Bar" (0x3456)
    // @param ret Test Foo Bar
    // @eturn 13398
    // tactic find " (
int convert( const QString& line, QString& ret ) {
    ret = QString::null;
    int i = 0;
    int pos = line.findRev("\" (");
    if ( pos > 0 ) { // it shouldn't be at pos 0
        ret = line.left(pos-1 ).stripWhiteSpace();
        qWarning("ret: %s",  ret.latin1() );
        ret = ret.replace(QRegExp("[\"]"), "");
        qWarning("ret: %s", ret.latin1() );
        QString dummy = line.mid(pos + 4 );
        qWarning("dummy: %s",  dummy.latin1() );
        dummy = dummy.remove( dummy.length() -1, 1 ); // remove the (
        bool ok;
        i = dummy.toInt(&ok, 16 );
    }
    return i;
}

};


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
                m_complete = true;
                continue;
            }
        }
        if (parseName(      (*it) ) ) ;//continue;
        if (parseRecHandle( (*it) ) ) ;//continue;
        if (parseClassId(   (*it) ) ) ;//continue;
        if (parseProtocol(  (*it) ) ) ;//continue;
        if (parseProfile(   (*it) ) ) ;//continue;
    }
    // missed the last one
    if (m_complete) {
        m_list.append(m_item );
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

        // "Obex Object Push" (0x1105)
        //  find backwards the " and the from 0 to pos and the mid pos+1
        // then stripWhiteSpace add name replace '"' with ""
        // and then convert 0x1105 toInt()
        QString classes;
        int ids;
        ids = convert( str, classes );
        qWarning("ids %d", ids );
        m_item.setClassIdList( classes );
        m_item.setClassIdList( ids );

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

    }else if (m_protocolOver && str.startsWith("  ") ) { // "L2CAP" (0x0100)
        qWarning("double protocol filter");

        if (!m_protocolAdded ) { // the protocol does neither supply a channel nor port so add it now
            Services::ProtocolDescriptor desc(  m_protName,  m_protId );
            m_item.insertProtocolDescriptor( desc );
        }
        m_protocolAdded = false;
        { // the find function
            m_protId = convert(str, m_protName );
        }
        return true;
    }else if (m_protocolOver && str.startsWith("   ") ) {
        qWarning("tripple protocol filter");
        m_protocolAdded = true;
        QString dummy = str.stripWhiteSpace();
        int pos = dummy.findRev(':');
        if ( pos > -1 ) {
            int port = dummy.mid(pos+1 ).stripWhiteSpace().toInt();
            Services::ProtocolDescriptor desc(  m_protName,  m_protId, port );
            m_item.insertProtocolDescriptor( desc );
        }
        return true;
    }else if (m_protocolOver ) {
        m_protocolOver = false;
    }
    return false;
}
bool Parser::parseProfile( const QString& str) {
    if (str.startsWith("Profile Descriptor List:") ) {
        m_profOver = true;
    }else if ( m_profOver && str.startsWith("  ") ) {
        m_profId = convert( str,  m_profName );
    }else if ( m_profOver && str.startsWith("   ") ) {
    //  now find
        int pos = str.findRev(':');
        if ( pos > 0 ) {
            int dummy = str.mid(pos+1 ).stripWhiteSpace().toInt();
            qWarning("dummyInt:%d",  dummy );
            Services::ProfileDescriptor desc( m_profName, m_profId, dummy );
            m_item.insertProfileDescriptor(desc);
        }
    }else
        m_profOver = false;


    return false;
}
