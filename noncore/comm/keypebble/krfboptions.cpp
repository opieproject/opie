#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include "krfboptions.h"

KRFBOptions::KRFBOptions()
{
  readSettings();
}

KRFBOptions::~KRFBOptions()
{
  writeSettings();
}

void KRFBOptions::readSettings()
{
  Config config( "keypebble" );
  config.setGroup("Settings");
  hexTile = config.readBoolEntry( "HexTile", 0 );
  corre = config.readBoolEntry( "CORRE", 0 );
  rre = config.readBoolEntry( "RRE", 0 );
  copyrect = config.readBoolEntry( "CopyRect", 1 );
  colors256 = config.readBoolEntry( "Colors256", 0 );
  shared = config.readBoolEntry( "Shared", 0 );
  readOnly = config.readBoolEntry( "ReadOnly", 0 );
  updateRate = config.readNumEntry( "UpdateRate", 50 );
  deIconify = config.readBoolEntry( "DeIconify", 0 );
}

void KRFBOptions::writeSettings()
{
  Config config( "keypebble" );
  config.setGroup("Settings");
  config.writeEntry( "HexTile", hexTile );
  config.writeEntry( "CORRE", corre );
  config.writeEntry( "RRE", rre );
  config.writeEntry( "CopyRect", copyrect );
  config.writeEntry( "Colors256", colors256 );
  config.writeEntry( "Shared", shared );
  config.writeEntry( "ReadOnly", readOnly );
  config.writeEntry( "UpdateRate", updateRate );
  config.writeEntry( "DeIconify", deIconify );
}

int KRFBOptions::encodings()
{
  // Initially one because we always support raw encoding
  int count = 1;

  count += hexTile ? 1 : 0;
  count += corre ? 1 : 0;
  count += rre ? 1 : 0;
  count += copyrect ? 1 : 0;

  return count;
}
