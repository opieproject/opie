/*  This file is part of the OPIE libraries
    Copyright (C) 2003 Maximilian Reiss  (harlekin@handhelds.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qpainter.h>

#include "owait.h"

#include <qpe/resource.h>

static int frame = 0;

OWait::OWait(QWidget *parent, const char* msg, bool dispIcon )
  :QDialog(parent, QObject::tr("Wait"), TRUE,WStyle_Customize) {

    QHBoxLayout *hbox  = new QHBoxLayout( this );

    m_lb = new QLabel( this );
    m_lb->setBackgroundMode ( NoBackground );

    hbox->addWidget( m_lb );
    hbox->activate();

    m_pix = Resource::loadPixmap( "BigBusy" );
    resize( 48, 48 );

    m_timerLength = 10;

    m_waitTimer = new QTimer( this );
    connect( m_waitTimer, SIGNAL( timeout() ), this, SLOT( hide() ) );
}

void OWait::timerEvent( QTimerEvent * )  {
    frame = (++frame) % 3;
    repaint();
}

void OWait::paintEvent( QPaintEvent * )  {
      QPainter p( m_lb  );
      p.drawPixmap( 0, 0, m_pix, 48 * frame,  0, 48, 48 );
}

void OWait::show()  {
    startTimer( 300 );
   m_waitTimer->start( m_timerLength * 1000, true );
    QDialog::show();
}

void OWait::hide()  {
    killTimers();
    m_waitTimer->stop();
    frame = 0;
    QDialog::hide();
}

void OWait::setTimerLength( int length )  {
    m_timerLength = length;
}

OWait::~OWait() {
}
