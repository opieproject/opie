#ifndef _MENU_TITLE_ITEM_H_
#define _MENU_TITLE_ITEM_H_

#include <qmenudata.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qpalette.h>
#include <qpe/config.h>
#include "ConfigEx.h"

class MenuTitle : public QCustomMenuItem
{
public:
	MenuTitle(const QString& s, const QFont& f, const QString& k = "default");
	virtual ~MenuTitle(){}

	bool fullSpan () const;
	bool isSeparator() const;
	void paint(QPainter* p, const QColorGroup& cg, bool act,
			bool enabled, int x, int y, int w, int h);
	QSize sizeHint();
private:
	QString caption;
	QString kind;
	QFont font;
};

#endif /* _MENU_TITLE_ITEM_H_ */
