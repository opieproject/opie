#ifndef MINIKDE_KFONTDIALOG_H
#define MINIKDE_KFONTDIALOG_H

#include <qfont.h>

class KFontDialog
{
  public:
    enum { Accepted };
    
    static int getFont( const QFont & );
};

#endif
