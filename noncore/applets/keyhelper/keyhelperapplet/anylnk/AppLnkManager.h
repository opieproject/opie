#ifndef _APPLNK_MANAGER_H_
#define _APPLNK_MANAGER_H_

#include <qsize.h>

#include <qpe/applnk.h>
#include <qpe/mimetype.h>

class AppLnkManager
{
public:
	AppLnkManager(){
	}
	~AppLnkManager(){
		if(m_pLnkSet) delete m_pLnkSet;
	}
	static void init(bool force=false){
		if(m_notfound || force){
			if(m_pLnkSet){
				delete m_pLnkSet;
			}
			qDebug("AppLnkManager::init()");
			m_pLnkSet = new AppLnkSet(MimeType::appsFolderName());
			m_notfound = false;
		}
	}
	static AppLnkSet* getInstance(){
		if(m_pLnkSet == NULL){
			init(true);
		}
		return(m_pLnkSet);
	}
	static const QSize& getIconSize(){
		if(m_oIconSize.isValid()){
			return(m_oIconSize);
		}
		const QList<AppLnk>& lnkList = getInstance()->children();
		QListIterator<AppLnk> it(lnkList);
		for(; it.current(); ++it){
			if((*it)->pixmap().isNull() == false){
				m_oIconSize = (*it)->pixmap().size();
				break;
			}
		}
		return(m_oIconSize);
	}
	static void notfound(){
		m_notfound = true;
	}
private:
	static bool m_notfound;
	static AppLnkSet* m_pLnkSet;
	static QSize m_oIconSize;
};

#endif /* _APPLNK_MANAGER_H_ */
