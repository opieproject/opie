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


#include <stdio.h>

#include <string.h>

#include "chordengine.h"

ChordEngine::ChordEngine(){

  string[0] = OPEN;
  string[1] = OPEN;
  string[2] = OPEN;
  string[3] = OPEN;
  string[4] = OPEN;
  string[5] = OPEN;

  base_note = 0;
  chord_type = 0;
  fret_pos = OPEN;
  span_size = 1;
  variation = 7;
  tuning = 0;

  js_tunit(tuning);
  js_whatchord(chord_type);
  js_vboy(variation);

  label_text[0] = 0;
}

int ChordEngine::finger(int f){
    return string[f];
}
#define BREAK (401)
void ChordEngine::calculate(){

  sprintf(label_text, "%s %s %s%s",
	  notes[base_note],
	  (chord_type)?keys[chord_type]:"",
	  (fret_pos==OPEN)?"":"fret-",
	  (fret_pos)?frets[fret_pos]:"");

  string[0] =
    string[1] =
    string[2] =
    string[3] =
    string[4] =
    string[5] = MUTED;

  for (int in=0;in<6;in++){
    notename[in]=0;
  }

  js_tunit(tuning);
  //  js_MMM[0] = 4;
  //js_MMM[1] = 9;
  //js_MMM[2] = 2;
  //js_MMM[3] = 7;
  //js_MMM[4] = 11;
  //js_MMM[5] = 4;
  
  int js_D = base_note;
  
  int js_Q = span_size;
  
  js_Q += 2;
  
  int js_V = variation;
  
  int js_FR = fret_pos;
  
  int js_G = 0;
  
  int js_A = 0;
  
  if (js_FR){
    js_G = 8;
  }
  
  js_L = 1;
  js_Y = 1;
  js_Z = 1;
  
  js_vboy(variation);
  
  js_Y += js_Q;
  int js_K = 0;
  int js_W = js_D - js_Z;
  int js_H = -js_Z;
  int js_N[6] = {0, 0, 0, 0, 0, 0};
  int js_TCK[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int js_E = 0;
  int js_X = 0;
  
  for (int j=0; j<41; j++){
    js_A = 0;
    for (int b=0; b<7; b++){
      if (js_N[b] == 1){
	js_A++;
      }
      if ((js_A==6)||(j>39)){
	for (int i=0; i<12; i++){
	  if (js_TCK[i] != js_T[i]){
	    memset((void *)js_N, 0, 6*sizeof(int));
	    memset((void *)js_TCK, 0, 12*sizeof(int));
	    js_V++;
	    js_X++;
	    if (js_V>23){
	      js_V = 0;
	    }
	    js_vboy(js_V);
	    j = 0;
	    i = BREAK;
	    b = BREAK;
	    if (js_X > 23){
	      js_N[0] = js_N[1] = js_N[2] = js_N[3] = js_N[4] = js_N[5] = 1;
	      j = BREAK;
	    }
	  }
	}
      }
    }
    int js_S = 0;
    js_W = (js_W + js_Z)%12;
    js_H = (js_H + js_Z)%12;
    
    while (js_S < js_Y){
      js_S++;
      if (js_E > js_Q){
	js_E = 0;
	js_K = (js_K + js_L)%6;
      }
      if (js_E<0){
	js_E = js_Q;
	js_K = (js_K + js_L)%6;
      }
      if (js_T[js_H] == 0){
	for (int c=0;c<13;c++){
	  js_W = (js_W+js_Z)%12;
	  js_H = (js_H+js_Z)%12;
	  if (js_T[js_H] == 1){
	    c=BREAK;
	  }
	}
      }
      if (js_N[js_K] == 1){
	for(int b=0; b<7; b++){
	  js_K = (js_K+js_L)%6;
	  if(js_N[js_K] == 0){
	    b = BREAK;
	  }
	  if(b==6){
	    js_S = BREAK;
	  }
	}
      }

      // js_K is the index of the string
      // js_MMM is the base note played by that string in the current tuning
      // js_FR is the fret position
      // js_E
      // js_F represents the note made by this string here
      // js_N[] records if we have found a finger position for this string
      // js_W - represents the note we want????

      int js_F = (js_MMM[js_K]+js_E+js_FR)%12;
      
      if ((js_W==js_F)&&(js_N[js_K]==0)){
	
	int js_EG = js_E+js_G;
	
	string[js_K] = js_EG;
	notename[js_K] = notes[js_W];
	
	js_N[js_K] = 1;
	js_TCK[js_H] = 1;
	js_S = BREAK;
      }
      js_E += js_VM;
      
    }
    
  }

}


const int ChordEngine::alt_tunings[][6] = {
  {4, 9,2,7,11,4},
  {4,11,4,8,11,4},
  {4, 9,4,9, 1,4},
  {4, 9,2,6,11,4},
  {4, 9,2,7, 0,5},
  {2, 9,2,7, 9,2},
  {2, 7,0,7, 0,2},
  {2, 9,2,6, 9,2},
  {2, 9,2,7,11,4},
  {2, 7,2,7,11,2},
  {2, 9,2,9, 0,2},
  {0, 7,0,7, 9,4},
  {5, 9,2,7,11,4},
  {7,10,2,7,10,2},
};

void ChordEngine::js_tunit(int t){
  int max=0;
  int octave=0;
  for (int i=0;i<6;i++){
    js_MMM[i] = ChordEngine::alt_tunings[t][i];
    if (js_MMM[i] < max){
      octave += 12;
    }
    max = js_MMM[i];
    note_indices[i] = octave + js_MMM[i];
  }
}

const int ChordEngine::chordbases[][12] = {
  {1,0,0,0,1,0,0,1,0,0,0,0}, // maj
  {1,0,0,1,0,0,0,1,0,0,0,0}, // min
  {1,0,0,0,1,0,0,1,0,0,1,0}, // 7th
  {1,0,0,1,0,0,0,1,0,0,1,0}, // m7
  {1,0,0,0,1,0,0,1,0,0,0,1}, // maj7
  {1,0,0,0,1,0,0,1,0,1,0,0}, // 6th
  {1,0,0,1,0,0,0,1,0,1,0,0}, // m6th
  {1,0,0,0,1,0,0,0,1,0,0,0}, // aug
  {1,0,0,1,0,0,1,0,0,1,0,0}, // dim
  {1,0,0,0,0,1,0,1,0,0,0,0}, // sus4
  {1,0,0,0,0,1,0,1,0,0,1,0}, // 7sus4
  {1,0,1,0,1,0,0,1,0,0,1,0}, // 9th
  {1,0,1,0,1,0,0,1,0,0,0,0}, // add9
  {1,0,1,1,0,0,0,1,0,0,1,0}, // m9th
  {1,1,0,0,1,0,0,1,0,0,0,1}, // maj9
  {1,0,1,0,0,0,0,1,0,0,0,0}, // sus2
  {1,0,1,0,0,0,0,1,0,0,1,0}, // 7sus2
  {1,0,1,0,0,1,0,1,0,0,1,0}, // 11th
  {1,0,1,1,0,1,0,1,0,0,1,0}, // m11th
  {1,0,0,0,1,0,0,1,0,1,1,0}, // 13th
  {1,0,0,1,0,0,0,1,0,1,1,0}, // m13th
  {1,0,0,1,0,0,0,1,0,1,0,1}, // maj13
  {1,0,1,0,1,0,0,1,0,1,0,0}, // 6/9
  {1,0,0,0,1,0,1,0,0,0,0,0}, // flat5
  {1,0,0,1,1,0,0,0,0,0,1,0}, // 7#9
  {1,0,0,1,0,0,1,0,0,0,1,0}, // Ø7
  {1,0,0,0,0,0,0,1,0,0,0,0}, // 5
};

void ChordEngine::js_whatchord(int c){
  for (int i=0;i<12;i++){
    js_T[i] = ChordEngine::chordbases[c][i];
  }  
}

void ChordEngine::js_vboy(int v){
  js_L = ((v/3)%2)?5:1;
  js_Y = ((v/6)%2)?45:4;
  js_Z = (v%3 == 0)?1:(v%3 == 1)?7:11;
  js_VM = ((v/12)%2)?-1:1;
}

const char* ChordEngine::notes[] = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B", 0};

const char* ChordEngine::keys[] = {"maj", "min", "7th", "m7", "maj7", "6th", "m6th", "aug", "dim", "sus4", "7sus4", "9th", "add9", "m9th", "maj9", "sus2", "7sus2", "11th", "m11th", "13th", "m13th", "maj13", "6|9", "flat5", "7#9", "Ø7", "5", 0};

const char* ChordEngine::frets[] = {"open", "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th", "11th", "12th", "13th", "14th", "15th", "16th", 0};

const char* ChordEngine::variations[] = {"V1", "V2", "V3", "V4", "V5", "V6", "V7", "V8", "V9", "V10", "V11", "V12", "V13", "V14", "V15", "V16", "V17", "V18", "V19", "V20", "V21", "V22", "V23", 0};

const char* ChordEngine::tunings[] = {"EADGBE", "EBEG#BE", "EAEAC#E", "EADF#BE", "EADGCF", "DADGAD", "DGCGCD", "DADF#AD", "DADGBE", "DGDGBD", "DADACD", "CGCGAE", "FADGBE", "Gminor",0};





