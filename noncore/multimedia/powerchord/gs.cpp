
//#include <FL/Fl.H>
//#include <FL/Fl_Widget.H>
//#include <FL/fl_draw.H>
#include <stdio.h>
//#include <qpixmap.h>
#include "gs.h"

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#include <errno.h>
#include <string.h>

#include <stdlib.h>

//#include <qpainter.h>


gs::gs()
{
  
  finger[0] = OPEN;
  finger[1] = OPEN;
  finger[2] = OPEN;
  finger[3] = OPEN;
  finger[4] = OPEN;
  finger[5] = OPEN;

  tuning[0] = 0;
  tuning[1] = 0;
  tuning[2] = 0;
  tuning[3] = 0;
  tuning[4] = 0;
  tuning[5] = 0;

  initial_fret = 0;

  audio_fd = -1;
  
  pb_rate0 = 0;
  pb_rate1 = 0;
  pb_rate2 = 0;
  pb_rate3 = 0;
  pb_rate4 = 0;
  pb_rate5 = 0;
  pb_rate6 = 0;

  pb_oct0 = 0;
  pb_oct1 = 0;
  pb_oct2 = 0;
  pb_oct3 = 0;
  pb_oct4 = 0;
  pb_oct5 = 0;
  pb_oct6 = 0;

  // initialise reverb buffer
  reverb = (signed short *)malloc(1024 * sizeof(signed short));

  for (reverb_ptr=0;reverb_ptr<1024;reverb_ptr++){
    reverb[reverb_ptr] = 0;
  }
  reverb_ptr = 0;
  reverb_max = 1024;

  // load sampled 'E' string
  int samplen = 25000;

  signed short *dsp_buf = (signed short *)malloc(samplen * sizeof(signed short));
  signed short *dsp_buf_ptr = dsp_buf;

  int raw_fd;

  QString path = getenv( "OPIEDIR" );
  path.append( "/share/powerchord/acguitar.raw" );

  raw_fd = open( (const char*) path, O_RDONLY);

  if (raw_fd < 0){
      fprintf(stderr, "Failed to open raw file (%s)\n", strerror(errno));
      exit(-1);
  }

  int totread = 0;
  int i;

  while (totread < samplen*2){
      int want = samplen*2 - totread;

      int numread = read(raw_fd, dsp_buf_ptr, want);
      fprintf(stderr, "read %d bytes\n", numread);
      totread += numread;
      dsp_buf_ptr += numread/2;

      if (numread == 0){
	  fprintf(stderr, "failed to read bytes\n");
	  exit(-1);
      }    
  }

  close(raw_fd);

  // scale down a bit for mixing
  for (i=0;i<samplen;i++){
    dsp_buf[i] /= 6;
  }
  
  set_tonebank(0, dsp_buf, samplen);
  set_tonebank(1, dsp_buf, samplen);
  set_tonebank(2, dsp_buf, samplen);
  set_tonebank(3, dsp_buf, samplen);
  set_tonebank(4, dsp_buf, samplen);
  set_tonebank(5, dsp_buf, samplen);
  set_tonebank(6, dsp_buf, samplen);
  
}

void gs::set_tonebank(int tb, signed short *buf, int length)
{
    switch(tb){
    case 0:
	tonebank0 = buf;
	tonebank_length0 = length;
	break;
    case 1:
	tonebank1 = buf;
	tonebank_length1 = length;
	break;
    case 2:
	tonebank2 = buf;
	tonebank_length2 = length;
	break;
    case 3:
	tonebank3 = buf;
	tonebank_length3 = length;
	break;
    case 4:
	tonebank4 = buf;
	tonebank_length4 = length;
	break;
    case 5:
	tonebank5 = buf;
	tonebank_length5 = length;
	break;
    case 6:
	tonebank6 = buf;
	tonebank_length6 = length;
	break;
	
    }
}


void gs::Finger(int f, int position){
  if (f < 0 || f > 5){
    fprintf(stderr, "Error - finger2 value was %d\n", f);
    return;
  }

  finger[f] = position;
}

void gs::Tuning(int t[6]){
  for (int i=0;i<6;i++){
    tuning[i] = t[i];
  }
}

// length in ps (seconds x 10^-9) of the period of a note.
// we use these as ratios in a breshenham-like algorithm to
// scale a deep note to a higher pitch
// They are derived from f(A) = 440Hz and multiply each successive
// semitone by the 12th root of 2 (such that after 12 multiplications for
// 12 semitones you have a note exactly 2x the frequency of the initial one,
// - an octave higher in other words.)

int gs::note_periods[12] = {
90703,
85612,
80802,
76272,
71991,
67950,
64137,
60537,
57139,
53932,
50905,
48048
};

int gs::octave_step[6] = {
  1,
  2,
  4,
  8,
  16,
  32
};

int gs::Play(){
    int format;
    int channels;
    int speed;

    frames = 0;

    if (audio_fd == -1){
	if ( (audio_fd = open("/dev/dsp", O_WRONLY, 0) ) == -1){    
	  audio_fd = -1;
	  return 1;
	}
	
	format = AFMT_S16_NE;
	
	if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format) == -1){
	    fprintf(stderr, "Error SNDCTL DSP SETFMT, %s\n", strerror(errno));
	    exit(0);
	}

	channels = 1;

	if (ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &channels) == -1){
	    fprintf(stderr, "Error SNDCTL DSP CHANNELS, %s\n", strerror(errno));
	    exit(0);
	}
	
	speed = 11025;
	
	if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &speed) == -1){
	    fprintf(stderr, "Error SNDCTL DSP SPEED, %s\n", strerror(errno));
	    exit(0);
	}

	// buffering q's
	//	audio_buf_info info;
	//	if (ioctl(audio_fd, SNDCTL_DSP_GETOSPACE, &info) == -1){
	//	    fprintf(stderr, "Error SNDCTL DSP GETOSPACE, %s\n", strerror(errno));
	//	    exit(0);
	//	}
	//	fprintf(stderr, "fragments %d\nfragstotal %d\nfragsize %d\nbytes %d\n", info.fragments, info.fragstotal, info.fragsize, info.bytes);
	
	
	
// audio math.
// A4 = 440Hz
// +1 octave = 2x freq
// -1 octave = /2 freq.
// +1 semitone = *= 12 root 2;
// ie. * 1.059463094

// tones, approx Hz, samples at 11025
// A4    440  25
// A#4   466  24
// B4    494  22
// C4    523  21
// C#4   554  20
// D4    587  19
// D#4   622  18
// E4    659  17
// F4    698  16
// F#4   740  15
// G4    784  14
// G#4   831  13

	
    }
    else{
	fprintf(stderr, "Already playing\n");
    }
    
    return 0;
}

void gs::note_start(int chan, int note, int octave)
{
    switch (chan){
	
    case 0:
	pb_rate0 = note_periods[note];
	pb_oct0 = octave_step[octave];
	pb_ratio0 = 0;
	pb_rsc0 = 0;
	break;
    case 1:
	pb_rate1 = note_periods[note];
	pb_oct1 = octave_step[octave];
	pb_ratio1 = 0;
	pb_rsc1 = 0;
	break;
    case 2:
	pb_rate2 = note_periods[note];
	pb_oct2 = octave_step[octave];
	pb_ratio2 = 0;
	pb_rsc2 = 0;
	break;
    case 3:
	pb_rate3 = note_periods[note];
	pb_oct3 = octave_step[octave];
	pb_ratio3 = 0;
	pb_rsc3 = 0;
	break;
    case 4:
	pb_rate4 = note_periods[note];
	pb_oct4 = octave_step[octave];
	pb_ratio4 = 0;
	pb_rsc4 = 0;
	break;
    case 5:
	pb_rate5 = note_periods[note];
	pb_oct5 = octave_step[octave];
	pb_ratio5 = 0;
	pb_rsc5 = 0;
	break;
    case 6:
	pb_rate6 = note_periods[note];
	pb_oct6 = octave_step[octave];
	pb_ratio6 = 0;
	pb_rsc6 = 0;
	break;
    default:
	fprintf(stderr, "Bad channel\n");
	exit(-1);
    }
    
  
}

void gs::write_buffer(){
  int num_read;
  num_read = write(audio_fd, (void *)audio_buf, BUFSIZE*2);
  //  fprintf(stderr, "Wrote %d bytes\n", num_read);
}

void gs::fill_buffer()
{
  frames ++;

  int i;

  for (i=0;i<BUFSIZE;i++){
      
      audio_buf[i] = 0;
      
      if (pb_rate0){
	  audio_buf[i] += tonebank0[pb_rsc0];
	  pb_rsc0 += pb_oct0;
	  pb_ratio0 += 90703;
	  pb_ratio0 -= pb_rate0;
	  if (pb_ratio0 >= pb_rate0){
	      pb_rsc0 += pb_oct0;
	      pb_ratio0 -= pb_rate0;
	      
	  }
	  if (pb_rsc0 >= tonebank_length0) pb_rate0 = 0;
      }

      if (pb_rate1){
	  audio_buf[i] += tonebank1[pb_rsc1];
	  pb_rsc1 += pb_oct1;
	  pb_ratio1 += 90703;
	  pb_ratio1 -= pb_rate1;
	  if (pb_ratio1 >= pb_rate1){
	      pb_rsc1 += pb_oct1;
	      pb_ratio1 -= pb_rate1;
	      
	  }
	  if (pb_rsc1 >= tonebank_length1) pb_rate1 = 0;
      }

      if (pb_rate2){
	  audio_buf[i] += tonebank2[pb_rsc2];
	  pb_rsc2 += pb_oct2;
	  pb_ratio2 += 90703;
	  pb_ratio2 -= pb_rate2;
	  if (pb_ratio2 >= pb_rate2){
	      pb_rsc2 += pb_oct2;
	      pb_ratio2 -= pb_rate2;
	      
	  }
	  if (pb_rsc2 >= tonebank_length2) pb_rate2 = 0;
      }

      if (pb_rate3){
	  audio_buf[i] += tonebank3[pb_rsc3];
	  pb_rsc3 += pb_oct3;
	  pb_ratio3 += 90703;
	  pb_ratio3 -= pb_rate3;
	  if (pb_ratio3 >= pb_rate3){
	      pb_rsc3 += pb_oct3;
	      pb_ratio3 -= pb_rate3;
	      
	  }
	  if (pb_rsc3 >= tonebank_length3) pb_rate3 = 0;
      }

      if (pb_rate4){
	  audio_buf[i] += tonebank4[pb_rsc4];
	  pb_rsc4 += pb_oct4;
	  pb_ratio4 += 90703;
	  pb_ratio4 -= pb_rate4;
	  if (pb_ratio4 >= pb_rate4){
	      pb_rsc4 += pb_oct4;
	      pb_ratio4 -= pb_rate4;
	      
	  }
	  if (pb_rsc4 >= tonebank_length4) pb_rate4 = 0;
      }

      if (pb_rate5){
	  audio_buf[i] += tonebank5[pb_rsc5];
	  pb_rsc5 += pb_oct5;
	  pb_ratio5 += 90703;
	  pb_ratio5 -= pb_rate5;
	  if (pb_ratio5 >= pb_rate5){
	      pb_rsc5 += pb_oct5;
	      pb_ratio5 -= pb_rate5;
	      
	  }
	  if (pb_rsc5 >= tonebank_length5) pb_rate5 = 0;
      }

      if (pb_rate6){
	  audio_buf[i] += tonebank6[pb_rsc6];
	  pb_rsc6 += pb_oct6;
	  pb_ratio6 += 90703;
	  pb_ratio6 -= pb_rate6;
	  if (pb_ratio6 >= pb_rate6){
	      pb_rsc6 += pb_oct6;
	      pb_ratio6 -= pb_rate6;
	      
	  }
	  if (pb_rsc6 >= tonebank_length6) pb_rate6 = 0;
      }

      // do reverb
      signed short rtmp = reverb[reverb_ptr];
      reverb[reverb_ptr] /= 2;
      reverb[reverb_ptr] += audio_buf[i]/4;
      audio_buf[i] += rtmp;
      reverb_ptr++;
      if (reverb_ptr >= reverb_max) reverb_ptr = 0;
  }
}


void gs::Stop(){
    if (audio_fd == -1){
	fprintf(stderr, "Already stopped\n");
    }
    else{
	//ioctl(audio_fd, SNDCTL_DSP_RESET, 0);
	
	close(audio_fd);
	audio_fd = -1;
    }
    
}

gs::~gs()
{}
