
#include <opie/odatebookaccess.h>

#include "bookmanager.h"
#include "mainwindow.h"
#include "view.h"

using namespace Datebook;

View::View( MainWindow* window, QWidget* )    {
    m_win = window;
}
View::~View() {
}
QDate View::dateFromWeek( int week, int year, bool startOnMon ) {

}
bool View::calcWeek( const QDate& d, int & week, int & year, bool ) {

}
void View::loadConfig( Config* conf ) {
    doLoadConfig( conf );
}
void View::saveConfig( Config* conf ) {
    doSaveConfig( conf );
}
void View::popup( int uid ) {
    m_win->viewPopup( uid );
}
void View::add( const QDate& start ) {
    m_win->viewAdd( start );
}
void View::add( const QDateTime& start, const QDateTime& end ) {
    m_win->viewAdd( start, end );
}
void View::edit( int uid ) {
    m_win->edit( uid );
}
void View::remove( int uid ) {
    m_win->remove( uid );
}
ODateBookAccess::List View::allEvents() const{
    return m_win->manager()->allRecords();
}
OEffectiveEvent::ValueList View::events( const QDate& from, const QDate& to ) {
    return m_win->manager()->list( from, to );
}
OEvent View::event( int uid ) const{
    return m_win->manager()->event( uid );
}
bool View::isAP()const {
    return m_win->viewAP();
}
bool View::weekStartOnMonday()const {
    return m_win->viewStartMonday();
}
QList<OPimRecord> View::records( const QDate& on ) {
    return m_win->manager()->records( on, on );
}
QList<OPimRecord> View::records( const QDate& on, const QDate& to ) {
    return m_win->manager()->records( on, to );
}
