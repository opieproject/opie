#include <qwidget.h>
#include <qpushbutton.h>

#include "TEWidget.h"
#include "TEmuVt102.h"

#include "profile.h"
#include "emulation_handler.h"
#include "script.h"

EmulationHandler::EmulationHandler( const Profile& prof, QWidget* parent,const char* name )
    : QObject(0, name )
{
    m_teWid = new TEWidget( parent, "TerminalMain");
	// use setWrapAt(0) for classic behaviour (wrap at screen width, no scrollbar)
	// use setWrapAt(80) for normal console with scrollbar
	m_teWid->setWrapAt(prof.readNumEntry("Wrap", 0) ? 0 : 80);
    m_teWid->setMinimumSize(150, 70 );
    m_script = 0;
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
    if (isRecording())
        clearScript();
    delete m_teEmu;
    delete m_teWid;
}

void EmulationHandler::load( const Profile& prof) {
    m_teWid->setVTFont( font( prof.readNumEntry("Font")  )  );
    int num = prof.readNumEntry("Color");
    setColor( foreColor(num), backColor(num) );
    m_teWid->setBackgroundColor(backColor(num) );

    int term = prof.readNumEntry("Terminal",  0) ;
    switch(term) {
    default:
    case Profile::VT102:
    case Profile::VT100:
        m_teEmu->setKeytrans("vt100.keytab");
        break;
    case Profile::Linux:
        m_teEmu->setKeytrans("linux.keytab");
        break;
    case Profile::XTerm:
        m_teEmu->setKeytrans("default.Keytab");
        break;
    }
}
void EmulationHandler::recv( const QByteArray& ar) {
    m_teEmu->onRcvBlock(ar.data(), ar.count() );
}
void EmulationHandler::recvEmulation(const char* src, int len ) {
    QByteArray ar(len);

    memcpy(ar.data(), src, sizeof(char) * len );
    if (isRecording())
        m_script->append(ar);
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
        /* color is black */
        co = Qt::white;
        break;
    case Profile::Black:
        co = Qt::black;
        break;
    case Profile::Green:
        qWarning("Foreground green");
        co = Qt::green;
        break;
    case Profile::Orange:
        qWarning("Foreground orange");
        co.setRgb( 231, 184, 98 );
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
        /* color is white */
        co = Qt::black;
        break;
    case Profile::Black:
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

QPushButton*  EmulationHandler::cornerButton() {
    return m_teWid->cornerButton();
}


Script *EmulationHandler::script() {
    return m_script;
}

bool EmulationHandler::isRecording() {
    return (m_script != 0);
}

void EmulationHandler::startRecording() {
    if (!isRecording())
        m_script = new Script();
}

void EmulationHandler::clearScript() {
    if (isRecording()) {
        delete m_script;
        m_script = 0;
    }
}

void EmulationHandler::runScript(const Script *script) {
    emit send(script->script());
}

void EmulationHandler::copy() {
    m_teWid->emitSelection();
}
void EmulationHandler::paste() {
    m_teWid->pasteClipboard();
}
