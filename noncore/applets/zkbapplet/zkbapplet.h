#include "qpe/taskbarappletinterface.h"

#ifndef ZKBAPPLET_H
#define ZKBAPPLET_H

class ZkbWidget;

class ZkbApplet : public TaskbarAppletInterface {
public:
	ZkbApplet();
	virtual ~ZkbApplet();

	QRESULT queryInterface(const QUuid&, QUnknownInterface**);
	Q_REFCOUNT

	virtual QWidget* applet(QWidget*);
	virtual int position() const;

protected:
	ZkbWidget* app;
};

#endif
