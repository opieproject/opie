//
// VUMeter class
//

// Copyright (c) 2001 Camilo Mesias
// camilo@mesias.co.uk
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <stdio.h>
#include "vumeter.h"
#include <qpainter.h>

#include <math.h>

VUMeter::VUMeter(QWidget *parent, const char *name )
  : QWidget( parent, name ), Lannot("A"), Rannot("440 Hz")
{
  //  setPalette( QPalette( QColor( 250, 250, 200) ) );
 
  vuvalue = 0;

  pix = 0;
  

}

VUMeter::~VUMeter()
{
  delete(pix);
}

void VUMeter::paintEvent(QPaintEvent *){

  vupdate();
}

void VUMeter::bupdate(){

  if (!pix){
    pix = new QPixmap(rect().size());
  }
  // redraw whole background to buffer

  pix->fill(this, 0, 0);

  QPainter p( pix );

  p.setBrush(white);
  p.setPen(NoPen);
  
  p.drawRect( QRect(0, 0, width(), height()) );

  p.translate(width()/2, 0);

  p.setBrush(black);
  p.setPen(black);

  //  for (int i=-50;i<50;i+=4){
  //  p.drawLine((vuvalue), height()-10, vuvalue*2, 25);
  //  }

  p.drawText(-100,15,Lannot);
  p.drawText(65,15,Rannot);

  p.drawText(-100, height()-25, QString("-"));
  p.drawText(85, height()-25, QString("+"));
  p.drawText(-2, 20, QString("0"));
  p.end();

}

void VUMeter::vupdate(){

  if (!pix){
    bupdate();
  }

  // redraw line only

  QPixmap tmp_pix(rect().size());

  QPainter p( &tmp_pix );
  p.drawPixmap(QPoint(0,0), *pix);

  p.translate(width()/2, 0);
  p.drawLine((vuvalue), height()-10, vuvalue*2, 25);
  p.end();
  bitBlt(this, rect().topLeft(), &tmp_pix);
}



