/**********************************************************************
** Copyright (C) 2003-2004 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include "vt.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/resource.h>
using namespace Opie::Core;

/* QT */
#include <qpopupmenu.h>

/* STD */
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/vt.h>

VTApplet::VTApplet ( )
    : QObject ( 0, "VTApplet" )
{
}

VTApplet::~VTApplet ( )
{
}

int VTApplet::position ( ) const
{
    return 2;
}

QString VTApplet::name ( ) const
{
    return tr( "VT shortcut" );
}

QString VTApplet::text ( ) const
{
    return tr( "Terminal" );
}

/*
QString VTApplet::tr( const char* s ) const
{
    return qApp->translate( "VTApplet", s, 0 );
}

QString VTApplet::tr( const char* s, const char* p ) const
{
    return qApp->translate( "VTApplet", s, p );
}
*/

QIconSet VTApplet::icon ( ) const
{
    QPixmap pix;
    QImage img = Resource::loadImage ( "terminal" );

    if ( !img. isNull ( ))
        pix. convertFromImage ( img. smoothScale ( 14, 14 ));
    return pix;
}

QPopupMenu *VTApplet::popup ( QWidget* parent ) const
{
    odebug << "VTApplet::popup" << oendl;

    struct vt_stat vtstat;
    int fd = ::open( "/dev/tty0", O_RDWR );
    if ( fd == -1 ) return 0;
    if ( ioctl( fd, VT_GETSTATE, &vtstat ) == -1 ) return 0;

    m_subMenu = new QPopupMenu( parent );
    m_subMenu->setCheckable( true );
    for ( int i = 1; i < 10; ++i )
    {
        int id = m_subMenu->insertItem( QString::number( i ), 500+i );
        m_subMenu->setItemChecked( id, id-500 == vtstat.v_active );
    }
    ::close( fd );

    connect( m_subMenu, SIGNAL( activated(int) ), this, SLOT( changeVT(int) ) );
    connect( m_subMenu, SIGNAL( aboutToShow() ), this, SLOT( updateMenu() ) );

    return m_subMenu;
}


void VTApplet::changeVT( int index )
{
    //odebug << "VTApplet::changeVT( " << index-500 << " )" << oendl;

    int fd = ::open("/dev/tty0", O_RDWR);
    if ( fd == -1 ) return;
    ioctl( fd, VT_ACTIVATE, index-500 );
}


void VTApplet::updateMenu()
{
    //odebug << "VTApplet::updateMenu()" << oendl;

    int fd = ::open( "/dev/console", O_RDONLY );
    if ( fd == -1 ) return;

    for ( int i = 1; i < 10; ++i )
    {
        int result = ioctl( fd, VT_DISALLOCATE, i );

        /*
        if ( result == -1 )
            odebug << "VT " << i << " disallocated == free" << oendl;
        else
            odebug << "VT " << i << " _not_ disallocated == busy" << oendl;
        */

        m_subMenu->setItemEnabled( 500+i, result == -1 );
    }

    ::close( fd );
}


void VTApplet::activated()
{
    odebug << "VTApplet::activated()" << oendl;
}


QRESULT VTApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
        *iface = this;
    else if ( uuid == IID_MenuApplet )
        *iface = this;
    else
        return QS_FALSE;

    if ( *iface )
        (*iface)-> addRef ( );
    return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
    Q_CREATE_INSTANCE( VTApplet )
}


