/*
                             This file is part of the Opie Project

              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include "volumeapplet.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/otaskbarapplet.h>
#include <opie2/osoundsystem.h>
#include <opie2/oledbox.h>
#include <qpe/applnk.h>
#include <qpe/resource.h>
using namespace Opie::Core;
using namespace Opie::MM;
using namespace Opie::Ui;

/* QT */
#include <qpainter.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>

/* STD */
#include <assert.h>

Channel::Channel( OMixerInterface* mixer, QWidget* parent, const char* name )
        :QVBox( parent, name )
{
    _name = new QLabel( name, this );
    _volume = new QSlider( 0, 100, 10, mixer->volume( name ), QSlider::Vertical, this );
    _mute = new OLedBox( green, this );
    _mute->setFocusPolicy( QWidget::NoFocus );
    _mute->setFixedSize( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
    _name->show();
    _volume->show();
    _mute->show();
}


Channel::~Channel()
{
}


VolumeAppletControl::VolumeAppletControl( OTaskbarApplet* parent, const char* name )
                    :QFrame( parent, name, WStyle_StaysOnTop | WType_Popup ), l(0)
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    l = new QGridLayout( this );
}


void VolumeAppletControl::build()
{
    OSoundSystem* sound = OSoundSystem::instance();
    OSoundSystem::CardIterator it = sound->iterator();

    OMixerInterface* mixer = new OMixerInterface( this, "/dev/mixer" );

    QStringList channels = mixer->allChannels();

    int x = 0;
    int y = 0;

    for ( QStringList::Iterator it = channels.begin(); it != channels.end(); ++it )
    {
        odebug << "OSSDEMO: Mixer has channel " <<  *it << "" << oendl;
        odebug << "OSSDEMO:              +--- volume " << ( mixer->volume( *it ) & 0xff )
               << " (left) | " << ( mixer->volume( *it ) >> 8 ) << " (right)" << oendl;

        l->addWidget( new Channel( mixer, this, *it ), x++, y );
    }

}


VolumeAppletControl::~VolumeAppletControl()
{
}


void VolumeAppletControl::showEvent( QShowEvent* e )
{
    odebug << "showEvent" << oendl;
    build();
    QWidget::showEvent( e );
}


void VolumeAppletControl::hideEvent( QHideEvent* e )
{
    odebug << "hideEvent" << oendl;
    QWidget::hideEvent( e );
}


QSize VolumeAppletControl::sizeHint() const
{
    return QFrame::sizeHint();
}


VolumeApplet::VolumeApplet( QWidget *parent, const char *name )
             :OTaskbarApplet( parent, name )
{
    setFixedHeight( AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );
    _pixmap.convertFromImage( Resource::loadImage( "volumeapplet/volume" ).smoothScale( height(), width() ) );
   _control = new VolumeAppletControl( this, "control" );
}


VolumeApplet::~VolumeApplet()
{
}


int VolumeApplet::position()
{
    return 4;
}


void VolumeApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    p.drawPixmap(0, 2, _pixmap );
}


void VolumeApplet::mousePressEvent( QMouseEvent* )
{
    if ( !_control->isVisible() )
    {
        popup( _control );
    }
    else
    {
        _control->hide();
    }
}

EXPORT_OPIE_APPLET_v1( VolumeApplet )
