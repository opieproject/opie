#include <qwidget.h>

#include "TEWidget.h"
#include "TEmuVt102.h"

#include "profile.h"
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
void EmulationHandler::load( const Profile& prof) {
    m_teWid->setVTFont( font( prof.readNumEntry("Font")  )  );
    int num = prof.readNumEntry("Color");
    setColor( foreColor(num), backColor(num) );
     m_teWid->setBackgroundColor(backColor(num) );
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
/*
 * allocate a new table of colors
 */
void EmulationHandler::setColor( const QColor& fore, const QColor& back ) {
    ColorEntry table[TABLE_COLORS];
    const ColorEntry *defaultCt = m_teWid->getdefaultColorTable();

    for (int i = 0; i < TABLE_COLORS; i++ ) {
        if ( i == 0 || i == 10 ) {
            table[i].color = fore;
        }else if ( i == 1 || i == 11 ) {
            table[i].color = back;
            table[i].transparent = 0;
        }else {
            table[i].color = defaultCt[i].color;
        }
    }
    m_teWid->setColorTable(table );
    m_teWid->update();
}
QFont EmulationHandler::font( int id ) {
    QString name;
    int size = 0;
    switch(id ) {
    default: // fall through
    case 0:
        name = QString::fromLatin1("Micro");
        size = 4;
        break;
    case 1:
        name = QString::fromLatin1("Fixed");
        size = 7;
        break;
    case 2:
        name = QString::fromLatin1("Fixed");
        size = 12;
        break;
    }
    QFont font(name, size, QFont::Normal );
    font.setFixedPitch(TRUE );
    return font;
}
QColor EmulationHandler::foreColor(int col) {
    QColor co;
    /* we need to switch it */
    switch( col ) {
    default:
    case Profile::White:
        qWarning("Foreground black");
        /* color is black */
        co = Qt::white;
        break;
    case Profile::Black:
        qWarning("Foreground white");
        co = Qt::black;
        break;
    case Profile::Green:
        qWarning("Foreground green");
        co = Qt::green;
        break;
    case Profile::Orange:
        qWarning("Foreground orange");
        // FIXME needs better color here
        co = Qt::darkYellow;
        break;
    }

    return co;
}
QColor EmulationHandler::backColor(int col ) {
  QColor co;
    /* we need to switch it */
    switch( col ) {
    default:
    case Profile::White:
        qWarning("Background white");
        /* color is white */
        co = Qt::black;
        break;
    case Profile::Black:
        qWarning("Background black");
        co = Qt::white;
        break;
    case Profile::Green:
        qWarning("Background black");
        co = Qt::black;
        break;
    case Profile::Orange:
        qWarning("Background black");
        co = Qt::black;
        break;
    }

    return co;
}
