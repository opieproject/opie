#ifndef MAILAPPLETIMPL_H
#define MAILAPPLETIMPL_H

#include <qpe/taskbarappletinterface.h>

class MailApplet;

class MailAppletImpl : public TaskbarAppletInterface {

public:
    MailAppletImpl();
    virtual ~MailAppletImpl();

    QRESULT queryInterface( const QUuid &uuid, QUnknownInterface **iface );
    Q_REFCOUNT

    virtual QWidget *applet( QWidget *parent );
    virtual int position() const;

private:
    MailApplet *m_mailApplet;
    ulong ref;

};

#endif

