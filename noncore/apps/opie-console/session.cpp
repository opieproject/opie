

#include "profile.h"
#include "io_layer.h"
#include "file_layer.h"
#include "emulation_handler.h"
#include "session.h"


Session::Session() {
    m_widget = 0l;
    m_layer = 0l;
    m_emu = 0l;
}
Session::Session( const QString& na, QWidgetStack* widget, IOLayer* lay)
    : m_name( na ), m_widget( widget ), m_layer( lay )
{
//    m_widLay = 0l;
//    m_emLay  = 0l;
    m_emu = 0l;
}
Session::~Session() {
    delete m_layer;
    delete m_emu;
    delete m_widget;
    /* the widget layer should be deleted by the m_widget */
}
QString Session::name()const {
    return m_name;
}
QWidgetStack* Session::widgetStack() {
    return m_widget;
}
IOLayer* Session::layer() {
    return m_layer;
}
EmulationHandler* Session::emulationHandler() {
    return m_emu;
}
QWidget* Session::widget() {
    if (!m_emu )
        return 0l;

    return m_emu->widget();
}
Profile Session::profile()const {
    return m_prof;
}
/*
WidgetLayer* Session::emulationWidget() {
    return m_widLay;
}
*/
void Session::connect() {
    if ( !m_layer || !m_emu )
        return;

    QObject::connect(m_layer, SIGNAL(received(const QByteArray&) ),
            m_emu, SLOT(recv(const QByteArray&) ) );
    QObject::connect(m_emu, SIGNAL(send(const QByteArray&) ),
            m_layer, SLOT(send(const QByteArray&) ) );
    QObject::connect(m_emu, SIGNAL(changeSize(int, int) ),
                     m_layer, SLOT(setSize(int, int) ) );
}

void Session::disconnect() {

    if ( !m_layer || !m_emu )
        return;

    QObject::disconnect(m_layer, SIGNAL(received(const QByteArray&) ),
            m_emu, SLOT(recv(const QByteArray&) ) );
    QObject::disconnect(m_emu, SIGNAL(send(const QByteArray&) ),
            m_layer, SLOT(send(const QByteArray&) ) );
}

void Session::setName( const QString& na){
    m_name = na;
}

void Session::setWidgetStack( QWidgetStack* wid ) {
    delete m_emu;
    m_emu = 0l;
    delete m_widget;
    /* the EmulationLayer was destroyed... */

    m_widget = wid;
}
void Session::setIOLayer( IOLayer* lay ) {
    delete m_layer;
    m_layer = lay;
}

void Session::setEmulationHandler( EmulationHandler* lay ) {
    delete m_emu;
    m_emu = lay;
}
void Session::setProfile( const Profile& prof ) {
    m_prof = prof;
}
/*
void Session::setEmulationWidget( WidgetLayer* lay ) {
    delete m_widLay;
    m_widLay = lay;
}
*/

