#include "kprocess.h"

void KProcess::clearArguments()
{
}
    
KProcess & KProcess::operator<<( const QString & )
{
  return *this;
}
    
bool KProcess::start()
{
  return false;
}
