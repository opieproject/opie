#ifndef _QCOPLNK_H_
#define _QCOPLNK_H_

#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>

#include "AnyLnk.h"
#include "KHUtil.h"

class QCopLnk : public AnyLnk
{
public:
	QCopLnk(){}
	QCopLnk(const QStringList& params)
		: AnyLnk(params){}
	virtual ~QCopLnk() {
	}

	virtual bool isValid() {
		return(true);
	}
	virtual QString name() {
		return("qcop");
	}
	virtual const QPixmap& pixmap() {
		return(m_pixmap);
	}

	virtual void execute();
protected:
};

#endif /* _QCOPLNK_H_ */

