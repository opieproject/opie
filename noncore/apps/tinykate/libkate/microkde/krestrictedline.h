#ifndef MINIKDE_KRESTRICTEDLINE_H
#define MINIKDE_KRESTRICTEDLINE_H

#include "klineedit.h"

class KRestrictedLine : public KLineEdit
{
  public:
    KRestrictedLine( QWidget *parent, const char *, const QString & ) :
        KLineEdit( parent ) {}
};

#endif
