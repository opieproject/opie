#include "flitedyn.h"

cst_voice *CFliteDyn::m_voice = NULL;

CFliteDyn::CFliteDyn()
{
  if (m_voice == NULL)
    {
      qDebug("Constucting flite");
      flite_init();
      m_voice = register_cmu_us_kal();
    }
}

QString CFliteDyn::about()
{
  return QString("FliteDyn output codec (c) Tim Wentford\n");
}

void CFliteDyn::output(const QString& _o)
{
  float tm = flite_text_to_speech((const char*)_o, m_voice, "play");
  qDebug("Produced %g seconds of speech", tm);
}

extern "C"
{
  COutput* newcodec() { return new CFliteDyn; }
}
