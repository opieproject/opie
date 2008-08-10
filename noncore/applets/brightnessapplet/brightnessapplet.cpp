/*
                     This file is part of the Opie Project

              =.             (C) 2004 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
            .=l.             Based on Qtopia 1.7 Brightnessapplet (C) 2003-2004 TrollTech
     .>+-=
_;:,   .>  :=|.         This program is free software; you can
.> <`_,  > .  <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This program is distributed in the hope that
  + . -:.    =       it will be useful,  but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=    =    ;      General Public License for more
++=  -.   .`   .:       details.
:   = ...= . :.=-
-.  .:....=;==+<;          You should have received a copy of the GNU
 -_. . .  )=. =           General Public License along with
  --    :-=`           this application; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "brightnessapplet.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/oresource.h>
#include <opie2/otaskbarapplet.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/power.h>
#include <qpe/qcopenvelope_qws.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qpainter.h>
#include <qlabel.h>
#include <qslider.h>
#include <qlayout.h>

/* STD */
#include <assert.h>

/* XPM */
static const char * const light_on_xpm[] = {
"9 16 5 1",
"       c None",
".      c #FFFFFFFF0000",
"X      c #000000000000",
"o      c #FFFFFFFFFFFF",
"O      c #FFFF6C6C0000",
"         ",
"   XXX   ",
"  XoooX  ",
" Xoooo.X ",
"Xoooooo.X",
"Xoooo...X",
"Xooo.o..X",
" Xooo..X ",
" Xoo...X ",
"  Xoo.X  ",
"  Xoo.XX ",
"  XOOOXX ",
"  XOOOXX ",
"   XOXX  ",
"    XX   ",
"         "};


/* XPM */
static const char * const light_off_xpm[] = {
"9 16 4 1",
"       c None",
".      c #000000000000",
"X      c #6B6B6C6C6C6C",
"o      c #FFFF6C6C0000",
"         ",
"         ",
"   ...   ",
"  .   .  ",
" .    X. ",
".      X.",
".    XXX.",
".   X XX.",
" .   XX. ",
" .  XXX. ",
"  .  X.  ",
"  .  X.. ",
"  .ooo.. ",
"  .ooo.. ",
"   .o..  ",
"    ..   "};

BrightnessAppletControl::BrightnessAppletControl( OTaskbarApplet* parent, const char* name )
                    :QFrame( parent, name, WStyle_StaysOnTop | WType_Popup )
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    QGridLayout *gl = new QGridLayout( this, 3, 2, 6, 3 );
    //gl->setRowStretch( 1, 100 );

    int maxbright = ODevice::inst()->displayBrightnessResolution();
    slider = new QSlider(this);
    slider->setMaxValue(maxbright);
    slider->setOrientation(QSlider::Vertical);
    slider->setTickmarks(QSlider::Right);
    slider->setTickInterval(QMAX(1, maxbright / 16));
    slider->setLineStep(QMAX(1, maxbright / 16));
    slider->setPageStep(QMAX(1, maxbright / 16));
    gl->addMultiCellWidget( slider, 0, 2, 0, 0 );

    QPixmap onPm( (const char **)light_on_xpm );
    QLabel *l = new QLabel( this );
    l->setPixmap( onPm );
    gl->addWidget( l, 0, 1 );

    QPixmap offPm( (const char **)light_off_xpm );
    l = new QLabel( this );
    l->setPixmap( offPm );
    gl->addWidget( l, 2, 1 );

    setFixedHeight( 100 );
    setFixedWidth( gl->sizeHint().width() );
    setFocusPolicy(QWidget::NoFocus);
}


BrightnessAppletControl::~BrightnessAppletControl()
{
}


void BrightnessAppletControl::hideEvent( QHideEvent* e )
{    
    BrightnessApplet* applet = static_cast<BrightnessApplet*>( parent() );
    applet->writeSystemBrightness( applet->calcBrightnessValue() );
    QFrame::hideEvent( e );
}

BrightnessApplet::BrightnessApplet( QWidget *parent, const char *name )
             :OTaskbarApplet( parent, name )
{
    setFixedHeight( AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );
    _pixmap = Opie::Core::OResource::loadPixmap( "brightnessapplet/icon", Opie::Core::OResource::SmallIcon );
   _control = new BrightnessAppletControl( this, "control" );
}


void BrightnessApplet::writeSystemBrightness(int brightness)
{
    PowerStatus ps = PowerStatusManager::readStatus();

    Config cfg( "apm" );
    if (ps.acStatus() == PowerStatus::Online) {
        cfg.setGroup("AC");
    } else {
        cfg.setGroup("Battery");
    }
    cfg.writeEntry("Brightness", brightness);
    odebug << "writing brightness " << brightness << oendl;
    cfg.write();
    sliderMoved(0); // ensures that screensaver picks up saved brightness
}


int BrightnessApplet::readSystemBrightness(void)
{
    PowerStatus ps = PowerStatusManager::readStatus();
    Config cfg( "apm" );

    if (ps.acStatus() == PowerStatus::Online) {
        cfg.setGroup("AC");
    } else {
        cfg.setGroup("Battery");
    }
    
    odebug << "reading brightness " << cfg.readNumEntry("Brightness", 128) << oendl;

    return cfg.readNumEntry("Brightness", 128);
}


BrightnessApplet::~BrightnessApplet()
{
}


int BrightnessApplet::position()
{
    return 7;
}


void BrightnessApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    p.drawPixmap(0, 0, _pixmap );
}


int BrightnessApplet::calcBrightnessValue()
{
    int v = _control->slider->maxValue() - _control->slider->value();
    return (v * 255 + _control->slider->maxValue() / 2) / _control->slider->maxValue();
}


void BrightnessApplet::sliderMoved( int /*value*/ )
{
#ifndef QT_NO_COP
    QCopEnvelope e("QPE/System", "setBacklight(int)");
    e << calcBrightnessValue();
#else
#error This Applet makes no sense without QCOP
#endif // QT_NO_COP
}


void BrightnessApplet::mousePressEvent( QMouseEvent* )
{
    if ( !_control->isVisible() )
    {
        int v = 255 - readSystemBrightness();
        popup( _control );
        _control->slider->setValue((_control->slider->maxValue() * v + 128) / 255);
        connect(_control->slider, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    }
}


EXPORT_OPIE_APPLET_v1( BrightnessApplet )

