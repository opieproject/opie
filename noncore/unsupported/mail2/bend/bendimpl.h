#ifndef BENDIMPL_H
#define BENDIMPL_H

#include <qpe/taskbarappletinterface.h>

class BenD;

class BenDImpl : public TaskbarAppletInterface
{
public:
	BenDImpl();
	virtual ~BenDImpl();

	QRESULT queryInterface(const QUuid &uuid, QUnknownInterface **iface);
	Q_REFCOUNT

	virtual QWidget *applet(QWidget *parent);
	virtual int position() const;

private:
	BenD *_benD;
	ulong ref;

};

#endif

