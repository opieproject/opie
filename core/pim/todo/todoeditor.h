#ifndef OPIE_TODO_EDITOR_H
#define OPIE_TODO_EDITOR_H

#include <opie2/opimtodo.h>


class OTaskEditor;
namespace Todo {
    class Editor {
    public:
        Editor();
        ~Editor();

        Opie::OPimTodo newTodo( int currentCatId,
                           QWidget* par );
        Opie::OPimTodo edit( QWidget* par,
                             const Opie::OPimTodo& ev = Opie::OPimTodo() );


        bool accepted()const;
    protected:
        OTaskEditor* self();
    private:
        bool m_accepted: 1;
        OTaskEditor* m_self;
    };
};

#endif
