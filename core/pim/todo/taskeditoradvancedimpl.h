#ifndef OPIE_TASK_EDITOR_ADVANCED_IMPL_H
#define OPIE_TASK_EDITOR_ADVANCED_IMPL_H

#include "taskeditoradvanced.h"

/**
 * This is the implementation of the Opie Task Editor Advanced tab
 * it features the State!
 *                 MaintainerMode
 *                 Description
 */
class QMultiLineEdit;
class OTodo;
class TaskEditorAdvancedImpl : public TaskEditorAdvanced {
    Q_OBJECT
public:
    TaskEditorAdvancedImpl( QWidget* parent = 0, const char* name = 0 );
    ~TaskEditorAdvancedImpl();

    /*
     * I could have a struct which returns a QWidget*
     * load and save to a OTodo
     * and use multiple inheretence with all other widgets
     * and then simply iterate over the list of structs
     * this way I could easily have plugins for the whole editor....
     * but I do not do it -zecke
     */
    void load( const  OTodo& );
    void save( OTodo& );

private:
    void initUI();
    QMultiLineEdit* m_edit;
};


#endif
