#ifndef OPIEMAIL_H
#define OPIEMAIL_H

#include "mainwindow.h"
#include "settings.h"

class OpieMail : public MainWindow
{
    Q_OBJECT

public:
    OpieMail( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    static QString appName() { return QString::fromLatin1("opiemail"); }

public slots:
    void slotComposeMail();
protected slots:
    void slotSendQueued();
    void slotSearchMails();
    void slotEditSettings();
    void slotEditAccounts();
private:
    Settings *settings;

};

#endif
