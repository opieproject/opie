#include "MenuTitle.h"

MenuTitle::MenuTitle(const QString& s, const QFont& f, const QString& k)
{
	font = f;
	kind = k;

	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");

	const QString curGroup = cfg.getGroup();
	cfg.setGroup("Style");

	caption = cfg.readEntry("Caption_" + k, s);
	cfg.setGroup(curGroup);
}

bool MenuTitle::fullSpan() const
{
	return(true);
}

bool MenuTitle::isSeparator() const
{
	return(true);
}

void MenuTitle::paint(QPainter* p, const QColorGroup& cg, bool /*act*/,
		bool /*enabled*/, int x, int y, int w, int h)
{
	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
	const QString& curGroup = cfg.getGroup();

	cfg.setGroup("Style");

	QString name;
	QColor color;

	p->setFont(font);

	/* set fontcolor */
	name = cfg.readEntry("FontColor_" + kind, QString::null);
	if(name != QString::null){
		color.setNamedColor(name);
		if(color.isValid()){
			p->setPen(color);
		}
	}

	/* set bgcolor */
	name = cfg.readEntry("BgColor_" + kind, QString::null);
	if(name != QString::null){
		color.setNamedColor(name);
		if(color.isValid() == false){
			color = cg.mid();
		}
	} else {
		color = cg.mid();
	}
	p->fillRect(x, y, w, h, QBrush(color));
	p->drawText(x, y, w, h, AlignCenter, caption);
	cfg.setGroup(curGroup);
}

QSize MenuTitle::sizeHint()
{
	return(QFontMetrics(font).size(AlignCenter, caption));
}
