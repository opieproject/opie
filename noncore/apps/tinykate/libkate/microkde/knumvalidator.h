#ifndef MINIKDE_KNUMVALIDATOR_H
#define MINIKDE_KNUMVALIDATOR_H

#include <qvalidator.h>

class QLineEdit;

class KIntValidator : public QIntValidator
{
  public:
    KIntValidator( int, int, QLineEdit * );
};

#endif
