#ifndef TABAPPLNK_H
#define TABAPPLNK_H

#include <qpe/applnk.h>

class TabAppLnk : public AppLnk {

public:
  TabAppLnk( const QString & file) :AppLnk(file){ };

  QString pixmapString() { return mIconFile; };

};

#endif

// tabapplnk.h

