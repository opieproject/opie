#include <qwidget.h>

#include "TEWidget.h"
#include "TEmuVt102.h"

#include "emulation_handler.h"


EmulationHandler::EmulationHandler( const Profile& prof, QWidget* parent,const char* name )
    : QObject(0, name )
{
    m_teWid = new TEWidget( parent, "TerminalMain");
    m_teWid->setMinimumSize(150, 70 );
    parent->resize( m_teWid->calcSize(80, 24 ) );
    m_teEmu = new TEmuVt102(m_teWid );

    connect(m_teEmu,SIGNAL(ImageSizeChanged(int, int) ),
            this, SIGNAL(changeSize(int, int) ) );
    connect(m_teEmu, SIGNAL(sndBlock(const char*, int) ),
            this, SLOT(recvEmulation(const char*, int) ) );
    m_teEmu->setConnect( true );
    m_teEmu->setHistory( TRUE );
    load( prof );



}
EmulationHandler::~EmulationHandler() {
    delete m_teEmu;
    delete m_teWid;
}
void EmulationHandler::load( const Profile& ) {
    QFont  font = QFont("Fixed", 12, QFont::Normal );
    font.setFixedPitch(TRUE);
    m_teWid->setVTFont( font );
    m_teWid->setBackgroundColor(Qt::gray );
}
void EmulationHandler::recv( const QByteArray& ar) {
    qWarning("received in EmulationHandler!");
    m_teEmu->onRcvBlock(ar.data(), ar.count() );
}
void EmulationHandler::recvEmulation(const char* src, int len ) {
    qWarning("received from te ");
    QByteArray ar(len);

    memcpy(ar.data(), src, sizeof(char) * len );

    emit send(ar);
}
QWidget* EmulationHandler::widget() {
    return m_teWid;
}
