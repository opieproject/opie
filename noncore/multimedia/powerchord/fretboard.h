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

#ifndef __FRETBOARD_H
#define __FRETBOARD_H

//#include <FL/Fl.H>
//#include <FL/Fl_Widget.H>

#include <qwidget.h>
#include <qpixmap.h>
#include <qstring.h>

#include "chordengine.h"

class FretBoard : public QWidget {

    Q_OBJECT

 private:

    // refresh display based on chord engine
    void refresh();

    // image stuff
    QPixmap stringtop_pix;
    QPixmap string_pix;
    QPixmap finger_pix;
    QPixmap mute_pix;

  //saved state of the fretboard

  int initial_fret;
  int finger[6];

 protected:
  //  void draw_box();
  //  void draw_label();
  void paintEvent(QPaintEvent *);

 public:
  // encapsulated chord engine class
  ChordEngine ce;

  static const int MUTED=7;
  static const int OPEN=0;

  //  void draw();
  FretBoard(QWidget *parent=0, const char *name=0);

  // not used yet
  //  int Finger(int finger);
  //  int Fret(){return initial_fret;};

  // slots
  public slots:
      void Finger(int finger, int position);
  
  void Fret(int position){
    initial_fret = position;
    ce.fret(position);
    refresh();
  };
  void Base(int b){ce.base(b);refresh();};
  void Chord(int b){ce.chord(b);refresh();};
  void Span(int b){ce.span(b);refresh();};
  void Vary(int b){ce.vary(b);refresh();};
  void Tune(int b){ce.tune(b);refresh();};

  signals:
  void nameChanged(const QString &);
  void s1nameChanged(const QString &);
  void s2nameChanged(const QString &);
  void s3nameChanged(const QString &);
  void s4nameChanged(const QString &);
  void s5nameChanged(const QString &);
  void s6nameChanged(const QString &);

};

#endif


