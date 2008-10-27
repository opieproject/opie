#ifndef _DATEBOOKWEEKLSTEVENT_H
#define _DATEBOOKWEEKLSTEVENT_H

#include <opie2/oclickablelabel.h>
#include <opie2/opimoccurrence.h>

#include "weeklstview.h"

class DateBookWeekLstEvent: public Opie::Ui::OClickableLabel
{
  Q_OBJECT
public:
    DateBookWeekLstEvent(bool ampm, const Opie::OPimOccurrence &ev, 
             Opie::Datebook::WeekLstView *view, int weeklistviewconfig =1,
             QWidget* parent = 0, const char* name = 0,
             WFlags fl = 0);
signals:
    void redraw();
private:
    const Opie::OPimOccurrence event;
    Opie::Datebook::WeekLstView *weekLstView;
protected:
    void mousePressEvent( QMouseEvent *e );
    bool ampm;
};


#endif
