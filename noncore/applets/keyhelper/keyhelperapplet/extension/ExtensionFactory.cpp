#include "ExtensionFactory.h"

ExtensionFactory::ExtensionFactory()
{
	qDebug("ExtensionFactory::ExtensionFactory()");
	m_pLoadList = NULL;
}

ExtensionFactory::~ExtensionFactory()
{
	qDebug("ExtensionFactory::~ExtensionFactory()");
}

ExtensionInterface* ExtensionFactory::createInstance(const QString& kind)
{
	ExtensionInterface* ext;
	QString kindstr = kind.lower();

	if(kindstr == "switch"){
		ext = new TaskSwitcher(kindstr);
	} else if(kindstr == "select"){
		ext = new TaskSelector(kindstr);
	} else if(kindstr.find("launch") == 0){
		ext = new KeyLauncher(kindstr);
	} else if(kindstr.find("menu") == 0){
		ext = new MenuLauncher(kindstr);
	} else {
		return(NULL);
	}
	m_oExtList.append(ext);
	return(ext);
}

ExtensionInterface* ExtensionFactory::createInstance(const QString& kind,
	int keycode, int keymask)
{
	ExtensionInterface* ext;
	QString kindstr = kind.lower();

	ext = loadInstance(kindstr, keycode, keymask);
	if(ext != NULL){
		return(ext);
	}

	if(kindstr == "switch"){
		ext = new TaskSwitcher(kindstr);
	} else if(kindstr == "select"){
		ext = new TaskSelector(kindstr);
	} else if(kindstr.find("launch") == 0){
		ext = new KeyLauncher(kindstr);
	} else if(kindstr.find("menu") == 0){
		ext = new MenuLauncher(kindstr);
	} else {
		return(NULL);
	}
	ext->setKeycode(keycode);
	ext->setKeymask(keymask);

	m_oExtList.append(ext);
	return(ext);
}

ExtensionInterface* ExtensionFactory::loadInstance(const QString& kindstr,
	int keycode, int keymask)
{
	if(m_pLoadList == NULL){
		return(NULL);
	}

	for(ExtensionList::Iterator it=m_pLoadList->begin();
			it!=m_pLoadList->end(); ++it){
		if((*it)->kind() == kindstr
			&& (*it)->getKeycode() == keycode
			&& (*it)->getKeymask() == keymask){
			m_oExtList.append(*it);
			return(*it);
		}
	}
	return(NULL);
}

void ExtensionFactory::clear()
{
	for(ExtensionList::Iterator it=m_oExtList.begin();
			it!=m_oExtList.end(); ++it){
		delete *it;
	}
	m_oExtList.clear();
}

void ExtensionFactory::reset()
{
	m_pLoadList = new ExtensionList(m_oExtList);
	m_oExtList.clear();
}

void ExtensionFactory::sweep()
{
	if(m_pLoadList == NULL){
		return;
	}
	for(ExtensionList::Iterator it=m_pLoadList->begin();
			it!=m_pLoadList->end(); ++it){
		if(m_oExtList.contains(*it) == false){
			delete *it;
		}
	}
	delete m_pLoadList;
	m_pLoadList = NULL;
}
