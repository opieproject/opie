

#include "io_layer.h"
#include "file_layer.h"
#include "widget_layer.h"
#include "emulation_layer.h"
#include "session.h"


Session::Session() {
    m_widget = 0l;
    m_layer = 0l;
    m_widLay = 0l;
    m_emLay = 0l;
}
Session::Session( const QString& na, QWidgetStack* widget, IOLayer* lay)
    : m_name( na ), m_widget( widget ), m_layer( lay )
{
    m_widLay = 0l;
    m_emLay  = 0l;
}
Session::~Session() {
    delete m_layer;
    delete m_emLay;
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
EmulationLayer* Session::emulationLayer() {
    return m_emLay;
}
WidgetLayer* Session::emulationWidget() {
    return m_widLay;
}
void Session::connect() {
    if ( !m_layer || !m_emLay )
        return;

    QObject::connect(m_layer, SIGNAL(received(const QByteArray&) ),
            m_emLay, SLOT(onRcvBlock(const QByteArray&) ) );
    QObject::connect(m_emLay, SIGNAL(sndBlock(const QByteArray&) ),
            m_layer, SLOT(send(const QByteArray&) ) );
}
void Session::disconnect() {
    if ( !m_layer || !m_emLay )
        return;

    QObject::disconnect(m_layer, SIGNAL(received(const QByteArray&) ),
            m_emLay, SLOT(onRcvBlock(const QByteArray&) ) );
    QObject::disconnect(m_emLay, SIGNAL(sndBlock(const QByteArray&) ),
            m_layer, SLOT(send(const QByteArray&) ) );
}
void Session::setName( const QString& na){
    m_name = na;
}
void Session::setWidgetStack( QWidgetStack* wid ) {
    delete m_widget;
    /* the EmulationLayer was destroyed... */
    delete m_emLay;
    m_widget = wid;
}
void Session::setIOLayer( IOLayer* lay ) {
    delete m_layer;
    m_layer = lay;
}
void Session::setEmulationLayer( EmulationLayer* lay ) {
    delete m_emLay;
    m_emLay = lay;
}
void Session::setEmulationWidget( WidgetLayer* lay ) {
    delete m_widLay;
    m_widLay = lay;
}
