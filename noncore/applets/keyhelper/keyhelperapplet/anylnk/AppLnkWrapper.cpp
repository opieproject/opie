#ifndef _APPLNK_WRAPPER_H_
#define _APPLNK_WRAPPER_H_

#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
#include "AnyLnk.h"

class AppLnkWrapper : public AnyLnk
{
public:
	AppLnkWrapper(){}
	AppLnkWrapper(const QStringList& params)
		: AnyLnk(params)
	{
		m_pLnk = new AppLnk(QPEApplication::qpeDir()
			+ "apps/" + m_params[0] + ".desktop");
	}
	virtual ~AppLnkWrapper(){
		delete m_pLnk;
	}

	virtual bool isValid() {
		return(m_pLnk->isValid());
	}
	virtual void execute(){
		parseText();
		m_pLnk->execute(m_params[1]);
	}
	virtual QString name() {
		return(m_pLnk->name());
	}
	virtual const QPixmap& pixmap(){
		if(m_pixmap.isNull()){
			return(m_pLnk->pixmap());
		} else {
			return(m_pixmap);
		}
	}
protected:
	AppLnk* m_pLnk;
};

#endif /* _APPLNK_WRAPPER_H_ */