#include "KeyHelperWidget.h"
#include "QSafeDataStream.h"
#include "KHUtil.h"

QWidget* g_Widget = NULL;
int g_level = 0;

static const char* _version_ = "1.2.2";

KeyHelperWidget::KeyHelperWidget(QWidget* parent, const char* name)
    : QLabel(parent, name),disabled(Opie::Core::OResource::loadPixmap("zkb-disabled"))
{
    qDebug("KeyHelperWidget::KeyHelperWidget()");
    g_Widget = this;
    m_defHandler = NULL;
    //m_reset = false;
    m_reset = true;
    m_useFilter = false;
    m_pHelper = NULL;
    m_status = false;

    //unset();
    initDebugLevel();

    m_pHelper = new KeyHelper();

    //qApp->installEventFilter(this);

    connect(qwsServer,
        SIGNAL(windowEvent(QWSWindow*, QWSServer::WindowEvent)),
        this,
        SLOT(windowEvent(QWSWindow*, QWSServer::WindowEvent)));

    m_pChannel = new QCopChannel("QPE/KeyHelper", this);
    connect(m_pChannel, SIGNAL(received(const QCString&, const QByteArray&)),
        this, SLOT(receiveMessage(const QCString&, const QByteArray&)));
    m_pSysChannel = new QCopChannel("QPE/System", this);
    connect(m_pSysChannel, SIGNAL(received(const QCString&, const QByteArray&)),
        this, SLOT(sysMessage(const QCString&, const QByteArray&)));

    //AppLnkManager::init();
    setFixedWidth ( AppLnk::smallIconSize()  );
    setFixedHeight ( AppLnk::smallIconSize()  );
    setPixmap(disabled);
    init();
}

KeyHelperWidget::~KeyHelperWidget()
{
    qDebug("KeyHelperWidget::~KeyHelperWidget()");
    disconnect(qwsServer,
        SIGNAL(windowEvent(QWSWindow*, QWSServer::WindowEvent)),
        this,
        SLOT(windowEvent(QWSWindow*, QWSServer::WindowEvent)));
    disconnect(m_pChannel, SIGNAL(received(const QCString&, const QByteArray&)),
        this, SLOT(receiveMessage(const QCString&, const QByteArray&)));
    unset();
    if (m_pHelper) delete m_pHelper;
    if (m_pChannel) delete m_pChannel;
    if (m_pSysChannel) delete m_pSysChannel;
    m_pHelper = NULL;
    setDebugLevel(0);
}

void KeyHelperWidget::mouseReleaseEvent(QMouseEvent*)
{
    ConfigEx::getInstance("keyhelper").setConfig("keyhelper");
}

void KeyHelperWidget::receiveMessage(
        const QCString& msg, const QByteArray& data)
{
    if(m_pHelper == NULL){
        return;
    }
    QSafeDataStream stream(data, IO_ReadOnly);
    if(msg == "event(int,int,int,int,int)"){
        int unicode;
        int keycode;
        int modifiers;
        int isPress;
        int autoRepeat;
        stream >> unicode >> keycode >> modifiers >> isPress >> autoRepeat;
        doEvent(unicode, keycode, modifiers, isPress, autoRepeat);
    } else if(msg == "event(QString,int)"){
        QString key;
        int isPress;
        stream >> key >> isPress;
        doEvent(key, isPress);
    } else if(msg == "enable()"){
        enable();
    } else if(msg == "disable()"){
        disable();
    } else if(msg == "pause()"){
        pause();
    } else if(msg == "restart()"){
        restart();
    } else if(msg == "reload()"){
        m_xmlfile = QString::null;
        doReload();
    } else if(msg == "reload(QString)"){
        stream >> m_xmlfile;
        doReload(false);
    } else if(msg == "version()"){
        version();
    } else if(msg == "repeater(int)"){
        int mode;
        stream >> mode;
        m_pHelper->m_oRepeater.setMode(mode);
    } else if(msg == "hook(QString)"){
        QString s;
        stream >> s;
        m_pHelper->m_oAction.setHook(s.local8Bit());
    } else if(msg == "unhook()"){
        m_pHelper->m_oAction.setHook("");
    } else if(msg == "config()"){
        ConfigEx::getInstance("keyhelper").setConfig("keyhelper");
    } else if(msg == "config(QString)"){
        QString name;
        stream >> name;
        if(name == QString::null){
            ConfigEx::getInstance("keyhelper").setConfig("keyhelper");
        } else {
            ConfigEx::getInstance("keyhelper").setConfig(name);
        }
    } else if(msg == "capture(int)"){
        int enable;
        stream >> enable;
        m_pHelper->m_oAction.setCapture(enable);
    } else if(msg == "statistics()"){
        int level = g_level;
        if(level == 0){
            setDebugLevel(1);
        }
        m_pHelper->statistics();
        if(level == 0){
            setDebugLevel(0);
        }
    } else if(msg == "dumpkeymap()"){
        int level = g_level;
        if(level == 0){
            setDebugLevel(1);
        }
        m_pHelper->dumpkeymap();
        if(level == 0){
            setDebugLevel(0);
        }
    } else if(msg == "debug(int)"){
        int level;
        stream >> level;
        setDebugLevel(level);
    }
}

void KeyHelperWidget::doReload(bool showstatus)
{
    ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
    QString oldgroup = cfg.getGroup();
    cfg.setGroup("Global");
    m_status = false;
    if(showstatus && (cfg.readNumEntry("ShowStatusOnReload", 1) == 1)){
        m_status = true;
        version();
        QCopEnvelope("QPE/System", "busy()");
    }
    cfg.setGroup(oldgroup);
    QTimer::singleShot(0, this, SLOT(reload()));
}

void KeyHelperWidget::doEvent(int unicode, int keycode, int modifiers, int isPress, int autoRepeat)
{
    if(isPress == 0 || isPress == 1){
        m_pHelper->m_oAction.setAction(unicode, keycode, modifiers, isPress, autoRepeat);
        m_pHelper->m_oAction.doAction();
    } else {
        /* press & release */
        m_pHelper->m_oAction.setAction(unicode, keycode, modifiers, 1, autoRepeat);
        m_pHelper->m_oAction.doAction();
        m_pHelper->m_oAction.setAction(unicode, keycode, modifiers, 0, autoRepeat);
        m_pHelper->m_oAction.doAction();
    }
}

void KeyHelperWidget::doEvent(const QString& key, int isPress)
{
    int unicode,keycode;
    int modifiers = 0;
    int pos;
    QString keyname = key;
    pos = keyname.find("+SHIFT", 0, FALSE);
    if(pos > 0){
        modifiers |= Qt::ShiftButton;
        keyname.remove(pos, 6);
    }
    pos = keyname.find("+CTRL", 0, FALSE);
    if(pos > 0){
        modifiers |= Qt::ControlButton;
        keyname.remove(pos, 5);
    }
    pos = keyname.find("+ALT", 0, FALSE);
    if(pos > 0){
        modifiers |= Qt::AltButton;
        keyname.remove(pos, 4);
    }
    if(keyname.length() > 1){
        unicode = 0xffff;
        keycode = KeyNames::getCode(keyname);
        /* get unicode */
        const QWSServer::KeyMap* m;
        for(m=QWSServer::keyMap(); m->key_code != 0; m++){
            if(m->key_code == keycode){
                if(modifiers & Qt::ControlButton){
                    unicode = m->ctrl_unicode;
                } else if(modifiers & Qt::ShiftButton){
                    unicode = m->shift_unicode;
                } else {
                    unicode = m->unicode;
                }
                break;
            }
        }
    } else {
        const QWSServer::KeyMap* m;
        keycode = 0;
        unicode = keyname[0].unicode();
        /* check unicode */
        for(m=QWSServer::keyMap(); keycode == 0 && m->key_code != 0; m++){
            if(m->unicode == unicode){
                keycode = m->key_code;
                break;
            }
        }
        /* check shift_unicode */
        for(m=QWSServer::keyMap(); keycode == 0 && m->key_code != 0; m++){
            if(m->shift_unicode == unicode){
                keycode = m->key_code;
                modifiers |= Qt::ShiftButton;
                break;
            }
        }
        /* check ctrl_unicode */
        for(m=QWSServer::keyMap(); keycode == 0 && m->key_code != 0; m++){
            if(m->ctrl_unicode == unicode){
                keycode = m->key_code;
                modifiers |= Qt::ControlButton;
                break;
            }
        }
    }
    doEvent(unicode, keycode, modifiers, isPress, 0);
}

void KeyHelperWidget::sysMessage(
        const QCString& msg, const QByteArray& data)
{
    QSafeDataStream stream(data, IO_ReadOnly);
    if(msg == "linkChanged(QString)"){
        ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
        QString oldgroup = cfg.getGroup();
        if(cfg.readNumEntry("DetectLinkChange", 1) == 1){
            AppLnkManager::init(true);
            reload();
        }
        cfg.setGroup(oldgroup);
    }
}

void MsgHandler(QtMsgType type, const char* msg)
{
    switch(type){
    case QtDebugMsg:
        if(g_level >= 2){
            syslog(LOG_LOCAL5|LOG_DEBUG,
                "<2>%s", msg);
        }
        break;
    case QtWarningMsg:
        if(g_level >= 1){
            syslog(LOG_LOCAL5|LOG_DEBUG,
                "<1>%s", msg);
        }
        break;
    default:
        break;
    }
}

void KeyHelperWidget::initDebugLevel()
{
    ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
    cfg.setGroup("Global");

    int level = cfg.readNumEntry("DebugLevel", 0);
    setDebugLevel(level);
}

void KeyHelperWidget::setDebugLevel(int level)
{
#ifdef QT_QWS_EBX
    static bool noDebug = true;
    g_level = level;
    if(g_level > 0){
        if(noDebug){
            m_defHandler = qInstallMsgHandler(MsgHandler);
            noDebug = false;
        }
    } else {
        qInstallMsgHandler(m_defHandler);
        noDebug = true;
    }
#endif
}

void KeyHelperWidget::enable()
{
    m_enable = true;
    m_pHelper->enable();
    //set();
    QTimer::singleShot(0, this, SLOT(set()));
}

void KeyHelperWidget::disable()
{
    m_enable = false;
    m_pHelper->disable();
    unset();
}

void KeyHelperWidget::pause()
{
    m_saved = m_enable;
    disable();
}

void KeyHelperWidget::restart()
{
    if(m_saved){
        enable();
    }
}

void KeyHelperWidget::reload()
{
    disable();
    ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
    cfg.reload();
    if(m_pHelper->reload(m_xmlfile) == false){
        if(m_status){
            Global::statusMessage("KeyHelper: Load Error");
        }
    }
    init();
    if(m_status){
        QCopEnvelope e("QPE/System", "notBusy(QString)");
        const QString app = KHUtil::currentApp();
        e << app;
        m_status = false;
    }
}

void KeyHelperWidget::version()
{
    QString ver = "KeyHelper ";
    ver.append(_version_);
    Global::statusMessage(ver);
}

void KeyHelperWidget::init()
{
    AppLnkManager::init();
#if 0
    if(m_pHelper == NULL){
        m_pHelper = new KeyHelper();
    }
#endif
    loadUseFilterApps();
    enable();
}

void KeyHelperWidget::set()
{
    if(m_pHelper != NULL && m_enable == true && m_useFilter == false){
        qWarning("KeyHelperWidget::set()");
        m_pHelper->set();
    }
}

void KeyHelperWidget::unset()
{
    m_pHelper->unset();
}

void KeyHelperWidget::loadUseFilterApps()
{
    ConfigEx& cfg = ConfigEx::getInstance("keyhelper");

    cfg.setGroup("Global");
    m_apps = cfg.readListEntry("UseFilterApps", ',');

    if(m_apps.isEmpty()){
        /* default */
        m_apps.append("CRIM");
        m_apps.append("Jpn50Pad");
        m_apps.append("JpnKtnPad");
        m_apps.append("JpnNumPad");
        m_apps.append("JpnSymPad");
        m_apps.append("Keyboard");
        m_apps.append("IMWidget");  /* IMKit */
        m_apps.append("POBox");     /* QPOBox */
    }
}

#if 0
void KeyHelperWidget::windowEvent(QWSWindow* w, QWSServer::WindowEvent e)
{
    if(m_apps.contains(w->name())){
        switch(e){
        case QWSServer::Hide:
        case QWSServer::Destroy:
            m_useFilter = false;
            //m_reset = true;
            QTimer::singleShot(0, this, SLOT(set()));
            break;
        case QWSServer::Create:
        case QWSServer::Raise:
        case QWSServer::Show:
            m_useFilter = true;
            //m_reset = false;
            m_reset = true;
            break;
        default:
            break;
        }
    } else if(w->client()->identity() != NULL && w->name() != NULL){
        switch(e){
#if 0
        case QWSServer::Create:
        case QWSServer::Hide:
            if(m_useFilter == false && m_reset){
                m_reset = false;
                set();
                //QTimer::singleShot(0, this, SLOT(set()));
            }
            break;
#else
        case QWSServer::Hide:
        case QWSServer::Destroy:
            //if(m_useFilter == false && m_reset){
            if(m_reset){
                m_reset = false;
                set();
                //QTimer::singleShot(0, this, SLOT(set()));
            }
            break;
        case QWSServer::Create:
        case QWSServer::Raise:
        case QWSServer::Show:
            m_reset = true;
            break;
#endif
        default:
#if 0
            if(m_reset == true){
                m_reset = false;
                set();
            }
#endif
            break;
        }
    }
    if(w->name() != NULL){
        qWarning("[%s][%s][%x][%s]",
            w->name().latin1(),
            w->caption().latin1(),
            e,
            w->client()->identity().latin1());
    }
}

#endif

bool KeyHelperWidget::eventFilter(QObject* o, QEvent* e)
{
    return QWidget::eventFilter(o, e);
}

int KeyHelperWidget::position()
{
    return 3;
}
