
#include "bticonloader.h"

#include <qpixmap.h>
#include <qpe/resource.h>

namespace OpieTooth {


    BTIconLoader::BTIconLoader() {

        // still need to find out real ids
        deviceIcons.insert( 100 , "computer_16" );
        deviceIcons.insert( 101 , "phone_16" );
        deviceIcons.insert( 102 , "brain_16" );
        deviceIcons.insert( 103 , "conduit_16" );


        serviceIcons.insert( 4357 , "obex_16" ); //OBEXObjectPush
        serviceIcons.insert( 4358 , "obex_16" ); //OBEXFileTransfer
        serviceIcons.insert( 4369 , "print_16" ); //Fax
        serviceIcons.insert( 4353 , "serial_16" ); //SerialPort
        serviceIcons.insert( 4356 , "sync_16" ); //IrMCSync
        serviceIcons.insert( 4359 , "sync_16" ); //IrMCSyncCommand
        serviceIcons.insert( 4354 , "network_16" ); //LANAccessUsingPPP
        serviceIcons.insert( 4355 , "network_16"); //DialupNetworking
        serviceIcons.insert( 4360 , "phone_16"); // Headset
        serviceIcons.insert( 4370 , "audio_16");  //HeadsetAudioGateway
        serviceIcons.insert( 4361 , "phone_16"); // CordlessTelephony
        serviceIcons.insert( 4362 , "audio_16"); // AudioSource
        serviceIcons.insert( 4363 , "audio_16"); // AudioSink
        serviceIcons.insert( 4390 , "print_16" ); //HCR_Print
        serviceIcons.insert( 4392 , "phone_16" ); //Common_ISDN_Access

        serviceIcons.insert( 4609 , "network_16" );  //GenericNetworking
        serviceIcons.insert( 4610 , "folder_16" ); //GenericFileTransfer
        serviceIcons.insert( 4392 , "audio_16" ); //GenericAudio
        //  serviceIcons.insert( 4392 , "phone_16" ); //GenericTelephony
        // the above is duplicated? -zecke

        //serviceIcons.insert( "106" , "link_16" );
        //serviceIcons.insert( "107" , "misc_16" );
    }

    BTIconLoader::~BTIconLoader() {
    }

    QPixmap BTIconLoader::deviceIcon( int deviceClass ) {

        QString iconName;

        QMap<int, QString>::Iterator it;

        it = deviceIcons.find( deviceClass );
        iconName = it.data();

        if ( iconName.isEmpty() ) {
            iconName = "unknown_16";
        }
        return( Resource::loadPixmap( "opietooth/icons/" + iconName ) );
    }

    QPixmap BTIconLoader::serviceIcon( int serviceClass ) {

        QString iconName;

        QMap<int, QString>::Iterator it;

        it = deviceIcons.find( serviceClass );
        iconName = it.data();

        if ( iconName.isEmpty() ) {
            iconName = "unknown_16";
        }
        return( Resource::loadPixmap( "opietooth/icons/" + iconName ) );
    }

}
