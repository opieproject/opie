#ifndef _EXECLNK_H_
#define _EXECLNK_H_

#include <qpe/qpeapplication.h>

#include "AnyLnk.h"
#include "ProcessInvoker.h"

class ExecLnk : public AnyLnk
{
public:
	ExecLnk(){}
	ExecLnk(const QStringList& params)
		: AnyLnk(params){}
	virtual ~ExecLnk() {
	}

	virtual bool isValid() {
		return(true);
	}
	virtual void execute() {
		parseText();
		ProcessInvoker& pi = ProcessInvoker::getInstance();
		pi.setArguments(m_params[1]);
		pi.setNotify();
		pi.run();
	}
	virtual QString name() {
		return("exec");
	}
	virtual const QPixmap& pixmap() {
		return(m_pixmap);
	}
protected:
};

#endif /* _EXECLNK_H_ */

