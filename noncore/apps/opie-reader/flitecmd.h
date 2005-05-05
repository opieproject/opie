#include "outputcodec.h"

class CFliteCmd : public COutput
{
 public:
  CFliteCmd() {}
   ~CFliteCmd() {}
  QString about();
  void output(const QString& _o);
};
