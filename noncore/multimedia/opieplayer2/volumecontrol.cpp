
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include "qpe/qcopenvelope_qws.h"
#include <qmessagebox.h>

#include "volumecontrol.h"

int VolumeControl::getVolume() {
    int volumePerc;
    Config cfg( "qpe" );
    cfg. setGroup( "Volume" );
    volumePerc = cfg. readNumEntry( "VolumePercent", 50 );
    m_volumePerc = volumePerc;
    return volumePerc;
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
}


void VolumeControl::incVol( int ammount ) {
    int oldVol = getVolume();
    setVolume( oldVol + ammount);
}

void VolumeControl::decVol( int ammount ) {
     int oldVol = getVolume();
    setVolume( oldVol - ammount);
}


VolumeControl::VolumeControl( ) {
    getVolume();
}

VolumeControl::~VolumeControl() {
    QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << false;
}






