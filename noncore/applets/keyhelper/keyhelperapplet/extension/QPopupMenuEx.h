#ifndef _QPOPUPMENUEX_H_
#define _QPOPUPMENUEX_H_

#include <qpopupmenu.h>
#include <qstring.h>
#include <qevent.h>

class QPopupMenuEx : public QPopupMenu
{
public:
	QPopupMenuEx(QWidget* parent=0, const char* name=0)
		: QPopupMenu(parent, name){}
protected:
	void keyPressEvent(QKeyEvent* e){
		QChar c = e->text()[0];
		QKeyEvent* ke = new QKeyEvent(
			e->type(),
			e->key(),
			c.lower().latin1(),
			0,
			c.lower(),
			e->isAutoRepeat());
		QPopupMenu::keyPressEvent(ke);
	}
private:
};

#endif /* _QPOPUPMENUEX_H_ */
