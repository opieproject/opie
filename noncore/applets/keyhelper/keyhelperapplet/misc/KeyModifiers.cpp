#include "KeyModifiers.h"

KeyModifiers::KeyModifiers()
{
	qDebug("KeyModifiers::KeyModifiers()");
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()),
			this, SLOT(resetToggles()));
	init();
}

KeyModifiers::~KeyModifiers()
{
	qDebug("KeyModifiers::~KeyModifiers()");
	delete m_pTimer;
	clear();
}

void KeyModifiers::addType(const QString& type)
{
	if(m_types.contains(type) == false){
		qDebug("addType[%s][%x]", type.latin1(), m_bitmask);
		m_types.insert(type, m_bitmask);
		m_bitmask = m_bitmask << 1;
	}
}

ModifierInfo* KeyModifiers::assign(const QString& type, int keycode,
	int keymask, bool mode)
{
	m_info = new ModifierInfo(type, keycode, keymask, mode);
	addType(type);
	m_modifiers.append(m_info);
	if(mode){
		m_togglekeys.append(m_info);
	}
	assignRelease(m_info, keycode);
	return(m_info);
}

void KeyModifiers::assignRelease(int keycode)
{
	assignRelease(m_info, keycode);
}

void KeyModifiers::assignRelease(ModifierInfo* info, int keycode)
{
	if(m_releasekeys.contains(keycode) == false){
		m_releasekeys.insert(keycode, new ModifierList);
	}
	m_releasekeys[keycode]->append(info);
}

void KeyModifiers::setToggle()
{
	setToggle(m_info);
}

void KeyModifiers::setToggle(ModifierInfo* info)
{
	info->toggle_mode = true;
	m_togglekeys.append(info);
}

void KeyModifiers::keepToggles()
{
	if(m_timeout > 0){
		m_pTimer->start(m_timeout, true);
	}
}

bool KeyModifiers::pressKey(int keycode, int modifiers)
{
	int keymask;

	keymask = getState(modifiers, true);
	
	for(ModifierList::Iterator it=m_modifiers.begin();
			it!=m_modifiers.end(); ++it){
		if((*it)->keycode == keycode
			&& ((*it)->keymask & keymask) == (*it)->keymask){
			(*it)->pressed = true;
			if((*it)->toggle_mode){
				/* change toggle state */
				(*it)->toggled = !((*it)->toggled);
				if((*it)->toggled){
					keepToggles();
				} else {
					m_pTimer->stop();
				}
			}
			return(true);
		}
	}
	return(false);
}

bool KeyModifiers::isModifier(int keycode)
{
	if(keycode == Qt::Key_Shift
		|| keycode == Qt::Key_Control
		|| keycode == Qt::Key_Alt
		|| keycode == Qt::Key_Meta
		|| keycode == Qt::Key_F22){
		return(true);
	} else {
		return(false);
	}
}

void KeyModifiers::releaseKey(int keycode)
{
	if(m_releasekeys.contains(keycode)){
		ModifierList* list = m_releasekeys[keycode];
		for(ModifierList::Iterator it=(*list).begin();
				it!=(*list).end(); ++it){
			(*it)->pressed = false;
		}
	}
}

int KeyModifiers::getState()
{
	int state = 0;
	for(ModifierList::Iterator it=m_modifiers.begin();
			it!=m_modifiers.end(); ++it){
		if((*it)->pressed || (*it)->toggled){
			state |= m_types[(*it)->type];
		}
	}
	return(state);
}

int KeyModifiers::getState(int modifiers, bool reset)
{
	int state = getState();
	int mask;

	mask = getMask("Shift");
	if(modifiers & Qt::ShiftButton){
		state |= mask;
	} else {
		if(reset){
			state &= ~mask;
		}
	}
	mask = getMask("Control");
	if(modifiers & Qt::ControlButton){
		state |= mask;
	} else {
		if(reset){
			state &= ~mask;
		}
	}
	mask = getMask("Alt");
	if(modifiers & Qt::AltButton){
		state |= mask;
	} else {
		if(reset){
			state &= ~mask;
		}
	}

	return(state);
}

int KeyModifiers::getModifiers(int modifiers)
{
	int state = getState();

	if(state & getMask("Shift")){
		modifiers |= Qt::ShiftButton;
	}
	if(state & getMask("Control")){
		modifiers |= Qt::ControlButton;
	}
	if(state & getMask("Alt")){
		modifiers |= Qt::AltButton;
	}

	return(modifiers);
}

int KeyModifiers::getMask(const QString& type)
{
	if(m_types.contains(type)){
		return(m_types[type]);
	} else {
		return(0);
	}
}

void KeyModifiers::clear()
{
	m_types.clear();

	m_togglekeys.clear();

	for(ModifierList::Iterator it=m_modifiers.begin();
			it!=m_modifiers.end(); ++it){
		delete *it;
	}
	m_modifiers.clear();

	for(QMap<int, ModifierList*>::Iterator it=m_releasekeys.begin();
			it!=m_releasekeys.end(); ++it){
		delete *it;
	}
	m_releasekeys.clear();
}

void KeyModifiers::init()
{
	m_bitmask = 0x00000001;
	addType("Shift");
	addType("Control");
	addType("Alt");

	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");

	cfg.setGroup("Global");

	QString timeout = cfg.readEntry("ToggleTimeout", "10");
	int msec = timeout.toInt();
	msec *= 1000; /* sec to msec */
	m_timeout = msec;
}

void KeyModifiers::reset()
{
	clear();
	init();
}

bool KeyModifiers::isToggled()
{
	for(ModifierList::Iterator it=m_togglekeys.begin();
			it!=m_togglekeys.end(); ++it){
		if((*it)->toggled){
			return(true);
		}
	}
	return(false);
}

void KeyModifiers::resetToggles()
{
	for(ModifierList::Iterator it=m_togglekeys.begin();
			it!=m_togglekeys.end(); ++it){
		(*it)->toggled = false;
	}
	m_pTimer->stop();
}

void KeyModifiers::resetStates()
{
	for(ModifierList::Iterator it=m_modifiers.begin();
			it!=m_modifiers.end(); ++it){
		(*it)->pressed = false;
	}
	resetToggles();
}

void KeyModifiers::statistics()
{
	qWarning("KeyModifiers::statistics()");
	for(ModifierList::Iterator it=m_modifiers.begin();
			it!=m_modifiers.end(); ++it){
		qWarning(" [%s][%x][%x][%d][%d]",
			(*it)->type.latin1(),
			(*it)->keycode,
			(*it)->keymask,
			(*it)->pressed,
			(*it)->toggled);
		for(QMap<int, ModifierList*>::Iterator it2=m_releasekeys.begin();
				it2!=m_releasekeys.end(); ++it2){
			for(ModifierList::Iterator it3=(*it2)->begin();
					it3!=(*it2)->end(); ++it3){
				if(*it == *it3){
					qWarning("  release[%x]", it2.key());
				}
			}
		}
	}
}
