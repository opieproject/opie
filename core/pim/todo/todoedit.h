#ifndef OPIE_TODO_EDIT_H
#define OPIE_TODO_EDIT_H

#include <opie/otodo.h>

namespace Todo {
    class Edit {
    public:
        Edit();
        virtual ~Edit();

        ToDoEvent newEvent( const OTodo& ) = 0;
        ToDoEvent editEvent( const OTodo& ) = 0;
    };
};

#endif
