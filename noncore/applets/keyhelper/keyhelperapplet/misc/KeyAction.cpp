#include "KeyAction.h"

KeyAction::KeyAction()
{
	qDebug("KeyAction::KeyAction()");
	enable();
}

KeyAction::~KeyAction()
{
	qDebug("KeyAction::~KeyAction()");
}

void KeyAction::init()
{
	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");

	QString oldgroup = cfg.getGroup();
	cfg.setGroup("Global");

	m_keepToggle = cfg.readBoolEntry("KeepToggle", true);

	m_excludeKeys.clear();
	if(cfg.hasKey("ExcludeKeys")){
		QStringList keys = cfg.readListEntry("ExcludeKeys", ',');
		for(QStringList::Iterator it=keys.begin();
				it!=keys.end(); ++it){
			int code;
			if((*it).find("0x") == 0){
				code = KHUtil::hex2int(*it);
			} else {
				code = KeyNames::getCode(*it);
			}
			if(code > 0){
				m_excludeKeys.append(code);
			}
		}
	} else {
		m_excludeKeys.append(Qt::Key_F34);
	}

	m_presscnt = 0;
	m_check = true;
	m_keep_toggle_code = 0;
	m_capture = false;
	cfg.setGroup(oldgroup);
}

void KeyAction::setAction(int unicode, int keycode, int modifiers,
		bool isPress, bool autoRepeat)
{
	m_unicode = unicode;
	m_keycode = keycode;
	m_modifiers = modifiers;
	m_isPress = isPress;
	m_autoRepeat = autoRepeat;
}

bool KeyAction::checkState()
{
	if(0 < m_unicode && m_unicode < 0xFFFF){
		QChar ch(m_unicode);
		QChar::Category category = ch.category();
		if(category == QChar::Letter_Lowercase){
			if(m_modifiers == 0){
				m_pMappings->setCapsLock(false);
				return(true);
			} else if(m_modifiers == Qt::ShiftButton){
				m_pMappings->setCapsLock(true);
				return(true);
			}
		} else if(category == QChar::Letter_Uppercase){
			if(m_modifiers == 0){
				m_pMappings->setCapsLock(true);
				return(true);
			} else if(m_modifiers == Qt::ShiftButton){
				m_pMappings->setCapsLock(false);
				return(true);
			}
		}
	}
	return(false);
}

void KeyAction::sendKeyEvent(int unicode, int keycode, int modifiers,
		bool isPress, bool autoRepeat)
{
	if(m_hookChannel.isEmpty()){
			QWSServer::sendKeyEvent(unicode, keycode, modifiers, isPress, autoRepeat);
	} else {
		if(QCopChannel::isRegistered(m_hookChannel)){
			QCopEnvelope e(m_hookChannel, "keyEvent(int,int,int,int,int)");
			e << unicode << keycode << modifiers << (int)isPress << (int)autoRepeat;
		} else {
			m_hookChannel = "";
			QWSServer::sendKeyEvent(unicode, keycode, modifiers, isPress, autoRepeat);
		}
	}
}

bool KeyAction::doAction()
{
	if(m_enable == false){
		return(false);
	}
#if 0
	if(m_excludeKeys.contains(m_keycode)){
		return(false);
	}
#endif
	if(!m_autoRepeat){
		qDebug("recv[%x][%x][%x][%d]",
			m_unicode,
			m_keycode,
			m_modifiers,
			m_isPress);
	}

	if(m_autoRepeat && !m_excludeKeys.contains(m_keycode)){
		KeyRepeater::RepeaterMode repMode = m_pRepeater->getMode();
		if(repMode == KeyRepeater::ENABLE
			|| repMode == KeyRepeater::KILL){
			/* autoRepeat は無視 */
			return(true);
		}
	}

	if(m_pRepeater->isRepeatable(m_keycode)){
		if(m_isPress){
			m_presscnt++;
		} else {
			m_presscnt--;
			if(m_presscnt <= 0){
				m_pRepeater->stop();
				m_presscnt = 0;
			}
		}
	}

	if(m_check && m_isPress){
		/* check capslock state */
		if(checkState()){
			m_check = false;
		}
	}

	int unicode, keycode, modifiers;
	/* keep toggle reset */
	if(m_keepToggle && m_keep_toggle_code != 0){
		if(m_keep_toggle_code != m_keycode){
			m_pModifiers->resetToggles();
			m_keep_toggle_code = 0;
		} else {
			m_pModifiers->keepToggles();
		}
	}

	/* modifier 状態取得 */
	int keymask = m_pModifiers->getState(m_modifiers);
	modifiers = m_pModifiers->getModifiers(m_modifiers);

	bool isModMapped = false;
	/* modifier 状態更新 */
	if(m_autoRepeat == false){
		if(m_isPress){
			isModMapped = m_pModifiers->pressKey(m_keycode, m_modifiers);
		} else {
			m_pModifiers->releaseKey(m_keycode);
		}
	}

	if(m_capture && m_isPress && m_autoRepeat == false){
		QCopEnvelope e("QPE/KeyHelperConf", "event(int,int,int)");
		e << m_keycode << m_unicode << modifiers;
	}

	/* keyextension 適用 */
	bool fKeyCancel = m_pExtensions->doKey(m_keycode, keymask, m_isPress);
	if(fKeyCancel){
		if(m_keepToggle){
			m_keep_toggle_code = m_keycode;
		} else {
			m_pModifiers->resetToggles();
		}
		m_pRepeater->stop();
		return(true);
	}

	/* keymapping 適用 */
#if 0
	bool isMapped = m_pMappings->apply(m_keycode, keymask, m_isPress);
	if(isMapped == false){
		if(m_pMappings->isDefined()){
			m_pMappings->setOriginal(m_unicode, m_modifiers);
		} else {
			m_pMappings->setUnicode(m_unicode);
			//m_pMappings->setKeycode(m_keycode);
		}
		//return(false);
	}
#else
	bool isMapped = m_pMappings->apply(m_unicode, m_keycode, m_modifiers,
				keymask, m_isPress);
#endif

	/* modifier 適用 */
	m_pMappings->setModifiers(modifiers);

	keycode = m_pMappings->getKeycode();
	if(keycode <= 0){
		return(true);
	}

	if(/*m_autoRepeat == false &&*/ m_isPress){
		if(isModMapped == false || (isModMapped && isMapped)){
			if(m_pModifiers->isModifier(keycode) == false){
				m_pModifiers->resetToggles();
			}
		}
	}
	unicode = m_pMappings->getUnicode();
	modifiers = m_pMappings->getModifiers();
	if(keycode > 0){
		/* disable mapping */
		if(m_excludeKeys.contains(keycode)){
			return(false);
		}
		/* send key event */
		sendKeyEvent(
			unicode,
			keycode,
			modifiers,
			m_isPress,
			m_autoRepeat);
	}
	if(m_isPress){
		/* repeater start */
		m_pRepeater->start(unicode, keycode, modifiers);
#if 0
	} else if(m_presscnt <= 0){
		m_presscnt = 0;
		m_pRepeater->stop();
#endif
	} else {
		m_pRepeater->stop(keycode);
	}
	qWarning("send[%x][%x][%x][%d]",
		m_pMappings->getUnicode(),
		m_pMappings->getKeycode(),
		m_pMappings->getModifiers(),
		m_isPress);
	return(true);
}
