#ifndef OPIEMAIL_H
#define OPIEMAIL_H

#include "mainwindow.h"
#include <libmailwrapper/settings.h>

#include <opie2/osmartpointer.h>
#include <libmailwrapper/mailtypes.h>

class OpieMail : public MainWindow
{
    Q_OBJECT

public:
    OpieMail( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    virtual ~OpieMail();
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
    virtual void displayMail();
    virtual void slotDeleteMail();
    virtual void mailHold(int, QListViewItem *,const QPoint&,int);
    virtual void slotShowFolders( bool show );
    virtual void refreshMailView(const QValueList<RecMailP>&);
    virtual void mailLeftClicked( int, QListViewItem *,const QPoint&,int  );
    virtual void slotMoveCopyMail();
    virtual void reEditMail();

private:
    Settings *settings;

};

#endif
