#include <qstringlist.h>
#include "GUIUtils.h"

bool validIP( const QString & S ) {

    QStringList SL = QStringList::split( '.', S, TRUE );
    if( SL.count() != 4  )
      return 0;

    for( int i = 0; i < 4 ; i ++ ) {
      if( SL[i].isEmpty() )
        return 0;
    }

    short x = SL[0].toShort();

    if( x < 1 || x > 255 )
      return 0;

    for( int i = 1; i < 4 ; i ++ ) {
      x = SL[i].toShort();
      if( x < 0 || x > 255 )
        return 0;
    }
    return 1;

}
