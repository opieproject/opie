

#include "io_layer.h"
#include "file_layer.h"
#include "session.h"

Session::Session( QWidget* widget, IOLayer* lay)
    : m_widget( widget ), m_layer( lay )
{
}
Session::~Session() {
    delete m_layer;
    delete m_widget;
}
QWidget* Session::widget() {
    return m_widget;
}
IOLayer* Session::layer() {
    return m_layer;
}
void Session::setWidget( QWidget* wid ) {
    m_widget = wid;
}
void Session::setIOLayer( IOLayer* lay ) {
    m_layer = lay;
}
