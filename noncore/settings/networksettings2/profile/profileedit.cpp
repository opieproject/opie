#include <qlabel.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qmultilineedit.h>
#include <qcheckbox.h>

#include <GUIUtils.h>
#include <netnode.h>
#include <resources.h>

#include "profileedit.h"

ProfileEdit::ProfileEdit( QWidget * Parent, ANetNodeInstance * TNNI ) : 
      ProfileGUI( Parent ), RefreshTimer(this) {
      InterfaceInfo * II;

      II = TNNI->networkSetup()->assignedInterface();
      Log(( "Interface %p %p %p: %d\n", II, 
            TNNI, TNNI->networkSetup(), (II) ? II->IsUp : 0 ));

      NNI = TNNI;
      Dev = NNI->runtime()->device();
      if( ( II = NNI->networkSetup()->assignedInterface() ) ) {

        Refresh_CB->setEnabled( TRUE );
        ResetODO_But->setEnabled( TRUE );
        Sending_GB->setEnabled( TRUE );
        Receiving_GB->setEnabled( TRUE );
        Misc_GB->setEnabled( TRUE );

        // show current content
        SLOT_Refresh();

        // initialize ODO
        SLOT_ResetODO();

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

void ProfileEdit::showData( ProfileData & Data ) {
    Description_LE->setText( Data.Description );
    Automatic_CB->setChecked( Data.Automatic );
    TriggersVPN_CB->setChecked( Data.TriggerVPN );
    Confirm_CB->setChecked( Data.Confirm );
    Enabled_CB->setChecked( Data.Enabled );
}


bool ProfileEdit::commit( ProfileData & Data ) {
    bool SM = 0;
    TXTM( Data.Description, Description_LE, SM );

    CBM( Data.Automatic, Automatic_CB, SM );
    CBM( Data.TriggerVPN, TriggersVPN_CB, SM );
    CBM( Data.Enabled, Enabled_CB, SM );
    CBM( Data.Confirm, Confirm_CB, SM );

    return SM;
}

void ProfileEdit::SLOT_Refresh( void ) {
    InterfaceInfo * II = NNI->networkSetup()->assignedInterface();
    QString S;
    NSResources->system().refreshStatistics( *II );

    RcvBytes_LBL->setText( II->RcvBytes );
    RcvPackets_LBL->setText( II->RcvPackets );
    RcvErrors_LBL->setText( II->RcvErrors );
    RcvDropped_LBL->setText( II->RcvDropped );
    S.setNum( II->RcvBytes.toLong() - RcvODO );
    RcvODO_LBL->setText( S );

    SndBytes_LBL->setText( II->SndBytes );
    SndPackets_LBL->setText( II->SndPackets );
    SndErrors_LBL->setText( II->SndErrors );
    SndDropped_LBL->setText( II->SndDropped );
    S.setNum( II->SndBytes.toLong() - SndODO );
    SndODO_LBL->setText( S );

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

void ProfileEdit::SLOT_ResetODO( void ) {
    InterfaceInfo * II = NNI->networkSetup()->assignedInterface();
    RcvODO = II->RcvBytes.toLong();
    SndODO = II->SndBytes.toLong();
    SLOT_Refresh();
}
