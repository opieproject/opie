#ifndef LISTVIEWPLUS_H
#define LISTVIEWPLUS_H

#include <qlistview.h>

class QPopupMenu;
class QPoint;
class QTimer;

class ListViewPlus : public QListView
{
	Q_OBJECT

public:
	ListViewPlus(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

	void setPopup(QPopupMenu *popup, int delay = 800);
	QPopupMenu *popup() { return _popup; }

protected:
	void keyPressEvent(QKeyEvent *event);

private slots:
	void _initPopup(QListViewItem *, const QPoint &point, int);
	void _cancelPopup(QListViewItem *, const QPoint &, int);
	void _showPopup();

private:
	QPopupMenu *_popup;
	QPoint _point;
	QTimer *_timer;
	int _delay;

};

#endif

