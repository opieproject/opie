#ifndef OPIEMAIL_H
#define OPIEMAIL_H

#include "mainwindow.h"
#include <libmailwrapper/settings.h>


class OpieMail : public MainWindow
{
    Q_OBJECT

public:
    OpieMail( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    static QString appName() { return QString::fromLatin1("opiemail"); }

public slots:
    virtual void slotwriteMail(const QString&name,const QString&email);
    virtual void slotComposeMail();
    virtual void appMessage(const QCString &msg, const QByteArray &data);
protected slots:
    virtual void slotSendQueued();
    virtual void slotSearchMails();
    virtual void slotEditSettings();
    virtual void slotEditAccounts();
private:
    Settings *settings;

};

#endif
