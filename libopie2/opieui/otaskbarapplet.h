/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef OTASKBARAPPLET_H
#define OTASKBARAPPLET_H

#include <qpe/taskbarappletinterface.h>

/*======================================================================================
 * OTaskbarAppletWrapper
 *======================================================================================*/

template<class T> class OTaskbarAppletWrapper : public TaskbarAppletInterface
{
  public:
    OTaskbarAppletWrapper():_applet( 0 )
    {
    }

    virtual ~OTaskbarAppletWrapper()
    {
        delete _applet;
    }

    QRESULT queryInterface( const QUuid& uuid, QUnknownInterface** iface )
    {
        qDebug( "OTaskbarAppletWrapper::queryInterface()" );
        *iface = 0;
        if ( uuid == IID_QUnknown )
            *iface = this;
        else if ( uuid == IID_TaskbarApplet )
            *iface = this;
	else
	    return QS_FALSE;

        if ( *iface ) (*iface)->addRef();
        return QS_OK;
    }

    Q_REFCOUNT

    virtual T* applet( QWidget* parent )
    {
        if ( !_applet ) _applet = new T( parent );
        return _applet;
    }

    virtual int position() const
    {
        return T::position();
    }

  private:
    T* _applet;
};

#include <qframe.h>
#include <qwidget.h>
#include <qpe/qpeapplication.h>

class QMouseEvent;

/*======================================================================================
 * OTaskbarApplet
 *======================================================================================*/

// Must be inline until after we shipped Opie 1.0

// Having OTaskBarApplet reside in libopieui2 is not possible
// until we link the launcher binary against libopieui2 -
// otherwise the necessary symbols are not present, when
// the dynamic loader [dlopen] tries to resolve an applet which
// inherits OTaskbarApplet

class OTaskbarApplet : public QWidget
{
  public:
    OTaskbarApplet( QWidget* parent, const char* name = 0 ):QWidget( parent, name )
    {
        setFixedHeight( 18 );
        setFixedWidth( 14 );
    }

    virtual ~OTaskbarApplet()
    {
    }

  protected:
    virtual void popup( QWidget* widget )
    {
        QPoint curPos = mapToGlobal( QPoint( 0, 0 ) );
        int w = widget->sizeHint().width();
        int x = curPos.x() - (w/2 );
        if ( (x+w) > QPEApplication::desktop()->width() )
            x = QPEApplication::desktop()->width()-w;
        widget->move( x, curPos.y()-widget->sizeHint().height() );
        widget->show();
    }
};

#endif

