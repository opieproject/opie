
#ifndef __GS_H
#define __GS_H

#include <qobject.h>
// sound includes
#include <sys/soundcard.h>

#define BUFSIZE 256
#define NUMCHANS 7
#define ACGUITAR_PATH_S "/usr/local/projects/opie-head/noncore/multimedia/powerchord/acguitar.raw"

class gs : public QObject {

  Q_OBJECT

  public:
  gs();
  ~gs();

 private:

  int initial_fret;
  int finger[6];
  int tuning[6];

  signed short *reverb;
  int reverb_ptr;
  int reverb_max;

  int audio_fd;
  signed short audio_buf[BUFSIZE];
  static int note_periods[12];
  static int octave_step[6];
  int frames;


  // 7 channel synth
  signed short *tonebank0; // source of waveform
  signed short *tonebank1;
  signed short *tonebank2;
  signed short *tonebank3;
  signed short *tonebank4;
  signed short *tonebank5;
  signed short *tonebank6;
  int tonebank_length0;
  int tonebank_length1;
  int tonebank_length2;
  int tonebank_length3;
  int tonebank_length4;
  int tonebank_length5;
  int tonebank_length6;
  int pb_rate0; // playback rate or 0
  int pb_rate1;
  int pb_rate2;
  int pb_rate3;
  int pb_rate4;
  int pb_rate5;
  int pb_rate6;
  int pb_ratio0; // accumulator for ratio
  int pb_ratio1;
  int pb_ratio2;
  int pb_ratio3;
  int pb_ratio4;
  int pb_ratio5;
  int pb_ratio6;
  int pb_rsc0; // pointer to current position
  int pb_rsc1;
  int pb_rsc2;
  int pb_rsc3;
  int pb_rsc4;
  int pb_rsc5;
  int pb_rsc6;
  int pb_oct0; // octave
  int pb_oct1;
  int pb_oct2;
  int pb_oct3;
  int pb_oct4;
  int pb_oct5;
  int pb_oct6;

  void set_tonebank(int bank, signed short *buffer, int length);
 protected:

 public:
  static const int MUTED=7;
  static const int OPEN=0;

  // accessors
  int Frames(){return frames;};
  int Finger(int finger);
  int Fret(){return initial_fret;};
  int Playing(){
    if (pb_rate0 == 0 &&
	pb_rate1 == 0 &&
	pb_rate2 == 0 &&
	pb_rate3 == 0 &&
	pb_rate4 == 0 &&
	pb_rate5 == 0 &&
	pb_rate6 == 0) return 0;
    return 1;
  };

 public :
  // mutators
      void Reverb(int size){reverb_max = 64 * size;};
  void Finger(int finger, int position);
  void Fret(int position){initial_fret = position;};
  void Tuning(int t[6]);
  void note_start(int chan, int note, int octave);
  
  // methods
  int Play();
  void fill_buffer();
  void write_buffer();
  void Stop();
};

#endif





