#include "datebookweeklstevent.h"
#include "datebooktypes.h"
#include "view.h"

#include <opie2/odebug.h>

#include <qpe/ir.h>

#include <qstring.h>
#include <qpopupmenu.h>

using namespace Opie;
using namespace Opie::Datebook;

DateBookWeekLstEvent::DateBookWeekLstEvent(bool ap, const OPimOccurrence &ev,
                        WeekLstView *view,
                        int weeklistviewconfig,
                        QWidget* parent,
                        const char* name,
                        WFlags fl ) : OClickableLabel(parent,name,fl), event(ev), ampm(ap)
{
    // old values... lastday = "__|__", middle="   |---", Firstday="00:00",
    QString s,start,middle,end,day;

    weekLstView = view;

    odebug << "weeklistviewconfig=" << weeklistviewconfig << oendl;
    if(weeklistviewconfig==NORMAL) { // "Normal", only display start time.
        if ( ampm ) {
            int shour = ev.startTime().hour();
            int smin = ev.startTime().minute();
            if ( shour >= 12 ) {
                if ( shour > 12 ) {
                    shour -= 12;
                }
                start.sprintf( "%.2d:%.2d PM", shour, smin );
                day.sprintf("%.2d:%.2d PM",shour,smin);
            }
            else {
                if ( shour == 0 ) {
                    shour = 12;
                }
                start.sprintf( "%.2d:%.2d AM", shour, smin );
                day.sprintf("%.2d:%.2d AM",shour,smin);
            }
        }
        else {
            start.sprintf("%.2d:%.2d",ev.startTime().hour(),ev.startTime().minute());
            day.sprintf("%.2d:%.2d",ev.startTime().hour(),ev.startTime().minute());
        }
        middle.sprintf("   |---");
        end.sprintf("__|__");
    }
    else if(weeklistviewconfig==EXTENDED) { // Extended mode, display start and end times.
        start.sprintf("%.2d:%.2d-",ev.startTime().hour(),ev.startTime().minute());
        middle.sprintf("<--->");
        end.sprintf("-%.2d:%.2d",ev.endTime().hour(),ev.endTime().minute());
        day.sprintf("%.2d:%.2d-%.2d:%.2d",ev.startTime().hour(),ev.startTime().minute(),ev.endTime().hour(),ev.endTime().minute());
    }

    if( !ev.isAllDay() ) {
        OPimOccurrence::Position pos = ev.position();
        if( pos == OPimOccurrence::StartEnd ) {  // day event.
            s = day;
        }
        else if( pos == OPimOccurrence::Start ) {  // start event.
            s = start;
        }
        else if( pos == OPimOccurrence::End ) { // end event.
            s = end;
        }
        else {    // middle day.
            s = middle;
        }
    }
    else {
        s="";
    }
    setText(QString(s) + " " + View::occurrenceDesc( ev ) );
//  connect(this, SIGNAL(clicked()), this, SLOT(editMe()));
    setAlignment( int( QLabel::WordBreak | QLabel::AlignLeft ) );
}

void DateBookWeekLstEvent::mousePressEvent( QMouseEvent *e )
{
    if (!event.toEvent().isValidUid()) {
        // this is just such a holiday event.
        return;
    }

    weekLstView->popup( event, e->globalPos() );
}
