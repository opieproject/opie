#ifndef _AUTHWIDGET_H
#define _AUTHWIDGET_H

#include <qwidget.h>

class ScriptWidget;
class PPPData;
class QCheckBox;
class QComboBox;
class QLabel;
class QGridLayout;
class QLineEdit;
class QToolButton;

class AuthWidget : public QWidget {
    Q_OBJECT
public:
    AuthWidget(PPPData*, QWidget *parent=0, bool isnewaccount = true, const char *name=0 );
    ~AuthWidget() {};

public slots:
    bool check();
    void save();

private slots:
    void authChanged(const QString&);
    void showUsernamePassword(bool); 
    void showScriptWindow(bool);
    void toggleEchoMode(bool);

private:
    ScriptWidget *scriptWidget;
    PPPData *_pppdata;
    bool isNewAccount;
    QGridLayout *layout;
    QComboBox *auth;
    QLabel *auth_l;
    QLabel *user_l;
    QLineEdit *userName;
    QLabel *pw_l;
    QLineEdit *passWord;
    QToolButton *hidePw;
    QCheckBox *store_password;
};

#endif
