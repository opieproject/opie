#include <qlineedit.h>
#include <qprogressbar.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qcheckbox.h>
#include <GUIUtils.h>
#include <resources.h>
#include <wextensions.h>

#include "wlanedit.h"
#include "wlan_NN.h"
#include "wlan_NNI.h"

WLanEdit::WLanEdit( QWidget * Parent, ANetNodeInstance * TNNI ) : 
      WLanGUI( Parent ), RefreshTimer(this){

      InterfaceInfo * II;

      NNI = TNNI;
      Dev = NNI->runtime()->device();
      WE = 0;
      if( ( II = Dev->assignedInterface() ) ) {
        // show data
        WE = new WExtensions( II->Name );

        if( WE->doesHaveWirelessExtensions() ) {
          QString S;
          Station_LBL->setText( WE->station() );
          ESSID_LBL->setText( WE->essid() );
          Mode_LBL->setText( WE->mode() );
          S.setNum( WE->frequency() );
          Frequency_LBL->setText( S );
          S.setNum( WE->channel() );
          Channel_LBL->setText( S );
          S.setNum( WE->rate() );
          Rate_LBL->setText( S );
          AP_LBL->setText( WE->ap() );

          SLOT_Refresh();

          connect( &RefreshTimer, SIGNAL( timeout() ),
                 this, SLOT( SLOT_Refresh() ) );
        }
      }
}

WLanEdit::~WLanEdit( void ) { 
      if( WE )
        delete WE;
}

QString WLanEdit::acceptable( void ) {
    if( ESSID_LE->text().isEmpty() ) {
      return QString("ESSID is mandatory");
    }
    if( SpecifyAP_CB->isChecked() &&
        APMac_LE->text().isEmpty() ) {
      return QString("APMac must have value");
    }
    return QString();
}

void WLanEdit::showData( WLanData_t & Data ) {
    Mode_CB->setCurrentItem( Data.Mode );
    ESSID_LE->setText( Data.ESSID );
    NodeName_LE->setText( Data.NodeName );
    SpecifyAP_CB->setChecked( Data.SpecificAP );
    APMac_LE->setText( Data.APMac );

    EnableWEP_CB->setChecked( Data.Encrypted );
    AcceptNonEncrypted_CB->setChecked( Data.AcceptNonEncrypted );
    Key1_LE->setText( Data.Key[0] );
    Key2_LE->setText( Data.Key[1] );
    Key3_LE->setText( Data.Key[2] );
    Key4_LE->setText( Data.Key[3] );
}

bool WLanEdit::commit( WLanData_t & Data ) {
    bool SM = 0;

    TXTM( Data.ESSID, ESSID_LE, SM );
    TXTM( Data.NodeName, NodeName_LE, SM );
    TXTM( Data.APMac, APMac_LE, SM );
    TXTM( Data.Key[0], Key1_LE, SM );
    TXTM( Data.Key[1], Key2_LE, SM );
    TXTM( Data.Key[2], Key3_LE, SM );
    TXTM( Data.Key[3], Key4_LE, SM );
    CBM( Data.SpecificAP, SpecifyAP_CB, SM );
    CBM( Data.Encrypted, EnableWEP_CB, SM );
    CBM( Data.AcceptNonEncrypted, AcceptNonEncrypted_CB, SM );
    CIM( Data.Mode, Mode_CB, SM );
    return 0;
}

void WLanEdit::SLOT_Refresh( void ) {
  if( WE ) {
    int signal, noise, quality;
    WE->stats( signal, noise, quality);

    Signal_PB->setProgress( signal );
    Noise_PB->setProgress( noise );
    Quality_PB->setProgress( quality );
  }
}

void WLanEdit::SLOT_AutoRefresh( bool ar ) {
    if( ar ) {
      RefreshTimer.start( 1000 );
      SLOT_Refresh();
    } else {
      RefreshTimer.stop();
    }
}

