//
// copyright 2003 Craig Graham <cgraham@accessdevices.co.uk>
#include "cgotobase.h"
class CGotoLine : public CGotoBase {
public:
  CGotoLine(int line);
  int line();
protected:
  virtual void keyPressEvent(QKeyEvent *e);
};
