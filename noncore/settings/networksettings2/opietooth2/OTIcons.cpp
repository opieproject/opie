#include <OTIcons.h>

#include <opie2/odebug.h>
#include <qpe/resource.h>

using namespace Opietooth2;

OTIcons::OTIcons() : deviceIcons(), serviceIcons() {

    // still need to find out real ids
    deviceIcons.insert( "unknown",    "unknown_16" );
    deviceIcons.insert( "misc",       "misc_16" );
    deviceIcons.insert( "computer",   "computer_16" );
    deviceIcons.insert( "phone",      "phone_16" );
    deviceIcons.insert( "lan",        "network_16" );
    deviceIcons.insert( "audiovideo", "audio_16" );
    deviceIcons.insert( "peripheral", "print_16" );
    deviceIcons.insert( "imaging",    "other_16" );

    serviceIcons.insert( 0x1101 , "serial_16" ); //SerialPort
    serviceIcons.insert( 0x1102 , "network_16" ); //LANAccessUsingPPP
    serviceIcons.insert( 0x1103 , "network_16"); //DialupNetworking
    serviceIcons.insert( 0x1104 , "sync_16" ); //IrMCSync
    serviceIcons.insert( 0x1105 , "obex_16" ); //OBEXObjectPush
    serviceIcons.insert( 0x1106 , "obex_16" ); //OBEXFileTransfer
    serviceIcons.insert( 0x1107 , "sync_16" ); //IrMCSyncCommand
    serviceIcons.insert( 0x1108 , "phone_16"); // Headset
    serviceIcons.insert( 0x1109 , "phone_16"); // CordlessTelephony
    serviceIcons.insert( 0x110A , "audio_16"); // AudioSource
    serviceIcons.insert( 0x110B , "audio_16"); // AudioSink
    //serviceIcons.insert( 0x110C , "remotecontrol_16" ); // remote control target ---
    //serviceIcons.insert( 0x110D , "audio_16" ); // advanced audio distribution ---
    //serviceIcons.insert( 0x110E , "remotecontrol_16" ); // remote control ---
    //serviceIcons.insert( 0x110F , "video_16" ); // video conferencing ---
    //serviceIcons.insert( 0x1110 , "audio_16" ); // intercom ---
    serviceIcons.insert( 0x1111 , "fax_16" ); //Fax
    serviceIcons.insert( 0x1112 , "audio_16");  //HeadsetAudioGateway
    //serviceIcons.insert( 0x1113 , "wap_16" ); // WAP ---
    //serviceIcons.insert( 0x1114 , "wap_16" ); // WAP client ---
    serviceIcons.insert( 0x1115 , "network_16"); // Network Access Point (PANU)
    serviceIcons.insert( 0x1116 , "network_16"); // Network Access Point (NAP)
    serviceIcons.insert( 0x1117 , "network_16"); // Network Access Point (GN)
    serviceIcons.insert( 0x1118 , "print_16" ); // direct printing
    serviceIcons.insert( 0x1119 , "print_16" ); // reference printing
    //serviceIcons.insert( 0x111A , "image_16" ); // imaging ---
    //serviceIcons.insert( 0x111B , "image_16" ); // imaging responder ---
    //serviceIcons.insert( 0x111C , "image_16" ); // imaging automatic archive  ---
    //serviceIcons.insert( 0x111D , "image_16" ); // imaging referenced objects ---
    //serviceIcons.insert( 0x111E , "handsfree_16" ); // handsfree ---
    //serviceIcons.insert( 0x111F , "handsfree_16" ); // handsfree audio gateway ---
    serviceIcons.insert( 0x1120 , "print_16" ); // direct printing referenced object service
    //serviceIcons.insert( 0x1121 , "handsfree_16" ); // reflected UI ---
    serviceIcons.insert( 0x1122 , "print_16" ); // basic printing
    serviceIcons.insert( 0x1123 , "print_16" ); // printing status
    //serviceIcons.insert( 0x1124 , "handsfree_16" ); // human interface device service ---
    //serviceIcons.insert( 0x1125 , "handsfree_16" ); // hardcopy cable replacement ---
    serviceIcons.insert( 0x1126 , "print_16" ); // HCR print
    serviceIcons.insert( 0x1127 , "print_16" ); // HCR scan
    serviceIcons.insert( 0x1128 , "phone_16" ); // Common_ISDN_Access
    //serviceIcons.insert( 0x1129 , "audio_16" ); // video conferencing GW ---
    //serviceIcons.insert( 0x112C , "audio_16" ); // audio video ---
    //serviceIcons.insert( 0x112D , "phone_16" ); // SIM access ---
    serviceIcons.insert( 0x1201 , "network_16" );  //GenericNetworking
    serviceIcons.insert( 0x1202 , "folder_16" ); //GenericFileTransfer
    serviceIcons.insert( 0x1203 , "audio_16" ); //GenericAudio
    serviceIcons.insert( 0x1204 , "phone_16" ); //GenericTelephony
    //serviceIcons.insert( 0x1303 , "video_16" ); // video source ---
    //serviceIcons.insert( 0x1304 , "video_16" ); // video sink ---

    Modems.resize( 3 );
    Modems[0].setUUID32( 0x1101 ); // SerialPort
    Modems[1].setUUID32( 0x1102 ); // Lan access using PPP
    Modems[2].setUUID32( 0x1103 ); // DialupNetworking

    Networks.resize( 3 );
    Networks[0].setUUID32( 0x1115 ); // PANU
    Networks[1].setUUID32( 0x1116 ); // NAP
    Networks[2].setUUID32( 0x1117 ); // GN
}

OTIcons::~OTIcons() {
}

QPixmap OTIcons::deviceIcon( const QString & deviceClass ) {

    QString iconName;

    QMap<QString, QString>::Iterator it;

    it = deviceIcons.find( deviceClass );
    iconName = it.data();

    if ( iconName.isEmpty() ) {
        iconName = "unknown_16";
    }
    return loadPixmap( iconName, 1 );
}

QPixmap OTIcons::serviceIcon( int serviceClass, bool & found ) {

    QString iconName;

    QMap<int, QString>::Iterator it;

    it = serviceIcons.find( serviceClass );
    iconName = it.data();

    if ( iconName.isEmpty() ) {
      iconName = "unknown_16";
      found = 0 ;
    } else
      found = 1 ;
    return loadPixmap( iconName, 1 );
}

QPixmap OTIcons::loadPixmap( const QString & name, bool Sub ) {
    return( Resource::loadPixmap( "opietooth/" +
                                  QString( (Sub) ? "icons/" : "" ) +
                                  name ) );
}
