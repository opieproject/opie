#ifndef OPIE_TODO_EDITOR_H
#define OPIE_TODO_EDITOR_H

#include <opie/todoevent.h>

using namespace Opie;

namespace Todo {
    class Editor {
    public:
        Editor();
        ~Editor();

        ToDoEvent newTodo( int currentCatId,
                           QWidget* par );
        ToDoEvent edit( QWidget* par,
                        const ToDoEvent& ev = ToDoEvent() );


        bool accepted()const;
    private:
        bool m_accepted: 1;
    };
};

#endif
