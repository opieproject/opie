#include <qlineedit.h>

#include "logindialog.h"

LoginDialog::LoginDialog( QString user, QString pass, QWidget *parent, const char *name, bool modal, WFlags flags )
    : LoginDialogUI( parent, name, modal, flags )
{
    userLine->setText( user );
    passLine->setText( pass );
    _user = user;
    _pass = pass;

    if ( user.isEmpty() ) {
        userLine->setFocus();
    } else {
        passLine->setFocus();
    }
}

void LoginDialog::accept()
{
    _user.replace( 0, _user.length(), userLine->text() );
    _pass.replace( 0, _pass.length(), passLine->text() );

    QDialog::accept();
}
