#include "cgotoline.h"
#include <qspinbox.h>
// copyright 2003 Craig Graham <cgraham@accessdevices.co.uk>

CGotoLine::CGotoLine(int l) : CGotoBase(0,0,true)
{
  mLine->setValue(l+1);
}

int CGotoLine::line()
{
  return mLine->value()-1;
}

void CGotoLine::keyPressEvent(QKeyEvent *e)
{
  if((e->key()==Key_Return)||(e->key()==Key_Enter))
  {
    accept();
  }else{
    QDialog::keyPressEvent(e);
  }
}
