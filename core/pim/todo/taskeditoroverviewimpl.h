#ifndef OPIE_TASK_EDITOR_VIEW_IMPL_H
#define OPIE_TASK_EDITOR_VIEW_IMPL_H

#include <qsize.h>

#include <opie/otodo.h>

#include "taskeditoroverview.h"

class DateBookMonth;
class QPushButton;
class QPopupMenu;
class TaskEditorOverViewImpl : public TaskEditorOverView {
    Q_OBJECT
public:
    TaskEditorOverViewImpl(QWidget* parent, const char* name = 0);
    ~TaskEditorOverViewImpl();

    void load( const OTodo& );
    void save( OTodo& );
signals:
    void recurranceEnabled( bool );

private:
    void init();
    void popup(QPushButton*, QPopupMenu*);
    DateBookMonth* m_dueBook;
    DateBookMonth* m_startBook;
    DateBookMonth* m_compBook;
    QDate m_start;
    QDate m_comp;
    QDate m_due;
    QPopupMenu* m_startPop;
    QPopupMenu* m_compPop;
    QPopupMenu* m_duePop;
    bool m_bDue : 1;

private slots:
    void slotStartChecked();
    void slotStartChanged(int, int, int );
    void slotDueChecked();
    void slotDueChanged(int, int, int );
    void slotCompletedChecked();
    void slotCompletedChanged(int, int, int );

    void hackySlotHack1();
    void hackySlotHack2();
    void hackySlotHack3();

    void slotRecClicked();
};


#endif
