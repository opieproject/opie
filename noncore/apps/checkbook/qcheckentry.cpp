#include "qcheckentry.h"

QCheckEntry::QCheckEntry()
	: QCheckEntryBase()
{
	connect(transAmount, SIGNAL(textChanged(const QString &)), this, SLOT(amountChanged(const QString &)));
	connect(transFee, SIGNAL(textChanged(const QString &)), this, SLOT(transFeeChanged(const QString &)));
	connect(payment, SIGNAL(clicked()), this, SLOT(paymentClicked()));
	connect(deposit, SIGNAL(clicked()), this, SLOT(depositClicked()));

	QString todaysdate = QString::number(QDate::currentDate().month());
	todaysdate.append("/");
	todaysdate.append(QString::number(QDate::currentDate().day()));
	todaysdate.append("/");
	todaysdate.append(QString::number(QDate::currentDate().year()));
	dateEdit->setText(todaysdate);

	descriptionCombo->setFocus();

	dateEdit->setValidChars("0123456789./-");
	dateEdit->setMaxLength(10);

	descriptionCombo->lineEdit()->setMaxLength(30);

	checkNumber->setValidChars("0123456789-");
	checkNumber->setMaxLength(10);

	transAmount->setValidChars("0123456789.");

	transFee->setMaxLength(5);
	transFee->setValidChars("0123456789.");
}

void QCheckEntry::paymentClicked()
{
	cmbCategory->clear();
	cmbCategory->insertItem( tr( "Automobile" ) );
	cmbCategory->insertItem( tr( "Bills" ) );
	cmbCategory->insertItem( tr( "CDs" ) );
	cmbCategory->insertItem( tr( "Clothing" ) );
	cmbCategory->insertItem( tr( "Computer" ) );
	cmbCategory->insertItem( tr( "DVDs" ) );
	cmbCategory->insertItem( tr( "Eletronics" ) );
	cmbCategory->insertItem( tr( "Entertainment" ) );
	cmbCategory->insertItem( tr( "Food" ) );
	cmbCategory->insertItem( tr( "Gasoline" ) );
	cmbCategory->insertItem( tr( "Misc" ) );
	cmbCategory->insertItem( tr( "Movies" ) );
	cmbCategory->insertItem( tr( "Rent" ) );
	cmbCategory->insertItem( tr( "Travel" ) );
	cmbCategory->setCurrentItem( 0 );
	transType->clear();
	transType->insertItem( tr( "Debit Charge" ) );
	transType->insertItem( tr( "Written Check" ) );
	transType->insertItem( tr( "Transfer" ) );
	transType->insertItem( tr( "Credit Card" ) );
}

void QCheckEntry::depositClicked()
{
	cmbCategory->clear();
	cmbCategory->insertItem( tr( "Work" ) );
	cmbCategory->insertItem( tr( "Family Member" ) );
	cmbCategory->insertItem( tr( "Misc. Credit" ) );
	cmbCategory->setCurrentItem( 0 );
	transType->clear();
	transType->insertItem( tr( "Written Check" ) );
	transType->insertItem( tr( "Automatic Payment" ) );
	transType->insertItem( tr( "Transfer" ) );
	transType->insertItem( tr( "Cash" ) );
}

QStringList QCheckEntry::popupEntry(const QStringList &originaldata)
{
	QCheckEntry qce;

	// This is how the list looks:
	// 0: true or false, true == payment, false == deposit
	// 1: description of the transaction
	// 2: category name
	// 3: transaction type (stores the integer value of the index of the combobox)
	// 4: check number of the transaction (if any)
	// 5: transaction amount
	// 6: transaction fee (e.g. service charge, or ATM charge).
	// 7: date of the transaction
	// 8: additional notes
	// 9: recently used descriptions
	if (originaldata.count() > 1)
	{
		if (originaldata[0] == "true")
		{
			qce.payment->setChecked(true);
			qce.paymentClicked();
		} else {
			if (originaldata[0] == "false")
			{
				qce.deposit->setChecked(true);
				qce.depositClicked();
			}
		}
		qce.descriptionCombo->lineEdit()->setText(originaldata[1]);
		qce.cmbCategory->lineEdit()->setText(originaldata[2]);
		qce.transType->setCurrentItem(originaldata[3].toInt());
		qce.checkNumber->setText(originaldata[4]);
		qce.transAmount->setText(originaldata[5]);
		qce.transFee->setText(originaldata[6]);
		qce.dateEdit->setText(originaldata[7]);
		qce.additionalNotes->setText(originaldata[8]);
		QStringList recentlist;
		if (!originaldata[9].isEmpty())
		{
			recentlist = QStringList::split(',', originaldata[9], false);
		}
		if (!recentlist.isEmpty())
		{
			qce.descriptionCombo->insertStringList(recentlist);
		}
	} else {
		QStringList recentlist;
		if (!originaldata[0].isEmpty())
		{
			recentlist = QStringList::split(',', originaldata[0], false);
		}
		if (!recentlist.isEmpty())
		{
			qce.descriptionCombo->insertStringList(recentlist);
		}
	}

	qce.setWFlags(Qt::WType_Modal);
	qce.showMaximized();
	
	qce.descriptionCombo->lineEdit()->clear();

	if (qce.exec() == QDialog::Accepted)
	{
		// Validate that the user has inputed a valid dollar amount
		if (qce.transFee->text().contains('.') == 0)
		{
			QString text = qce.transFee->text();
			text.append(".00");
			qce.transFee->setText(text);
		} else {
			QString tmp = qce.transFee->text();
			if (tmp.mid(tmp.find('.'), tmp.length()).length() == 1)
			{
				tmp.append("00");
				qce.transFee->setText(tmp);
			} else {
				if (tmp.mid(tmp.find('.'), tmp.length()).length() == 2)
				{
					tmp.append("0");
					qce.transFee->setText(tmp);
				}
			}
		}
		if (qce.transAmount->text().contains('.') == 0)
		{
			QString text = qce.transAmount->text();
			text.append(".00");
			qce.transAmount->setText(text);
		} else {
			QString tmp = qce.transAmount->text();
			if (tmp.mid(tmp.find('.'), tmp.length()).length() == 1)
			{
				tmp.append("00");
				qce.transAmount->setText(tmp);
			} else {
				if (tmp.mid(tmp.find('.'), tmp.length()).length() == 2)
				{
					tmp.append("0");
					qce.transAmount->setText(tmp);
				}
			}
		}

		QString recent;
		if (qce.descriptionCombo->count() != 0)
		{
			QStringList recentlist = QStringList::split(',', originaldata[9], false);
			if (recentlist.count() >= 10)
			{
				recentlist.remove(recentlist.last());
			}
			recentlist.prepend(qce.descriptionCombo->lineEdit()->text());
			recent = recentlist.join(",");
		} else {
			recent = qce.descriptionCombo->lineEdit()->text();
		}

		QString checkNumberString = qce.checkNumber->text();
		if (checkNumberString.isEmpty() == true)
		{
			checkNumberString = "0";
		}

		QString paymentChecked = "true";
		if (qce.payment->isChecked() == false)
		{
			paymentChecked = "false";
		}
		QStringList returnvalue;
		returnvalue << paymentChecked << qce.descriptionCombo->lineEdit()->text() << qce.cmbCategory->lineEdit()->text() << QString::number(qce.transType->currentItem()) << checkNumberString << qce.transAmount->text() << qce.transFee->text() << qce.dateEdit->text() << qce.additionalNotes->text() << recent;
		return returnvalue;
	} else {
		QStringList blank;
		return blank;
	}
}

void QCheckEntry::transFeeChanged(const QString &input)
{
	QString tmpinput = input;
	if (tmpinput.contains('.') > 1)
	{
		int first = tmpinput.find('.');
		tmpinput = tmpinput.remove(tmpinput.find('.', (first + 1)), 1);
	}
	if (tmpinput.contains(QRegExp("\\.[0-9][0-9]{2}$")) >= 1)
	{
		tmpinput = tmpinput.remove((tmpinput.length() - 1), 1);
	}
	transFee->setText(tmpinput);
}

void QCheckEntry::amountChanged(const QString &input)
{
	QString tmpinput = input;
	if (tmpinput.contains('.') > 1)
	{
		int first = tmpinput.find('.');
		tmpinput = tmpinput.remove(tmpinput.find('.', (first + 1)), 1);
	}
	if (tmpinput.contains(QRegExp("\\.[0-9][0-9]{2}$")) >= 1)
	{
		tmpinput = tmpinput.remove((tmpinput.length() - 1), 1);
	}
	transAmount->setText(tmpinput);
}

void QCheckEntry::accept()
{
	// Does the description combo not have any text in it? Do something if it doesn't!
	if (descriptionCombo->lineEdit()->text().isEmpty() == true)
	{
		QMessageBox::critical(this, "Field Missing.", "<qt>You didn't enter a description for this transaction. Please fill out the \"Transaction Description\" field and try again.</qt>");
		descriptionCombo->setFocus();
		return;
	}
	QDialog::accept();
}
