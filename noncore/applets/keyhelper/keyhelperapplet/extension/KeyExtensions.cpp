#include "KeyExtensions.h"

KeyExtensions::KeyExtensions()
{
	qDebug("KeyExtensions::KeyExtensions()");
	m_cancelcode = 0;
}

KeyExtensions::~KeyExtensions()
{
	qDebug("KeyExtensions::~KeyExtensions()");
	clear();
}

void KeyExtensions::assign(const QString& kind, int keycode,
	int keymask, const QValueList<int>& modcodes)
{
	ExtensionInterface* ext;
#if 0
	ext = m_oExtFactory.createInstance(kind);
	if(ext != NULL){
		ext->setKeycode(keycode);
		ext->setKeymask(keymask);
		ext->setModcodes(modcodes);
	}
#else
	ext = m_oExtFactory.createInstance(kind, keycode, keymask);
	if(ext != NULL){
		ext->setModcodes(modcodes);
	}
#endif
}

void KeyExtensions::assign(const QString& kind, int keycode,
	int keymask, int modcode)
{
	QValueList<int> modcodes;
	modcodes.append(modcode);
	assign(kind, keycode, keymask, modcodes);
}

bool KeyExtensions::doKey(int keycode, int keymask, bool isPress)
{
	bool fCancel = false;
	ExtensionList& list = m_oExtFactory.getList();
	for(ExtensionList::Iterator it=list.begin();
			it!=list.end(); ++it){
		if(isPress){
			int code = (*it)->getKeycode();
			if((*it)->getKeymask() == keymask
				&& (code == 0 || code == keycode)){
				if((*it)->onKeyPress(keycode)){
					fCancel = true;
				}
				qWarning("ext:onKeyPress[%s][%x][%d]",
					(*it)->kind().latin1(),
					(*it)->getKeycode(),
					fCancel);
			}
		} else {
			if(keycode == m_cancelcode){
				fCancel = true;
			}
			const QValueList<int>& rlist = (*it)->getModcodes();
			if(rlist.contains(keycode)){
				if((*it)->onModRelease(keycode)){
					m_pModifiers->resetToggles();
				}
				qWarning("ext:onModRelease[%s][%x]",
					(*it)->kind().latin1(),
					keycode);
			}
		}
	}
	if(isPress && fCancel){
		m_cancelcode = keycode;
	} else {
		m_cancelcode = 0;
	}
	return(fCancel);
}

void KeyExtensions::clear()
{
	m_oExtFactory.clear();
}

void KeyExtensions::reset()
{
	//clear();
	m_oExtFactory.reset();
}

void KeyExtensions::init()
{
	m_oExtFactory.sweep();
}

void KeyExtensions::statistics()
{
	qWarning("KeyExtensions::statistics()");
	ExtensionList& list = m_oExtFactory.getList();
	for(ExtensionList::Iterator it=list.begin();
			it!=list.end(); ++it){
		qWarning(" [%s][%x][%x]",
			(*it)->kind().latin1(),
			(*it)->getKeycode(),
			(*it)->getKeymask());
	}
}

