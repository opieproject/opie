#ifndef _DOCLNK_WRAPPER_H_
#define _DOCLNK_WRAPPER_H_

#include <qpe/applnk.h>
#include "AnyLnk.h"

class DocLnkWrapper : public AnyLnk
{
public:
	DocLnkWrapper(){}
	DocLnkWrapper(const QStringList& params)
		: AnyLnk(params)
	{
		m_pLnk = new DocLnk(m_params[0], false);
	}
	virtual ~DocLnkWrapper(){
		delete m_pLnk;
	}

	virtual bool isValid() {
		if(m_pLnk->exec().length() > 0){
			return(true);
		} else {
			return(false);
		}
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
	DocLnk* m_pLnk;
};

#endif /* _DOCLNK_WRAPPER_H_ */


