//
// FretBoard class to display interface for chord finder application
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

//#include <FL/Fl.H>
//#include <FL/Fl_Widget.H>
//#include <FL/fl_draw.H>
#include <stdio.h>
#include <qpixmap.h>
#include <qpe/resource.h>
#include "fretboard.h"
#include <qpainter.h>

void FretBoard::refresh(){
  ce.calculate();

  emit nameChanged(QString(ce.label()));

  for (int i=0;i<6;i++){
    Finger(i, ce.finger(i));
    // yuk
    switch (i) {
    case 0:
      emit s1nameChanged(QString(ce.name(i)));
      break;
    case 1:
      emit s2nameChanged(QString(ce.name(i)));
      break;
    case 2:
      emit s3nameChanged(QString(ce.name(i)));
      break;
    case 3:
      emit s4nameChanged(QString(ce.name(i)));
      break;
    case 4:
      emit s5nameChanged(QString(ce.name(i)));
      break;
    case 5:
      emit s6nameChanged(QString(ce.name(i)));
      break;
    }
  }

  update();
}

FretBoard::FretBoard(QWidget *parent, const char *name )
  : QWidget( parent, name ), ce()
{
  
  ce.base(9);
  ce.chord(0);
  ce.fret(0);
  ce.span(1);
  ce.vary(7);
  ce.tune(0);
  refresh();

  //  setPalette( QPalette( QColor( 250, 250, 200) ) );

  stringtop_pix = Resource::loadPixmap( "powerchord/stringtop_s");
  string_pix = Resource::loadPixmap( "powerchord/string_s");
  finger_pix = Resource::loadPixmap( "powerchord/justfing_s");
  mute_pix = Resource::loadPixmap( "powerchord/muted_s");

  //  box(FL_NO_BOX);

  //  finger[0] = OPEN;
  //  finger[1] = OPEN;
  //  finger[2] = OPEN;
  //  finger[3] = OPEN;
  //  finger[4] = OPEN;
  //  finger[5] = OPEN;

  initial_fret = 0;

  //  label(FL_NORMAL_LABEL, "Chord");
}

//  int FretBoard::Finger(int f){
//    if (f < 0 || f > 5){
//      fprintf(stderr, "Error - finger value was %d\n", f);
//      return 0;
//    }
//    return finger[f];
//  }

void FretBoard::Finger(int f, int position){
  if (f < 0 || f > 5){
    fprintf(stderr, "Error - finger2 value was %d\n", f);
    return;
  }
  
  finger[f] = position;
}

//  void FretBoard::draw(void) {
//    draw_box();
//    draw_label();
//  }

//  void FretBoard::draw_label(void) {
//    align(FL_ALIGN_CENTER | FL_ALIGN_TOP | FL_ALIGN_WRAP);

//    this->Fl_Widget::draw_label();
//  }


//void FretBoard::draw_box(void) {
  
void FretBoard::paintEvent(QPaintEvent *){

  //  fl_color(FL_WHITE);

  //  Fl_Widget::draw_box();

  //  fl_color(FL_BLACK);

  QPainter p( this );

  p.setBrush(black);
  p.translate(0, 0);

  // draw each string
  for (int f=0; f<=5; f++){
    // draw a string starting from the highest visible fret
    for (int pp=0; pp<=6; pp++){
      int this_fret = initial_fret + pp;
      int cx = /* x() + */ 28*f;
      int cy = /* y() + */ pp*30;

      //	fl_color(FL_WHITE);
//  	fl_rectf(cx, cy, 24, 24);
      //      p.setBrush(white);
      //      p.setPen(NoPen);

      //      p.drawRect( QRect(cx, cy, 24, 24) );
//          fl_color(FL_BLACK);
      //      p.setBrush(black);

      //      p.setPen(black);

      // draw the string unless at top
      if (this_fret!=0){
	//	fl_line(cx+12, cy, cx+12, cy+23);
	//	p.drawLine(cx+12, cy, cx+12, cy+23);
      }

      // draw mutes if at top
      if (pp == 0 &&
	  finger[f] == MUTED){
	p.drawPixmap(cx,cy,mute_pix);
	//	fl_line(cx+4, cy+19, cx+19, cy+4);
	//	p.drawLine(cx+4, cy+19, cx+19, cy+4);
	//	fl_line(cx+5, cy+19, cx+20, cy+4);
	//	p.drawLine(cx+5, cy+19, cx+20, cy+4);
	//	fl_line(cx+4, cy+4, cx+19, cy+19);
	//	p.drawLine(cx+4, cy+4, cx+19, cy+19);
	//	fl_line(cx+5, cy+4, cx+20, cy+19);
	//	p.drawLine(cx+5, cy+4, cx+20, cy+19);
      }

      if (this_fret==0){
	// above, blank
      }else if(this_fret==1){
	// top
	p.drawPixmap(cx,cy,stringtop_pix);
      }else{
	// normal
	p.drawPixmap(cx,cy,string_pix);
      }

      // frets at bottom
      // thick line if at top
      //      int x0 = (f==0)?(cx+12):cx;
      //      int x1 = (f==5)?(cx+12):(cx+23);
      //      for (int offset=23; offset>((this_fret==0)?20:22); offset--){
	//	fl_line(x0, cy+offset, x1, cy+offset);
	//	p.drawLine(x0, cy+offset, x1, cy+offset);
      //      }

      // finger if at that position
      if ((initial_fret==0 && pp == finger[f]) ||
	  (initial_fret!=0 && pp == finger[f]-8)){
	p.drawPixmap(cx,cy,finger_pix);

//  	fl_color(FL_WHITE);
//  	p.setBrush(white);
//  	p.setPen(NoPen);
	
//  	fl_rectf(cx+4, cy+4, 16, 16);
	//	p.drawRect( QRect(cx+4, cy+4, 16, 16));
//          fl_color(FL_BLACK);
	//p.setPen(black);
//  	fl_line(cx+4, cy+11, cx+7, cy+16);
	//p.drawLine(cx+4, cy+11, cx+7, cy+16);
//  	fl_line(cx+7, cy+16, cx+12, cy+19);
	//p.drawLine(cx+7, cy+16, cx+12, cy+19);
//  	fl_line(cx+12, cy+19, cx+16, cy+16);
	//p.drawLine(cx+12, cy+19, cx+16, cy+16);
//  	fl_line(cx+16, cy+16, cx+19, cy+12);
	//p.drawLine(cx+16, cy+16, cx+19, cy+12);
//  	fl_line(cx+19, cy+12, cx+16, cy+7);
	//p.drawLine(cx+19, cy+12, cx+16, cy+7);
//  	fl_line(cx+16, cy+7, cx+12, cy+4);
	//p.drawLine(cx+16, cy+7, cx+12, cy+4);
//  	fl_line(cx+11, cy+4, cx+7, cy+7);
	//p.drawLine(cx+11, cy+4, cx+7, cy+7);
//  	fl_line(cx+7, cy+7, cx+4, cy+11);
	//p.drawLine(cx+7, cy+7, cx+4, cy+11);
      }
    }
  }
  p.end();
}


