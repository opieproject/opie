#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include "krfboptions.h"

KRFBOptions::KRFBOptions()
{
  Config config( "keypebble" );
  readSettings( &config );
}

KRFBOptions::~KRFBOptions()
{
  Config config( "keypebble" );
  writeSettings( &config );
}

void KRFBOptions::readSettings( Config *config )
{
  hexTile = config->readBoolEntry( "HexTile" );
  corre = config->readBoolEntry( "CORRE" );
  rre = config->readBoolEntry( "RRE" );
  copyrect = config->readBoolEntry( "CopyRect", true );
  colors256 = config->readBoolEntry( "Colors256" );
  shared = config->readBoolEntry( "Shared" );
  readOnly = config->readBoolEntry( "ReadOnly" );
  updateRate = config->readNumEntry( "UpdateRate", 50 );
}

void KRFBOptions::writeSettings( Config *config )
{
  config->writeEntry( "HexTile", hexTile );
  config->writeEntry( "CORRE", corre );
  config->writeEntry( "RRE", rre );
  config->writeEntry( "CopyRect", copyrect );
  config->writeEntry( "Colors256", colors256 );
  config->writeEntry( "Shared", shared );
  config->writeEntry( "ReadOnly", readOnly );
  config->writeEntry( "UpdateRate", updateRate );
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
