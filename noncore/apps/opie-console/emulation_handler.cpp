#include <qwidget.h>

#include "TEWidget.h"
#include "TEmuVt102.h"

#include "emulation_handler.h"


EmulationHandler::EmulationHandler( const Profile& prof, QWidget* parent, const char* name )
    : QObject(0, name )
{
    load(prof );
    m_teWid = new TEWidget( parent, "TerminalMain" );
    parent->resize( m_teWid->calcSize(80, 24 ) );
    m_teEmu = new TEmuVt102(m_teWid );

    connect(m_teEmu,SIGNAL(ImageSizeChanged(int, int) ),
            this, SIGNAL(changeSize(int, int) ) );
    connect(m_teEmu, SIGNAL(sndBlock(const char*, int) ),
            this, SLOT(recvEmulation(const char*, int) ) );

}
EmulationHandler::~EmulationHandler() {
    delete m_teEmu;
    delete m_teWid;
}
void EmulationHandler::load( const Profile& ) {

}
void EmulationHandler::recv( const QByteArray& ar) {
    m_teEmu->onRcvBlock(ar.data(), ar.count() );
}
void EmulationHandler::recvEmulation(const char* src, int len ) {
    QByteArray ar(len);
    memcpy(ar.data(), src, sizeof(char) * len );

    emit send(ar);
}
QWidget* EmulationHandler::widget() {
    return m_teWid;
}
