#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "logindialogui.h"

class LoginDialog : public LoginDialogUI
{
    Q_OBJECT

public:
    LoginDialog(const QString&user,const QString&pass, QWidget *parent = 0, const char *name = 0, bool modal = false, WFlags flags = 0 );
    QString getUser() { return _user; }
    QString getPassword() { return _pass; }

protected slots:
    void accept();

private:
    QString _user, _pass;

};

#endif
