#include "qcheckname.h"
#include <qmessagebox.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include "qrestrictedline.h"

QCheckName::QCheckName()
	: QCheckNameBase()
{
	connect(cmdDone, SIGNAL(clicked()), this, SLOT(clicked()));
}

QString QCheckName::getName()
{
	QCheckName qcn;
	qcn.setWFlags(Qt::WType_Modal);
	qcn.leText->setValidChars("abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	if (qcn.exec() == QDialog::Accepted)
	{
		return qcn.leText->text();
	} else {
		return QString("");
	}
}

void QCheckName::clicked()
{
	if (leText->text().isEmpty() == false)
	{
		hide();
		accept();
	} else {
		QMessageBox::critical(this, "Missing Information", "<qt>Please enter the name of your Check Book and try again.</qt>");
	}
}
