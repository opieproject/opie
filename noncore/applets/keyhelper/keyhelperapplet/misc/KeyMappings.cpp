#include "KeyMappings.h"

static QIntDict<QWSServer::KeyMap> g_mapCache(127);

MapInfo::MapInfo(int code, int mod, int uni,
	int shift_uni, int ctrl_uni)
{
	const QWSServer::KeyMap* map;
	if(uni == 0 || shift_uni == 0 || ctrl_uni == 0){
		map = MapInfo::findKeyMap(code);
		if(map != NULL){
			isDefined = true;
			unicode = map->unicode;
			shift_unicode = map->shift_unicode;
			ctrl_unicode = map->ctrl_unicode;
		} else {
			isDefined = false;
			unicode = 0;
			shift_unicode = 0;
			ctrl_unicode = 0;
		}
	} else {
		isDefined = true;
	}
	keycode = code;
	modifiers = mod;
	if(uni != 0){
		unicode = uni;
	}
	if(shift_uni != 0){
		shift_unicode = shift_uni;
	}
	if(ctrl_uni != 0){
		ctrl_unicode = ctrl_uni;
	}
}

const QWSServer::KeyMap* MapInfo::findKeyMap(int keycode)
{
	const QWSServer::KeyMap* m = QWSServer::keyMap();

	while(m->key_code != 0){
		if(m->key_code == keycode){
			return(m);
		}
		m++;
	}
	return(NULL);
}

KeyMappings::KeyMappings()
{
	qDebug("KeyMappings::KeyMappings()");
	init();
}

KeyMappings::~KeyMappings()
{
	qDebug("KeyMappings::~KeyMappings()");
	clear();
}

void KeyMappings::init()
{
	m_capslock = false;
}

void KeyMappings::reset()
{
	clear();
}

void KeyMappings::clear()
{
	for(QMap<int, CodeMaps*>::Iterator it = m_keymaps.begin();
			it!=m_keymaps.end(); ++it){
		delete (*it);
	}
	m_keymaps.clear();
	g_mapCache.setAutoDelete(true);
	g_mapCache.clear();
}

void KeyMappings::assign(int keycode, int keymask, int mapcode,
	int mapmodifiers, int unicode, int shift_unicode, int ctrl_unicode)
{
	CodeMaps* map;
	if(m_keymaps.contains(keycode)){
		map = m_keymaps[keycode];
	} else {
		map = new CodeMaps();
		m_keymaps.insert(keycode, map);
	}

	MapInfo info(mapcode, mapmodifiers, unicode, shift_unicode, ctrl_unicode);
	m_it = map->insert(keymask, info);
}

void KeyMappings::assignModifier(const QString& type, const QString& state)
{
	int maskbit = 0;
	QString str;
	str = type.lower();
	if(str == "shift"){
		maskbit = Qt::ShiftButton;
	} else if(str == "control"){
		maskbit = Qt::ControlButton;
	} else if(str == "alt"){
		maskbit = Qt::AltButton;
	}
	str = state.lower();
	if(str == "off"){
		maskbit = (maskbit << 16) & 0xFFFF0000;
	}
	(*m_it).modifiers |= maskbit;
}

void KeyMappings::assignUnicode(const QString& kind, const QString& ch)
{
	QString str;
	int code = ch[0].unicode();
	str = kind.lower();
	if(str == "unicode"){
		(*m_it).unicode = code;
	} else if(str == "shift_unicode"){
		(*m_it).shift_unicode = code;
	} else if(str == "ctrl_unicode"){
		(*m_it).ctrl_unicode = code;
	}
}

void KeyMappings::assignUnicode(int unicode)
{
	(*m_it).unicode = (*m_it).shift_unicode = 
		(*m_it).ctrl_unicode = unicode;
}

void KeyMappings::statistics()
{
	qWarning("KeyMappings::statistics()");
	for(QMap<int, CodeMaps*>::Iterator it=m_keymaps.begin();
			it!=m_keymaps.end(); ++it){
		qWarning(" code = %x", it.key());
		for(QMap<int, MapInfo>::Iterator it2=(*it)->begin();
				it2!=(*it)->end(); ++it2){
			qDebug("  [%x]-[%x][%x][%x][%x][%x]", it2.key(),
				(*it2).keycode,
				(*it2).modifiers,
				(*it2).unicode,
				(*it2).shift_unicode,
				(*it2).ctrl_unicode);
		}
	}
}

bool KeyMappings::apply(int unicode, int keycode, int modifiers,
	int keymask, bool isPress)
{
	CodeMaps* map;
	m_isMapped = false;

	if(m_keymaps.contains(keycode)){
		map = m_keymaps[keycode];
		if(map->contains(keymask)){
			m_isMapped = true;
			m_keyinfo = (*map)[keymask];
		} else {
			int mask = -1;
			for(CodeMaps::Iterator it=map->begin();
					it!=map->end(); ++it){
				if((keymask & it.key()) == it.key()
					&& it.key() > mask){
					mask = it.key();
				}
			}
			if(mask != -1){
				m_isMapped = true;
				m_keyinfo = (*map)[mask];
			}
		}
	}

	if(m_isMapped == false){
		QWSServer::KeyMap* cache = g_mapCache[keycode];
		if(cache == NULL){
			cache = new QWSServer::KeyMap();
			g_mapCache.insert(keycode, cache);
			cache->unicode = cache->shift_unicode = cache->ctrl_unicode = 0;
		}
		if(cache->unicode == 0 || cache->shift_unicode == 0 || cache->ctrl_unicode == 0){
			QChar ch(unicode);
			if(modifiers & Qt::ControlButton){
				cache->ctrl_unicode = unicode;
			} else if(modifiers & Qt::ShiftButton){
				cache->shift_unicode = ch.upper().unicode();
			} else {
				cache->unicode = ch.lower().unicode();
			}
		}
		m_keyinfo = MapInfo(keycode, 0,
			cache->unicode, cache->shift_unicode, cache->ctrl_unicode);
		if(m_keyinfo.isDefined){
			setOriginal(unicode, modifiers);
		} else {
			setUnicode(unicode);
		}
	}

#if 1
	if(isPress){
		if(m_keyinfo.keycode == Qt::Key_CapsLock){
			m_capslock = !m_capslock;
		}
	}
#endif
	return(m_isMapped);
}

bool KeyMappings::apply(int keycode, int keymask, bool isPress)
{
	CodeMaps* map;
	m_isMapped = false;

	if(m_keymaps.contains(keycode)){
		map = m_keymaps[keycode];
		if(map->contains(keymask)){
			m_isMapped = true;
			m_keyinfo = (*map)[keymask];
		} else {
			int mask = -1;
			for(CodeMaps::Iterator it=map->begin();
					it!=map->end(); ++it){
				if((keymask & it.key()) == it.key()
					&& it.key() > mask){
					mask = it.key();
				}
			}
			if(mask != -1){
				m_isMapped = true;
				m_keyinfo = (*map)[mask];
			}
		}
	}
	if(m_isMapped == false){
		m_keyinfo = MapInfo(keycode);
	}
#if 1
	if(isPress){
		if(m_keyinfo.keycode == Qt::Key_CapsLock){
			m_capslock = !m_capslock;
		}
	}
#endif
	return(m_isMapped);
}

/**
 * set original unicode
 */
void KeyMappings::setOriginal(int unicode, int modifiers)
{
	if(modifiers & Qt::ControlButton){
		m_keyinfo.ctrl_unicode = unicode;
	} else if(modifiers & Qt::ShiftButton){
		m_keyinfo.shift_unicode = unicode;
	} else {
		m_keyinfo.unicode = unicode;
	}
}

void KeyMappings::setModifiers(int modifiers)
{
	m_modifiers = modifiers;
	m_modifiers |= (m_keyinfo.modifiers & 0xFFFF);
	m_modifiers &= ~((m_keyinfo.modifiers >> 16) & 0xFFFF);
}

void KeyMappings::setUnicode(int unicode)
{
	m_keyinfo.unicode = unicode;
	m_keyinfo.shift_unicode = unicode;
	m_keyinfo.ctrl_unicode = unicode;
}

void KeyMappings::setKeycode(int keycode)
{
	m_keyinfo.keycode = keycode;
}

int KeyMappings::getUnicode()
{
	int unicode;
	if(m_modifiers & Qt::ControlButton){
		unicode = m_keyinfo.ctrl_unicode;
	} else if(m_modifiers & Qt::ShiftButton){
		unicode = m_keyinfo.shift_unicode;
		QChar ch(unicode);
		if(m_capslock){
			unicode = ch.lower().unicode();
		} else {
			unicode = ch.upper().unicode();
		}
	} else {
		unicode = m_keyinfo.unicode;
		QChar ch(unicode);
		if(m_capslock){
			unicode = ch.upper().unicode();
		} else {
			unicode = ch.lower().unicode();
		}
	}
	qDebug("UNICODE[%d][%x][%x]", m_capslock, unicode, m_keyinfo.unicode);
#if 0
	if(m_isMapped && m_capslock){
		QChar ch(unicode);
		QChar::Category category = ch.category();
		if(category == QChar::Letter_Uppercase){
			unicode = ch.lower().unicode();
		} else if(category == QChar::Letter_Lowercase){
			unicode = ch.upper().unicode();
		}
	}
#endif
	return(unicode);
}

int KeyMappings::getKeycode()
{
	return(m_keyinfo.keycode);
}

int KeyMappings::getModifiers()
{
	return(m_modifiers);
}

bool KeyMappings::isDefined()
{
	return(m_keyinfo.isDefined);
}
