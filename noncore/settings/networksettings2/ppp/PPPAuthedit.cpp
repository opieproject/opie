#include <stdio.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <GUIUtils.h>
#include "PPPAuthedit.h"

PPPAuthEdit::PPPAuthEdit( QWidget * Parent ) : PPPAuthGUI( Parent ){
}

bool PPPAuthEdit::PAP_Checked( void ) {
      return ( Pap_RB->isChecked() ||
               Chap_RB->isChecked() ||
               EAP_RB->isChecked()
             );
}

QString PPPAuthEdit::acceptable( void ) {
    if( Login_RB->isChecked() ) {
      if( LoginSend_LE->text().isEmpty() )
        return tr("Login send missing");
      if( LoginExpect_LE->text().isEmpty() )
        return tr("Login expect missing");
      if( PasswordSend_LE->text().isEmpty() )
        return tr("Password send missing");
      if( PasswordExpect_LE->text().isEmpty() )
        return tr("Password expect missing");
    } else if( PAP_Checked() ) {
      if( Client_LE->text().isEmpty() )
        return tr("Pap/Chap/EAP client id missing");
      if( Server_LE->text().isEmpty() )
        return tr("Pap/Chap/EAP server id missing");
      if( Secret_LE->text().isEmpty() )
        return tr("Pap/Chap/EAP secret id missing");
    }
    return QString();
}

bool PPPAuthEdit::commit( PPPData & D ) {
    bool SM = 0;

    if( ( D.Auth.Mode == 0 && ! Login_RB->isChecked() ) ||
        ( D.Auth.Mode == 1 && ! PAP_Checked() ) ||
        ( D.Auth.Mode == 2 && ! Terminal_RB->isChecked() ) ) {
      // mode modifed
      SM = 1;
      D.Auth.Mode = ( Login_RB->isChecked() ) ?
                    0 :
                    ( PAP_Checked() ) ? 1 : 2;
    }

    if( Login_RB->isChecked() ) {
      TXTM( D.Auth.Login.Expect, LoginExpect_LE, SM );
      TXTM( D.Auth.Login.Send, LoginSend_LE, SM );
      TXTM( D.Auth.Password.Expect, PasswordExpect_LE, SM );
      TXTM( D.Auth.Password.Send, PasswordSend_LE, SM );
    } else if( PAP_Checked() ) {
      TXTM( D.Auth.Client, Client_LE, SM );
      TXTM( D.Auth.Server, Server_LE, SM );
      TXTM( D.Auth.Secret, Secret_LE, SM );
      if( Pap_RB->isChecked() ) {
        D.Auth.PCEMode = 0;
      } else if( Chap_RB->isChecked() ) {
        D.Auth.PCEMode = 1;
      } else if( EAP_RB->isChecked() ) {
        D.Auth.PCEMode = 2;
      }
    }
    return SM;
}

void PPPAuthEdit::showData( PPPData & D ) {

    switch( D.Auth.Mode ) {
      case 0 :
        Login_RB->setChecked( TRUE );
        break;
      case 1 :
        switch( D.Auth.PCEMode ) {
          case 0 :
            Pap_RB->setChecked( TRUE );
            break;
          case 1 :
            Chap_RB->setChecked( TRUE );
            break;
          case 2 :
            EAP_RB->setChecked( TRUE );
            break;
        }
        break;
      case 2 :
        Terminal_RB->setChecked( TRUE );
        break;
    }

    LoginExpect_LE->setText( D.Auth.Login.Expect );
    PasswordExpect_LE->setText( D.Auth.Password.Expect );
    LoginSend_LE->setText( D.Auth.Login.Send );
    PasswordSend_LE->setText( D.Auth.Password.Send );

    Client_LE->setText( D.Auth.Client );
    Server_LE->setText( D.Auth.Server );
    Secret_LE->setText( D.Auth.Secret );
}
