
//#include <FL/Fl.H>
//#include <FL/Fl_Widget.H>
//#include <FL/fl_draw.H>
#include <stdio.h>
//#include <qpixmap.h>
#include "gt.h"

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#include <errno.h>
#include <string.h>

#include <stdlib.h>

gt::gt()
{
  
  audio_fd = -1;
  
}

void gt::Stop(){
}

void gt::Listen(){
}

void gt::read_buffer(){
}

void gt::process_buffer(){
}

gt::~gt()
{}
