#ifndef MINIKDE_KCOLORDIALOG_H
#define MINIKDE_KCOLORDIALOG_H

#include <qcolor.h>

class KColorDialog
{
  public:
    enum { Accepted };
  
    static int getColor( const QColor & );
};

#endif
