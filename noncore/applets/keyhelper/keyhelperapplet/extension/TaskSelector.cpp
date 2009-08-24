#include "TaskSelector.h"
extern QWidget* g_Widget;

static const char* defkeys =
"QWERTYUIOPASDFGHJKLZXCVBNM1234567890";

#define START_INDEX		1

TaskSelector::TaskSelector(const QString& kind) : m_kind(kind)
{
	qDebug("TaskSelector::TaskSelector()");
	m_pMenu = new QPopupMenuEx(g_Widget);
	m_pMenu->installEventFilter(this);

	m_isShowing = false;
	m_index = START_INDEX-1;
	connect(m_pMenu, SIGNAL(activated(int)), this, SLOT(select(int)));
	connect(m_pMenu, SIGNAL(highlighted(int)), this, SLOT(highlight(int)));
}

TaskSelector::~TaskSelector()
{
	qDebug("TaskSelector::~TaskSelector()");
	delete m_pMenu;
}

bool TaskSelector::onKeyPress(int /*keycode*/)
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

bool TaskSelector::onModRelease(int /*modcode*/)
{
	if(m_pMenu->isVisible()){
		//m_pMenu->hide();
		QTimer::singleShot(0, this, SLOT(select()));
		return(true);
	} else {
		return(false);
	}
}

int TaskSelector::buildMenu()
{
	const AppLnk* lnk;

	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
	QString oldgroup;

	oldgroup = cfg.getGroup();
	cfg.setGroup("Global");
	QString accesskeys = cfg.readEntry("AccessKeys", defkeys);
	if(accesskeys.length() <= 0){
		accesskeys = defkeys;
	}
	cfg.setGroup(oldgroup);

	/* get list */
	int cnt = 0;
	m_index = START_INDEX+1;
	m_applist.clear();
	m_pMenu->clear();
	MenuTitle* pTitle = new MenuTitle("TaskSelector", m_pMenu->font(), kind());
	m_pMenu->insertItem(pTitle);
	const QList<QWSWindow>& list = qwsServer->clientWindows();
	QWSWindow* w;
	for(QListIterator<QWSWindow> it(list); (w=it.current()); ++it){
		if(w->isVisible() == false
			|| w->caption() == QString::null){
			continue;
		}
		QString app = w->client()->identity();
		if(app == NULL || m_applist.contains(app)){
			continue;
		}
		/* exclude "launcher" */
		if(app == "launcher"){
			if(cnt == 0){
				m_index--;
			}
			continue;
		}
		m_applist.append(app);
		/* append menu */
		cnt++;
		AppLnkSet* lnkSet = AppLnkManager::getInstance();
		lnk = lnkSet->findExec(app);
		QString text;
		QPixmap icon;
#if 0
		if(lnk != NULL){
			icon = lnk->pixmap();
			text = lnk->name();
		} else {
			AppLnkManager::notfound();
			icon = QPixmap();
			text = w->caption();
		}
#else
		if(lnk != NULL){
			icon = lnk->pixmap();
			if(w->caption().length() > 0){
				text = w->caption();
			} else {
				text = lnk->name();
			}
		} else {
			AppLnkManager::notfound();
			icon = QPixmap();
			text = w->caption();
		}
#endif
		if(cnt <= (int)accesskeys.length()){
			text.append("(&");
			text.append(accesskeys[cnt-1].upper());
			text.append(")");
		}
		m_pMenu->insertItem(icon, text, cnt);
	}
	return(cnt);
}

void TaskSelector::show()
{
	/* build task selector menu */
	int cnt = buildMenu();
	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
	QString oldgroup = cfg.getGroup();
	cfg.setGroup("Global");
	int min = cfg.readNumEntry("SelectMenuMin", 2);
	if(min != 1 && min != 3){
		min = 2;
	}
	cfg.setGroup(oldgroup);

	if(cnt == 0){
		qDebug("no applications");
	} else if(cnt < min){
		//m_index = START_INDEX;
		if(m_index > cnt){
			m_index = cnt;
		}
		QTimer::singleShot(0, this, SLOT(select()));
	} else {
		if(m_index > cnt){
			m_index = cnt;
		}
		ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
		cfg.setGroup("Style");
		int x,y;
		QString key = "Position_" + kind();
		if(cfg.hasKey(key)){
			const QStringList& list = cfg.readListEntry(key, ',');
			x = list[0].toInt();
			y = list[1].toInt();
		} else {
			x = (qt_screen->width() - m_pMenu->sizeHint().width()) / 2;
			y = (qt_screen->height() - m_pMenu->sizeHint().height()) / 2;
		}
		QPoint pos(x, y);
		m_pMenu->popup(pos);
		m_pMenu->setActiveItem(m_index);
	}
	m_isShowing = false;
}

void TaskSelector::next()
{
	m_index++;
	if(m_index > (signed int)m_applist.count()){
		m_index = START_INDEX;
	}
	m_pMenu->setActiveItem(m_index);
}

void TaskSelector::select()
{
	//select(m_index);
	m_pMenu->activateItemAt(m_index);
}

void TaskSelector::select(int index)
{
	if(index > 0){
		Global::execute(m_applist[index-1]);
	}
	m_index = 0;
}

void TaskSelector::highlight(int index)
{
	if(m_pMenu->isVisible()){
		m_index = index;
	}
}

bool TaskSelector::eventFilter(QObject* o, QEvent* e)
{
	if(m_pMenu->isVisible()){
		QKeyEvent* ke = (QKeyEvent*)e;
		switch(e->type()){
		case QEvent::Accel:
			if(ke->key() == Qt::Key_Space
				&& ke->isAutoRepeat() == false){
				select();
			}
			break;
		default:
			//qDebug(">>>>> [%p][%d] <<<<<", o, e->type());
			break;
		}
	}
	return QObject::eventFilter(o, e);
}
