#include "mainwindow.h"

#include "todoview.h"

using namespace Todo;

TodoView::TodoView( MainWindow* win )
    : m_main( win )
{
    hack = new InternQtHack;
}
TodoView::~TodoView() {
    delete hack;
};

void TodoView::connectShow(QObject* obj,
                           const char* slot ) {
    QObject::connect( hack, SIGNAL(showTodo(int) ),
                      obj, slot );
}
void TodoView::connectEdit( QObject* obj,
                            const char* slot ) {
    QObject::connect( hack, SIGNAL(edit(int) ),
                      obj, slot );
}
void TodoView::connectUpdateSmall( QObject* obj,
                                   const char* slot ) {
    QObject::connect( hack, SIGNAL(update(int, const Todo::SmallTodo& ) ),
                      obj, slot );
}
void TodoView::connectUpdateBig( QObject* obj,
                       const char* slot ) {
    QObject::connect( hack, SIGNAL(update(int, const Opie::ToDoEvent& ) ),
                      obj, slot );
}
void TodoView::connectUpdateView( QObject* obj,
                        const char* slot ) {
    QObject::connect( hack, SIGNAL(update(QWidget*) ),
                      obj, slot );
}
void TodoView::connectRemove( QObject* obj,
                              const char* slot ) {
    QObject::connect( hack, SIGNAL(remove(int) ),
                      obj, slot );
}
MainWindow* TodoView::todoWindow() {
    return m_main;
}

ToDoEvent TodoView::event(int uid ) { return m_main->event( uid ); }
ToDoDB::Iterator TodoView::begin() { return m_main->begin(); }
ToDoDB::Iterator TodoView::end() { return m_main->end(); }

void TodoView::update(int uid, const SmallTodo& to ) {
    //m_main->slotUpate1( uid, to );
}
void TodoView::update(int uid, const ToDoEvent& ev ) {
    m_main->updateTodo( ev );
}
