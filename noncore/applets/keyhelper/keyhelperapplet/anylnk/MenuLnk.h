#ifndef _MENULNK_H_
#define _MENULNK_H_

#include <qpe/qpeapplication.h>

#include "AnyLnk.h"
#include "ConfigEx.h"

class MenuLnk : public AnyLnk
{
public:
	MenuLnk(){}
	MenuLnk(const QStringList& params)
		: AnyLnk(params){}
	virtual ~MenuLnk() {
	}

	virtual bool isValid() {
		ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
		QString group = cfg.getGroup();
		cfg.setGroup(name());
		bool valid = (cfg.getKeys().isEmpty() == false);
		cfg.setGroup(group);
		return(valid);
	}
	virtual void execute() {
	}
	virtual QString name() {
		QString group;
		group = m_params[1];
		group[0] = group[0].upper();
		return(group);
	}
	virtual const QPixmap& pixmap() {
		return(m_pixmap);
	}
protected:
};

#endif /* _MENULNK_H_ */

