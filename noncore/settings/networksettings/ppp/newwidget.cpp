/////////////////////////////////////////////////////////////////////////////
//
// functions generating layout-aware widgets
//
/////////////////////////////////////////////////////////////////////////////

#include "newwidget.h"

QLineEdit *newLineEdit(int visiblewidth, QWidget *parent) {
  QLineEdit *l = new QLineEdit(parent);
  if(visiblewidth == 0)
    l->setMinimumWidth(l->sizeHint().width());
  else
    l->setFixedWidth(l->fontMetrics().width('H') * visiblewidth);
  
  return l;
}
