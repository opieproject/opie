
#include <qtextstream.h>

#include "rfcommconfhandler.h"


using namespace OpieTooth;

// move to lib


RfCommConfObject::RfCommConfObject( int number, QString mac, int channel, QString comment ) {
    m_number = number;
    m_mac = mac;
    m_channel = channel;
    m_comment = comment;
    // m_foundEntries = 0;
}

void RfCommConfObject::setNumber( int number )  {
    m_number = number;
}

void RfCommConfObject::setMac( QString mac )  {
    m_mac = mac;
}

void RfCommConfObject::setChannel( int channel )  {
    m_channel = channel;
}

void RfCommConfObject::setComment( QString comment )  {
    m_comment = comment;
}


RfCommConfObject::~RfCommConfObject()  {
}


RfCommConfHandler::RfCommConfHandler( const QString & filename ) {

    m_filename = filename;
    load();
}

RfCommConfHandler::~RfCommConfHandler() {

}

void RfCommConfHandler::save( QMap<QString, RfCommConfObject*> devices )  {

    QFile rfCommConf( "/tmp/test" );
    QTextStream outStream( &rfCommConf );
    if (  rfCommConf.open( IO_WriteOnly ) )  {

        QMap<QString,  RfCommConfObject*>::Iterator it;
        for( it = devices.begin(); it != devices.end(); ++it )  {
            outStream << "rfcomm" + QString("%1").arg( it.data()->number() ) + " {\n";
            outStream << "  device " + it.data()->mac() + ";\n";
            outStream << "  channel " + QString( "%1" ).arg( it.data()->channel() ) + ";\n";
            outStream << "  comment \"" + it.data()->comment() + "\";\n";
            outStream << "}\n\n";
        }

        rfCommConf.close();
    }
}


QMap<QString, RfCommConfObject*> RfCommConfHandler::foundEntries()  {
    return m_foundEntries;
}

void RfCommConfHandler::load()  {

    QFile rfCommConf( m_filename );
    if ( rfCommConf.open( IO_ReadOnly ) )  {

        QStringList list;
        QTextStream inStream( &rfCommConf );
        list = QStringList::split( "\n", inStream.read() );

        QString number;
        QString mac;
        QString channel;
        QString comment;

        for ( QStringList::Iterator line=list.begin(); line != list.end(); line++ )  {

            QString tmpLine = ( *line ).stripWhiteSpace();

            if ( tmpLine.startsWith("rfcomm") )  {
                QString number = tmpLine.mid( 6,1 );
                odebug << tmpLine << oendl; 
                odebug << "TEST " + number << oendl; 
            } else if ( tmpLine.startsWith( "}" ) ) {
                m_foundEntries.insert( number, new RfCommConfObject( number.toInt(), mac, channel.toInt(),  comment ) );
            } else if ( tmpLine.startsWith( "device" ) )  {
                mac = tmpLine.mid( 7, 17 );
                odebug << "mac" + mac << oendl; 
            } else if ( tmpLine.startsWith( "channel" ) ) {
                channel = tmpLine.mid( 8, 1 );
                qDebug ( "Channel :" + channel );
            } else if ( tmpLine.startsWith( "comment" ) ) {
                comment = tmpLine.mid( 9, tmpLine.find( ';' ) - 9 - 1 );
                odebug << "Comment: " + comment << oendl; 
            }
        }
        rfCommConf.close();
    }
    save( m_foundEntries );
    odebug << QString( "ENTries: %1").arg( m_foundEntries.count() ) << oendl; 
}
