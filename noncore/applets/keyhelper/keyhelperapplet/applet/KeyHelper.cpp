#include "KeyHelper.h"
#include <opie2/okeyfilter.h>
#include <opie2/odebug.h>

KeyHelper::KeyHelper()
{
    qDebug("KeyHelper::KeyHelper()");
    load();

    m_oAction.setKeyModifiers(&m_oModifiers);
    m_oAction.setKeyMappings(&m_oMappings);
    m_oAction.setKeyExtensions(&m_oExtensions);
    m_oAction.setKeyRepeater(&m_oRepeater);
    m_oExtensions.setKeyModifiers(&m_oModifiers);
}

KeyHelper::~KeyHelper()
{
    unset();
    qDebug("KeyHelper::~KeyHelper()");
}

bool KeyHelper::filter(int unicode, int keycode, int modifiers,
        bool isPress, bool autoRepeat)
{
    odebug << "KeyHelper <<< "<<unicode<<" - "<<keycode<<" - "<<modifiers<<"\n";
    m_oAction.setAction(unicode, keycode, modifiers,
        isPress, autoRepeat);
    return(m_oAction.doAction());
}

void KeyHelper::unset()
{
    Opie::Core::OKeyFilter::inst()->remHandler(this);;
}

void KeyHelper::set()
{
    Opie::Core::OKeyFilter::inst()->addHandler(this);
    m_oModifiers.resetStates();
}

void KeyHelper::enable()
{
    m_oAction.enable();
}

void KeyHelper::disable()
{
    m_oAction.disable();
    m_oRepeater.stop();
}

bool KeyHelper::load(const QString& file)
{
    KeycfgReader oReader;
    oReader.setKeyModifiers(&m_oModifiers);
    oReader.setKeyMappings(&m_oMappings);
    oReader.setKeyExtensions(&m_oExtensions);
    oReader.setKeyRepeater(&m_oRepeater);

    bool success;
    if(file.length() == 0){
        success = oReader.load();
    } else if(file[0] == '/'){
        success = oReader.load(file);
    } else {
        //QString filepath = QString(::getenv("HOME")) + "/Settings/" + file;
        QString filepath = QDir::homeDirPath() + "/Settings/" + file;
        success = oReader.load(filepath);
    }
    if(success == false){
        qDebug("config xml load error");
        setDefault();
    }
    return(success);
}

bool KeyHelper::reload(const QString& file)
{
    m_oModifiers.reset();
    m_oMappings.reset();
    m_oExtensions.reset();
    m_oRepeater.reset();

    return(load(file));
}

void KeyHelper::setDefault()
{
    /* default settings */
    m_oExtensions.assign("switch", Qt::Key_F12,
        m_oModifiers.getMask("Shift"), KeyNames::getCode("Shift"));
    m_oExtensions.assign("select", Qt::Key_F11,
        m_oModifiers.getMask("Shift"), KeyNames::getCode("Shift"));
}

void KeyHelper::statistics()
{
    m_oModifiers.statistics();
    m_oMappings.statistics();
    m_oExtensions.statistics();
    m_oRepeater.statistics();
}

void KeyHelper::dumpkeymap()
{
    const QWSServer::KeyMap* m = QWSServer::keyMap();
    qWarning("KeyHelper::dumpkeymap()");
    while(m->key_code != 0){
        qWarning(" [%04x][%04x][%04x][%04x]", m->key_code, m->unicode, m->shift_unicode, m->ctrl_unicode);
        m++;
    }
}
