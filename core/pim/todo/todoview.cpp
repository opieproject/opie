#include "mainwindow.h"

#include "todoview.h"

using namespace Todo;




TodoView::TodoView( MainWindow* win )
    : m_main( win )
{
    m_asc = false;
    m_sortOrder = -1;
}
TodoView::~TodoView() {
};
MainWindow* TodoView::todoWindow() {
    return m_main;
}

OTodo TodoView::event(int uid ) {
    return m_main->event( uid );
}
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
void TodoView::update(int, const SmallTodo&  ) {
    //m_main->slotUpate1( uid, to );
}
void TodoView::update(int , const OTodo& ev ) {
    m_main->updateTodo( ev );
}
void TodoView::showTodo( int uid ) {
    m_main->slotShow( uid );
}
void TodoView::edit( int uid ) {
    m_main->slotEdit( uid );
}
void TodoView::remove( int uid ) {
    m_main->m_todoMgr.remove( uid );
}
void TodoView::complete( int uid ) {
    m_main->slotComplete( uid );
}
void TodoView::complete( const OTodo& ev ) {
    m_main->slotComplete( ev );
}
