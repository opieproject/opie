
#include "bticonloader.h"

#include <qpixmap.h>
#include <qpe/resource.h>

namespace OpieTooth {


    BTIconLoader::BTIconLoader() {

        // still need to find out real ids
        deviceIcons.insert( "100" , "computer_16" );
        deviceIcons.insert( "101" , "phone_16" );
        deviceIcons.insert( "102" , "brain_16" );
        deviceIcons.insert( "103" , "conduit_16" );


        serviceIcons.insert( "0x1105" , "obex_16" ); //OBEXObjectPush
        serviceIcons.insert( "0x1106" , "obex_16" ); //OBEXFileTransfer
        serviceIcons.insert( "0x1111" , "print_16" ); //Fax
        serviceIcons.insert( "0x1101" , "serial_16" ); //SerialPort
        serviceIcons.insert( "0x1104" , "sync_16" ); //IrMCSync
        serviceIcons.insert( "0x1107" , "sync_16" ); //IrMCSyncCommand
        serviceIcons.insert( "0x1102" , "network_16" ); //LANAccessUsingPPP
        serviceIcons.insert( "0x1103" , "network_16"); //DialupNetworking
        serviceIcons.insert( "0x1108" , "phone_16"); // Headset
        serviceIcons.insert( "0x1112" , "audio_16");  //HeadsetAudioGateway
        serviceIcons.insert( "0x1109" , "phone_16"); // CordlessTelephony
        serviceIcons.insert( "0x110A" , "audio_16"); // AudioSource
        serviceIcons.insert( "0x110B" , "audio_16"); // AudioSink
        serviceIcons.insert( "0x1126" , "print_16" ); //HCR_Print
        serviceIcons.insert( "0x1128" , "phone_16" ); //Common_ISDN_Access

        serviceIcons.insert( "0x1201" , "network_16" );  //GenericNetworking
        serviceIcons.insert( "0x1202" , "folder_16" ); //GenericFileTransfer
        serviceIcons.insert( "0x1128" , "audio_16" ); //GenericAudio
        serviceIcons.insert( "0x1128" , "phone_16" ); //GenericTelephony

        //serviceIcons.insert( "106" , "link_16" );
        //serviceIcons.insert( "107" , "misc_16" );
    }

    BTIconLoader::~BTIconLoader() {
    }

    QPixmap BTIconLoader::deviceIcon( const QString &deviceClass ) {

        QString iconName;

        QMap<QString, QString>::Iterator it;

        it = deviceIcons.find( deviceClass );
        iconName = it.data();

        if ( iconName.isEmpty() ) {
            iconName = "unknown_16";
        }
        return( Resource::loadPixmap( "opietooth/icons/" + iconName ) );
    }

    QPixmap BTIconLoader::serviceIcon( const QString &serviceClass ) {

        QString iconName;

        QMap<QString, QString>::Iterator it;

        it = deviceIcons.find( serviceClass );
        iconName = it.data();

        if ( iconName.isEmpty() ) {
            iconName = "unknown_16";
        }
        return( Resource::loadPixmap( "opietooth/icons/" + iconName ) );
    }

}
