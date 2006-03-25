
#include <qtextstream.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

#include "rfcommconfhandler.h"


using namespace OpieTooth;

// move to lib


RfCommConfObject::RfCommConfObject(int number, QString mac, int channel, 
    QString comment, bool bind) {
    m_number = number;
    m_mac = mac;
    m_channel = channel;
    m_comment = comment;
    m_doBind = bind;
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

////    For debugging purposes
////    QFile rfCommConf( "/mnt/net/opie/bin/rfcomm.conf" );
    QFile rfCommConf( m_filename );
    QTextStream outStream( &rfCommConf );
    if (  rfCommConf.open( IO_WriteOnly ) )  {
        QMap<QString,  RfCommConfObject*>::Iterator it;
        outStream << "#\n";
        outStream << "# RFCOMM configuration file.\n";
        outStream << "#\n";
        outStream << "# $Id: rfcommconfhandler.cpp,v 1.6 2006-03-25 18:10:13 korovkin Exp $\n";
        outStream << "#\n\n";
        for( it = devices.begin(); it != devices.end(); ++it )  {
            outStream << "rfcomm" + QString("%1").arg( it.data()->number() ) + " {\n";
            outStream << "\t# Automatically bind the device at startup\n";
            outStream << "\tbind " << ((it.data()->isBind())? "yes": "no") << ";\n";
            outStream << "\n";
            outStream << "\t# Bluetooth address of the device\n";
            outStream << "\tdevice " + it.data()->mac() + ";\n";
            outStream << "\n";
            outStream << "\t# RFCOMM channel for the connection\n";
            outStream << "\tchannel\t" + QString( "%1" ).arg( it.data()->channel() ) + ";\n";
            outStream << "\n";
            outStream << "\t# Description of the connection\n";
            outStream << "\tcomment \"" + it.data()->comment() + "\";\n";
            outStream << "}\n\n";
        }
        rfCommConf.close();
    }
}


QMap<QString, RfCommConfObject*> RfCommConfHandler::foundEntries()  {
    return m_foundEntries;
}

void RfCommConfHandler::load()  {
    //Keywords
    QCString k_rfcomm("rfcomm");
    QCString k_device("device ");
    QCString k_channel("channel ");
    QCString k_comment("comment ");
    QCString k_bind("bind ");

    m_foundEntries.clear();
    QFile rfCommConf(m_filename); //File we read
    if (rfCommConf.open(IO_ReadOnly))  {

        QStringList list;
        QTextStream inStream( &rfCommConf );
        list = QStringList::split( "\n", inStream.read() );

        QString mac;
        QString channel;
        QString comment;
        QString bind;
        bool bbind;
        QString number;

        for (QStringList::Iterator line = list.begin(); 
            line != list.end(); line++)  {

            QString tmpLine = (*line).simplifyWhiteSpace();

            if (tmpLine.startsWith(k_rfcomm))  {
                number = tmpLine.mid( k_rfcomm.length(), 1 );
                odebug << tmpLine << oendl;
                odebug << "device " << number << oendl;
            } else if ( tmpLine.startsWith( "}" ) ) {
                m_foundEntries.insert(number, 
                    new RfCommConfObject(number.toInt(), mac, channel.toInt(), 
                    comment, bbind));
            } else if ( tmpLine.startsWith(k_device) )  {
                mac = tmpLine.mid(k_device.length(), 
                    tmpLine.find(';') - k_device.length());
                odebug << "mac " + mac << oendl;
            } else if ( tmpLine.startsWith(k_channel) ) {
                channel = tmpLine.mid(k_channel.length(), 
                    tmpLine.find(';') - k_channel.length());
                odebug << "Channel: " << channel << oendl;
            } else if ( tmpLine.startsWith(k_comment) ) {
                comment = tmpLine.mid(k_comment.length(), 
                    tmpLine.find(';') - k_comment.length());
                if (comment.left(1) == "\"") 
                    comment.remove(0, 1);
                if (comment.right(1) == "\"") 
                    comment.remove(comment.length() - 1, 1);
                odebug << "Comment: " + comment << oendl;
            } else if ( tmpLine.startsWith(k_bind) ) {
                bind = tmpLine.mid(k_bind.length(), 
                    tmpLine.find(';') - k_bind.length());
                if (bind == "no")
                    bbind = false;
                else if (bind == "yes")
                    bbind = true;
                else
                    bbind = true;
                odebug << "bind: " + bind << oendl;
            }
        }
        rfCommConf.close();
    }
    save(m_foundEntries);
    odebug << QString( "ENTries: %1").arg( m_foundEntries.count() ) << oendl;
}

//eof
