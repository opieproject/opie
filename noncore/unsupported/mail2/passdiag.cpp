#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>

#include "passdiag.h"

PassDiag::PassDiag(QString text, QWidget *parent, const char *name, bool modal, WFlags fl)
	: PassDiagBase(parent, name, modal, fl)
{
	infoLabel->setText(text);

	connect(ok, SIGNAL(clicked()), SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));
}

void PassDiag::accept()
{
	if (password->text().isEmpty()) {
		QMessageBox::information(this, tr("Error"), tr("<p>You have to enter a password or click on cancel.</p>"), tr("Ok"));
		return;
	}

	QDialog::accept();
}

QString PassDiag::getPassword(QWidget *parent, QString text)
{
	PassDiag *diag = new PassDiag(text, parent, 0, true);
	diag->show();

	if (QDialog::Accepted == diag->exec()) {
		return diag->password->text();
	}

	return 0;
}

