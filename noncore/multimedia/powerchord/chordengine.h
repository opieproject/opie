//
// ChordEngine class to calculate chords
//

// Copyright (c) 2001 Camilo Mesias
// camilo@mesias.co.uk
//
// derived from JavaScript code by Jim Cranwell, used with permission
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


#ifndef __CHORDENGINE_H
#define __CHORDENGINE_H

#include <stdio.h>

class ChordEngine;

class ChordEngine {

 private:
  //saved state

  int base_note;
  int chord_type;
  int fret_pos;
  int span_size;
  int variation;
  int tuning;
  
  int string[6];
  const char *notename[6];

  // unfathomable stuff ported from javascript: js_prefix
  // may ask Jim Cranwell if he'd like to comment on it
  int js_MMM[6];
  int note_indices[6];

  void js_tunit(int t);

  int js_T[12];

  void js_whatchord(int c);

  int js_L, js_Y, js_Z, js_VM;

  void js_vboy(int v);

  // stuff I put in
  char label_text[20];
  static const int chordbases[][12];
  static const int alt_tunings[][6];

 public:
  static const char* notes[];
  static const char* keys[];
  static const char* frets[];
  static const char* variations[];
  static const char* tunings[];


  static const int OPEN = 0;
  static const int MUTED = 7;

  ChordEngine();

  // accessors
  const char *name(int f){return notename[f];};
  int noteindex(int f){
    if (string[f] == MUTED){
      return -1;
    }else{
      return note_indices[f] + string[f];
    }
  };

  void base(int b){base_note = b;};
  int base(){return base_note;};

  void chord(int c){chord_type = c; js_whatchord(c);};
  int chord(){return chord_type;};

  void fret(int f){fret_pos = f;};
  int fret(){return fret_pos;};

  void span(int s){span_size = s;};
  int span(){return span_size;};

  void vary(int v){variation = v; js_vboy(v);};
  int vary(){return variation;};

  void tune(int t){tuning = t; js_tunit(t);};
  int tune(){return tuning;};

  // methods
  void calculate();

  const char *label(){return label_text;};
  
  int finger(int string); // returns fret position or MUTED

};

#endif







