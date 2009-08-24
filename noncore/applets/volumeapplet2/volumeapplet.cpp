/*
                     This file is part of the Opie Project

              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.
     .>+-=
_;:,   .>  :=|.         This program is free software; you can
.> <`_,  > .  <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU Library General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This program is distributed in the hope that
  + . -:.    =       it will be useful,  but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=    =    ;      Library General Public License for more
++=  -.   .`   .:       details.
:   = ...= . :.=-
-.  .:....=;==+<;          You should have received a copy of the GNU
 -_. . .  )=. =           Library General Public License along with
  --    :-=`           this library; see the file COPYING.LIB.
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
#include <opie2/oresource.h>

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>

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
    _name->setFont( QFont( "Vera", 8 ) );
    _volume = new QSlider( 0, 100, 10, mixer->volume( name ) & 0xff, QSlider::Vertical, this );
    _volume->setTickmarks( QSlider::Both );
    _volume->setTickInterval( 20 );
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
    build();
}


void VolumeAppletControl::build()
{
    OSoundSystem* sound = OSoundSystem::instance();
    OSoundSystem::CardIterator it = sound->iterator();

//    OMixerInterface*
    mixer = new OMixerInterface( this, "/dev/mixer" );

    QStringList channels = mixer->allChannels();

    int x = 0;
    // int y = 0;

    for ( QStringList::Iterator it = channels.begin(); it != channels.end(); ++it )
    {
       if((*it) == mixer->volume( "Vol")) {
       m_vol_percent=mixer->volume( *it ) >> 8;
    }
        owarn << "OSSDEMO: Mixer has channel " <<  *it << "" << oendl;
        owarn << "OSSDEMO:              +--- volume " << ( mixer->volume( *it ) & 0xff )
               << " (left) | " << ( mixer->volume( *it ) >> 8 ) << " (right)" << oendl;

        l->addWidget( new Channel( mixer, this, *it ), 0, x++, AlignCenter );
    }

}


VolumeAppletControl::~VolumeAppletControl()
{
}

int VolumeAppletControl::volPercent ( ) const
{
  return m_vol_percent;
}

bool VolumeAppletControl::volMuted ( ) const
{
  return m_vol_muted;
}


void VolumeAppletControl::showEvent( QShowEvent* e )
{
    odebug << "showEvent" << oendl;
    QWidget::showEvent( e );
}


void VolumeAppletControl::hideEvent( QHideEvent* e )
{
    odebug << "hideEvent" << oendl;
    QWidget::hideEvent( e );
}


 QSize VolumeAppletControl::sizeHint() const
 {
   int wd =  QPEApplication::desktop()->width();
   return QSize( wd, 200 ); //QFrame::sizeHint();
}


VolumeApplet::VolumeApplet( QWidget *parent, const char *name )
             :OTaskbarApplet( parent, name )
{
    setFixedHeight( AppLnk::smallIconSize() +4);
    setFixedWidth( AppLnk::smallIconSize() );
    _pixmap = Opie::Core::OResource::loadPixmap( "volume", Opie::Core::OResource::SmallIcon );
//    _pixmap =  new QPixmap (Opie::Core::OResource::loadPixmap( "volume", Opie::Core::OResource::SmallIcon ));
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

/*
  p. drawPixmap ( (width()- _pixmap->width())/2, QMAX( (height()-4-_pixmap->height() )/2, 1), *_pixmap );
  p. setPen ( darkGray );
  p. drawRect ( 1, height() - 4, width() - 2, 4 );

  OMixerInterface* mixer = new OMixerInterface( this, "/dev/mixer" );

  int volPercent =   mixer->volume( "Vol" ) >> 8;

  int pixelsWide =  volPercent  * ( width() - 4 ) / 100;
  p. fillRect ( 2, height() - 3, pixelsWide, 2, red );
  p. fillRect ( pixelsWide + 2, height() - 3, width() - 4 - pixelsWide, 2, lightGray );
*/

//   if ( _control-> volMuted ( )) {
//     p. setPen ( red );
//     p. drawLine ( 1, 2, width() - 2, height() - 5 );
//     p. drawLine ( 1, 3, width() - 2, height() - 4 );
//     p. drawLine ( width() - 2, 2, 1, height() - 5 );
//     p. drawLine ( width() - 2, 3, 1, height() - 4 );
//   }

//  QPainter p(this);
  //  p.drawPixmap(0, 2, _pixmap );
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
