
#ifndef __GT_H
#define __GT_H

#include <qobject.h>
// sound includes
#include <sys/soundcard.h>

#define BUFSIZE 256
#define NUMCHANS 7

class gt : public QObject {

    Q_OBJECT

 public:
  gt();
  ~gt();

 private:

  int audio_fd;
  signed short audio_buf[BUFSIZE];

  int note;
  int tune;

  short min;
  short max;
  int phase;
  
 protected:

 public:

  // accessors
  int Tuning(){return tune;};
  int Note(){return note;};
  int Listening(){return (audio_fd == -1);};

 public slots :
  // methods
  void Listen();
  void process_buffer();
  void read_buffer();
  void Stop();
};

#endif





