/**********************************************************************
** Copyright (C) 2003 Michael 'Mickey' Lauer.  All rights reserved.
**
** Contact me @ mickeyl@handhelds.org
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/
#ifndef __OPIE_VTAPPLET_H__
#define __OPIE_VTAPPLET_H__

#include <qpe/menuappletinterface.h>
#include <qobject.h>

class VTApplet : public QObject, public MenuAppletInterface
{

    Q_OBJECT

public:
    VTApplet ( );
    virtual ~VTApplet ( );

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual int position() const;

    virtual QString name ( ) const;
    virtual QIconSet icon ( ) const;
    virtual QString text ( ) const;
    //virtual QString tr( const char* ) const;
    //virtual QString tr( const char*, const char* ) const;
    virtual QPopupMenu *popup ( QWidget *parent ) const;

    virtual void activated ();

public slots:
    virtual void changeVT( int index );
    virtual void updateMenu();
private:
    mutable QPopupMenu* m_subMenu;
};


#endif
