#include <qdatetime.h>

#include "../oevent.h"

int main(int argc, char* argv ) {
    OEvent ev;
    ev.setUid( 20 );

    ev.setDescription( "Foo" );

    OEvent ev2 = ev;
    ev2.setDescription("Foo2");
    qWarning("%s", ev2.description().latin1() );
    qWarning("%s", ev.description().latin1() );

    QDateTime time = QDateTime::currentDateTime();
    ev2.setStartDateTime( time );
    ev2.setTimeZone( "Europe/London" );

    qWarning("%s", ev2.startDateTime().toString().latin1() );
    qWarning("%s", ev2.startDateTimeInZone().toString().latin1() );
    return 0;
}
