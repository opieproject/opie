#ifndef OPIE_TODO_EDIT_H
#define OPIE_TODO_EDIT_H

#include <opie2/opimtodo.h>

namespace Todo {
    class Edit {
    public:
        Edit();
        virtual ~Edit();

        ToDoEvent newEvent( const OPimTodo& ) = 0;
        ToDoEvent editEvent( const OPimTodo& ) = 0;
    };
};

#endif
