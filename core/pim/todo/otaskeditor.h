#ifndef OPIE_TASK_EDITOR_H
#define OPIE_TASK_EDITOR_H

#include <qdialog.h>

#include <opie/otodo.h>
#include <opie/otabwidget.h>

class TaskEditorOverView;
class TaskEditorAdvancedImpl;
class TaskEditorAlarms;
class ORecurranceWidget;
class QMultiLineEdit;

class OTaskEditor : public QDialog {
    Q_OBJECT
public:
    OTaskEditor(int cur);
    OTaskEditor( const OTodo& todo );
    ~OTaskEditor();

    /*
     * same as the c'tor but this gives us the
     * power to 'preload' the dialog
     */
    void init( int cur );
    void init( const OTodo& todo );

    OTodo todo()const;
private:
    void load( const OTodo& );
    void init();

    OTabWidget         *m_tab;
    TaskEditorOverView *m_overView;
    TaskEditorAdvancedImpl *m_adv;
    TaskEditorAlarms   *m_alarm;
    TaskEditorAlarms   *m_remind;
    ORecurranceWidget  *m_rec;
    int                 m_uid;

};

#endif
