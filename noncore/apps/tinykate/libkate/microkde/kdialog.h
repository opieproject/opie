#ifndef MINIKDE_KDIALOG_H
#define MINIKDE_KDIALOG_H

#include <qdialog.h>

class KDialog : public QDialog
{
  public:
    KDialog( QWidget *parent=0, const char *name=0, bool modal=true ) :
      QDialog( parent, name, modal ) {}

    static int spacingHint() { return 3; }
    static int marginHint() { return 3; }
};

#endif
