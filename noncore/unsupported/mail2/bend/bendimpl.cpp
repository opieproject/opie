#include "bendimpl.h"
#include "bend.h"

BenDImpl::BenDImpl()
	: _benD(0), ref(0)
{

}

BenDImpl::~BenDImpl()
{
	delete _benD;
}

QWidget *BenDImpl::applet(QWidget *parent)
{
	if (!_benD) _benD = new BenD(parent);
	return _benD;
}

int BenDImpl::position() const
{
	return 4;
}

QRESULT BenDImpl::queryInterface(const QUuid &uuid, QUnknownInterface **iface)
{
	*iface = 0;
	if (uuid == IID_QUnknown) *iface = this;
	else if (uuid == IID_TaskbarApplet) *iface = this;

	if (*iface) (*iface)->addRef();
	return QS_OK;
}

Q_EXPORT_INTERFACE()
{
	Q_CREATE_INSTANCE(BenDImpl)
}
