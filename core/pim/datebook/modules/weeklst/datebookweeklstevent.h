#ifndef _DATEBOOKWEEKLSTEVENT_H
#define _DATEBOOKWEEKLSTEVENT_H

#include <opie2/oclickablelabel.h>

#include <qpe/event.h>

class DateBookWeekLstEvent: public Opie::Ui::OClickableLabel
{
  Q_OBJECT
public:
    DateBookWeekLstEvent(const EffectiveEvent &ev, int weeklistviewconfig =1,
             QWidget* parent = 0, const char* name = 0,
             WFlags fl = 0);
signals:
    void editEvent(const Event &e);
    void duplicateEvent(const Event &e);
    void removeEvent(const Event &e);
    void beamEvent(const Event &e);
    void redraw();
private slots:
    void editMe();
    void duplicateMe();
    void deleteMe();
    void beamMe();
private:
    const EffectiveEvent event;
    QPopupMenu* popmenue;
protected:
    void mousePressEvent( QMouseEvent *e );
};


#endif
