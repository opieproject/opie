#include <qlineedit.h>

#include "knumvalidator.h"

KIntValidator::KIntValidator( int a, int b, QLineEdit *c )
  : QIntValidator( a, b, c )
{
}
