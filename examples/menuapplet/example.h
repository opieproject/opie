#ifndef OPIE_EXAMPLE_MENU_APPLET_H
#define OPIE_EXAMPLE_MENU_APPLET_H

#include <qpe/menuappletinterface.h>
#include <qobject.h>

#include <qobject.h>

class MenuAppletExample : public QObject, public MenuAppletInterface
{

    Q_OBJECT

public:
    MenuAppletExample ( );
    virtual ~MenuAppletExample ( );

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
