#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <GUIUtils.h>
#include "PPPIPedit.h"

PPPIPEdit::PPPIPEdit( QWidget * Parent ) : PPPIPGUI( Parent ){
}

QString PPPIPEdit::acceptable( void ) {
    if( GWFixed_RB->isChecked() ) {
      if( GWAddress_LE->text().isEmpty() )
        return tr("Gateway address needed" );
      if( ! validIP( GWAddress_LE->text() ) )
        return tr("Gateway address not valid" );
    };

    return QString();
}

bool PPPIPEdit::commit( PPPData & D ) {

      bool SM = 0;

      CBM( D.IP.LocalOverrule, ServerOverrulesLocal_CB, SM );
      CBM( D.IP.RemoteOverrule, ServerOverrulesRemote_CB, SM );

      TXTM( D.IP.LocalAddress, LocalAddress_LE, SM );
      TXTM( D.IP.RemoteAddress, RemoteAddress_LE, SM );

      CBM( D.IP.GWAutomatic, GWServerAssigned_RB, SM );

      if( ! D.IP.GWAutomatic ) {
        TXTM( D.IP.GWAddress, GWAddress_LE, SM );
      }

      CBM( D.IP.GWIsDefault, GWIsDefault_CB, SM );
      CBM( D.IP.GWIfNotSet, GWIfNotSet_CB, SM );

      return SM;
}

void PPPIPEdit::showData( PPPData & D ) {
      ServerOverrulesLocal_CB->setChecked( D.IP.LocalOverrule );
      ServerOverrulesRemote_CB->setChecked( D.IP.RemoteOverrule );
      LocalAddress_LE->setText( D.IP.LocalAddress );
      RemoteAddress_LE->setText( D.IP.RemoteAddress );

      GWServerAssigned_RB->setChecked( D.IP.GWAutomatic );
      GWAddress_LE->setText( D.IP.GWAddress );
      GWIsDefault_CB->setChecked( D.IP.GWIsDefault );
      GWIfNotSet_CB->setChecked( D.IP.GWIfNotSet );
}
