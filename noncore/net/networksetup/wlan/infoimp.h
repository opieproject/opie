#ifndef INFOIMP_H
#define INFOIMP_H

#include "info.h"

class QTimer;
class WExtensions;

class WlanInfoImp : public WlanInfo {
  Q_OBJECT

public:
  WlanInfoImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );

private slots:
  void update();

private:
  //WExtensions *wExtensions;
  QTimer *timer;
  
};

#endif

// infoimp.h

