#ifndef ZKBWIDGET_H
#define ZKBWIDGET_H

#include <qwidget.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qpixmap.h>
#include <qcopchannel_qws.h>

#include "zkb.h"

class ZkbWidget : public QLabel {
Q_OBJECT

public:
	ZkbWidget(QWidget* parent);
	~ZkbWidget();
	static int position();

	QSize sizeHint() const;

protected:
	QLabel* label;
	Keymap* keymap;
	QPopupMenu* labels;
	QCopChannel* channel;
	int w, h;
	QPixmap disabled;

	bool loadKeymap();
	void mouseReleaseEvent(QMouseEvent*);

protected slots:
	void stateChanged(const QString&);
	void labelChanged(int id);
	void signalReceived(const QCString& msg, const QByteArray& data);
	void reload();
};
#endif
