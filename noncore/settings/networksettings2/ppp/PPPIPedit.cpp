#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <GUIUtils.h>
#include "PPPIPedit.h"

PPPIPEdit::PPPIPEdit( QWidget * Parent ) : PPPIPGUI( Parent ){
}

QString PPPIPEdit::acceptable( void ) {
    if( IPFixed_RB->isChecked() ) {
      if( IPAddress_LE->text().isEmpty() )
        return tr("IPAddress needed" );
      if( ! validIP( IPAddress_LE->text() ) )
        return tr("IPAddress not valid" );
      if( IPSubMask_LE->text().isEmpty() )
        return tr("Subnet mask needed" );
      if( ! validIP( IPSubMask_LE->text() ) )
        return tr("Subnet mask not valid" );
    } else if( GWFixed_RB->isChecked() ) {
      if( GWAddress_LE->text().isEmpty() )
        return tr("Gateway address needed" );
      if( ! validIP( GWAddress_LE->text() ) )
        return tr("Gateway address not valid" );
    };

    return QString();
}

bool PPPIPEdit::commit( PPPData_t & D ) {

      bool SM = 0;

      CBM( D.IP.IPAutomatic, IPServerAssigned_RB, SM );
      if( ! D.IP.IPAutomatic ) {
        TXTM( D.IP.IPAddress, IPAddress_LE, SM );
        TXTM( D.IP.IPSubMask, IPSubMask_LE, SM );
      }

      CBM( D.IP.GWAutomatic, GWServerAssigned_RB, SM );
      if( ! D.IP.GWAutomatic ) {
        TXTM( D.IP.GWAddress, GWAddress_LE, SM );
      }

      CBM( D.IP.GWIsDefault, GWIsDefault_CB, SM );
      return SM;
}

void PPPIPEdit::showData( PPPData_t & D ) {
      IPServerAssigned_RB->setChecked( D.IP.IPAutomatic );
      IPAddress_LE->setText( D.IP.IPAddress );
      IPSubMask_LE->setText( D.IP.IPSubMask );
      GWServerAssigned_RB->setChecked( D.IP.GWAutomatic );
      GWAddress_LE->setText( D.IP.GWAddress );
      GWIsDefault_CB->setChecked( D.IP.GWIsDefault );
}
