#include <qpopupmenu.h>
#include <qtimer.h>

#include "listviewplus.h"

ListViewPlus::ListViewPlus(QWidget *parent, const char *name, WFlags fl)
	: QListView(parent, name, fl)
{

}

void ListViewPlus::keyPressEvent(QKeyEvent *event)
{
	switch(event->key()) {
		case Qt::Key_Space:		// FALLTHROUGH
		case Qt::Key_Enter:
			if (currentItem() != 0)
				emit clicked(currentItem());
			break;
		default: break;
	}

	QListView::keyPressEvent(event);
}

void ListViewPlus::setPopup(QPopupMenu *popup, int delay)
{
	_popup = popup;
	_delay = delay;

	connect(this, SIGNAL(pressed(QListViewItem *, const QPoint &, int)), SLOT(_initPopup(QListViewItem *, const QPoint &, int)));
	connect(this, SIGNAL(clicked(QListViewItem *, const QPoint &, int)), SLOT(_cancelPopup(QListViewItem *, const QPoint &, int)));
}

void ListViewPlus::_initPopup(QListViewItem *, const QPoint &point, int)
{
	_point = point;

	_timer = new QTimer();
	_timer->start(_delay, true);

	connect(_timer, SIGNAL(timeout()), this, SLOT(_showPopup()));
}

void ListViewPlus::_cancelPopup(QListViewItem *, const QPoint &, int)
{
	delete _timer;
}

void ListViewPlus::_showPopup()
{
	_popup->popup(_point);
}

