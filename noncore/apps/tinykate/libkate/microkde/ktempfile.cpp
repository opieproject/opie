#include <qtextstream.h>

#include "ktempfile.h"

KTempFile::KTempFile()
{
}

KTempFile::KTempFile( const QString &filename, const QString &extension )
{
}

void KTempFile::setAutoDelete( bool )
{
}

QString KTempFile::name()
{
  return QString::null;
}

QTextStream *KTempFile::textStream()
{
  return 0;
}
