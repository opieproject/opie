#ifndef MINIKDE_KCOLORDIALOG_H
#define MINIKDE_KCOLORDIALOG_H

#include <qcolor.h>
#include <opie/colordialog.h>

class KColorDialog : OColorDialog
{
    Q_OBJECT
  public:
    enum { Accepted };
  
    static int getColor( const QColor & );
};

#endif
