/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "volume.h"

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qpainter.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>


VolumeControl::VolumeControl( QWidget *parent, const char *name )
    : QFrame( parent, name, WDestructiveClose | WStyle_StaysOnTop | WType_Popup )
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );

    QVBoxLayout *vbox = new QVBoxLayout( this );
    slider = new QSlider( this );
    muteBox = new QCheckBox( tr("Mute"), this );
    slider->setRange( 0, 100 );
    slider->setTickmarks( QSlider::Both );
    slider->setTickInterval( 20 );
    slider->setFocusPolicy( QWidget::NoFocus );
    muteBox->setFocusPolicy( QWidget::NoFocus );
    vbox->setMargin( 6 );
    vbox->setSpacing( 3 );
    vbox->addWidget( slider, 0, Qt::AlignVCenter | Qt::AlignHCenter );
    vbox->addWidget( muteBox );
    setFixedHeight( 100 );
    setFixedWidth( sizeHint().width() );
    setFocusPolicy(QWidget::NoFocus);
}

void VolumeControl::keyPressEvent( QKeyEvent *e) 
{
    switch(e->key()) {
	case Key_Up:
	    slider->subtractStep();
	    break;
	case Key_Down:
	    slider->addStep();
	    break;
	case Key_Space:
	    muteBox->toggle();
	    break;
	case Key_Escape:
	    close();
	    break;
    }
}

//===========================================================================

VolumeApplet::VolumeApplet( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    setFixedHeight( 18 );
    setFixedWidth( 14 );
    volumePixmap = Resource::loadPixmap( "volume" );
    muted = FALSE; // ### read from pref
    volumePercent = 50; // ### read from pref
    connect( qApp, SIGNAL( volumeChanged(bool) ), this, SLOT( volumeChanged(bool) ) );
    writeSystemVolume();
}

VolumeApplet::~VolumeApplet()
{
}

void VolumeApplet::mousePressEvent( QMouseEvent *)
{
    // Create a small volume control window to adjust the volume with
    VolumeControl *vc = new VolumeControl;
    vc->slider->setValue( 100 - volumePercent );
    vc->muteBox->setChecked( muted );
    connect( vc->slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( vc->muteBox, SIGNAL( toggled( bool ) ), this, SLOT( mute( bool ) ) );
    QPoint curPos = mapToGlobal( rect().topLeft() );
    vc->move( curPos.x()-(vc->sizeHint().width()-width())/2, curPos.y() - 100 );
    vc->show();
}

void VolumeApplet::volumeChanged( bool nowMuted )
{
    int previousVolume = volumePercent;

    if ( !nowMuted )
        readSystemVolume();

    // Handle case where muting it toggled
    if ( muted != nowMuted ) {
	muted = nowMuted;
	repaint( TRUE );
	return;
    }

    // Avoid over repainting
    if ( previousVolume != volumePercent )
	repaint( 2, height() - 3, width() - 4, 2, FALSE );
}


void VolumeApplet::mute( bool toggled )
{
    muted = toggled;
    // clear if removing mute
    repaint( !toggled );
    writeSystemVolume();
}


void VolumeApplet::sliderMoved( int percent )
{
    setVolume( 100 - percent );
}


void VolumeApplet::readSystemVolume()
{
    Config cfg("Sound");
    cfg.setGroup("System");
    volumePercent = cfg.readNumEntry("Volume");
}


void VolumeApplet::setVolume( int percent )
{
    // clamp volume percent to be between 0 and 100
    volumePercent = (percent < 0) ? 0 : ((percent > 100) ? 100 : percent);
    // repaint just the little volume rectangle
    repaint( 2, height() - 3, width() - 4, 2, FALSE );
    writeSystemVolume();
}


void VolumeApplet::writeSystemVolume()
{
    {
	Config cfg("Sound");
	cfg.setGroup("System");
	cfg.writeEntry("Volume",volumePercent);
    }
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
    // Send notification that the volume has changed
    QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << muted;
#endif
}


void VolumeApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);

    if (volumePixmap.isNull())
        volumePixmap = Resource::loadPixmap( "volume" );
    p.drawPixmap( 0, 1, volumePixmap );
    p.setPen( darkGray );
    p.drawRect( 1, height() - 4, width() - 2, 4 );

    int pixelsWide = volumePercent * (width() - 4) / 100;
    p.fillRect( 2, height() - 3, pixelsWide, 2, red );
    p.fillRect( pixelsWide + 2, height() - 3, width() - 4 - pixelsWide, 2, lightGray );

    if ( muted ) {
	p.setPen( red );
	p.drawLine( 1, 2, width() - 2, height() - 5 );
	p.drawLine( 1, 3, width() - 2, height() - 4 );
	p.drawLine( width() - 2, 2, 1, height() - 5 );
	p.drawLine( width() - 2, 3, 1, height() - 4 );
    }
}


