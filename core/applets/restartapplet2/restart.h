/**********************************************************************
** Copyright (C) 2002 ljp <llornkcor@handhelds.org>

** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
**********************************************************************/
#ifndef __OPIE_RESTART_APPLET_H__
#define __OPIE_RESTART_APPLET_H__

#include <qpe/menuappletinterface.h>

class RestartApplet : public QObject, public MenuAppletInterface
{
public:
    RestartApplet ( );
    virtual ~RestartApplet ( );

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual int position() const;
    
    virtual QString name ( ) const;
    virtual QIconSet icon ( ) const;
    virtual QString text ( ) const;
    virtual QPopupMenu *popup ( QWidget *parent ) const;
    
    virtual void activated ( );

private:
    ulong ref;
};

#endif
