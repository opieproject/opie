#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include "krfbserver.h"

KRFBServer::KRFBServer()
{
	QString name;
	QString hostname;
	QString password;
	display=0;

	scaleFactor=1;

  hexTile=0;
  corre=0;
  rre=0;
  copyrect=1;
    
  colors256=1;
  shared=0;
  readOnly=0;
  deIconify=0;

  updateRate=0;
}
KRFBServer::~KRFBServer()
{
}

int KRFBServer::encodings()
{
  // Initially one because we always support raw encoding
  int count = 1;

  count += hexTile ? 1 : 0;
  count += corre ? 1 : 0;
  count += rre ? 1 : 0;
  count += copyrect ? 1 : 0;

  return count;
}
