#ifndef _ANYLNK_H_
#define _ANYLNK_H_

#include <time.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qclipboard.h>
#include <qtextcodec.h>

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

#include "AppLnkManager.h"
#include "ConfigEx.h"

class AnyLnk
{
public:
	AnyLnk(){}
	AnyLnk(const QStringList& params){
		m_params = params;
		loadPixmap();
	}
	virtual ~AnyLnk(){
	}
	virtual bool isValid() = 0;
	virtual void execute() = 0;
	virtual QString name() = 0;
	virtual const QPixmap& pixmap() = 0;

protected:
	QStringList m_params;
	QPixmap m_pixmap;

	virtual void loadPixmap();
	virtual void parseText();
	virtual void replaceText(QString& str, const QString& s1, const QString& s2);
	virtual void replaceDate(QString& str);
	virtual void replaceKeyword(QString& str);
};

#endif /* _ANYLNK_H_ */
