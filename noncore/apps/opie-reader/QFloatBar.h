#ifndef __QFLOATBAR_H
#define __QFLOATBAR_H

#include <qtoolbar.h>

class QFloatBar : public QToolBar
{
    Q_OBJECT
	virtual void hideEvent(QHideEvent* e)
	{
	    /*if (e->spontaneous())*/ emit OnHide();
	}
	public:
    QFloatBar(char* t, QMainWindow* mw, QMainWindow::ToolBarDock td, bool f) : QToolBar(t, mw, td, f) {}
 signals:
    void OnHide();
};

#endif
