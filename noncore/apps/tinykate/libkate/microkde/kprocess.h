#ifndef MINIKDE_KPROCESS_H
#define MINIKDE_KPROCESS_H

#include <qobject.h>

class KProcess : public QObject
{
  public:
    void clearArguments();
    
    KProcess & operator<<( const QString & );
    
    bool start();
};

#endif
