#include "outputcodec.h"
extern "C"
{
#include <flite.h>
  cst_voice *register_cmu_us_kal();
}

class CFliteDyn : public COutput
{
  static cst_voice *m_voice;
 public:
  CFliteDyn();
   ~CFliteDyn() {}
  QString about();
  void output(const QString& _o);
};
