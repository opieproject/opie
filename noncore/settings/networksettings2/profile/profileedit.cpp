#include <qlabel.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qmultilineedit.h>
#include <qcheckbox.h>

#include <GUIUtils.h>
#include <asdevice.h>
#include <resources.h>

#include "profileedit.h"

ProfileEdit::ProfileEdit( QWidget * Parent, ANetNodeInstance * TNNI ) : 
      ProfileGUI( Parent ), RefreshTimer(this) {
      InterfaceInfo * II;

      NNI = TNNI;
      Dev = NNI->runtime()->device();
      if( ( II = Dev->assignedInterface() ) ) {

        Refresh_CB->setEnabled( TRUE );
        Snd_GB->setEnabled( TRUE );
        Rcv_GB->setEnabled( TRUE );
        Collisions_FRM->setEnabled( TRUE );

        // show current content
        SLOT_Refresh();

        // fill in static data
        InterfaceName_LBL->setText( II->Name );
        IPAddress_LBL->setText( II->Address );
        SubnetMask_LBL->setText( II->Netmask );
        Broadcast_LBL->setText( II->BCastAddress );
        MACAddress_LBL->setText( II->MACAddress );
        if( II->IsPointToPoint ) {
          PointToPoint_LBL->setText( II->DstAddress );
        }
        QString S;
        InterfaceName_LBL->setText( II->Name );
        if( II->HasMulticast ) {
          S += "Multicast";
        }
        if( ! S.isEmpty() ) {
          S.prepend( " : " );
        }
        InterfaceOptions_LBL->setText( S  );

        connect( &RefreshTimer, SIGNAL( timeout() ),
                 this, SLOT( SLOT_Refresh() ) );
      }

}

QString ProfileEdit::acceptable( void ) {
    return QString();
}

void ProfileEdit::showData( ProfileData_t & Data ) {
    Description_LE->setText( Data.Description );
    Automatic_CB->setChecked( Data.Automatic );
    Confirm_CB->setChecked( Data.Confirm );
    Disabled_CB->setChecked( Data.Disabled );
}


bool ProfileEdit::commit( ProfileData_t & Data ) {
    bool SM = 0;
    TXTM( Data.Description, Description_LE, SM );

    CBM( Data.Automatic, Automatic_CB, SM );
    CBM( Data.Disabled, Disabled_CB, SM );
    CBM( Data.Confirm, Confirm_CB, SM );

    return SM;
}

void ProfileEdit::SLOT_Refresh( void ) {
    InterfaceInfo * II = Dev->assignedInterface();
    NSResources->system().refreshStatistics( *II );
    RcvBytes_LBL->setText( II->RcvBytes );
    SndBytes_LBL->setText( II->SndBytes );
    RcvErrors_LBL->setText( II->RcvErrors );
    SndErrors_LBL->setText( II->SndErrors );
    RcvDropped_LBL->setText( II->RcvDropped );
    SndDropped_LBL->setText( II->SndDropped );
    Collisions_LBL->setText( II->Collisions );
}

void ProfileEdit::SLOT_AutoRefresh( bool ar ) {
    if( ar ) {
      RefreshTimer.start( 1000 );
      SLOT_Refresh();
    } else {
      RefreshTimer.stop();
    }
}
