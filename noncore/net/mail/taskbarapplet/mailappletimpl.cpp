#include "mailappletimpl.h"
#include "mailapplet.h"

MailAppletImpl::MailAppletImpl()
	: m_mailApplet(0), ref(0) {

}

MailAppletImpl::~MailAppletImpl() {
	delete m_mailApplet;
}

QWidget *MailAppletImpl::applet(QWidget *parent) {
	if (!m_mailApplet) m_mailApplet = new MailApplet(parent);
	return m_mailApplet;
}

int MailAppletImpl::position() const {
	return 4;
}

QRESULT MailAppletImpl::queryInterface(const QUuid &uuid, QUnknownInterface **iface) {
	*iface = 0;
	if (uuid == IID_QUnknown) *iface = this;
	else if (uuid == IID_TaskbarApplet) *iface = this;

	if (*iface) (*iface)->addRef();
	return QS_OK;
}

Q_EXPORT_INTERFACE() {
	Q_CREATE_INSTANCE( MailAppletImpl )
}
