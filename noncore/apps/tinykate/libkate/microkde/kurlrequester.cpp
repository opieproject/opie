#include "klineedit.h"

#include "kurlrequester.h"

KURLRequester::KURLRequester( QWidget *parent ) :
  QWidget( parent )
{
  mLineEdit = new KLineEdit( parent );
}

KLineEdit *KURLRequester::lineEdit()
{
  return mLineEdit;
}
