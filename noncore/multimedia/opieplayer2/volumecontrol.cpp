
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include "qpe/qcopenvelope_qws.h"
#include <qmessagebox.h>

#include "volumecontrol.h"

int VolumeControl::volume() {
    Config cfg( "qpe" );
    cfg. setGroup( "Volume" );
    m_volumePerc = cfg. readNumEntry( "VolumePercent", 50 );
    
    return m_volumePerc;
}


void VolumeControl::setVolume( int volumePerc ) {
     Config cfg("qpe");
    cfg.setGroup("Volume");

    if ( volumePerc > 100 ) {
        volumePerc = 100;
    }
    if ( volumePerc < 0 ) {
        volumePerc = 0;
    }

    m_volumePerc = volumePerc;
    cfg.writeEntry("VolumePercent", volumePerc );
    QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << false;
//    QCopEnvelope( "QPE/System", "setVolume(int,int)" ) << 0, volumePerc;
}


void VolumeControl::incVol( int ammount ) {
    int oldVol = volume();
    setVolume( oldVol + ammount);
}

void VolumeControl::decVol( int ammount ) {
     int oldVol = volume();
    setVolume( oldVol - ammount);
}


VolumeControl::VolumeControl( ) {
    volume();
}

VolumeControl::~VolumeControl() {
    QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << false;
}

void VolumeControl::setMute(bool on) {
   QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << on;
}




