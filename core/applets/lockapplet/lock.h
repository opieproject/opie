/**
 * \file lock.h
 * \brief defines a lock button that goes in the 'O' Opie menu
 * It's based on the examples/menuapplet code of 2004/10/06.
 */
#ifndef CORE_SETTINGS_SECURITY_LOCKAPPLET_LOCK_H
#define CORE_SETTINGS_SECURITY_LOCKAPPLET_LOCK_H

#include <qpe/menuappletinterface.h>
#include <qobject.h>

class LockMenuApplet: public QObject, public MenuAppletInterface
{

    Q_OBJECT

public:
    LockMenuApplet ( );
    virtual ~LockMenuApplet ( );

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual int position() const;
    
    virtual QString name ( ) const;
    virtual QIconSet icon ( ) const;
    virtual QString text ( ) const;
    /* virtual QString tr( const char* ) const;
    virtual QString tr( const char*, const char* ) const;
    */
    virtual QPopupMenu *popup ( QWidget *parent ) const;
    virtual void activated ( );
};

#endif
