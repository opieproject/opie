#include "mainwindow.h"

#include "todoview.h"

using namespace Todo;

TodoView::TodoView( MainWindow* win )
    : m_main( win )
{
    hack = new InternQtHack;
    m_asc = false;
    m_sortOrder = -1;
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
    QObject::connect( hack, SIGNAL(update(int, const OTodo& ) ),
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

OTodo TodoView::event(int uid ) { return m_main->event( uid ); }
OTodoAccess::List TodoView::list(){
    todoWindow()->updateList();
    return todoWindow()->list();
}
OTodoAccess::List TodoView::sorted()const{
    return m_sort;
}
void TodoView::sort() {
    m_sort = todoWindow()->sorted(m_asc,m_sortOrder );
    qWarning("m_sort.count() = %d", m_sort.count() );
}
void TodoView::sort(int sort) {
    m_sort = todoWindow()->sorted(m_asc, m_sortOrder, sort );
}
void TodoView::setSortOrder( int order ) {
    m_sortOrder = order;
}
void TodoView::setAscending( bool b ) {
    qWarning("setAscending %d", b );
    m_asc = b;
}
void TodoView::update(int uid, const SmallTodo& to ) {
    //m_main->slotUpate1( uid, to );
}
void TodoView::update(int uid, const OTodo& ev ) {
    m_main->updateTodo( ev );
}
