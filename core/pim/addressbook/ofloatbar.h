#ifndef __OFLOATBAR_H
#define __OFLOATBAR_H

#include <qtoolbar.h>
#include <qevent.h> 

class OFloatBar : public QToolBar
{
    Q_OBJECT
	virtual void hideEvent(QHideEvent* e)
	{
	    /*if (e->spontaneous())*/ emit OnHide();
	}
	public:
    OFloatBar(char* t, QMainWindow* mw, QMainWindow::ToolBarDock td, bool f) : QToolBar(t, mw, td, f) {}
 signals:
    void OnHide();
};

#endif
