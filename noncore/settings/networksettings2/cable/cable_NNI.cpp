#include "cableedit.h"
#include "cable_NNI.h"
#include "cable_NN.h"

ACable::ACable( CableNetNode * PNN ) : ANetNodeInstance( PNN ) {
    // default values
    Data.Device = "/dev/ttyS0";
    Data.LockFile = "/var/Lock";
    Data.Speed = 115200;
    Data.DataBits = 8;
    Data.Parity = 0;
    Data.StopBits = 1;
    Data.HardwareControl = 0;
    Data.SoftwareControl = 0;
    GUI = 0;
    RT = 0;
}

void ACable::setSpecificAttribute( QString & A, QString & V) {
    if( A == "device" ) {
      Data.Device = V;
    } else if ( A == "lockfile" ) {
      Data.LockFile = V;
    } else if ( A == "speed" ) {
      Data.Speed = V.toLong();
    } else if ( A == "parity" ) {
      Data.Parity = V.toShort();
    } else if ( A == "databits" ) {
      Data.DataBits = V.toShort();
    } else if ( A == "stopbits" ) {
      Data.StopBits = V.toShort();
    } else if ( A == "hardwarecontrol" ) {
      Data.HardwareControl = ( V == "yes" );
    } else if ( A == "softwarecontrol" ) {
      Data.SoftwareControl = ( V == "yes" );
    }
}

void ACable::saveSpecificAttribute( QTextStream & TS ) {
    TS << "device=" << quote( Data.Device ) << endl;
    TS << "lockfile=" << quote( Data.LockFile ) << endl;
    TS << "speed=" << Data.Speed << endl;
    TS << "parity=" << Data.Parity << endl;
    TS << "databits=" << Data.DataBits << endl;
    TS << "stopbits=" << Data.StopBits << endl;
    TS << "hardwarecontrol=" << 
      ((Data.HardwareControl) ? "yes" : "no") << endl;
    TS << "softwarecontrol=" << 
      ((Data.SoftwareControl) ? "yes" : "no") << endl;
}

QWidget * ACable::edit( QWidget * parent ) {
    GUI = new CableEdit( parent );
    GUI->showData( Data ); 
    return GUI;
}

QString ACable::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void ACable::commit( void ) {
    if( GUI && GUI->commit( Data ) ) {
      setModified( 1 );
    }
}
