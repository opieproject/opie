#ifndef PPPIMP_H
#define PPPIMP_H

#include "ppp.h"

class PPPConfigureImp : public PPP {

Q_OBJECT
	
public:
  PPPConfigureImp(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
  void setPeer(const QString &peer);

protected:
  void accept();

private:
  QString peer;
  
};

#endif

// pppconfigureimp.h

