#include <stdio.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <GUIUtils.h>
#include "PPPAuthedit.h"

PPPAuthEdit::PPPAuthEdit( QWidget * Parent ) : PPPAuthGUI( Parent ){
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
    } else if( PapChap_RB->isChecked() ) {
      if( Client_LE->text().isEmpty() )
        return tr("Pap/Chap/EAP client id missing");
      if( Server_LE->text().isEmpty() )
        return tr("Pap/Chap/EAP server id missing");
      if( Secret_LE->text().isEmpty() )
        return tr("Pap/Chap/EAP secret id missing");
    }
    return QString();
}

bool PPPAuthEdit::commit( PPPData_t & D ) {
    bool SM = 0;

    if( ( D.Auth.Mode == 0 && ! Login_RB->isChecked() ) ||
        ( D.Auth.Mode == 1 && ! PapChap_RB->isChecked() ) ||
        ( D.Auth.Mode == 2 && ! Terminal_RB->isChecked() ) ) {
      // mode modifed
      SM = 1;
      D.Auth.Mode = ( Login_RB->isChecked() ) ? 
                    0 :
                    ( ( PapChap_RB->isChecked() ) ? 
                      1 : 2 );
    }

    if( Login_RB->isChecked() ) {
      TXTM( D.Auth.Login.Expect, LoginExpect_LE, SM );
      TXTM( D.Auth.Login.Send, LoginSend_LE, SM );
      TXTM( D.Auth.Password.Expect, PasswordExpect_LE, SM );
      TXTM( D.Auth.Password.Send, PasswordSend_LE, SM );
    } else if( PapChap_RB->isChecked() ) {
      TXTM( D.Auth.Client, Client_LE, SM );
      TXTM( D.Auth.Server, Server_LE, SM );
      TXTM( D.Auth.Secret, Secret_LE, SM );
      CIM( D.Auth.PCEMode, AuthMethod_CB, SM );
    }
    return SM;
}

void PPPAuthEdit::showData( PPPData_t & D ) {

    switch( D.Auth.Mode ) {
      case 0 : 
        Login_RB->isChecked();
        break;
      case 1 : 
        PapChap_RB->isChecked();
        break;
      case 2 : 
        Terminal_RB->isChecked();
        break;
    }

    LoginExpect_LE->setText( D.Auth.Login.Expect );
    PasswordExpect_LE->setText( D.Auth.Password.Expect );
    LoginSend_LE->setText( D.Auth.Login.Send );
    PasswordSend_LE->setText( D.Auth.Password.Send );

    Client_LE->setText( D.Auth.Client );
    Server_LE->setText( D.Auth.Server );
    Secret_LE->setText( D.Auth.Secret );

    AuthMethod_CB->setCurrentItem( D.Auth.PCEMode );
}
