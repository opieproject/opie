#include "wlanrun.h"

bool WLanRun::handlesInterface( const QString & S ) {
    return Pat.match( S ) >= 0;
}
