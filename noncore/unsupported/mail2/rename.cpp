#include <qlineedit.h>
#include <qlabel.h>

#include "rename.h"

Rename::Rename(QWidget *parent, const char *name, bool modal, WFlags fl)
	: RenameBase(parent, name, modal, fl)
{
}

QString Rename::rename(const QString &on, QWidget *parent)
{
	Rename *rename = new Rename(parent, 0, true);
	rename->oldName->setText(on);
	rename->newName->setText(on);
	rename->newName->setFocus();

	if (rename->exec() == QDialog::Accepted) {
		return rename->newName->text();
	}

	return QString(0);
}

QString Rename::getText(const QString &caption, const QString &secondLabel, QWidget *parent)
{
	Rename *rename = new Rename(parent, 0, true);
	rename->firstLabel->hide();
	rename->oldName->hide();
	rename->setCaption(caption);
	rename->secondLabel->setText(secondLabel);

	if (rename->exec() == QDialog::Accepted) {
		return rename->newName->text();
	}

	return QString(0);
}

