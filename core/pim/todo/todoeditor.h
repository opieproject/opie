#ifndef OPIE_TODO_EDITOR_H
#define OPIE_TODO_EDITOR_H

#include <opie2/opimtodo.h>


class OTaskEditor;
namespace Todo {
    class Editor {
    public:
        Editor();
        ~Editor();

        OPimTodo newTodo( int currentCatId,
                           QWidget* par );
        OPimTodo edit( QWidget* par,
                        const OPimTodo& ev = OPimTodo() );


        bool accepted()const;
    protected:
        OTaskEditor* self();
    private:
        bool m_accepted: 1;
        OTaskEditor* m_self;
    };
};

#endif
