#ifndef MINIKDE_KTEMPFILE_H
#define MINIKDE_KTEMPFILE_H

#include <qstring.h>

class QTextStream;

class KTempFile
{
  public:
    KTempFile();
    KTempFile( const QString &filename, const QString &extension );
  
    void setAutoDelete( bool );
    QString name();
    
    QTextStream *textStream();
};

#endif
