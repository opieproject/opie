#ifndef _PASTELNK_H_
#define _PASTELNK_H_

#include <qwindowsystem_qws.h>

#include <qpe/qpeapplication.h>
#include <qclipboard.h>
#include <qregexp.h>

#include "AnyLnk.h"

class TextLnk : public AnyLnk
{
public:
	TextLnk(){}
	TextLnk(const QStringList& params)
		: AnyLnk(params){}
	virtual ~TextLnk() {
	}

	virtual bool isValid() {
		return(true);
	}
	virtual QString name() {
		return("text");
	}
	virtual const QPixmap& pixmap() {
		return(m_pixmap);
	}

	virtual void execute();
protected:
	virtual void parse(QString& str);
	virtual void replace(QString& str, const QString& s1, const QString& s2);
};

#endif /* _PASTELNK_H_ */

