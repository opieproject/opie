#ifndef OPIE_TASK_EDITOR_H
#define OPIE_TASK_EDITOR_H

#include <qdialog.h>

#include <opie/otodo.h>
#include <opie/otabwidget.h>

class TaskEditorOverViewImpl;
class TaskEditorAdvanced;
class TaskEditorAlarms;
class ORecurranceWidget;
class QMultiLineEdit;
class OTaskEditor : public QDialog {
    Q_OBJECT
public:
    OTaskEditor(int cur);
    OTaskEditor( const OTodo& todo );
    ~OTaskEditor();

    OTodo todo()const;
private:
    void load( const OTodo& );
    void init();

    OTabWidget *m_tab;
    TaskEditorOverViewImpl* m_overView;
    TaskEditorAdvanced *m_adv;
    TaskEditorAlarms *m_alarm;
    TaskEditorAlarms* m_remind;
    ORecurranceWidget* m_rec;
    QMultiLineEdit* m_line;
    int m_uid;

};

#endif
