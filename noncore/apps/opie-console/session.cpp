

#include "io_layer.h"
#include "file_layer.h"
#include "session.h"

Session::Session() {
    m_widget = 0l;
    m_layer = 0l;
}
Session::Session( const QString& na, QWidget* widget, IOLayer* lay)
    : m_name( na ), m_widget( widget ), m_layer( lay )
{
}
Session::~Session() {
    delete m_layer;
    delete m_widget;
}
QString Session::name()const {
    return m_name;
}
QWidget* Session::widget() {
    return m_widget;
}
IOLayer* Session::layer() {
    return m_layer;
}
void Session::setName( const QString& na){
    m_name = na;
}
void Session::setWidget( QWidget* wid ) {
    delete m_widget;
    m_widget = wid;
}
void Session::setIOLayer( IOLayer* lay ) {
    delete m_layer;
    m_layer = lay;
}
