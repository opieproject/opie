#include "MenuLauncher.h" 
extern QWidget* g_Widget;

MenuLauncher::MenuLauncher(const QString& kind) : m_kind(kind)
{
	qDebug("MenuLauncher::MenuLauncher()");
	m_pMenu = m_pTopMenu = NULL;
	
	m_isShowing = false;
	m_id = -1;

	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()),
		this, SLOT(select()));

	init();
}

MenuLauncher::~MenuLauncher()
{
	qDebug("MenuLauncher::~MenuLauncher()");
	delete m_pTopMenu;
	delete m_pTimer;
}

void MenuLauncher::init()
{
	buildMenu();
}

QPopupMenu* MenuLauncher::initMenu(QWidget* parent, const QString& name)
{
	QPopupMenu* pMenu;
	pMenu = new QPopupMenuEx(parent, name);
	pMenu->installEventFilter(this);
	connect(pMenu, SIGNAL(activated(int)), this, SLOT(select(int)));
	connect(pMenu, SIGNAL(highlighted(int)), this, SLOT(highlight(int)));
	//connect(pMenu, SIGNAL(aboutToHide()), this, SLOT(execute()));
	return(pMenu);
}

bool MenuLauncher::onKeyPress(int /*keycode*/)
{
	if(m_isShowing){
		qDebug("showing ...");
	} else if(m_pMenu->isVisible()){
		next();
	} else {
		ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
		cfg.setGroup("Global");
		int delay = cfg.readNumEntry("DelayPopup", 5);
		QTimer::singleShot(delay, this, SLOT(show()));
		m_isShowing = true;
	}
	return true;
}

bool MenuLauncher::onModRelease(int /*modcode*/)
{
	if(m_pMenu->isVisible()){
		QTimer::singleShot(0, this, SLOT(select()));
		return(true);
	} else {
		return(false);
	}
}

QString MenuLauncher::getMenuText(const QString& key, const QString& name)
{
	QRegExp rx("^[0-9]+_");
	QString text;
	QString ackey;
	int len;
	if(rx.match(key, 0, &len) == 0){
		ackey = key.mid(len);
	} else {
		ackey = key;
	}
	if(ackey.length() == 1){
		text = name;
		text.append("(&");
		text.append(ackey);
		text.append(")");
	} else {
		text = ackey;
	}
	return(text);
}

int MenuLauncher::buildMenu(const QString& section,
		QPopupMenu* pMenu, int& id)
{
	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");

	if(m_oMenuList.contains(pMenu)){
		/* 無限ループ防止 */
		return(0);
	}
	m_oMenuList.append(pMenu);

	QString oldgroup = cfg.getGroup();

	QString group = section;
	group[0] = group[0].upper();

	cfg.setGroup(group);

	QStringList apps = cfg.getKeys();
	int cnt = 0;
	if(apps.isEmpty() == false){
		for(QStringList::Iterator it=apps.begin();
				it!=apps.end(); ++it){
			QStringList args = cfg.readListEntry(*it, '\t');
			LnkWrapper lnk(args);
			if(lnk.isValid()){
				cnt++;
				QString text = getMenuText(*it, lnk.instance().name());
				if(args[0] == "@menu"){
					QPopupMenu* pSubMenu = initMenu(m_pTopMenu, args[1]);
					pMenu->insertItem(lnk.instance().pixmap(), text,
						pSubMenu, id);
					m_oItemList.append(ItemInfo(section));
					id++;
					buildMenu(args[1], pSubMenu, id);
				} else {
					pMenu->insertItem(lnk.instance().pixmap(), text, id);
					m_oItemList.append(ItemInfo(section, *it));
					id++;
				}
			}
		}
	}
	cfg.setGroup(oldgroup);
	return(cnt);
}

void MenuLauncher::clearSubMenu()
{
	for(QValueList<QPopupMenu*>::Iterator it=m_oMenuList.begin();
			it!=m_oMenuList.end(); ++it){
		if(*it != m_pTopMenu){
			delete *it;
		}
	}
	m_oMenuList.clear();
	m_oItemList.clear();
}

int MenuLauncher::buildMenu(bool force)
{
	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
	if(!force && m_lastmodify == cfg.lastRead()){
		return(m_pTopMenu->count());
	}
	qDebug("buildMenu");

	QString oldgroup = cfg.getGroup();

	cfg.setGroup("Global");
	m_submenuTimeout = cfg.readNumEntry("SubMenuTimeout", 500);

	if(m_pTopMenu){
		delete m_pTopMenu;
	}
	m_pMenu = m_pTopMenu = initMenu(g_Widget, kind());
	m_oLastId.clear();
	m_oMenuList.clear();
	m_oItemList.clear();

	MenuTitle* pTitle = new MenuTitle("MenuLauncher",
		m_pTopMenu->font(), kind());
	m_pTopMenu->insertItem(pTitle);

	int id = 0;
	int cnt = buildMenu(kind(), m_pTopMenu, id);
	if(cnt > 0){
		m_lastmodify = cfg.lastRead();
	}

	cfg.setGroup(oldgroup);
	return(cnt);
}

void MenuLauncher::show()
{
	m_enablePopup = false;
	int cnt = buildMenu();
	if(cnt > 0){
		m_pMenu = m_pTopMenu;
		ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
		cfg.setGroup("Style");
		int x,y;
		QString key = "Position_" + kind();
		if(cfg.hasKey(key)){
			const QStringList& list = cfg.readListEntry(key, ',');
			x = list[0].toInt();
			y = list[1].toInt();
		} else {
			x = (qt_screen->width() - m_pTopMenu->sizeHint().width()) / 2;
			y = (qt_screen->height() - m_pTopMenu->sizeHint().height()) / 2;
		}
		QPoint pos(x, y);
		m_pTopMenu->popup(pos);
		m_pTopMenu->setActiveItem(1);
	}
	m_isShowing = false;
}

void MenuLauncher::next()
{
	int index = m_pMenu->indexOf(m_id);
	index++;
	if(index >= (signed int)m_pMenu->count()){
		if(m_pMenu == m_pTopMenu){
	 		index = 1;
		} else {
			index = 0;
		}
	}
	m_pMenu->setActiveItem(index);
	m_id = m_pMenu->idAt(index);
}

void MenuLauncher::select()
{
	if(m_pMenu->isVisible()){
		QMenuItem* item = m_pMenu->findItem(m_id);
		int index = m_pMenu->indexOf(m_id);
		QPopupMenu* p = m_pMenu;
		//m_pMenu->activateItemAt(index);
		if(item && item->popup()){
			m_pMenu = item->popup();
		}
		p->activateItemAt(index);
	}
}

void MenuLauncher::select(int id)
{
	if(id >= 0 && m_oItemList[id].entry != QString::null){
	  	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");

		cfg.setGroup("Global");
		int delay = cfg.readNumEntry("DelayExec", 100);

		QString group = m_oItemList[id].group;
		group[0] = group[0].upper();
		cfg.setGroup(group);

		//QStringList args = cfg.readListEntry(m_oItemList[id].entry, '\t');
		m_args = cfg.readListEntry(m_oItemList[id].entry, '\t');

#if 0
		LnkWrapper lnk(args);
		if(lnk.isValid()){
			lnk.instance().execute();
		}
#else
		QTimer::singleShot(delay, this, SLOT(execute()));
#endif
	}
	m_pMenu = m_pTopMenu;
	m_id = -1;
}

void MenuLauncher::execute()
{
	LnkWrapper lnk(m_args);
	if(lnk.isValid()){
		lnk.instance().execute();
	}
	m_args.clear();
}

void MenuLauncher::highlight(int id)
{
	if(m_pMenu && m_pMenu->isVisible()){
		m_id = id;
		if(m_enablePopup){
			QMenuItem* item = m_pMenu->findItem(m_id);
			if(item && item->popup()){
				if(m_submenuTimeout > 0){
					m_pTimer->start(m_submenuTimeout, true);
				}
			} else {
				m_pTimer->stop();
			}
		} else {
			/* メニュー表示直後はポップアップしない */
			m_enablePopup = true;
		}
	}
}

bool MenuLauncher::eventFilter(QObject* o, QEvent* e)
{
	if(m_pTopMenu->isVisible()){
		QKeyEvent* ke = (QKeyEvent*)e;
		switch(e->type()){
		case QEvent::Accel:
			if(ke->key() == Qt::Key_Space
				&& ke->isAutoRepeat() == false){
				select();
			}
			break;
		case QEvent::FocusIn:
			//qDebug("FocusIn[%p][%p]", o, m_pMenu);
			m_pMenu = (QPopupMenu*)o;
			if(m_oLastId.contains(o)){
				m_id = m_oLastId[o];
			}
			m_pMenu->updateItem(m_id);
			break;
		case QEvent::FocusOut:
			//qDebug("FocusOut[%p][%p]", o, m_pMenu);
			m_oLastId[o] = m_id;
			break;
		default:
			//qDebug(">>>>> [%p][%d] <<<<<", o, e->type());
			break;
		}
	}
	return QObject::eventFilter(o, e);
}
