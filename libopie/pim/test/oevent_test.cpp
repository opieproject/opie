#include <qdatetime.h>

#include "../oevent.h"
#include "../odatebookaccess.h"

int main(int argc, char* argv ) {
    ODateBookAccess acc;
    if(!acc.load() ) qWarning("could not load");

    ODateBookAccess::List::Iterator it;
    ODateBookAccess::List list = acc.allRecords();

    for( it = list.begin(); it != list.end(); ++it ){
	OEvent ev = (*it);
	qWarning("Summary: %s",ev.description().latin1() );
	qWarning("Start: %s End: %s",ev.startDateTime().toString().latin1(), ev.endDateTime().toString().latin1() );
	qWarning("All Day: %d Recurrence: %d",ev.isAllDay(), ev.hasRecurrence() );
        qWarning("UID %d", ev.uid() );

    }
    QDate date1(2003,02,01 );
    QDate date2(2003,03,01 );

    OEffectiveEvent::ValueList effList = acc.effectiveEvents( date1,date2 );
    OEffectiveEvent::ValueList::Iterator effIt;

    for( effIt = effList.begin(); effIt != effList.end(); ++effIt ){
	OEffectiveEvent ef = (*effIt);
	qWarning("Summary: %s", ef.description().latin1() );
	qWarning("Date: %s", ef.date().toString().latin1() );
    }
    acc.save();

    return 0;
}
