#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qlistbox.h>
#include <qfile.h>

#include <qpe/resource.h>

#include <stdlib.h>

#include "addresspicker.h"

AddressPicker::AddressPicker(QWidget *parent, const char *name, bool modal,
	WFlags fl) : AddressPickerBase(parent, name, modal, fl)
{
	okButton->setIconSet(Resource::loadPixmap("enter"));
	cancelButton->setIconSet(Resource::loadPixmap("editdelete"));

	connect(okButton, SIGNAL(clicked()), SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), SLOT(close()));

	QFile f((QString) getenv("HOME") + "/Applications/"
		+ "addressbook/addressbook.xml");

	if (f.open(IO_ReadOnly)) {
		QTextStream stream(&f);
		QString content;
		while (!f.atEnd()) content += stream.readLine() + "\n";
		QStringList lines = QStringList::split(QRegExp("\\n"), content);
		QStringList::Iterator it;
		for (it = lines.begin(); it != lines.end(); it++) {
			if ((*it).find(QRegExp("^<Contact.*")) != -1) {
				int pos = (*it).find("FirstName=\"");
				QString fname;
				if (pos != -1) {
					int i = 1;
					QChar c;
					while (c != '"') {
						c = (*it)[pos + 10 + i];
						if (c != '"') fname += c;
						i++;
					}
				}
				pos = (*it).find("LastName=\"");
				QString lname;
				if (pos != -1) {
					int i = 1;
					QChar c;
					while (c != '"') {
						c = (*it)[pos + 9 + i];
						if (c != '"') lname += c;
						i++;
					}
				}
				pos = (*it).find("DefaultEmail=\"");
				QString email;
				if (pos != -1) {
					int i = 1;
					QChar c;	
					while (c != '"') {
						c = (*it)[pos + 13 + i];
						if (c != '"') email += c;
						i++;
					}
				}
				QString tname, temail;
				if (!fname.isEmpty()) tname += fname;
				if (!lname.isEmpty()) tname += fname.isEmpty() ? lname : (" " + lname);
				if (!email.isEmpty()) temail += tname.isEmpty() ? email : (" <" + email + ">");
				if (!email.isEmpty()) addressList->insertItem(tname + temail);
			}
		}
	}
	if (addressList->count() <= 0) {
		addressList->insertItem(tr("There are no entries in the addressbook."));
		addressList->setEnabled(false);
		okButton->setEnabled(false);
	}
}

void AddressPicker::accept()
{
	QListBoxItem *item = addressList->firstItem();
	QString names;

	while (item) {
		if (item->selected())
			names += item->text() + ", ";
		item = item->next();
	}
	names.replace(names.length() - 2, 2, "");

	if (names.isEmpty()) {
		QMessageBox::information(this, tr("Error"), tr("<p>You have to select"
			" at least one address entry.</p>"), tr("Ok"));
		return;
	}

	selectedNames = names;
	QDialog::accept();
}

QString AddressPicker::getNames()
{
	QString names = 0;
	
	AddressPicker picker(0, 0, true);
	picker.showMaximized();
	picker.show();

	int ret = picker.exec();
	if (QDialog::Accepted == ret) {
		return picker.selectedNames;
	}
	return 0;
}

