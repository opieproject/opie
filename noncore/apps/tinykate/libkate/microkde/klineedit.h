#ifndef MINIKDE_KLINEEDIT_H
#define MINIKDE_KLINEEDIT_H

#include <qlineedit.h>

class KLineEdit : public QLineEdit
{
  public:
    KLineEdit( QWidget *parent=0, const char *name=0 ) :
      QLineEdit( parent, name ) {}
};

#endif
